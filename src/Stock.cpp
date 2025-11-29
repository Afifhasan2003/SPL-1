// Stock.cpp
#include "../include/Stock.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

// Helper function to trim spaces
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return str.substr(start, end - start + 1);
}

// Constructor
Stock::Stock(string sym, string stockName) {
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
    
    // Calculate indicators after loading data
    calculateAllIndicators();
    
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

// Calculate Simple Moving Average
void Stock::calculateSMA(int period) {
    vector<double>* targetVector;
    
    // Decide which vector to fill
    if (period == 20) {
        targetVector = &sma20;
    } else if (period == 50) {
        targetVector = &sma50;
    } else {
        cout << "Error: SMA period " << period << " not supported" << endl;
        return;
    }
    
    targetVector->clear();
    
    // Calculate SMA for each day
    for (int i = 0; i < closePrices.size(); i++) {
        if (i < period - 1) {
            // Not enough data yet, store 0 or -1 as placeholder
            targetVector->push_back(0.0);
        } else {
            // Calculate average of last 'period' days
            double sum = 0.0;
            for (int j = i - period + 1; j <= i; j++) {
                sum += closePrices[j];
            }
            double sma = sum / period;
            targetVector->push_back(sma);
        }
    }
}

// Calculate all indicators
void Stock::calculateAllIndicators() {
    cout << "Calculating indicators for " << symbol << "..." << endl;
    
    calculateSMA(20);
    calculateSMA(50);
    calculateEMA(12);
    calculateEMA(26);
    calculateMACD();
    calculateBollingerBands(20, 2.0);
    calculateMomentum(10);
    calculateRSI(14);
    
    cout << "✓ Indicators calculated!" << endl;
}

// Get SMA values
double Stock::getSMA20(int index) const {
    if (index >= 0 && index < sma20.size()) {
        return sma20[index];
    }
    return 0.0;
}

// Calculate Exponential Moving Average
void Stock::calculateEMA(int period) {
    vector<double>* targetVector;
    
    if (period == 12) {
        targetVector = &ema12;
    } else if (period == 26) {
        targetVector = &ema26;
    } else {
        return;
    }
    
    targetVector->clear();
    
    double multiplier = 2.0 / (period + 1);
    
    for (int i = 0; i < closePrices.size(); i++) {
        if (i < period - 1) {
            targetVector->push_back(0.0);
        } else if (i == period - 1) {
            // First EMA = SMA
            double sum = 0.0;
            for (int j = 0; j < period; j++) {
                sum += closePrices[i - period + 1 + j];
            }
            targetVector->push_back(sum / period);
        } else {
            // EMA = (Close * multiplier) + (EMA_prev * (1 - multiplier))
            double ema = (closePrices[i] * multiplier) + ((*targetVector)[i-1] * (1 - multiplier));
            targetVector->push_back(ema);
        }
    }
}

// Calculate MACD
void Stock::calculateMACD() {
    macd.clear();
    macdSignal.clear();
    macdHistogram.clear();
    
    // Calculate MACD Line = EMA12 - EMA26
    for (int i = 0; i < closePrices.size(); i++) {
        if (i < 25) {  // Need 26 days for EMA26
            macd.push_back(0.0);
        } else {
            double macdValue = ema12[i] - ema26[i];
            macd.push_back(macdValue);
        }
    }
    
    // Calculate Signal Line = 9-day EMA of MACD
    double multiplier = 2.0 / (9 + 1);
    
    for (int i = 0; i < macd.size(); i++) {
        if (i < 33) {  // Need 26 + 9 - 1 = 34 days
            macdSignal.push_back(0.0);
        } else if (i == 33) {
            // First signal = SMA of MACD
            double sum = 0.0;
            for (int j = 0; j < 9; j++) {
                sum += macd[i - 8 + j];
            }
            macdSignal.push_back(sum / 9);
        } else {
            // Signal EMA
            double signal = (macd[i] * multiplier) + (macdSignal[i-1] * (1 - multiplier));
            macdSignal.push_back(signal);
        }
    }
    
    // Calculate Histogram = MACD - Signal
    for (int i = 0; i < macd.size(); i++) {
        if (macdSignal[i] == 0.0) {
            macdHistogram.push_back(0.0);
        } else {
            macdHistogram.push_back(macd[i] - macdSignal[i]);
        }
    }
}

double Stock::getMACD(int index) const {
    if (index >= 0 && index < macd.size()) {
        return macd[index];
    }
    return 0.0;
}

double Stock::getMACDSignal(int index) const {
    if (index >= 0 && index < macdSignal.size()) {
        return macdSignal[index];
    }
    return 0.0;
}

double Stock::getMACDHistogram(int index) const {
    if (index >= 0 && index < macdHistogram.size()) {
        return macdHistogram[index];
    }
    return 0.0;
}

// Calculate Bollinger Bands
void Stock::calculateBollingerBands(int period, double numStdDev) {
    bollingerUpper.clear();
    bollingerMiddle.clear();
    bollingerLower.clear();
    
    for (int i = 0; i < closePrices.size(); i++) {
        if (i < period - 1) {
            bollingerUpper.push_back(0.0);
            bollingerMiddle.push_back(0.0);
            bollingerLower.push_back(0.0);
        } else {
            // Calculate SMA (middle band)
            double sum = 0.0;
            for (int j = i - period + 1; j <= i; j++) {
                sum += closePrices[j];
            }
            double sma = sum / period;
            
            // Calculate standard deviation
            double variance = 0.0;
            for (int j = i - period + 1; j <= i; j++) {
                double diff = closePrices[j] - sma;
                variance += diff * diff;
            }
            double stdDev = sqrt(variance / period);
            
            // Calculate bands
            bollingerMiddle.push_back(sma);         
            bollingerUpper.push_back(sma + (numStdDev * stdDev));  
            bollingerLower.push_back(sma - (numStdDev * stdDev));
        }
    }
}

double Stock::getBollingerUpper(int index) const {
    if (index >= 0 && index < bollingerUpper.size()) {
        return bollingerUpper[index];
    }
    return 0.0;
}

double Stock::getBollingerMiddle(int index) const {
    if (index >= 0 && index < bollingerMiddle.size()) {
        return bollingerMiddle[index];
    }
    return 0.0;
}

double Stock::getBollingerLower(int index) const {
    if (index >= 0 && index < bollingerLower.size()) {
        return bollingerLower[index];
    }
    return 0.0;
}

// Calculate Momentum
void Stock::calculateMomentum(int period) {
    momentum.clear();
    
    for (int i = 0; i < closePrices.size(); i++) {
        if (i < period) {
            momentum.push_back(0.0);
        } else {
            double currentPrice = closePrices[i];
            double oldPrice = closePrices[i - period];
            double mom = ((currentPrice - oldPrice) / oldPrice) * 100.0;
            momentum.push_back(mom);
        }
    }
}

double Stock::getMomentum(int index) const {
    if (index >= 0 && index < momentum.size()) {
        return momentum[index];
    }
    return 0.0;
}

double Stock::getSMA50(int index) const {
    if (index >= 0 && index < sma50.size()) {
        return sma50[index];
    }
    return 0.0;
}

// Calculate RSI (Relative Strength Index)
void Stock::calculateRSI(int period) {
    rsi.clear();
    
    if (closePrices.size() < period + 1) {
        cout << "Not enough data for RSI calculation" << endl;
        return;
    }
    
    // First, calculate price changes
    vector<double> gains;
    vector<double> losses;
    
    for (int i = 1; i < closePrices.size(); i++) {
        double change = closePrices[i] - closePrices[i-1];
        
        if (change > 0) {
            gains.push_back(change);
            losses.push_back(0.0);
        } else {
            gains.push_back(0.0);
            losses.push_back(-change);  // Make positive
        }
    }
    
    // Calculate RSI for each day
    for (int i = 0; i < gains.size(); i++) {
        if (i < period - 1) {
            // Not enough data yet
            rsi.push_back(0.0);
        } else if (i == period - 1) {
            // First RSI - use simple average
            double avgGain = 0.0;
            double avgLoss = 0.0;
            
            for (int j = 0; j < period; j++) {
                avgGain += gains[j];
                avgLoss += losses[j];
            }
            
            avgGain /= period;
            avgLoss /= period;
            
            if (avgLoss == 0.0) {
                rsi.push_back(100.0);
            } else {
                double rs = avgGain / avgLoss;
                double rsiValue = 100.0 - (100.0 / (1.0 + rs));
                rsi.push_back(rsiValue);
            }
        } else {
            // Subsequent RSI - use smoothed average
            double prevAvgGain = 0.0;
            double prevAvgLoss = 0.0;
            
            // Get previous averages (simplified - using window average)
            for (int j = i - period + 1; j <= i; j++) {
                prevAvgGain += gains[j];
                prevAvgLoss += losses[j];
            }
            
            prevAvgGain /= period;
            prevAvgLoss /= period;
            
            if (prevAvgLoss == 0.0) {
                rsi.push_back(100.0);
            } else {
                double rs = prevAvgGain / prevAvgLoss;
                double rsiValue = 100.0 - (100.0 / (1.0 + rs));
                rsi.push_back(rsiValue);
            }
        }
    }
    
    // Add one more 0 at the beginning to align with closePrices
    rsi.insert(rsi.begin(), 0.0);
}

double Stock::getRSI(int index) const {
    if (index >= 0 && index < rsi.size()) {
        return rsi[index];
    }
    return 0.0;
}

// Debug RSI calculation for specific index
void Stock::debugRSI(int index) const {
    if (index < 15) {
        cout << "Need at least 15 days of data for RSI" << endl;
        return;
    }
    
    cout << "\n=== RSI Debug for Day " << index << " ===" << endl;
    cout << "Last 14 days of prices and changes:" << endl;
    cout << "Day\tClose\tChange\tGain\tLoss" << endl;
    
    double totalGain = 0.0;
    double totalLoss = 0.0;
    
    for (int i = index - 14; i <= index; i++) {
        double change = (i > 0) ? closePrices[i] - closePrices[i-1] : 0;
        double gain = (change > 0) ? change : 0;
        double loss = (change < 0) ? -change : 0;
        
        if (i > index - 14) {  // Skip first day (no change)
            totalGain += gain;
            totalLoss += loss;
        }
        
        cout << i << "\t" << closePrices[i] << "\t";
        if (i > 0) {
            cout << change << "\t" << gain << "\t" << loss;
        } else {
            cout << "N/A\tN/A\tN/A";
        }
        cout << endl;
    }
    
    double avgGain = totalGain / 14;
    double avgLoss = totalLoss / 14;
    double rs = (avgLoss > 0) ? avgGain / avgLoss : 0;
    double calculatedRSI = 100.0 - (100.0 / (1.0 + rs));
    
    cout << "\nAverage Gain: " << avgGain << endl;
    cout << "Average Loss: " << avgLoss << endl;
    cout << "RS: " << rs << endl;
    cout << "Calculated RSI: " << calculatedRSI << endl;
    cout << "Stored RSI: " << getRSI(index) << endl;
}