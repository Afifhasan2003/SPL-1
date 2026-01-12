#include "../include/UIHelpers.h"
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

bool UIHelpers::loadStockIfNeeded(string symbol, map<string, Stock*>& stocks) {
    // Already loaded?
    if (stocks.find(symbol) != stocks.end()) {
        return true;
    }
    
    // Check if CSV file exists
    string filename = "data/stocks/" + symbol + ".csv";
    
    if (!fs::exists(filename)) {
        cout << "✗ Error: " << symbol << " not found in data/ folder" << endl;
        return false;
    }
    
    // Load the stock
    cout << "Loading " << symbol << "..." << endl;
    Stock* newStock = new Stock(symbol, symbol);
    
    if (newStock->loadFromCSV(filename)) {
        stocks[symbol] = newStock;
        cout << "✓ " << symbol << " loaded successfully!" << endl;
        return true;
    } else {
        cout << "✗ Failed to load " << symbol << endl;
        delete newStock;
        return false;
    }
}

pair<int, int> UIHelpers::getDateRange(Stock* stock) {
    int dataSize = stock->getDataSize();
    
    cout << "\n=== Select Time Period ===" << endl;
    cout << "1. Last 30 days" << endl;
    cout << "2. Last 90 days (3 months)" << endl;
    cout << "3. Last 180 days (6 months)" << endl;
    cout << "4. Last 365 days (1 year)" << endl;
    cout << "5. All time" << endl;
    cout << "6. Custom range" << endl;
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;
    
    int startDay = 0;
    int endDay = dataSize - 1;
    
    if (choice == 1) {
        startDay = max(0, endDay - 30);
    } else if (choice == 2) {
        startDay = max(0, endDay - 90);
    } else if (choice == 3) {
        startDay = max(0, endDay - 180);
    } else if (choice == 4) {
        startDay = max(0, endDay - 365);
    } else if (choice == 5) {
        startDay = 0;
    } else if (choice == 6) {
        cout << "Enter start day (0 to " << endDay << "): ";
        cin >> startDay;
        cout << "Enter end day (" << startDay << " to " << endDay << "): ";
        cin >> endDay;
        
        // Validate
        if (startDay < 0) startDay = 0;
        if (endDay >= dataSize) endDay = dataSize - 1;
        if (startDay > endDay) startDay = endDay;
    } else {
        cout << "Invalid choice. Using all time." << endl;
    }
    
    int numDays = endDay - startDay + 1;
    cout << "✓ Analyzing " << numDays << " days of data" << endl;
    
    return make_pair(startDay, endDay);
}

void UIHelpers::clearScreen() {
    #ifdef _WIN32       
        system("cls");
    #else
        system("clear");
    #endif
}


void UIHelpers::pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}