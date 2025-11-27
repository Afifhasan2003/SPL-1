// Portfolio.h
#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <string>
#include <map>
#include <vector>
#include "Stock.h"

using namespace std;

// Represents a single holding (one stock in portfolio)
struct Holding {
    string symbol;
    int quantity;
    double avgCost;        // Average purchase price
    string purchaseDate;   // When first bought
};

class Portfolio {
private:
    string name;
    map<string, Holding> holdings;  // symbol -> Holding
    vector<string> transactions;     // History of all buys/sells
    double cashBalance;
    
public:
    // Constructor
    Portfolio(string portfolioName);
    
    // Buy/Sell operations
    void buyStock(string symbol, int quantity, double price, string date);
    void sellStock(string symbol, int quantity, double price, string date);
    
    // Portfolio info
    string getName() const;
    double getCashBalance() const;
    void addCash(double amount);
    
    // Display functions
    void displayHoldings() const;
    void displayTransactions() const;
    void displaySummary(const map<string, Stock*>& stockData) const;
    
    // Check if portfolio has a stock
    bool hasStock(string symbol) const;
    int getQuantity(string symbol) const;
    
    // Save/Load portfolio to file
    bool saveToFile(string filename) const;
    bool loadFromFile(string filename);
};

#endif