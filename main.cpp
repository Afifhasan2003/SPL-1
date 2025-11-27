// main.cpp
#include <iostream>
#include <vector>
#include <map>
#include "include/Stock.h"
#include "include/Portfolio.h"

using namespace std;

void displayMainMenu() {
    cout << "\n======================================" << endl;
    cout << "      QuantLab - Main Menu" << endl;
    cout << "======================================" << endl;
    cout << "1. Manage Portfolios" << endl;
    cout << "2. Load Stock Data" << endl;
    cout << "3. View Stock Info" << endl;
    cout << "4. Exit" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}

void displayPortfolioMenu() {
    cout << "\n======================================" << endl;
    cout << "      Portfolio Management" << endl;
    cout << "======================================" << endl;
    cout << "1. Create new portfolio" << endl;
    cout << "2. View all portfolios" << endl;
    cout << "3. Select portfolio" << endl;
    cout << "4. Back to main menu" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}

void displaySelectedPortfolioMenu(string portfolioName) {
    cout << "\n======================================" << endl;
    cout << "  Portfolio: " << portfolioName << endl;
    cout << "======================================" << endl;
    cout << "1. Add cash" << endl;
    cout << "2. Buy stock" << endl;
    cout << "3. Sell stock" << endl;
    cout << "4. View holdings" << endl;
    cout << "5. View transactions" << endl;
    cout << "6. View summary" << endl;
    cout << "7. Back" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}

int main() {
    map<string, Stock*> stocks;           // symbol -> Stock object
    vector<Portfolio*> portfolios;         // All portfolios
    
    cout << "\n*** Welcome to QuantLab ***\n" << endl;
    
    while (true) {
        displayMainMenu();
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            // ===== PORTFOLIO MANAGEMENT =====
            while (true) {
                displayPortfolioMenu();
                int portfolioChoice;
                cin >> portfolioChoice;
                
                if (portfolioChoice == 1) {
                    // Create new portfolio
                    string name;
                    cout << "\nEnter portfolio name: ";
                    cin.ignore();
                    getline(cin, name);
                    
                    Portfolio* newPortfolio = new Portfolio(name);
                    portfolios.push_back(newPortfolio);
                    
                    cout << "✓ Portfolio '" << name << "' created!" << endl;
                    
                } else if (portfolioChoice == 2) {
                    // View all portfolios
                    if (portfolios.empty()) {
                        cout << "\nNo portfolios yet. Create one first!" << endl;
                    } else {
                        cout << "\n=== Your Portfolios ===" << endl;
                        for (int i = 0; i < portfolios.size(); i++) {
                            cout << i + 1 << ". " << portfolios[i]->getName() 
                                 << " (Cash: $" << portfolios[i]->getCashBalance() << ")" << endl;
                        }
                    }
                    
                } else if (portfolioChoice == 3) {
                    // Select portfolio
                    if (portfolios.empty()) {
                        cout << "\nNo portfolios yet. Create one first!" << endl;
                        continue;
                    }
                    
                    cout << "\n=== Select Portfolio ===" << endl;
                    for (int i = 0; i < portfolios.size(); i++) {
                        cout << i + 1 << ". " << portfolios[i]->getName() << endl;
                    }
                    
                    int select;
                    cout << "Enter number: ";
                    cin >> select;
                    
                    if (select < 1 || select > portfolios.size()) {
                        cout << "Invalid selection." << endl;
                        continue;
                    }
                    
                    Portfolio* currentPortfolio = portfolios[select - 1];
                    
                    // Selected portfolio menu
                    while (true) {
                        displaySelectedPortfolioMenu(currentPortfolio->getName());
                        int action;
                        cin >> action;
                        
                        if (action == 1) {
                            // Add cash
                            double amount;
                            cout << "Enter amount to add: $";
                            cin >> amount;
                            currentPortfolio->addCash(amount);
                            
                        } else if (action == 2) {
                            // Buy stock
                            string symbol, date;
                            int quantity;
                            double price;
                            
                            cout << "Enter stock symbol: ";
                            cin >> symbol;
                            cout << "Enter quantity: ";
                            cin >> quantity;
                            cout << "Enter price per share: $";
                            cin >> price;
                            cout << "Enter date (YYYY-MM-DD): ";
                            cin >> date;
                            
                            currentPortfolio->buyStock(symbol, quantity, price, date);
                            
                        } else if (action == 3) {
                            // Sell stock
                            string symbol, date;
                            int quantity;
                            double price;
                            
                            cout << "Enter stock symbol: ";
                            cin >> symbol;
                            cout << "Enter quantity: ";
                            cin >> quantity;
                            cout << "Enter price per share: $";
                            cin >> price;
                            cout << "Enter date (YYYY-MM-DD): ";
                            cin >> date;
                            
                            currentPortfolio->sellStock(symbol, quantity, price, date);
                            
                        } else if (action == 4) {
                            // View holdings
                            currentPortfolio->displayHoldings();
                            
                        } else if (action == 5) {
                            // View transactions
                            currentPortfolio->displayTransactions();
                            
                        } else if (action == 6) {
                            // View summary
                            currentPortfolio->displaySummary(stocks);
                            
                        } else if (action == 7) {
                            // Back
                            break;
                            
                        } else {
                            cout << "Invalid choice." << endl;
                        }
                    }
                    
                } else if (portfolioChoice == 4) {
                    // Back to main menu
                    break;
                    
                } else {
                    cout << "Invalid choice." << endl;
                }
            }
            
        } else if (choice == 2) {
            // ===== LOAD STOCK DATA =====
            string symbol, name, filename;
            
            cout << "\nEnter stock symbol: ";
            cin >> symbol;
            cout << "Enter company name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter CSV filename: ";
            getline(cin, filename);
            
            Stock* newStock = new Stock(symbol, name);
            
            if (newStock->loadFromCSV(filename)) {
                stocks[symbol] = newStock;
                cout << "✓ Successfully loaded " << symbol << "!" << endl;
            } else {
                cout << "✗ Failed to load stock." << endl;
                delete newStock;
            }
            
        } else if (choice == 3) {
            // ===== VIEW STOCK INFO =====
            if (stocks.empty()) {
                cout << "\nNo stocks loaded yet." << endl;
            } else {
                cout << "\n=== Loaded Stocks ===" << endl;
                for (const auto& pair : stocks) {
                    cout << "- " << pair.first << endl;
                }
                
                string symbol;
                cout << "Enter symbol to view: ";
                cin >> symbol;
                
                if (stocks.find(symbol) != stocks.end()) {
                    stocks[symbol]->displaySummary();
                    
                    int days;
                    cout << "\nShow recent days (0 to skip): ";
                    cin >> days;
                    
                    if (days > 0) {
                        stocks[symbol]->displayRecentData(days);
                    }
                } else {
                    cout << "Stock not found." << endl;
                }
            }
            
        } else if (choice == 4) {
            // ===== EXIT =====
            cout << "\nThank you for using QuantLab!" << endl;
            
            // Clean up memory
            for (auto& pair : stocks) {
                delete pair.second;
            }
            for (Portfolio* p : portfolios) {
                delete p;
            }
            
            break;
            
        } else {
            cout << "Invalid choice." << endl;
        }
    }
    
    return 0;
}