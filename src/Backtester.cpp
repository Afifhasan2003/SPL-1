#include "../include/Backtester.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

Backtester::Backtester(Stock *s, Strategy *strat, double initialCash)
{
    stock = s;
    strategy = strat;
    startingCash = initialCash;
    cash = initialCash;
    shares = 0;
    finalValue = 0.0;
    totalReturn = 0.0;
    numTrades = 0;
    winningTrades = 0;
    maxDrawdown = 0.0;
    startDay = 0;
    endDay = stock->getDataSize() - 1;
}
Backtester::Backtester(Stock *s, Strategy *strat, double initialCash, int st, int end)
{
    stock = s;
    strategy = strat;
    startingCash = initialCash;
    cash = initialCash;
    shares = 0;
    finalValue = 0.0;
    totalReturn = 0.0;
    numTrades = 0;
    winningTrades = 0;
    maxDrawdown = 0.0;
    startDay = st;
    endDay = end;
}

void Backtester::run()
{
    bool holding = false;
    double buyPrice = 0;
    double peak = startingCash;

    cout << "\nRunning backtest for: " << strategy->getName() << endl;
    cout << "Starting cash: $" << startingCash << endl;
    cout << "Stock: " << stock->getSymbol() << endl;
    cout << "Period: from day: " << startDay << " to day: " << endDay << " (" << endDay - startDay + 1 << ")days" << endl;
    cout << "Processing..." << endl;

    // cout<<" ###data size"<<dataSize<<endl;
    // now check for all day
    for (int day = startDay; day < endDay; day++)
    {
        double currPrice = stock->getClosePrice(day);

        // cout<<day<<".currPrice: "<<currPrice;

        // buy signal
        if (strategy->shouldBuy(stock, day, holding))
        {

            // cout<<"###inside buy signal"<<endl;
            int sharesToBuy = cash / currPrice;

            if (sharesToBuy)
            {
                // cout<<"### inside if statement of shareToBUy"<<endl;

                double cost = sharesToBuy * currPrice;
                cash -= cost;
                shares += sharesToBuy; // actually shares was 0
                holding = true;
                buyPrice = currPrice;

                Trade t;
                t.day = day;
                t.type = "buy";
                t.price = currPrice;
                t.shares = sharesToBuy;
                trades.push_back(t);
                numTrades++;
            }
        }

        // sell signal
        if (strategy->shouldSell(stock, day, holding))
        {
            // cout<<"###inside sell signal"<<endl;
            if (shares > 0)
            {
                double revenue = shares * currPrice;
                cash += revenue;

                if (currPrice > buyPrice)
                    winningTrades++;

                Trade t;
                t.day = day;
                t.price = currPrice;
                t.shares = shares;
                t.type = "sell";
                numTrades++;
                trades.push_back(t);
                shares = 0;
                holding = false;
            }
        }

        double portfolioValue = cash + shares * currPrice;

        if (portfolioValue > peak)
            peak = portfolioValue;

        double drawdown = ((peak - portfolioValue) / peak) * 100;

        if (maxDrawdown < drawdown)
            maxDrawdown = drawdown;
    }

    // if shares remaining
    if (shares > 0)
    {
        double lastPrice = stock->getClosePrice(endDay);
        cash += shares * lastPrice;
        shares = 0;
    }

    // final result

    finalValue = cash;
    totalReturn = ((finalValue - startingCash) / startingCash) * 100;

    cout << "back testing complete!" << endl;
}

void Backtester::displayResult()
{
    cout << "\n========================================" << endl;
    cout << "    BACKTEST RESULTS" << endl;
    cout << "========================================" << endl;
    cout << "Strategy: " << strategy->getName() << endl;
    cout << "Stock: " << stock->getSymbol() << endl;
    cout << "----------------------------------------" << endl;
    cout << fixed << setprecision(2);

    cout << "\nPerformance:" << endl;
    cout << "  Starting Capital: $" << startingCash << endl;
    cout << "  Final Value: $" << finalValue << endl;
    cout << "  Total Return: " << totalReturn << "%" << endl;
    cout << "  Max Drawdown: " << maxDrawdown << "%" << endl;

    cout << "\nTrading Activity:" << endl;
    cout << "  Total Trades: " << numTrades << endl;

    if (numTrades > 0)
    {
        int completedTrades = numTrades / 2; // buy+sell = 1 complete trade
        if (completedTrades > 0)
        {
            double winningRate = ((double)winningTrades / completedTrades) * 100;
            cout << "Winning Trades: " << winningTrades << " out of " << completedTrades << endl;
            cout << "Winning Rate: " << winningRate << "% " << endl;
        }
    }
    // Show recent trades
    if (trades.size() > 0)
    {
        cout << "\nRecent Trades (last 5):" << endl;
        int start = max(0, (int)trades.size() - 5);
        for (int i = start; i < trades.size(); i++)
        {
            cout << "  Day " << trades[i].day << ": "
                 << trades[i].type << " "
                 << trades[i].shares << " shares @ $"
                 << trades[i].price << endl;
        }
    }

    cout << "========================================" << endl;
}

double Backtester::getTotalReturn()
{
    return totalReturn;
}

double Backtester::getFinalValue()
{
    return finalValue;
}
