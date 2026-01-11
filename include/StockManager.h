#ifndef STOCKMANAGER_H
#define STOCKMANAGER_H

#include <map>
#include <string>
#include "Stock.h"

class StockManager {
public:
    // Load stock data from CSV
    static void loadStockData(std::map<std::string, Stock*>& stocks);
    
    // View stock info
    static void viewStockInfo(std::map<std::string, Stock*>& stocks);
    
    // View indicators
    static void viewIndicators(std::map<std::string, Stock*>& stocks);
    
    // View analytics
    static void viewAnalytics(std::map<std::string, Stock*>& stocks);
    
    // Backtest strategy
    static void backtestStrategy(std::map<std::string, Stock*>& stocks);
};

#endif 