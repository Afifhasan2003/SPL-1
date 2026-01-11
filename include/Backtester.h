#ifndef BACKTESTER_H
#define BACKTESTER_H

#include "Stock.h"
#include "Strategy.h"
#include <vector>
#include <string>

using namespace std;

struct Trade
{
    int day;
    string type; // buy,sell
    double price;
    int shares;
};

class Backtester
{
private:
    Stock *stock;
    Strategy *strategy;
    double startingCash;
    int startDay;
    int endDay;

    // results
    double cash;
    int shares;
    vector<Trade> trades;
    double finalValue;
    double totalReturn;
    int numTrades;
    int winningTrades;
    double maxDrawdown;

public:
    Backtester(Stock *s, Strategy *strat, double initialCash = 10000.0);
    Backtester(Stock *s, Strategy *strat, double initialCash, int start, int end);

    void run(); // run the backTest

    void displayResult();
    double getTotalReturn();
    double getFinalValue();
};

#endif