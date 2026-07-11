# Personal Finance & Budget Tracker (C++17)

A command-line application to track daily expenses, built to practice
Object-Oriented Programming and modern C++17 features.

## Features
- Add expenses with amount, category, date, and description
- View all recorded expenses in a formatted table
- Delete an expense by its number in the list
- View a summary of total spending grouped by category
- Automatically saves data to a CSV file and loads it on startup
- Handles invalid input gracefully (e.g., text entered instead of a number)

## Tech Used
- C++17 (`std::filesystem`, structured bindings, `std::string_view`)
- STL containers: `std::vector`, `std::map`
- Object-Oriented Design (separate `Expense` and `FinanceTracker` classes)

## How to Run
\`\`\`bash
g++ -std=c++17 -Wall -Wextra -o finance_tracker main.cpp Expense.cpp FinanceTracker.cpp
./finance_tracker
\`\`\`
