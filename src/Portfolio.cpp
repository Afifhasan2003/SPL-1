// Portfolio.cpp
#include "../include/Portfolio.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Constructor
Portfolio::Portfolio(string portfolioName) {
    name = portfolioName;
    cashBalance = 0.0;
}

// Buy stock
void Portfolio::buyStock(string symbol, int quantity, double price, string date) {
    double totalCost = quantity * price;
    
    // Check if enough cash
    if (totalCost > cashBalance) {
        cout << "Error: Not enough cash. Need $" << totalCost 
             << " but have $" << cashBalance << endl;
        return;
    }
    
    // Deduct cash
    cashBalance -= totalCost;
    
    // Add or update holding
    if (holdings.find(symbol) != holdings.end()) {
        // Already own this stock - update average cost
        Holding& h = holdings[symbol];
        double totalValue = (h.quantity * h.avgCost) + totalCost;
        h.quantity += quantity;
        h.avgCost = totalValue / h.quantity;
    } else {
        // New stock
        Holding h;
        h.symbol = symbol;
        h.quantity = quantity;
        h.avgCost = price;
        h.purchaseDate = date;
        holdings[symbol] = h;
    }
    
    // Record transaction
    string transaction = "BUY " + to_string(quantity) + " " + symbol + 
                        " @ $" + to_string(price) + " on " + date;
    transactions.push_back(transaction);
    
    cout << "✓ Bought " << quantity << " shares of " << symbol << endl;
}

// Sell stock
void Portfolio::sellStock(string symbol, int quantity, double price, string date) {
    // Check if we own this stock
    if (holdings.find(symbol) == holdings.end()) {
        cout << "Error: You don't own " << symbol << endl;
        return;
    }
    
    Holding& h = holdings[symbol];
    
    // Check if enough quantity
    if (h.quantity < quantity) {
        cout << "Error: You only have " << h.quantity << " shares of " << symbol << endl;
        return;
    }
    
    // Add cash from sale
    double revenue = quantity * price;
    cashBalance += revenue;
    
    // Update holding
    h.quantity -= quantity;
    
    // Remove if quantity is 0
    if (h.quantity == 0) {
        holdings.erase(symbol);
    }
    
    // Record transaction
    string transaction = "SELL " + to_string(quantity) + " " + symbol + 
                        " @ $" + to_string(price) + " on " + date;
    transactions.push_back(transaction);
    
    cout << "✓ Sold " << quantity << " shares of " << symbol << endl;
}

// Getters
string Portfolio::getName() const {
    return name;
}

double Portfolio::getCashBalance() const {
    return cashBalance;
}

void Portfolio::addCash(double amount) {
    cashBalance += amount;
    cout << "✓ Added $" << amount << " to portfolio" << endl;
}

// Display holdings
void Portfolio::displayHoldings() const {
    cout << "\n=== Holdings in '" << name << "' ===" << endl;
    
    if (holdings.empty()) {
        cout << "No holdings yet." << endl;
        return;
    }
    
    cout << fixed << setprecision(2);
    cout << "\nSymbol\tQuantity\tAvg Cost\tTotal Cost" << endl;
    cout << "------------------------------------------------" << endl;
    
    for (const auto& pair : holdings) {
        const Holding& h = pair.second;
        double totalCost = h.quantity * h.avgCost;
        
        cout << h.symbol << "\t" 
             << h.quantity << "\t\t$"
             << h.avgCost << "\t\t$"
             << totalCost << endl;
    }
    
    cout << "\nCash Balance: $" << cashBalance << endl;
}

// Display transactions
void Portfolio::displayTransactions() const {
    cout << "\n=== Transaction History ===" << endl;
    
    if (transactions.empty()) {
        cout << "No transactions yet." << endl;
        return;
    }
    
    for (int i = 0; i < transactions.size(); i++) {
        cout << i + 1 << ". " << transactions[i] << endl;
    }
}

// Display summary with current values
void Portfolio::displaySummary(const map<string, Stock*>& stockData) const {
    cout << "\n=== Portfolio Summary: '" << name << "' ===" << endl;
    
    if (holdings.empty()) {
        cout << "No holdings yet." << endl;
        cout << "Cash Balance: $" << fixed << setprecision(2) << cashBalance << endl;
        return;
    }
    
    cout << fixed << setprecision(2);
    cout << "\nSymbol\tQty\tAvg Cost\tCurrent\t\tProfit/Loss" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    double totalValue = 0.0;
    double totalCost = 0.0;
    
    for (const auto& pair : holdings) {
        const Holding& h = pair.second;
        double costBasis = h.quantity * h.avgCost;
        totalCost += costBasis;
        
        // Get current price if stock data available
        if (stockData.find(h.symbol) != stockData.end()) {
            // For now, we'll add a method to get latest price later
            cout << h.symbol << "\t" << h.quantity << "\t$" << h.avgCost 
                 << "\t\t(Load stock data)" << "\t$0.00" << endl;
        } else {
            cout << h.symbol << "\t" << h.quantity << "\t$" << h.avgCost 
                 << "\t\tN/A\t\tN/A" << endl;
        }
    }
    
    cout << "\nTotal Cost Basis: $" << totalCost << endl;
    cout << "Cash Balance: $" << cashBalance << endl;
    cout << "Total Portfolio Value: $" << (totalCost + cashBalance) << endl;
}

// Check functions
bool Portfolio::hasStock(string symbol) const {
    return holdings.find(symbol) != holdings.end();
}

int Portfolio::getQuantity(string symbol) const {
    if (holdings.find(symbol) != holdings.end()) {
        return holdings.at(symbol).quantity;
    }
    return 0;
}

// Save/Load functions (basic version)
bool Portfolio::saveToFile(string filename) const {
    // TODO: Implement file saving
    cout << "Save functionality coming soon..." << endl;
    return true;
}

bool Portfolio::loadFromFile(string filename) {
    // TODO: Implement file loading
    cout << "Load functionality coming soon..." << endl;
    return true;
}