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
    
    // Technical indicators
    vector<double> sma20;  // 20-day Simple Moving Average
    vector<double> sma50;  // 50-day Simple Moving Average
    vector<double> rsi;    // 14-day Relative Strength Index
    vector<double> ema12;  // 12-day Exponential Moving Average
    vector<double> ema26;  // 26-day Exponential Moving Average
    vector<double> macd;   // MACD Line
    vector<double> macdSignal;  // MACD Signal Line
    vector<double> macdHistogram;  // MACD Histogram
    vector<double> bollingerUpper;  // Bollinger Upper Band
    vector<double> bollingerMiddle; // Bollinger Middle Band
    vector<double> bollingerLower;  // Bollinger Lower Band
    vector<double> momentum;        // Price momentum (10-day)
    
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
    
    // Calculate indicators
    void calculateSMA(int period);
    void calculateEMA(int period);
    void calculateRSI(int period = 14);
    void calculateMACD();
    void calculateBollingerBands(int period = 20, double numStdDev = 2.0);
    void calculateMomentum(int period = 10);
    void calculateAllIndicators();
    
    // Get indicator value at index
    double getSMA20(int index) const;
    double getSMA50(int index) const;
    double getRSI(int index) const;
    double getMACD(int index) const;
    double getMACDSignal(int index) const;
    double getMACDHistogram(int index) const;
    double getBollingerUpper(int index) const;
    double getBollingerMiddle(int index) const;
    double getBollingerLower(int index) const;
    double getMomentum(int index) const;
    
    // Debug function
    void debugRSI(int index) const;
};

#endif