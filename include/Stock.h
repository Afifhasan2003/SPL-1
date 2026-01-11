#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <vector>

using namespace std;

class Stock
{
private:
    // basic info
    string symbol;
    string name;

    // Historical data
    vector<string> dates;
    vector<double> openPrices;
    vector<double> highPrices;
    vector<double> lowPrices;
    vector<double> closePrices;
    vector<long long> volumes;

    // Technical indicators
    vector<double> sma20; // simple moving average of 20days
    vector<double> sma50;
    vector<double> rsi;   // 14-day relative strength index
    vector<double> ema12; // exponential moving average of 12 days
    vector<double> ema26;
    vector<double> macd; // moving average convergence divergence line
    vector<double> macdSignal;
    vector<double> macdHistogram;
    vector<double> bollingerUpper; // boillinger is the bang whose range is the standard deviation
    vector<double> bollingerMiddle;
    vector<double> bollingerLower;
    vector<double> momentum; // just the difference between today's close price and the close price n days ago

public:
    // this is the constructor
    Stock(string sym, string stockName);

    bool loadFromCSV(string filename);

    string getSymbol();
    string getName();
    int getDataSize();
    double getClosePrice(int index);
    vector<double> getAllClosePrices();

    // Display functions
    void displaySummary();
    void displayRecentData(int days);

    // calculate indicators (these will store the values into the vector declared before)
    void calculateSMA(int period);
    void calculateEMA(int period);
    void calculateRSI(int period = 14); // default period is 14 days
    void calculateMACD();
    void calculateBoillingerBands(int period = 20, double numStdDev = 2.0);
    void calculateMomentum(int period = 10);
    void calculateAllIndicators();

    // get indicator values
    double getSMA20(int index);
    double getSMA50(int index);
    double getRSI(int index);
    double getMACD(int index);
    double getMACDSignal(int index);
    double getMACDHistogram(int index);
    double getBollingerUpper(int index);
    double getBollingerMiddle(int index);
    double getBollingerLower(int index);
    double getMomentum(int index);
};

#endif
