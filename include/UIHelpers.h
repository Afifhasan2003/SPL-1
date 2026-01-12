#ifndef UIHELPERS_H
#define UIHELPERS_H

#include <string>
#include <map>
#include <utility>
#include "Stock.h"

class UIHelpers {
public:
    // Load stock if not already loaded
    static bool loadStockIfNeeded(std::string symbol, std::map<std::string, Stock*>& stocks);
    
    // Get date range from user
    static std::pair<int, int> getDateRange(Stock* stock);

    static void clearScreen();
    static void pauseScreen();
};

#endif