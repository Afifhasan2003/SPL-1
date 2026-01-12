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
    // Auto-load stocks from watchlist
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
            cout << "\n✓ " << loadedCount << " stock(s) loaded from watchlist!" << endl;
        }
    }
    else
    {
        cout << "Note: No watchlist.txt found. You can create one with stock symbols (one per line)." << endl;
    }
}


int main()
{
    map<string, Stock *> stocks;    // symbol -> Stock object
    vector<Portfolio *> portfolios; // All portfolios

    cout << "\n*** Welcome to QuantLab ***\n"
         << endl;

    loadPortfolios(portfolios);
    loadStocksFromWatchlist(stocks);
    cout<<"\n\n\n\n\n\n\n\n";

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
                    cout<<"--CREATE NEW PORTFOLIO--"<<endl;
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
                        UIHelpers::clearScreen();
                        MenuSystem::displaySelectedPortfolioMenu(currentPortfolio->getName());
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

                            if (!UIHelpers::loadStockIfNeeded(symbol, stocks))
                            {
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
                        else if (action == 4){
                            UIHelpers::clearScreen();
                            currentPortfolio->displayHoldings();
                            UIHelpers::pauseScreen();
                        }   
                        else if (action == 5){
                            UIHelpers::clearScreen();
                            currentPortfolio->displayTransactions();
                            UIHelpers::pauseScreen();
                        }
                        else if (action == 6)
                        {
                            if (stocks.empty())
                                cout << "\tno stocks loaded yeetttt" << endl;
                            
                            else{
                            UIHelpers::clearScreen();
                            currentPortfolio->displaySummary(stocks);
                            UIHelpers::pauseScreen();
                            }
                        }
                        else if (action == 7)
                        {
                            if (stocks.empty())
                                cout << "worning! no stocks loaded yet";

                            else { 
                                UIHelpers::clearScreen();
                                currentPortfolio->displayPerformanceAnalytics(stocks);
                                UIHelpers::pauseScreen();
                            }
                        }
                        else if (action == 8)
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