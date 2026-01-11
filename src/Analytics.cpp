#include "../include/Analytics.h"
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

vector<double> Analytics::calculateDailyReturns(Stock *stock)
{
    return calculateDailyReturns(stock, 1, stock->getDataSize() - 1);
}

vector<double> Analytics::calculateDailyReturns(Stock *stock, int startDay, int endDay)
{
    vector<double> returns;

    for (int i = startDay; i <= endDay; i++)
    {
        double today = stock->getClosePrice(i);
        double yesterday = stock->getClosePrice(i - 1);

        if (yesterday != 0)
        {
            double dailyReturn = ((today - yesterday) / yesterday) * 100;
            returns.push_back(dailyReturn);
        }
    }
    return returns;
}

double Analytics::calculateCumulativeReturn(Stock *stock)
{
    return calculateCumulativeReturn(stock, 0, stock->getDataSize() - 1);
}

double Analytics::calculateCumulativeReturn(Stock *stock, int startDay, int endDay)
{

    if (startDay > endDay || startDay < 0)
        return 0;

    double firstClose = stock->getClosePrice(startDay);
    double lastClose = stock->getClosePrice(endDay);
    if (firstClose == 0)
        return 0;

    double cumulative = ((lastClose - firstClose) / firstClose) * 100;
    return cumulative;
}
double Analytics::calculateVolatility(vector<double> &returns)
{
    if (returns.size() < 2)
        return 0;

    double stdDev = calculateStdDev(returns);

    return stdDev * sqrt(252);
}
double Analytics::calculateSharpeRatio(vector<double> &returns, double riskFreeRate)
{ // for example bonds give 2% return which is risk free
    if (returns.size() < 2)
        return 0;
    double avgReturn = calculateMean(returns);
    double vol = calculateVolatility(returns);

    if (vol == 0)
        return 0;

    double annualReturn = avgReturn * 252;

    double sharpe = (annualReturn - riskFreeRate) / vol;
    return sharpe;
    // risk free ra
}
double Analytics::calculateMaxDrawdown(Stock *stock)
{
    int dataSize = stock->getDataSize();
    if (dataSize < 2)
        return 0;

    double drawDown, maxDrawDown = 0;
    double peak = stock->getClosePrice(0);

    for (int i = 1; i < dataSize; i++)
    {
        double currPrice = stock->getClosePrice(i);

        if (currPrice > peak)
        {
            peak = currPrice;
        }

        drawDown = ((peak - currPrice) / peak) * 100;

        if (drawDown > maxDrawDown)
        {
            maxDrawDown = drawDown;
        }
    }

    return maxDrawDown;
}

void Analytics::displayAnalyticsReport(Stock *stock)
{
    displayAnalyticsReport(stock, 0, stock->getDataSize() - 1);
}

void Analytics::displayAnalyticsReport(Stock *stock, int startDay, int endDay)
{
    cout << "\n=== Analytics Report for " << stock->getSymbol() << " ===" << endl;
    cout << fixed << setprecision(2);

    vector<double> returns = calculateDailyReturns(stock, startDay, endDay);

    cout << "\nPerformance Metrics: From day: " << startDay << " to: " << endDay << " (" << (endDay - startDay + 1) << " days)" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Cumulative Return: " << calculateCumulativeReturn(stock, startDay, endDay) << "%" << endl;
    cout << "Volatility (Annualized): " << calculateVolatility(returns) << "%" << endl;
    cout << "Sharpe Ratio: " << calculateSharpeRatio(returns, 2) << endl;
    cout << "Maximum Drawdown: " << calculateMaxDrawdown(stock) << "%" << endl;
}
double Analytics::calculateMean(vector<double> &data)
{
    if (data.empty())
        return 0;

    double sum = 0;
    for (auto val : data)
        sum += val;

    return sum / data.size();
}
double Analytics::calculateStdDev(vector<double> &data)
{
    if (data.size() < 2)
        return 0;

    double mean = calculateMean(data);
    double var = 0;

    for (auto val : data)
    {
        double diff = val - mean;
        var += diff * diff;
    }

    var = var / data.size();

    return sqrt(var);
}