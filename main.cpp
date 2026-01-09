#include <iostream>
#include <vector>
#include <map>
#include "./include/Stock.h"
#include "./include/Portfolio.h"
#include "include/Analytics.h"
#include "include/Strategy.h"
#include "include/Backtester.h"
#include <iomanip>
#include <filesystem>
#include <fstream>


using namespace std;
namespace fs = std::filesystem; // fs is an alias, to avoid writing std::filesystem every time

bool loadStockIfNeeded(string symbol, map<string, Stock*>& stocks) {
    if (stocks.find(symbol) != stocks.end()) {  //already loaded
        return true;
    }
    
    string filename = "data/stocks/" + symbol + ".csv";
    
    if (!fs::exists(filename)) {
        cout << "Error: " << symbol << " not found" << endl;
        return false;
    }
    
    // Load the stock
    Stock* newStock = new Stock(symbol, symbol);
    
    if (newStock->loadFromCSV(filename)) {
        stocks[symbol] = newStock;
        cout << symbol << " loaded successfully!" << endl;
        return true;
    } else {
        cout << " Failed to load " << symbol << endl;
        delete newStock;
        return false;
    }
}

void displayMainMenu()
{
    cout << "\n======================================" << endl;
    cout << "      Finance Bazar - Main Menu" << endl;
    cout << "======================================" << endl;
    cout << "1. Manage Portfolios" << endl;
    cout << "2. Load Stock Data" << endl;
    cout << "3. View Stock Info" << endl;
    cout << "4. View Indicators" << endl;
    cout << "5. View Analytics" << endl;
    cout << "6. Backtest Strategy" << endl;
    cout << "7. Exit" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}

void displayPortfolioMenu()
{
    cout << "\n======================================" << endl;
    cout << "      Portfolio Management" << endl;
    cout << "======================================" << endl;
    cout << "1. Create new portfolio" << endl;
    cout << "2. View all portfolios" << endl;
    cout << "3. Select portfolio" << endl;
    cout << "4. Back to main menu" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}

void displaySelectedPortfolioMenu(string portfolioName)
{
    cout << "\n======================================" << endl;
    cout << "  Portfolio: " << portfolioName << endl;
    cout << "======================================" << endl;
    cout << "1. Add cash" << endl;
    cout << "2. Buy stock" << endl;
    cout << "3. Sell stock" << endl;
    cout << "4. View holdings" << endl;
    cout << "5. View transactions" << endl;
    cout << "6. View summary" << endl;
    cout << "7. Back" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}

void loadPortfolios(vector<Portfolio *> &portfolios)
{    
       cout << " Loading saved Portfolios ... ... " << endl;
        try
        {
            if (fs::exists("portfolios") && fs::is_directory("portfolios"))
            {
                for (auto &each : fs::directory_iterator("portfolios"))
                {
                    if (each.path().extension() == ".csv")
                    {
                        Portfolio *p = new Portfolio("");
                        if (p->loadFromFile(each.path().string()))
                        {
                            portfolios.push_back(p);
                            cout << " loaded- " << p->getName() << endl;
                        }
                        else
                        {
                            cout << "something went wrong with loading " << p->getName() << endl;
                            delete p;
                        }
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            cout << "No portfolios...";
        }

        if (portfolios.size() > 0)
            cout << "\n Portfolios loaded successfully" << endl;
    
}

void loadStocksFromWatchlist(map<string, Stock*>& stocks){
        // Auto-load stocks from watchlist
    cout << "\nLoading watchlist stocks..." << endl;
    ifstream watchlistFile("watchList.txt");
    
    if (watchlistFile.is_open()) {
        string symbol;
        int loadedCount = 0;
        
        while (getline(watchlistFile, symbol)) {
            // Trim whitespace
            symbol.erase(0, symbol.find_first_not_of(" \t\r\n"));
            symbol.erase(symbol.find_last_not_of(" \t\r\n") + 1);
            
            if (symbol.empty()) continue;
            
            if (loadStockIfNeeded(symbol, stocks)) {
                loadedCount++;
            }
        }
        
        watchlistFile.close();
        
        if (loadedCount > 0) {
            cout << "\n✓ " << loadedCount << " stock(s) loaded from watchlist!" << endl;
        }
    } else {
        cout << "Note: No watchlist.txt found. You can create one with stock symbols (one per line)." << endl;
    }
}
int main()
{
    map<string, Stock *> stocks;    // symbol -> Stock object
    vector<Portfolio *> portfolios; // All portfolios

    cout << "\n*** Welcome to QuantLab ***\n"<< endl;

    loadPortfolios(portfolios);
    loadStocksFromWatchlist(stocks);

    

    while (true)
    {
        displayMainMenu();
        int choice;
        cin >> choice;

        if (choice == 1) // manage portfolios
        {
            while (true)
            {
                displayPortfolioMenu();
                int portfolioChoice;
                cin >> portfolioChoice;

                if (portfolioChoice == 1) // create new portfolio
                {
                    string name;
                    cout << "\nEnter portfolio name: ";
                    cin.ignore();
                    getline(cin, name);

                    Portfolio *newPortfolio = new Portfolio(name);
                    portfolios.push_back(newPortfolio);

                    string filename = "portfolios/" + name + ".csv";

                    for (char &c : filename) // if space used in portfolio name
                        if (c == ' ')
                            c = '_';

                    newPortfolio->saveToFile(filename);
                    cout << "\t\tNew Portfolio '" << name << "' created!" << endl;
                }
                else if (portfolioChoice == 2) // view all portfolios
                {
                    if (portfolios.empty())
                        cout << "\nNo portfolios yet. Create one first!" << endl;
                    else
                    {
                        cout << "\n=== Your Portfolios ===" << endl;
                        for (int i = 0; i < portfolios.size(); i++)
                        {
                            cout << i + 1 << ". " << portfolios[i]->getName() << " (Cash: $" << portfolios[i]->getCashBalance() << ")" << endl;
                        }
                    }
                }
                else if (portfolioChoice == 3) // select portfolio
                {
                    if (portfolios.empty())
                    {
                        cout << "\nNo portfolios yet. Create one first!" << endl;
                        continue;
                    }

                    cout << "\n=== Select Portfolio ===" << endl;
                    for (int i = 0; i < portfolios.size(); i++)
                        cout << i + 1 << ". " << portfolios[i]->getName() << endl;

                    int select;
                    cout << "Enter number: ";
                    cin >> select;

                    if (select < 1 || select > portfolios.size())
                    {
                        cout << "Invalid selection." << endl;
                        continue;
                    }

                    Portfolio *currentPortfolio = portfolios[select - 1];

                    // Selected portfolio menu
                    while (true)
                    {
                        displaySelectedPortfolioMenu(currentPortfolio->getName());
                        int action;
                        cin >> action;

                        if (action == 1) // add cash
                        {
                            double amount;
                            cout << "Enter amount to add: $";
                            cin >> amount;
                            currentPortfolio->addCash(amount);

                            string filename = "portfolios/" + currentPortfolio->getName() + ".csv";
                            for(char& c:filename)
                                if(c==' ') c='_';

                            currentPortfolio->saveToFile(filename); // portfolio was already created
                        }
                        else if (action == 2) // buy stock
                        {
                            string symbol, date;
                            int quantity;
                            double price;

                            cout << "Enter stock symbol: ";
                            cin >> symbol;

                            if (!loadStockIfNeeded(symbol, stocks)) {
                                cout << "Cannot buy " << symbol << ". Stock not available." << endl;
                                continue;
                            }

                            cout << "Enter quantity: ";
                            cin >> quantity;
                            cout << "Enter price per share: $";
                            cin >> price;
                            // cout << "Enter date (YYYY-MM-DD): ";
                            // cin >> date;

                            // currentPortfolio->buyStock(symbol, quantity, price, date);
                            currentPortfolio->buyStock(symbol, quantity, price, "unknown date");

                            string filename = "portfolios/" + currentPortfolio->getName() + ".csv";
                            for(char& c:filename)
                                if(c==' ') c='_';

                            currentPortfolio->saveToFile(filename);
                        }
                        else if (action == 3) // sell stock
                        {
                            string symbol, date;
                            int quantity;
                            double price;

                            cout << "Enter stock symbol: ";
                            cin >> symbol;
                            cout << "Enter quantity: ";
                            cin >> quantity;
                            cout << "Enter price per share: $";
                            cin >> price;
                            // cout << "Enter date (YYYY-MM-DD): ";
                            // cin >> date;

                            // currentPortfolio->sellStock(symbol, quantity, price, date);
                            currentPortfolio->sellStock(symbol, quantity, price, "unknown date");

                            string filename = "portfolios/" + currentPortfolio->getName() + ".csv";
                            for(char& c:filename)
                                if(c==' ') c='_';

                            currentPortfolio->saveToFile(filename);
                        }
                        else if (action == 4)
                            currentPortfolio->displayHoldings();
                        else if (action == 5)
                            currentPortfolio->displayTransactions();
                        else if (action == 6){
                            if(stocks.empty()) 
                                cout<<"\tno stocks loaded yeetttt"<<endl;
                            currentPortfolio->displaySummary(stocks);
                        }
                        else if (action == 7)
                            break;
                        else
                            cout << "Invalid choice." << endl;
                    }
                }
                else if (portfolioChoice == 4) // back to main menu
                {
                    break;
                }
                else
                {
                    cout << "Invalid choice." << endl;
                }
            }
        }
        else if (choice == 2) // load stock data
        {
            string symbol, name, filename;

            cout << "\nEnter stock symbol: ";
            cin >> symbol;
            cout << "Enter company name: ";
            cin.ignore();
            getline(cin, name);
            // cout << "Enter CSV filename: ";
            // getline(cin, filename);
            // filename = "data/AAPL.csv";
            filename = "data/AAPL2.csv";

            Stock *newStock = new Stock(symbol, name);

            if (newStock->loadFromCSV(filename))
            {
                stocks[symbol] = newStock;
                cout << "✓ Successfully loaded " << symbol << "!" << endl;
            }
            else
            {
                cout << "✗ Failed to load stock." << endl;
                delete newStock;
            }
        }
        else if (choice == 3) // view stock info
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
        else if (choice == 4) // view indicators
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
        else if (choice == 5) // view Analytics
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

                if (stocks.find(sym) != stocks.end())
                    Analytics::displayAnalyticsReport(stocks[sym]);
                else
                    cout << "Stock not found" << endl;
            }
        }
        else if (choice == 6) // Backtesting Strategy
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

                if (stocks.find(symbol) == stocks.end())
                {
                    cout << "stock not founc" << endl;
                }
                else
                {
                    cout << "\n=== Select Strategy ===" << endl;
                    cout << "1. RSI Strategy (Buy < 30, Sell > 70)" << endl;
                    cout << "2. Moving Average Crossover" << endl;
                    cout << "3. MACD Strategy" << endl;
                    cout << "4. Momentum Strategy" << endl;
                    cout << "5. Buy and Hold" << endl;
                    cout << "6. Compare All Strategy" << endl
                         << endl;

                    cout << "Enter choice: ";
                    int stratChoice;
                    cin >> stratChoice;
                    double giveCash;
                    cout << "Enter initial Cash: $";
                    cin >> giveCash;

                    if (stratChoice != 6)
                    { // single strategy
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

                        // run backtester
                        Backtester backtester(stocks[symbol], strategy, giveCash);
                        backtester.run();
                        backtester.displayResult();

                        delete strategy;
                    }
                    else if (stratChoice == 6) // all strategy
                    {
                        Strategy *strategy[] = {
                            new RSIStrategy(),
                            new MAStrategy(),
                            new MACDStrategy(),
                            new MomentumStrategy(),
                            new BuyHoldStrategy()};

                        vector<double> returns;
                        vector<string> names;

                        for (int i = 0; i < 5; i++) // returns of each strategy
                        {
                            Backtester backtester(stocks[symbol], strategy[i], giveCash);
                            backtester.run();
                            returns.push_back(backtester.getTotalReturn());
                            names.push_back(strategy[i]->getName());
                        }

                        cout << "##Comparison of returns of" << stocks[symbol]->getName() << " company for different Strategies  with initial cash " << giveCash << " ##" << endl;

                        cout << fixed << setprecision(2);

                        int bestInd = 0;
                        double bestReturn = returns[0];
                        for (int i = 0; i < returns.size(); i++)
                        {
                            if (returns[i] > bestReturn)
                            {
                                bestReturn = returns[i];
                                bestInd = i;
                            }
                        }

                        for (int i = 0; i < returns.size(); i++)
                        {
                            cout << i + 1 << ". " << names[i] << endl;
                            cout << "\tReturns: " << returns[i] << "%";
                            if (i == bestInd)
                                cout << " $best";
                            cout << endl;
                            cout << "\tFinal Value: " << giveCash + giveCash * (returns[i] / 100) << endl;
                        }

                        cout << "===========================================" << endl;
                        cout << "best strategy: " << names[bestInd] << endl;
                        cout << "best Return: " << bestReturn << "%" << endl;
                    }
                    else
                    {
                        cout << "invalid choise.." << endl;
                        continue;
                    }
                }
            }
        }
        else if (choice == 7) // Exit
        {
            cout << "\nThank you for using Finance Bazar!" << endl;
            break;
        }
        else
            cout << "Invalid choice." << endl;
    }

    return 0;
}