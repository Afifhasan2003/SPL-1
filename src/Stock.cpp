#include "../include/Stock.h" //.. for going back to the parent directory
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

string trimf(string &str)
{
    int start = str.find_first_not_of(" \t\r\n");
    int end = str.find_last_not_of(" \t\r\n");
    if (start == string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

Stock::Stock(string sym, string stcockName)
{
    symbol = sym;
    name = name;
}

bool Stock::loadFromCSV(string filename)
{
    ifstream file(filename); // creates an ifstream(input file stream) object named file and opens the csv file
    if (!file.is_open())
    { // check if file opened successfully
        cout << "could not open file: " << filename << endl;
        return false;
    }

    string oneline;
    int lineNumber = 0;

    while (getline(file, oneline))
    {
        lineNumber++;

        if (lineNumber == 1)
            continue; // first line is headings

        stringstream ss(oneline); // convert the line into stringstream, sparsing works on this
        string date, open, high, low, close, volume;

        getline(ss, date, ',');
        getline(ss, open, ',');
        getline(ss, high, ',');
        getline(ss, low, ',');
        getline(ss, close, ',');
        getline(ss, volume, ',');

        // trim extra
        dates.push_back(trimf(date));
        openPrices.push_back(stod(trimf(open))); // stod: string to double
        highPrices.push_back(stod(trimf(high)));
        lowPrices.push_back(stod(trimf(low)));
        closePrices.push_back(stod(trimf(close)));
        volumes.push_back(stoll(trimf(volume)));
    }

    file.close();

    // now calculating indicators and storing them in the vectors (defined in the header file)
    calculateAllIndicators();
    return true;
}

// some getters
string Stock::getSymbol()
{
    return symbol;
}
string Stock::getName()
{
    return name;
}
int Stock::getDataSize(){
    return dates.size();
}

void Stock::displaySummary()
{
    cout << "\n===" << symbol << " - " << name << "===" << endl;
    cout << "Total days of data: " << dates.size() << endl;
    if (dates.size() > 0)
    {
        cout << "date range: " << dates[0] << " to " << dates[dates.size() - 1] << endl;
        cout << "latest close: $" << closePrices[dates.size() - 1] << endl;
    }
}

void Stock::displayRecentData(int day)
{
    if (dates.size() == 0)
    {
        cout << "no data available" << endl;
        return;
    }

    int start = dates.size() - day;

    cout << "recent data (last " << day << "days): " << endl;
    cout << "date \t\t Open \t\t High \t\t Low \t\t Close \t\t Volume" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    for (int i = start; i < dates.size(); i++)
    {
        cout << dates[i] << "\t\t"
             << openPrices[i] << "\t\t"
             << highPrices[i] << "\t\t"
             << lowPrices[i] << "\t\t"
             << closePrices[i] << "\t\t"
             << volumes[i] << endl;
    }
}

void Stock::calculateSMA(int period){ // simple moving average
    vector<double> *vec;

    // decide which vector to work on
    if (period == 20)
        vec = &sma20;
    else if (period == 50)
        vec = &sma50;
    else
    {
        cout << "period " << period << " not supported";
        return;
    }

    vec->clear(); // clear previous data if stored

    // calculate sma for each day
    double sum = 0.0;
    for (int i = 0; i < period - 1; i++)
    {
        vec->push_back(0.0);
        sum += closePrices[i];
    }

    for (int i = period - 1; i < closePrices.size(); i++)
    {
        sum += closePrices[i];
        double sma = sum / period;
        vec->push_back(sma);
        sum -= closePrices[i - period];
    }
}

void Stock::calculateAllIndicators(){
    //this is called d
    cout << "Calculating indicators of " << name << " company ...." << endl;
    calculateSMA(20);
    calculateSMA(50);
    calculateEMA(12);
    calculateEMA(26);
    calculateMACD();
    calculateBoillingerBands(20, 2.0);
    calculateMomentum(10);
    calculateRSI(14);

    cout << " DONE!!! all indicators calculated" << endl;
}

void Stock::calculateEMA(int period){ // exponential moving average
    vector<double> *vec;
    if (period == 12)
        vec = &ema12;
    else if (period == 26)
        vec = &ema26;
    else
    {
        cout << "invalid period of ema";
        return;
    }

    vec->clear();

    double multiplier = 2.0 / (period + 1);

    double sum = 0.0;
    for (int i = 0; i < period - 1; i++)
    {
        // vec->push_back(0.0);
        vec->push_back(nan("")); // it says the value is not a number
        sum += closePrices[i];
    }
    sum += closePrices[period - 1];
    vec->push_back(sum / period);

    for (int i = period; i < closePrices.size(); i++)
    {
        double ema = (closePrices[i] * multiplier) + ((*vec)[i - 1] * (1 - multiplier));
        vec->push_back(ema);
    }
}

void Stock::calculateMACD(){
    macd.clear();
    macdSignal.clear();
    macdHistogram.clear();

    // calculate macd line = ema12-ema26
    for (int i = 0; i < closePrices.size(); i++)
    {
        if (i < 25)
        {
            macd.push_back(nan(""));
        }
        else
        {
            double val = ema12[i] - ema26[i];
            macd.push_back(val);
        }
    }

    // calculate signal line = 9-day ema of macd
    double multiplier = 2.0 / (9 + 1);

    for (int i = 0; i < macd.size(); i++)
    {
        if (i < 33)
        {
            macdSignal.push_back(nan("")); // need 26+9-1 = 34 days
        }
        else if (i == 33)
        { // first signal = sma of macd
            double sum = 0.0;
            for (int j = 0; j < 9; j++)
            {
                sum += macd[i - 8 + j];
            }
            macdSignal.push_back(sum / 9);
        }
        else
        {
            double signal = (macd[i] * multiplier) + (macdSignal[i - 1] * (1 - multiplier));
            macdSignal.push_back(signal);
        }
    }

    // calculate histogram = macd-signal  (when one line crosses another, it's a buy/sell signal)
    for (int i = 0; i < macd.size(); i++)
    {
        if (isnan(macdSignal[i])) // to avoid nan subtraction
        {
            macdHistogram.push_back(nan(""));
        }
        else
        {
            macdHistogram.push_back(macd[i] - macdSignal[i]);
        }
    }
}

void Stock::calculateBoillingerBands(int period, double n){
    bollingerLower.clear();
    bollingerMiddle.clear();
    bollingerUpper.clear();

    for (int i = 0; i < closePrices.size(); i++)
    {
        if (i < period - 1)
        {
            bollingerLower.push_back(0.0);
            bollingerMiddle.push_back(0.0);
            bollingerUpper.push_back(0.0);
        }
        else
        {
            double sum = 0;
            for (int j = i - period + 1; j <= i; j++)
                sum += closePrices[j];
            double sma = sum / period; // sma is the middleband

            double var = 0;
            for (int j = i - period + 1; j <= i; j++)
            {
                double diff = closePrices[j] - sma;
                var += diff * diff;
            }

            double stdDev = sqrt(var / period);

            bollingerLower.push_back(sma - n * stdDev);
            bollingerMiddle.push_back(sma);
            bollingerUpper.push_back(sma + n * stdDev);
        }
    }
}

void Stock::calculateMomentum(int period){
    momentum.clear();

    for (int i = 0; i < period; i++)
    {
        momentum.push_back(0.0);
    }
    for (int i = period; i < closePrices.size(); i++)
    {
        double currPrice = closePrices[i];
        double oldPrice = closePrices[i - period];
        double moment = ((currPrice - oldPrice) / oldPrice) * 100;
        momentum.push_back(moment);
    }
}

void Stock::calculateRSI(int period)
{
    rsi.clear();
    if (closePrices.size() < period + 1)
    {
        cout << "not enough data to calculate RSI";
        return;
    }

    // rs=gains/losses of period time
    vector<double> gains;
    vector<double> losses;
    for (int i = 1; i < closePrices.size(); i++)
    {
        double change = closePrices[i] - closePrices[i - 1];
        if (change > 0)
        {
            gains.push_back(change);
            losses.push_back(0.0);
        }
        else
        {
            gains.push_back(0.0);
            losses.push_back(-change);
        }
    }

    // now RSI = 100 - 100/(1+rs)

    double gainSum = 0.0, avgGain, avgLoss,rs,rsiVal;
    double lossSum = 0.0;
   
    for (int i = 0; i < period-1; i++)  //first 14days
    {
        gainSum += gains[i];
        lossSum += losses[i];

        rsi.push_back(0.0);
    }

    for(int i = period -1 ;i<gains.size(); i++){
        gainSum += gains[i];
        lossSum += losses[i];

        avgGain = gainSum/period;
        avgLoss = lossSum/period;
        if(avgLoss == 0.0)
            rsi.push_back(100);
        else{
            rs=avgGain/avgLoss;
            rsiVal=100 - (100/(1+rs));
            rsi.push_back(rsiVal);
        }

        gainSum -= gains[i-period];
        lossSum -=losses[i-period];
    }

}




double Stock::getSMA20(int index){
    if (index >= 0 && index < sma20.size())
    {
        return sma20[index];
    }
    return 0.0;
}

double Stock::getSMA50(int index){
    if (index >= 0 && index < sma50.size())
    {
        return sma50[index];
    }
    return 0.0;
}

double Stock::getMACD(int index){
    if (index >= 0 && index < macd.size())
    {
        return macd[index];
    }
    return 0.0;
}
double Stock::getMACDSignal(int index){
    if (index >= 0 && index < macdSignal.size())
    {
        return macdSignal[index];
    }
    return 0.0;
}
double Stock::getMACDHistogram(int index){
    if (index >= 0 && index < macdHistogram.size())
    {
        return macdHistogram[index];
    }
    return 0.0;
}

double Stock::getBollingerLower(int index)
{
    if (index >= 0 && index < bollingerLower.size())
    {
        return bollingerLower[index];
    }
    return 0.0;
}
double Stock::getBollingerMiddle(int index){
    if (index >= 0 && index < bollingerMiddle.size())
    {
        return bollingerMiddle[index];
    }
    return 0.0;
}
double Stock::getBollingerUpper(int index)
{
    if (index >= 0 && index < bollingerUpper.size())
    {
        return bollingerUpper[index];
    }
    return 0.0;
}

double Stock::getMomentum(int index)
{
    if (index >= 0 && index < momentum.size())
    {
        return momentum[index];
    }
    return 0.0;
}

double Stock::getRSI(int index){
    if (index >= 0 && index < rsi.size())
    {
        return rsi[index];
    }
    return 0.0;
}


