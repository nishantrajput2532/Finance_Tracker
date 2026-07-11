#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>
#include <string_view>

// ---------------------------------------------------------------------------
// Expense Class
// ---------------------------------------------------------------------------
// OOP Principle: ENCAPSULATION
// All data members are private. Outside code can only read/modify them
// through the public getter/setter interface below. This protects the
// object's internal state from being changed in unexpected ways and keeps
// the "what data does an Expense have" concern separate from how it's
// stored, displayed, or persisted (that logic lives in FinanceTracker).
// ---------------------------------------------------------------------------
class Expense {
private:
    double amount_;
    std::string category_;
    std::string date_;         // Stored as "YYYY-MM-DD" for consistency
    std::string description_;

public:
    // Default constructor - required so an Expense can be default-created
    // (e.g., as an "out parameter" in Expense::fromCSVLine) before being
    // fully populated.
    Expense() : amount_(0.0) {}

    // Parameterized constructor.
    // NOTE the parameter types: std::string_view instead of `const std::string&`.
    // string_view is a C++17 lightweight, NON-OWNING "view" over a string
    // (internally just a pointer + length). Passing it costs no heap
    // allocation, unlike passing/copying a std::string. We only pay for an
    // actual copy ONCE inside the constructor body, at the point the data
    // needs to be OWNED long-term by this object.
    Expense(double amount, std::string_view category,
            std::string_view date, std::string_view description)
        : amount_(amount),
          category_(category),       // string_view -> std::string conversion
          date_(date),                // happens here, exactly once
          description_(description) {}

    // ---- Getters (const: guarantee they don't modify object state) ----
    double getAmount() const { return amount_; }
    const std::string& getCategory() const { return category_; }
    const std::string& getDate() const { return date_; }
    const std::string& getDescription() const { return description_; }

    // ---- Setters ----
    void setAmount(double amount) { amount_ = amount; }
    void setCategory(std::string_view category) { category_ = category; }
    void setDate(std::string_view date) { date_ = date; }
    void setDescription(std::string_view description) { description_ = description; }

    // Serializes this Expense into one CSV line for file storage.
    std::string toCSVLine() const;

    // Factory-style static function: parses one CSV line from the data
    // file back into an Expense object. Returns true on success, false if
    // the line was malformed (so callers can skip corrupted rows safely).
    static bool fromCSVLine(const std::string& line, Expense& outExpense);
};

#endif // EXPENSE_H
