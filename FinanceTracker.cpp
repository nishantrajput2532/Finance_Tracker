#include "FinanceTracker.h"
#include <iostream>
#include <fstream>
#include <iomanip>

namespace fs = std::filesystem; // convenience alias for the C++17 filesystem library

FinanceTracker::FinanceTracker(std::string_view dataDirectory, std::string_view fileName) {
    dataDirectory_ = fs::path(dataDirectory);
    dataFilePath_  = dataDirectory_ / std::string(fileName); // path::operator/ joins paths safely

    ensureDataDirectoryExists();
    loadFromFile(); // Auto-load: read any existing data the moment the tracker is created
}

void FinanceTracker::ensureDataDirectoryExists() const {
    // std::filesystem::exists() (C++17) checks presence in one portable call -
    // no manual OS-specific stat()/GetFileAttributes() code needed.
    if (!fs::exists(dataDirectory_)) {
        fs::create_directory(dataDirectory_); // C++17: creates the folder if missing
        std::cout << "[Info] Created data directory: " << dataDirectory_ << "\n";
    }
}

void FinanceTracker::addExpense(double amount, std::string_view category,
                                 std::string_view date, std::string_view description) {
    // The string_view parameters above mean nothing is copied just to pass
    // arguments into this function. The one real copy happens inside
    // Expense's constructor, where the data must be OWNED for the long term.
    expenses_.emplace_back(amount, category, date, description);

    std::cout << "\n[Success] Expense added successfully!\n";

    // Auto-save immediately so data survives an unexpected exit (crash,
    // closed terminal, power cut) instead of only saving once at the end.
    saveToFile();
}

void FinanceTracker::viewAllExpenses() const {
    if (expenses_.empty()) {
        std::cout << "\nNo expenses recorded yet.\n";
        return;
    }

    std::cout << "\n===================== ALL EXPENSES =====================\n";
    std::cout << std::left
              << std::setw(6)  << "No."
              << std::setw(12) << "Amount"
              << std::setw(15) << "Category"
              << std::setw(14) << "Date"
              << "Description\n";
    std::cout << "----------------------------------------------------------\n";

    int index = 1;
    for (const auto& expense : expenses_) { // range-based for over std::vector
        std::cout << std::left
                  << std::setw(6)  << index++
                  << std::setw(12) << std::fixed << std::setprecision(2) << expense.getAmount()
                  << std::setw(15) << expense.getCategory()
                  << std::setw(14) << expense.getDate()
                  << expense.getDescription() << "\n";
    }

    std::cout << "----------------------------------------------------------\n";
    std::cout << "Total Expenses: " << expenses_.size()
              << " | Total Spent: " << std::fixed << std::setprecision(2)
              << getTotalSpent() << "\n";
    std::cout << "==========================================================\n";
}

void FinanceTracker::viewSummaryByCategory() const {
    if (expenses_.empty()) {
        std::cout << "\nNo expenses recorded yet.\n";
        return;
    }

    // std::map<std::string, double> chosen over std::unordered_map here
    // because:
    //   1. std::map keeps keys in SORTED order automatically, so the
    //      category summary prints alphabetically with zero extra sort
    //      logic - cleaner output "for free".
    //   2. The number of distinct categories is always small (Food, Rent,
    //      Travel, ...), so std::map's O(log n) operations vs
    //      unordered_map's average O(1) make no practical performance
    //      difference here - readability wins.
    std::map<std::string, double> categoryTotals;
    std::map<std::string, int> categoryCounts;

    for (const auto& expense : expenses_) {
        categoryTotals[expense.getCategory()] += expense.getAmount();
        categoryCounts[expense.getCategory()]++;
    }

    std::cout << "\n================ SUMMARY BY CATEGORY ================\n";
    std::cout << std::left << std::setw(20) << "Category"
              << std::setw(15) << "Total Spent"
              << "Count\n";
    std::cout << "------------------------------------------------------\n";

    // Structured Bindings (C++17): `auto [category, total]` destructures
    // each std::pair<const std::string, double> straight into two named
    // variables, instead of the old `it->first` / `it->second` style.
    // This is one of the most interview-worthy C++17 features to mention.
    for (const auto& [category, total] : categoryTotals) {
        std::cout << std::left << std::setw(20) << category
                   << std::setw(15) << std::fixed << std::setprecision(2) << total
                   << categoryCounts[category] << "\n";
    }

    std::cout << "------------------------------------------------------\n";
    std::cout << "Grand Total: " << std::fixed << std::setprecision(2)
              << getTotalSpent() << "\n";
    std::cout << "=======================================================\n";
}

void FinanceTracker::saveToFile() const {
    // Truncate-and-rewrite is the simplest reliable persistence strategy
    // for a small, personal-scale dataset like this.
    std::ofstream outFile(dataFilePath_);
    if (!outFile.is_open()) {
        std::cerr << "[Error] Could not open file for saving: " << dataFilePath_ << "\n";
        return;
    }

    for (const auto& expense : expenses_) {
        outFile << expense.toCSVLine() << "\n";
    }
    // outFile's destructor closes the file automatically (RAII) -
    // no manual outFile.close() needed, and it's exception-safe.
}

void FinanceTracker::loadFromFile() {
    // Only attempt to read if the file actually exists. Checking this
    // with std::filesystem::exists() up front avoids relying on
    // std::ifstream's fail state to distinguish "file missing" (expected
    // on first run) from a real I/O error.
    if (!fs::exists(dataFilePath_)) {
        return; // First run - nothing to load yet, which is normal.
    }

    std::ifstream inFile(dataFilePath_);
    if (!inFile.is_open()) {
        std::cerr << "[Error] Could not open file for loading: " << dataFilePath_ << "\n";
        return;
    }

    expenses_.clear();
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        Expense parsedExpense;
        if (Expense::fromCSVLine(line, parsedExpense)) {
            expenses_.push_back(parsedExpense);
        }
        // Malformed lines are silently skipped (see Expense::fromCSVLine) -
        // one bad row shouldn't prevent the rest of the data from loading.
    }

    if (!expenses_.empty()) {
        std::cout << "[Info] Loaded " << expenses_.size()
                  << " existing expense(s) from " << dataFilePath_ << "\n";
    }
}

size_t FinanceTracker::getExpenseCount() const {
    return expenses_.size();
}

double FinanceTracker::getTotalSpent() const {
    double total = 0.0;
    for (const auto& expense : expenses_) {
        total += expense.getAmount();
    }
    return total;
}
