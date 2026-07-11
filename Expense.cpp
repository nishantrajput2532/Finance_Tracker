#include "Expense.h"
#include <sstream>
#include <stdexcept>

std::string Expense::toCSVLine() const {
    // Format: amount,category,date,description
    // Description is placed LAST on purpose: even if it contains a comma
    // (e.g., "Lunch, coffee and snacks"), parsing still works correctly
    // because we only split on the FIRST three commas (see fromCSVLine).
    std::ostringstream oss;
    oss << amount_ << ',' << category_ << ',' << date_ << ',' << description_;
    return oss.str();
}

bool Expense::fromCSVLine(const std::string& line, Expense& outExpense) {
    if (line.empty()) return false;

    // Locate the first three commas -> they separate amount, category, date.
    // Everything after the 3rd comma (however many commas it contains) is
    // treated as the description.
    size_t pos1 = line.find(',');
    if (pos1 == std::string::npos) return false;

    size_t pos2 = line.find(',', pos1 + 1);
    if (pos2 == std::string::npos) return false;

    size_t pos3 = line.find(',', pos2 + 1);
    if (pos3 == std::string::npos) return false;

    std::string amountStr   = line.substr(0, pos1);
    std::string category    = line.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string date        = line.substr(pos2 + 1, pos3 - pos2 - 1);
    std::string description = line.substr(pos3 + 1);

    try {
        double amount = std::stod(amountStr);
        outExpense = Expense(amount, category, date, description);
        return true;
    } catch (const std::exception&) {
        // A corrupted / hand-edited line shouldn't crash the whole program -
        // we simply skip it and keep loading the rest of the file.
        return false;
    }
}
