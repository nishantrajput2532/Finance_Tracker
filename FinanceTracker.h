#ifndef FINANCE_TRACKER_H
#define FINANCE_TRACKER_H

#include "Expense.h"
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <filesystem>

// ---------------------------------------------------------------------------
// FinanceTracker Class
// ---------------------------------------------------------------------------
// OOP Principle: ENCAPSULATION + SINGLE RESPONSIBILITY
// This class OWNS the collection of Expense objects and is solely
// responsible for adding, displaying, saving, and loading them. The
// Expense class itself has no idea about vectors or files - that
// separation of concerns keeps each class focused on one job, which
// makes the code easier to read, test, and extend later.
// ---------------------------------------------------------------------------
class FinanceTracker {
private:
    // std::vector<Expense> chosen because:
    //  - Expenses are added far more often than removed -> O(1) amortized
    //    push_back is ideal.
    //  - We want to preserve insertion order (roughly chronological).
    //  - Contiguous memory layout -> cache-friendly iteration when
    //    printing all expenses or computing totals.
    std::vector<Expense> expenses_;

    // std::filesystem::path (C++17) instead of a raw std::string for file
    // paths. It transparently handles path-separator differences across
    // operating systems (Windows "\" vs Linux/macOS "/") and provides
    // safe path-joining via operator/.
    std::filesystem::path dataDirectory_;
    std::filesystem::path dataFilePath_;

    // Makes sure the data directory exists before any read/write attempt.
    void ensureDataDirectoryExists() const;

public:
    // Constructor: sets up the storage location, ensures the folder
    // exists, and immediately auto-loads any previously saved data.
    explicit FinanceTracker(std::string_view dataDirectory = "data",
                             std::string_view fileName = "expenses.csv");

    // ---- Core Functionality ----
    void addExpense(double amount, std::string_view category,
                     std::string_view date, std::string_view description);

    void viewAllExpenses() const;
    void viewSummaryByCategory() const;

    // Deletes the expense at the given 1-based position (matching the
    // numbered list shown by viewAllExpenses). Returns true if deleted,
    // false if the index was out of range - so the caller can show an
    // appropriate error message instead of crashing.
    bool deleteExpense(size_t oneBasedIndex);

    // ---- Persistence ----
    void saveToFile() const;
    void loadFromFile();

    // ---- Utility ----
    size_t getExpenseCount() const;
    double getTotalSpent() const;
};

#endif // FINANCE_TRACKER_H
