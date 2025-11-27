// Stock.cpp
#include "../include/Stock.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Helper function to trim spaces
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return str.substr(start, end - start + 1);
}

// Constructor
Stock::Stock(string sym, string stockName) {        //returns
    symbol = sym; 
    name = stockName;
}

// Load data from CSV file
bool Stock::loadFromCSV(string filename) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }
    
    string line;
    int lineNumber = 0;
    
    while (getline(file, line)) {
        lineNumber++;
        
        // Skip header
        if (lineNumber == 1) continue;
        
        // Parse line
        stringstream ss(line);
        string date, open, high, low, close, volume;
        
        getline(ss, date, ',');
        getline(ss, open, ',');
        getline(ss, high, ',');
        getline(ss, low, ',');
        getline(ss, close, ',');
        getline(ss, volume, ',');
        
        // Trim and store
        dates.push_back(trim(date));
        openPrices.push_back(stod(trim(open)));
        highPrices.push_back(stod(trim(high)));
        lowPrices.push_back(stod(trim(low)));
        closePrices.push_back(stod(trim(close)));
        volumes.push_back(stoll(trim(volume)));
    }
    
    file.close();
    return true;
}

// Getters
string Stock::getSymbol() const {
    return symbol;
}

string Stock::getName() const {
    return name;
}

int Stock::getDataSize() const {
    return dates.size();
}

// Display summary
void Stock::displaySummary() const {
    cout << "\n=== " << symbol << " - " << name << " ===" << endl;
    cout << "Total days of data: " << dates.size() << endl;
    
    if (dates.size() > 0) {
        cout << "Date range: " << dates[0] << " to " << dates[dates.size()-1] << endl;
        cout << "Latest close: $" << closePrices[closePrices.size()-1] << endl;
    }
}

// Display recent data
void Stock::displayRecentData(int numDays) const {
    if (dates.size() == 0) {
        cout << "No data available." << endl;
        return;
    }
    
    int start = max(0, (int)dates.size() - numDays);
    
    cout << "\nRecent " << numDays << " days:" << endl;
    cout << "Date\t\tOpen\tHigh\tLow\tClose\tVolume" << endl;
    cout << "--------------------------------------------------------" << endl;
    
    for (int i = start; i < dates.size(); i++) {
        cout << dates[i] << "\t"
             << openPrices[i] << "\t"
             << highPrices[i] << "\t"
             << lowPrices[i] << "\t"
             << closePrices[i] << "\t"
             << volumes[i] << endl;
    }
}