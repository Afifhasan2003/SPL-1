#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include "Stock.h"

using namespace std;

class Analytics
{
public:
    static vector<double> calculateDailyReturns(Stock *stock);
    static vector<double> calculateDailyReturns(Stock *stock, int startDay, int endDay);

    static double calculateCumulativeReturn(Stock *stock);
    static double calculateCumulativeReturn(Stock *stock, int startDay, int endDay);

    static double calculateVolatility(vector<double> &returns); // returs are filtered by time here

    static double calculateSharpeRatio(vector<double> &returns, double riskFreeRate = 0.02); // for example bonds give 2% return which is risk free

    static double calculateMaxDrawdown(Stock *stock);

    static void displayAnalyticsReport(Stock *stock);
    static void displayAnalyticsReport(Stock *stock, int startDay, int endDay);

private:
    static double calculateMean(vector<double> &data);
    static double calculateStdDev(vector<double> &data);
};

#endif