#include "../include/StockManager.h"
#include "../include/UIHelpers.h"
#include "../include/Analytics.h"
#include "../include/Strategy.h"
#include "../include/Backtester.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

     void StockManager::loadStockData(map<string, Stock*>& stocks){
        string symbol, name, filename;

            cout << "\nEnter stock symbol: ";
            cin >> symbol;
            cout << "Enter company name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter CSV filename: ";
            getline(cin, filename);
            

            Stock *newStock = new Stock(symbol, name);

            if (newStock->loadFromCSV(filename))
            {
                stocks[symbol] = newStock;
                cout << "Successfully loaded " << symbol << "!" << endl;
            }
            else
            {
                cout << " Failed to load stock." << endl;
                delete newStock;
            }
     }

    void StockManager::viewStockInfo(map<string, Stock*>& stocks){
        if (stocks.empty())
            {
                cout << "\nNo stocks loaded yet." << endl;
            }
            else
            {
                cout << "\n=== Loaded Stocks ===" << endl;
                for (auto &pair : stocks)
                {
                    cout << "- " << pair.first << endl;
                }

                string symbol;
                cout << "Enter symbol to view: ";
                cin >> symbol;

                if (stocks.find(symbol) != stocks.end())
                {
                    stocks[symbol]->displaySummary();

                    int days;
                    cout << "\nShow recent days (0 to skip): ";
                    cin >> days;

                    if (days > 0)
                    {
                        stocks[symbol]->displayRecentData(days);
                    }
                }
                else
                    cout << "Stock not found." << endl;
            }
    }

    void StockManager::viewIndicators(map<string,Stock*> &stocks){
        
    }