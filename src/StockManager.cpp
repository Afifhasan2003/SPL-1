#include "../include/StockManager.h"
#include "../include/UIHelpers.h"
#include "../include/Analytics.h"
#include "../include/Strategy.h"
#include "../include/Backtester.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

void StockManager::loadStockData(map<string, Stock *> &stocks)
{
    string symbol, name, filename;
    cout << "--LOAD STOCk--" << endl
         << endl
         << endl;

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

void StockManager::viewStockInfo(map<string, Stock *> &stocks)
{
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
        for (char &c : symbol)
            c = toupper(c);

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

void StockManager::viewIndicators(map<string, Stock *> &stocks)

{
    // here stocks is the loaded stock from csv files, not my portfolio stocks
    if (stocks.empty())
    {
        cout << "\nNo stocks loaded yet." << endl;
    }
    else
    {
        cout << "\n=== Loaded Stocks ===" << endl;
        for (const auto &pair : stocks)
        {
            cout << "- " << pair.first << endl;
        }

        string symbol;
        cout << "Enter symbol: ";
        cin >> symbol;
        for (char &c : symbol)
            c = toupper(c);

        if (stocks.find(symbol) == stocks.end())
            cout << "Stock not found." << endl;
        else
        {
            Stock *stock = stocks[symbol];
            int dataSize = stock->getDataSize();

            cout << "\n=== Technical Indicators for " << symbol << " ===" << endl;

            // Submenu for indicators
            {
                cout << "\n1. View Moving Averages (SMA)" << endl;
                cout << "2. View RSI" << endl;
                cout << "3. View MACD" << endl;
                cout << "4. View Bollinger Bands" << endl;
                cout << "5. View Momentum" << endl;
                cout << "6. View All Indicators Summary" << endl;
                cout << "Enter choice: ";
            }

            int indicatorChoice;
            cin >> indicatorChoice;

            int numDays;
            cout << "How many recent days to display? ";
            cin >> numDays;

            int start = max(0, dataSize - numDays);

            if (indicatorChoice == 1) // view SMA - simple moving average
            {
                cout << "\n=== Moving Averages ===" << endl;
                cout << "Day\tSMA-20\t\tSMA-50" << endl;
                cout << "------------------------------------" << endl;
                for (int i = start; i < dataSize; i++)
                {
                    cout << i << "\t";
                    double sma20 = stock->getSMA20(i);
                    double sma50 = stock->getSMA50(i);

                    if (sma20 > 0)
                        cout << "$" << sma20;
                    else
                        cout << "N/A";
                    cout << "\t\t";

                    if (sma50 > 0)
                        cout << "$" << sma50;
                    else
                        cout << "N/A";
                    cout << endl;
                }
            }
            else if (indicatorChoice == 2) // view RSI - relative strength index
            {
                cout << "\n=== RSI (Relative Strength Index) ===" << endl;
                cout << "Day\tRSI\t\tSignal" << endl;
                cout << "------------------------------------" << endl;
                for (int i = start; i < dataSize; i++)
                {
                    cout << i << "\t";
                    double rsi = stock->getRSI(i);

                    if (rsi > 0)
                    {
                        cout << rsi << "\t\t";
                        if (rsi > 70)
                            cout << "Overbought";
                        else if (rsi < 30)
                            cout << "Oversold";
                        else
                            cout << "Neutral";
                    }
                    else
                    {
                        cout << "N/A\t\tN/A";
                    }
                    cout << endl;
                }
            }
            else if (indicatorChoice == 3) // MACD - moving average convergence divergence
            {
                cout << "\n=== MACD ===" << endl;
                cout << "Day\tMACD\t\tSignal\t\tHistogram\tTrend" << endl;
                cout << "------------------------------------------------------------" << endl;
                for (int i = start; i < dataSize; i++)
                {
                    cout << i << "\t";
                    double macd = stock->getMACD(i);
                    double signal = stock->getMACDSignal(i);
                    double hist = stock->getMACDHistogram(i);

                    if (macd != 0)
                    {
                        cout << macd << "\t" << signal << "\t" << hist << "\t\t";
                        if (hist > 0)
                            cout << "Bullish";
                        else if (hist < 0)
                            cout << "Bearish";
                    }
                    else
                    {
                        cout << "N/A\t\tN/A\t\tN/A\t\tN/A";
                    }
                    cout << endl;
                }
            }
            else if (indicatorChoice == 4) // Bollinger Bands
            {
                cout << "\n=== Bollinger Bands ===" << endl;
                cout << "Day\tUpper\t\tMiddle\t\tLower\t\tPosition" << endl;
                cout << "------------------------------------------------------------" << endl;
                for (int i = start; i < dataSize; i++)
                {
                    cout << i << "\t";
                    double upper = stock->getBollingerUpper(i);
                    double middle = stock->getBollingerMiddle(i);
                    double lower = stock->getBollingerLower(i);

                    if (upper > 0)
                    {
                        cout << "$" << upper << "\t$" << middle << "\t$" << lower << "\t";
                        // Could add price position relative to bands
                        cout << "---";
                    }
                    else
                    {
                        cout << "N/A\t\tN/A\t\tN/A\t\tN/A";
                    }
                    cout << endl;
                }
            }
            else if (indicatorChoice == 5) // Momentum - difference between  close price n days ago
            {
                cout << "\n=== Momentum (10-day) ===" << endl;
                cout << "Day\tMomentum %\tTrend" << endl;
                cout << "------------------------------------" << endl;
                for (int i = start; i < dataSize; i++)
                {
                    cout << i << "\t";
                    double mom = stock->getMomentum(i);

                    if (mom != 0)
                    {
                        cout << mom << "%\t\t";
                        if (mom > 5)
                            cout << "Strong Up";
                        else if (mom > 0)
                            cout << "Up";
                        else if (mom > -5)
                            cout << "Down";
                        else
                            cout << "Strong Down";
                    }
                    else
                    {
                        cout << "N/A\t\tN/A";
                    }
                    cout << endl;
                }
            }
            else if (indicatorChoice == 6) // All indicators summary
            {
                cout << "\n=== All Indicators Summary ===" << endl;
                cout << "Day\tSMA20\tRSI\tMACD\tMomentum" << endl;
                cout << "----------------------------------------------------" << endl;
                for (int i = start; i < dataSize; i++)
                {
                    cout << i << "\t";

                    double sma20 = stock->getSMA20(i);
                    if (sma20 > 0)
                        cout << "$" << (int)sma20;
                    else
                        cout << "N/A";
                    cout << "\t";

                    double rsi = stock->getRSI(i);
                    if (rsi > 0)
                        cout << (int)rsi;
                    else
                        cout << "N/A";
                    cout << "\t";

                    double macd = stock->getMACD(i);
                    if (macd != 0)
                        cout << (int)macd;
                    else
                        cout << "N/A";
                    cout << "\t";

                    double mom = stock->getMomentum(i);
                    if (mom != 0)
                        cout << (int)mom << "%";
                    else
                        cout << "N/A";

                    cout << endl;
                }
            }
        }
    }
}

void StockManager::viewAnalytics(map<string, Stock *> &stocks)
{
    if (stocks.empty())
    {
        cout << "\nNo Stocks loaded yet." << endl;
    }
    else
    {
        cout << "\n===Loaded Stocks===" << endl;
        for (auto &pair : stocks)
            cout << "-" << pair.first << endl;

        string sym;
        cout << "Enter symbol: ";
        cin >> sym;
        for (char &c : sym)
            c = toupper(c);

        if (stocks.find(sym) != stocks.end())
        {
            pair dateRAnge = UIHelpers::getDateRange(stocks[sym]);
            int st = dateRAnge.first, end = dateRAnge.second;
            Analytics::displayAnalyticsReport(stocks[sym], st, end);
        }
        else
            cout << "Stock not found" << endl;
    }
}

void StockManager::backtestStrategy(map<string, Stock *> &stocks)
{
    if (stocks.empty())
    {
        cout << "\nNo Stocks loaded yet" << endl;
    }
    else
    {
        cout << "\n===Loaded Stocks===" << endl;
        for (auto &pair : stocks)
        {
            cout << "- " << pair.first << endl;
        }

        string symbol;
        cout << "Enter symbol: ";
        cin >> symbol;
        for (char &c : symbol)
            c = toupper(c);

        if (stocks.find(symbol) == stocks.end())
        {
            cout << "stock not found" << endl;
        }
        else
        {
            int stratChoice;

            bool isValidStratChoice = true;
            while (isValidStratChoice)
            {
                cout << "\n=== Select Strategy ===" << endl;
                cout << "1. RSI Strategy (Buy < 30, Sell > 70)" << endl;
                cout << "2. Moving Average Crossover" << endl;
                cout << "3. MACD Strategy" << endl;
                cout << "4. Momentum Strategy" << endl;
                cout << "5. Buy and Hold" << endl;
                cout << "6. Regression Strategy" << endl;
                cout << "7. Compare All Strategy" << endl;
                cout << "8. Back" << endl
                     << endl;

                cout << "Enter choice: ";
                cin >> stratChoice;
                if (stratChoice > 0 && stratChoice <= 8)
                    isValidStratChoice = false;

                else
                    cout << "!!invalid choice" << endl;
            }

            double giveCash;
            cout << "Enter initial Cash: $";
            cin >> giveCash;

            pair<int, int> range = UIHelpers::getDateRange(stocks[symbol]);

            if (stratChoice != 7)// single strategy
            { 
                Strategy *strategy = nullptr;
                if (stratChoice == 1)
                {
                    strategy = new RSIStrategy();
                }
                else if (stratChoice == 2)
                {
                    strategy = new MAStrategy();
                }
                else if (stratChoice == 3)
                {
                    strategy = new MACDStrategy();
                }
                else if (stratChoice == 4)
                {
                    strategy = new MomentumStrategy();
                }
                else if (stratChoice == 5)
                {
                    strategy = new BuyHoldStrategy();
                }
                else if (stratChoice == 6)
                {
                    // ── Regression Strategy single backtest ───────────────
                    RegressionStrategy *regStrat = new RegressionStrategy();
                    regStrat->trainModel(stocks[symbol], range.first, range.second);

                    if (!regStrat->trained())
                    {
                        cout << " Regression model failed to train. Try a larger date range." << endl;
                        delete regStrat;
                    }
                    else
                    {
                        Backtester backtester(stocks[symbol], regStrat, giveCash, range.first, range.second);
                        backtester.run();
                        backtester.displayResult();

                        UIHelpers::pauseScreen();
                        delete regStrat;
                    }
                }
                else if (stratChoice == 8)
                {
                    return;
                }

                // run backtester for strategies 1-5
                if (stratChoice >= 1 && stratChoice <= 5)
                {
                    Backtester backtester(stocks[symbol], strategy, giveCash, range.first, range.second);
                    backtester.run();
                    backtester.displayResult();

                    UIHelpers::pauseScreen();
                    delete strategy;
                }
            }
            else if (stratChoice == 7) // all strategy
            {
                cout << "\n=== COMPARING ALL STRATEGIES ===" << endl;

                // Standard strategies
                RSIStrategy *s1 = new RSIStrategy();
                MAStrategy *s2 = new MAStrategy();
                MACDStrategy *s3 = new MACDStrategy();
                MomentumStrategy *s4 = new MomentumStrategy();
                BuyHoldStrategy *s5 = new BuyHoldStrategy();
                RegressionStrategy *s6 = new RegressionStrategy();

                // Train regression on this range before backtesting
                s6->trainModel(stocks[symbol], range.first, range.second);

                Strategy *strategies[] = {s1, s2, s3, s4, s5, s6};
                int numStrategies = 6;

                vector<double> returns;
                vector<string> names;

                for (int i = 0; i < numStrategies; i++)
                {
                    Backtester backtester(stocks[symbol], strategies[i], giveCash, range.first, range.second);
                    backtester.run();
                    returns.push_back(backtester.getTotalReturn());
                    names.push_back(strategies[i]->getName());
                }

                // Display comparison table
                cout << "\n========================================" << endl;
                cout << "    STRATEGY COMPARISON" << endl;
                cout << "========================================" << endl;
                cout << "Stock: " << symbol << endl;
                cout << "Starting Capital: $" << giveCash << endl;
                cout << "----------------------------------------" << endl;
                cout << fixed << setprecision(2);

                // Find best strategy
                int bestIdx = 0;
                double bestReturn = returns[0];
                for (int i = 1; i < (int)returns.size(); i++)
                {
                    if (returns[i] > bestReturn)
                    {
                        bestReturn = returns[i];
                        bestIdx = i;
                    }
                }

                for (int i = 0; i < (int)names.size(); i++)
                {
                    cout << "\n"
                         << i + 1 << ". " << names[i] << endl; // names = {"RSI Strategy", "MA Crossover", "MACD Strategy", "Momentum Strategy", "Buy & Hold", "Regression Strategy"};
                    cout << "   Return: " << returns[i] << "%";
                    if (i == bestIdx)
                    {
                        cout << " ⭐ BEST";
                    }
                    cout << endl;
                    cout << "   Final Value: $" << (giveCash * (1 + returns[i] / 100.0)) << endl;
                }

                cout << "\n========================================" << endl;
                cout << "Best Strategy: " << names[bestIdx] << endl;
                cout << "Best Return: " << bestReturn << "%" << endl;
                cout << "========================================" << endl;

                UIHelpers::pauseScreen();

                // Clean up
                for (int i = 0; i < numStrategies; i++)
                {
                    delete strategies[i];
                }
            }
        }
    }
}