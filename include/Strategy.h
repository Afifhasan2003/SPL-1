#ifndef STRATEGY_H
#define STRATEGY_H

#include "Stock.h"
#include "Regression.h"
#include <string>

using namespace std;

class Strategy
{
protected:
    string name;

public:
    Strategy(string name);
    // virtual ~Strategy(){}  //virtual destructor, to ensure proper cleanup of derived classes

    // check if should buy on this day
    virtual bool shouldBuy(Stock *stock, int day, bool currentlyHolding) = 0; // vertual makes the function overridable, =0 makes it pure virtual, means derived classes must implement it, so bacially the class has become abstract class

    // if should sell tody
    virtual bool shouldSell(Stock *stock, int day, bool currentlyHolding) = 0;

    string getName();
};

// rsi strategy, buy when <30 ,sell when >70
class RSIStrategy : public Strategy
{
public:
    RSIStrategy();
    bool shouldBuy(Stock *stock, int day, bool currentlyHolding) override;
    bool shouldSell(Stock *stock, int day, bool currentlyHolding) override;
};

// moving average crossover: buy when sma20>sma50 ,sell otherwise
class MAStrategy : public Strategy
{
private:
    bool previousCrossAbove;

public:
    MAStrategy();
    bool shouldBuy(Stock *stock, int day, bool currentlyHolding) override;
    bool shouldSell(Stock *stock, int day, bool currentlyHolding) override;
};

class BuyHoldStrategy : public Strategy
{
public:
    BuyHoldStrategy();
    bool shouldBuy(Stock *stock, int day, bool currentlyHolding) override;
    bool shouldSell(Stock *stock, int day, bool currentlyHolding) override;
};

// MACD strategy: buy when MACD crosses above signal,sell when crosses below
class MACDStrategy : public Strategy
{
public:
    MACDStrategy();
    bool shouldBuy(Stock *stock, int day, bool currentHolding) override;
    bool shouldSell(Stock *stock, int day, bool currentHolding) override;
};

// momentum Strategy : Buy when momentum > 5% , sell when < -5%
class MomentumStrategy : public Strategy
{
public:
    MomentumStrategy();
    bool shouldBuy(Stock *stock, int day, bool currentHolding) override;
    bool shouldSell(Stock *stock, int day, bool currentHolding) override;
};


// Regression Strategy: Train on first 70% of range, trade on remaining 30%
// Buy if predicted increase > threshold%, Sell if predicted decrease > threshold%
class RegressionStrategy : public Strategy {
private:
    Regression model;   // the regression model instance, has all the methods to train and predict
    bool modelTrained;
    int trainEndDay;   // Last day used for training
    
public:
    RegressionStrategy();
    
    // Must be called before backtesting to train on the given range
    void trainModel(Stock* stock, int startDay, int endDay);
    
    bool shouldBuy(Stock* stock, int day, bool currentlyHolding) override;
    bool shouldSell(Stock* stock, int day, bool currentlyHolding) override;
    
    // Get the day training ended (backtest should start from trainEndDay+1)
    int getTrainEndDay() const;
    
    // Check if model has been trained successfully
    bool trained() const;
};

#endif