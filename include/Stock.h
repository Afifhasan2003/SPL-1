// Stock.h
#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <vector>

using namespace std;

class Stock {
private:
    // Basic info
    string symbol;
    string name;
    
    // Historical data
    vector<string> dates;
    vector<double> openPrices;
    vector<double> highPrices;
    vector<double> lowPrices;
    vector<double> closePrices;
    vector<long long> volumes;
    
public:
    // Constructor
    Stock(string sym, string stockName);
    
    // Load data from CSV
    bool loadFromCSV(string filename);
    
    // Getters
    string getSymbol() const;
    string getName() const;
    int getDataSize() const;
    
    // Display functions
    void displaySummary() const;
    void displayRecentData(int numDays) const;
};

#endif