#include <iostream>
#include <vector>
#include <map>
#include "./include/Stock.h"
#include "./include/Portfolio.h"
#include "include/Analytics.h"
#include "include/Strategy.h"
#include "include/Backtester.h"
#include "include/MenuSystem.h"
#include "include/UIHelpers.h"
#include "include/StockManager.h"
#include <iomanip>
#include <filesystem>
#include <fstream>
#include "include/DataFetcher.h"
#include <thread>
#include <chrono>
#include "include/Regression.h"

using namespace std;
namespace fs = std::filesystem; // fs is an alias, to avoid writing std::filesystem every time

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

void loadStocksFromWatchlist(map<string, Stock *> &stocks)
{
    cout << "\nLoading watchlist stocks..." << endl;
    ifstream watchlistFile("watchList.txt");

    if (watchlistFile.is_open())
    {
        string symbol;
        int loadedCount = 0;

        while (getline(watchlistFile, symbol))
        {
            // Trim whitespace
            symbol.erase(0, symbol.find_first_not_of(" \t\r\n"));
            symbol.erase(symbol.find_last_not_of(" \t\r\n") + 1);

            if (symbol.empty())
                continue;

            if (UIHelpers::loadStockIfNeeded(symbol, stocks))
            {
                loadedCount++;
            }
        }

        watchlistFile.close();

        if (loadedCount > 0)
        {
            cout << "\nSS " << loadedCount << " stock(s) loaded from watchlist!" << endl;
        }
    }
    else
    {
        cout << "Note: No watchlist.txt found. You can create one with stock symbols (one per line)." << endl;
    }
}

void updateStocksFromWatchlist(map<string, Stock *> &stocks)
{
    // Auto-update watchlist stock data
    cout << "\nChecking for outdated stock data..." << endl;
    DataFetcher::updateWatchlist("watchList.txt");
    // Reload any stocks that were updated
    for (auto &pair : stocks)
    {
        string filename = "data/" + pair.first + ".csv";
        if (fs::exists(filename))
        {
            pair.second->loadFromCSV(filename);
        }
    }
}

void getPrediction(const string &sym,
                   map<string, Stock *> &stocks,
                   map<string, Regression *> &regressions,
                   double &pctOut, string &sigOut)
{
    pctOut = 0.0;
    sigOut = "N/A";

    // Stock not loaded
    if (stocks.find(sym) == stocks.end())
        return;

    Stock *s = stocks[sym];

    // Create regression model if it doesn't exist
    if (regressions.find(sym) == regressions.end())
        regressions[sym] = new Regression();

    Regression *reg = regressions[sym];

    // Train if not already trained
    if (!reg->trained())
        reg->train(s, 0, s->getDataSize() - 1);

    // Predict
    int lastDay = s->getDataSize() - 1;
    double lastClose = s->getClosePrice(lastDay);
    double predicted = reg->predict(s, lastDay);

    if (predicted < 0 || lastClose == 0)
        return;

    pctOut = ((predicted - lastClose) / lastClose) * 100.0;
    sigOut = reg->getSignal(s, lastDay);
}

int main()
{
    map<string, Stock *> stocks;           // symbol -> Stock object
    vector<Portfolio *> portfolios;        // All portfolios
    map<string, Regression *> regressions; // symbol -> trained regression models

    cout << "\n*** Welcome to QuantLab ***\n"
         << endl;

    loadPortfolios(portfolios);
    loadStocksFromWatchlist(stocks);
    updateStocksFromWatchlist(stocks);

    cout << "\n\n\n\n\n\n\n\n";

    while (true)
    {
        MenuSystem::displayMainMenu();
        int choice;
        cin >> choice;

        if (choice == 1) // manage portfolios
        {
            while (true)
            {
                MenuSystem::displayPortfolioMenu();
                int portfolioChoice;
                cin >> portfolioChoice;

                if (portfolioChoice == 1) // create new portfolio
                {
                    UIHelpers::clearScreen();
                    cout << "--CREATE NEW PORTFOLIO--" << endl;
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
                    {
                        cout << "\nNo portfolios yet. Create one first!" << endl;
                        UIHelpers::pauseScreen();
                    }
                    else
                    {
                        UIHelpers::clearScreen();
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
                        UIHelpers::pauseScreen();
                        continue;
                    }

                    UIHelpers::clearScreen();
                    cout << "\n=== Select Portfolio ===" << endl;
                    for (int i = 0; i < portfolios.size(); i++)
                        cout << i + 1 << ". " << portfolios[i]->getName() << " (Cash: $" << portfolios[i]->getCashBalance() << ")" << endl;

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
                        UIHelpers::clearScreen();
                        MenuSystem::displaySelectedPortfolioMenu(currentPortfolio->getName(), currentPortfolio->getCashBalance());
                        int action;
                        cin >> action;

                        if (action == 1) // add cash
                        {
                            double amount;
                            cout << "Enter amount to add: $";
                            cin >> amount;
                            currentPortfolio->addCash(amount);

                            string filename = "portfolios/" + currentPortfolio->getName() + ".csv";
                            for (char &c : filename)
                                if (c == ' ')
                                    c = '_';

                            currentPortfolio->saveToFile(filename); // portfolio was already created
                        }
                        else if (action == 2) // buy stock
                        {
                            string symbol, date;
                            int quantity;
                            double price;

                            cout << "Enter stock symbol: ";
                            cin >> symbol;
                            for(char &c : symbol)
                                c = toupper(c);

                            if (!UIHelpers::loadStockIfNeeded(symbol, stocks))
                            {
                                cout << "Cannot buy " << symbol << ". Stock not available." << endl;
                                continue;
                            }

                            cout << "Enter quantity: ";
                            cin >> quantity;

                            cout << "Enter price per share: $";
                            cin >> price;

                            // price = getcloseprice(symbol, stocks);
                            // cout << "Enter date (YYYY-MM-DD): ";
                            // cin >> date;

                            // currentPortfolio->buyStock(symbol, quantity, price, date);
                            currentPortfolio->buyStock(symbol, quantity, price, "unknown date");

                            string filename = "portfolios/" + currentPortfolio->getName() + ".csv";
                            for (char &c : filename)
                                if (c == ' ')
                                    c = '_';

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
                            for (char &c : filename)
                                if (c == ' ')
                                    c = '_';

                            currentPortfolio->saveToFile(filename);
                        }
                        else if (action == 4)
                        { // displayHoldings
                            UIHelpers::clearScreen();
                            currentPortfolio->displayHoldings();
                            UIHelpers::pauseScreen();
                        }
                        else if (action == 5)
                        { // display transactions
                            UIHelpers::clearScreen();
                            currentPortfolio->displayTransactions();
                            UIHelpers::pauseScreen();
                        }
                        else if (action == 6) // display summary
                        {
                            if (stocks.empty())
                                cout << "\tno stocks loaded yeetttt" << endl;

                            else
                            {
                                UIHelpers::clearScreen();
                                currentPortfolio->displaySummary(stocks);
                                UIHelpers::pauseScreen();
                            }
                        }
                        else if (action == 7) // display performance analytics
                        {
                            if (stocks.empty())
                                cout << "worning! no stocks loaded yet";

                            else
                            {
                                UIHelpers::clearScreen();
                                currentPortfolio->displayPerformanceAnalytics(stocks);
                                UIHelpers::pauseScreen();
                            }
                        }
                        else if (action == 8) {
                            // Today's best performer prediction
                            cout << "\n=== Today's Performance Prediction ===" << endl;
                            cout << "Training models... (this may take a moment)" << endl;

                            // ── Read watchlist ────────────────────────────────
                            vector<string> watchlist;
                            ifstream wf("watchlist.txt");
                            if (wf.is_open()) {
                                string wline;
                                while (getline(wf, wline)) {
                                    while (!wline.empty() && (wline.back() == '\r' || wline.back() == '\n' || wline.back() == ' '))
                                        wline.pop_back();
                                    if (!wline.empty()) {
                                        for (char& c : wline) c = toupper(c);
                                        watchlist.push_back(wline);
                                    }
                                }
                                wf.close();
                            }

                            cout << fixed << setprecision(2);

                            // ── Holdings: loop through portfolio holdings ──────
                            cout << "\n Your Holdings:" << endl;
                            cout << "----------------------------------------" << endl;
                            string bestHoldingSym = "";
                            double bestHoldingPct = -1e9;

                            for (const auto& stockPair : stocks) {
                                string sym = stockPair.first;
                                if (!currentPortfolio->hasStock(sym)) continue;

                                // Load regression model if needed
                                if (regressions.find(sym) == regressions.end())
                                    regressions[sym] = new Regression();
                                Regression* reg = regressions[sym];
                                Stock* s = stockPair.second; 

                                // Train if not trained
                                if (!reg->trained())
                                    reg->train(s, 0, s->getDataSize() - 1);

                                // Predict
                                int lastDay = s->getDataSize() - 1;
                                double lastClose = s->getClosePrice(lastDay);
                                double predicted = reg->predict(s, lastDay);
                                if (predicted < 0 || lastClose == 0) continue;

                                double pct = ((predicted - lastClose) / lastClose) * 100.0;
                                string sig = reg->getSignal(s, lastDay);

                                cout << "  " << left << setw(8) << sym
                                     << " → " << (pct >= 0 ? "+" : "") << pct << "%"
                                     << "   " << sig << endl;

                                if (pct > bestHoldingPct) {
                                    bestHoldingPct = pct;
                                    bestHoldingSym = sym;
                                }
                            }
                            if (bestHoldingSym.empty())
                                cout << "  (no loaded stocks in portfolio)" << endl;



                            // ── Watchlist: only stocks not in portfolio ────────
                            cout << "\n Watchlist (not owned):" << endl;
                            cout << "----------------------------------------" << endl;
                            string bestWatchSym = "";
                            double bestWatchPct = -1e9;

                            for (const string& sym : watchlist) {
                                if (currentPortfolio->hasStock(sym)) continue;
                                if (stocks.find(sym) == stocks.end()) continue;

                                // Load regression model if needed
                                if (regressions.find(sym) == regressions.end())
                                    regressions[sym] = new Regression();
                                Regression* reg = regressions[sym];
                                Stock* s = stocks[sym];

                                // Train if not trained
                                if (!reg->trained())
                                    reg->train(s, 0, s->getDataSize() - 1);

                                // Predict
                                int lastDay = s->getDataSize() - 1;
                                double lastClose = s->getClosePrice(lastDay);
                                double predicted = reg->predict(s, lastDay);
                                if (predicted < 0 || lastClose == 0) continue;

                                double pct = ((predicted - lastClose) / lastClose) * 100.0;
                                string sig = reg->getSignal(s, lastDay);
                                string action_label = (sig == "BUY") ? "Consider buying" : (sig == "SELL" ? "Skip" : "Watch");

                                cout << "  " << left << setw(8) << sym
                                     << " → " << (pct >= 0 ? "+" : "") << pct << "%"
                                     << "   " << action_label << endl;

                                if (pct > bestWatchPct) {
                                    bestWatchPct = pct;
                                    bestWatchSym = sym;
                                }
                            }
                            if (bestWatchSym.empty())
                                cout << "  (no unowned watchlist stocks loaded)" << endl;

                            // ── Summary ───────────────────────────────────────
                            cout << "\n----------------------------------------" << endl;
                            if (!bestHoldingSym.empty())
                                cout << "Best holding today:   " << bestHoldingSym
                                     << " (" << (bestHoldingPct >= 0 ? "+" : "-") << bestHoldingPct << "%)" << endl;
                                
                            if (!bestWatchSym.empty())
                                cout << "Best watchlist pick:  " << bestWatchSym
                                     << " (" << (bestWatchPct >= 0 ? "+" : "-") << bestWatchPct << "%)" << endl;
                            cout << "========================================" << endl;

                            UIHelpers::pauseScreen();

                        }
                        else if (action == 9) // exit to portfolio selection menu
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
            UIHelpers::clearScreen();
            StockManager::loadStockData(stocks);
        }

        else if (choice == 3) // view stock info
        {
            UIHelpers::clearScreen();
            StockManager::viewStockInfo(stocks);
        }

        else if (choice == 4) // view indicators
        {
            UIHelpers::clearScreen();
            StockManager::viewIndicators(stocks);
        }
        else if (choice == 5) // view Analytics
        {
            UIHelpers::clearScreen();
            StockManager::viewAnalytics(stocks);
        }
        else if (choice == 6) // Backtesting Strategy
        {
            UIHelpers::clearScreen();
            StockManager::backtestStrategy(stocks);
        }

        else if (choice == 7) {
            // ===== REGRESSION MODEL =====
            if (stocks.empty()) {
                cout << "\nNo stocks loaded yet. Load a stock first." << endl;
            } else {
                cout << "\n=== Loaded Stocks ===" << endl;
                for (const auto& pair : stocks) {
                    cout << "- " << pair.first << endl;
                }

                string symbol;
                cout << "Enter symbol: ";
                cin >> symbol;

                
                for (char& c : symbol) c = toupper(c);

                if (stocks.find(symbol) == stocks.end()) {
                    cout << "Stock not found." << endl;
                    continue;
                }

                // Create regression model for this symbol if not exists
                if (regressions.find(symbol) == regressions.end()) {
                    regressions[symbol] = new Regression();     //regression is a map (symbol -> regression model) 
                }

                Regression* reg = regressions[symbol];
                Stock* stock = stocks[symbol];

                while (true) {
                    UIHelpers::clearScreen();
                    MenuSystem::displayRegressionMenu(symbol);
                    int regChoice;
                    cin >> regChoice;

                    if (regChoice == 1) {
                        // ── Train model ───────────────────────────────────────
                        cout << "\n=== Train Regression Model ===" << endl;
                        auto range = UIHelpers::getDateRange(stock);

                        cout << "\nTraining model on " << symbol << "..." << endl;
                        reg->train(stock, range.first, range.second);

                        UIHelpers::pauseScreen();

                    } else if (regChoice == 2) {
                        // ── Predict next day ──────────────────────────────────
                        if (!reg->trained()) {
                            cout << "\n✗ Model not trained yet. Please train first (option 1)." << endl;
                            UIHelpers::pauseScreen();
                            continue;
                        }

                        int dataSize = stock->getDataSize();
                        int lastDay  = dataSize - 1;
                        double lastClose = stock->getClosePrice(lastDay);

                        // Predict using the last available day's indicators
                        double predicted = reg->predict(stock, lastDay);
                        string signal    = reg->getSignal(stock, lastDay);

                        cout << "\n=== Next-Day Prediction: " << symbol << " ===" << endl;
                        cout << fixed << setprecision(2);
                        cout << "Today's Close:    $" << lastClose << endl;

                        if (predicted < 0) {
                            cout << " Not enough indicator data to predict." << endl;
                        } else {
                            double pctChange = ((predicted - lastClose) / lastClose) * 100.0;
                            cout << "Predicted Next:   $" << predicted << endl;
                            cout << "Expected Change:  ";
                            if (pctChange >= 0) cout << "+";
                            cout << pctChange << "%" << endl;
                            cout << "Signal:           " << signal << endl;
                        }

                        UIHelpers::pauseScreen();

                    } else if (regChoice == 3) {
                        // ── View report ───────────────────────────────────────
                        reg->displayReport(stock);
                        UIHelpers::pauseScreen();

                    } else if (regChoice == 4) {
                        // Back
                        break;

                    } else {
                        cout << "Invalid choice." << endl;
                    }
                }
            }

        }
        else if (choice == 8)
        {
            // ===== UPDATE STOCK DATA =====
            cout << "\n=== Update Stock Data ===" << endl;
            cout << "1. Update watchlist stocks" << endl;
            cout << "2. Update specific stock by symbol" << endl;
            cout << "Enter choice: ";

            int updateChoice;
            cin >> updateChoice;

            if (updateChoice == 1)
            {
                // Update all watchlist stocks
                DataFetcher::updateWatchlist("watchlist.txt");
                // Reload any already-loaded stocks with fresh data
                for (auto &pair : stocks)
                {
                    string filename = "data/" + pair.first + ".csv";
                    if (fs::exists(filename))
                    {
                        pair.second->loadFromCSV(filename);
                    }
                }
                UIHelpers::pauseScreen();
            }
            else if (updateChoice == 2)
            {
                // Update specific stock by symbol
                string symbol;
                cout << "\nEnter stock symbol: ";
                cin >> symbol;

                for (char &c : symbol)
                    c = toupper(c);

                if (DataFetcher::updateStock(symbol))
                {
                    // If already loaded in memory, reload with fresh data
                    if (stocks.find(symbol) != stocks.end())
                    {
                        stocks[symbol]->loadFromCSV("data/" + symbol + ".csv");
                        cout << "✓ " << symbol << " reloaded with fresh data!" << endl;
                    }
                    else
                    {
                        // Not loaded yet — offer to load it
                        char load;
                        cout << "Load " << symbol << " into memory? (y/n): ";
                        cin >> load;
                        if (load == 'y' || load == 'Y')
                        {
                            UIHelpers::loadStockIfNeeded(symbol, stocks);
                        }
                    }
                }
                UIHelpers::pauseScreen();
            }
            else
            {
                cout << "Invalid choice." << endl;
            }
        }
        else if (choice == 9) // Exit
        {
            cout << "\nThank you for using OUR PRODUCT!" << endl;
            break;
        }
        else
            cout << "Invalid choice." << endl;
    }

    return 0;
}