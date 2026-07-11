#include "FinanceTracker.h"
#include <iostream>
#include <limits>
#include <string>
#include <cctype>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Reads one full line of input safely (so entries like "Grocery Shopping"
// with spaces work correctly, unlike std::cin >> which stops at whitespace).
// ---------------------------------------------------------------------------
std::string readLine(std::string_view prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// ---------------------------------------------------------------------------
// Basic structural validation for a "YYYY-MM-DD" date string.
// Deliberately simple (no third-party date library) but still catches
// the vast majority of real user typos.
// ---------------------------------------------------------------------------
bool isValidDateFormat(std::string_view date) {
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i]))) return false;
    }

    int month = std::stoi(std::string(date.substr(5, 2)));
    int day   = std::stoi(std::string(date.substr(8, 2)));
    return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

// Keeps re-prompting until the user provides a validly formatted date.
std::string readValidDate() {
    while (true) {
        std::string date = readLine("Enter Date (YYYY-MM-DD): ");
        if (isValidDateFormat(date)) {
            return date;
        }
        std::cout << "[Error] Invalid date format. Please use YYYY-MM-DD (e.g., 2025-01-31).\n";
    }
}

// ---------------------------------------------------------------------------
// Keeps re-prompting until a valid positive amount is entered.
// Demonstrates GRACEFUL ERROR HANDLING for the classic "user typed letters
// instead of a number" case, using std::stod + try/catch rather than
// letting the program crash or silently misbehave.
// ---------------------------------------------------------------------------
double readValidAmount() {
    while (true) {
        std::string input = readLine("Enter Amount (e.g., 250.50): ");
        try {
            size_t charsProcessed = 0;
            double amount = std::stod(input, &charsProcessed);

            // std::stod stops at the first non-numeric character instead of
            // failing outright (e.g. "250abc" -> 250.0). We explicitly check
            // that the WHOLE string was consumed to catch cases like that.
            if (charsProcessed != input.size()) {
                throw std::invalid_argument("Trailing characters after number");
            }
            if (amount <= 0) {
                std::cout << "[Error] Amount must be greater than zero.\n";
                continue;
            }
            return amount;
        } catch (const std::invalid_argument&) {
            std::cout << "[Error] That's not a valid number. Please try again.\n";
        } catch (const std::out_of_range&) {
            std::cout << "[Error] That number is too large. Please try again.\n";
        }
    }
}

// ---------------------------------------------------------------------------
// Reads an integer menu choice safely. Uses the classic
// cin.fail() / cin.clear() / cin.ignore() pattern to recover from bad
// input (e.g., user types "abc" at the menu) without crashing or looping
// infinitely.
// ---------------------------------------------------------------------------
int readMenuChoice() {
    while (true) {
        std::cout << "\nEnter your choice: ";
        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear(); // reset the stream's error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard bad input
            std::cout << "[Error] Please enter a valid number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush trailing newline
        return choice;
    }
}

void printMenu() {
    std::cout << "\n==================================================\n";
    std::cout << "     PERSONAL FINANCE & BUDGET TRACKER (C++17)\n";
    std::cout << "==================================================\n";
    std::cout << "1. Add Expense\n";
    std::cout << "2. View All Expenses\n";
    std::cout << "3. View Summary by Category\n";
    std::cout << "4. Delete an Expense\n";
    std::cout << "5. Exit\n";
    std::cout << "==================================================\n";
}

int main() {
    // Constructing FinanceTracker automatically:
    //   - Ensures the "data/" directory exists (std::filesystem)
    //   - Loads any previously saved expenses.csv into memory
    FinanceTracker tracker("data", "expenses.csv");

    bool running = true;
    while (running) {
        printMenu();
        int choice = readMenuChoice();

        switch (choice) {
            case 1: {
                std::cout << "\n--- Add New Expense ---\n";
                double amount = readValidAmount();
                std::string category = readLine("Enter Category (e.g., Food, Rent, Travel): ");
                std::string date = readValidDate();
                std::string description = readLine("Enter Description: ");

                tracker.addExpense(amount, category, date, description);
                break;
            }
            case 2:
                tracker.viewAllExpenses();
                break;
            case 3:
                tracker.viewSummaryByCategory();
                break;
            case 4: {
                // Show the numbered list first so the user knows which
                // number corresponds to which expense before picking one.
                tracker.viewAllExpenses();
                if (tracker.getExpenseCount() == 0) {
                    break;
                }

                std::cout << "\nEnter the expense number to delete: ";
                int indexToDelete = 0;
                std::cin >> indexToDelete;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "[Error] Please enter a valid number.\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (indexToDelete <= 0) {
                    std::cout << "[Error] Please enter a positive expense number.\n";
                    break;
                }

                // static_cast is needed since deleteExpense takes an unsigned
                // size_t but the user types a signed int at the console.
                if (tracker.deleteExpense(static_cast<size_t>(indexToDelete))) {
                    std::cout << "[Success] Expense #" << indexToDelete << " deleted.\n";
                } else {
                    std::cout << "[Error] No expense with that number exists.\n";
                }
                break;
            }
            case 5:
                std::cout << "\nSaving data and exiting. Goodbye!\n";
                tracker.saveToFile(); // final explicit save, belt-and-braces
                running = false;
                break;
            default:
                std::cout << "[Error] Invalid choice. Please select 1-5.\n";
        }
    }

    return 0;
}
