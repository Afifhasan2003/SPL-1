#ifndef ANALYTICS_H
#define ANALYTICS_H

#include<vector>
#include "Stock.h"

using namespace std;

class Analytics{
public:
    static vector<double> calculateDailyReturns(Stock* stock);
    static double calculateCumulativeReturn(Stock* stock);
    static double calculateVolatility(vector<double>& returns);
    static double calculateSharpeRatio(vector<double>& returns, double riskFreeRate = 0.02);     //for example bonds give 2% return which is risk free
    static double calculateMaxDrawdown(Stock* stock);
    static void displayAnalyticsReport(Stock* stock);

private:
    static double calculateMean(vector<double>& data);
    static double calculateStdDev(vector<double>& data);
};

#endif