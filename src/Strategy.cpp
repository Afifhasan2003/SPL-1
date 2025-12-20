#include "../include/Strategy.h"

using namespace std;

Strategy::Strategy(string strategyName){
    name = strategyName;
}

string Strategy::getName(){
    return name;
}

RSIStrategy::RSIStrategy() : Strategy("RSI Strategy"){  //cnsures the base class part is properly initialized before the derived class does anything

}
bool RSIStrategy::shouldBuy(Stock* stock, int day, bool currentlyHolding){
    if(currentlyHolding) return false;

    double rsi = stock->getRSI(day);
    return (rsi> 0 && rsi<30); //means oversold
}
bool RSIStrategy::shouldSell(Stock* stock, int day, bool currentlyHOlding){
    if(currentlyHOlding) return false;

    double rsi = stock->getRSI(day);
    return (rsi>70); //means overbaught
}

MAStrategy::MAStrategy() : Strategy("Moving average crossOver"){
    previousCrossAbove = false;
}

bool MAStrategy::shouldBuy(Stock* stock, int day, bool currentlyHolding){
    if(currentlyHolding) return false;

    if(day<50) return false;  //not enough data to get sma50

    double sma20 = stock->getSMA20(day);
    double sma50 = stock->getSMA50(day);
    double prevsma20 = stock->getSMA20(day-1);
    double prevsma50 = stock->getSMA50(day-1);

    if(sma20==0 || sma50==0) return false;

    bool hasCrossed = (prevsma20 <=prevsma50) && (sma20 > sma50); //age cross kore nai, but ajke cross korlo, this way we are certain, even tho the price was going up for some time, we didnt react early

    return hasCrossed;
}

bool MAStrategy::shouldSell(Stock* stock, int day, bool currentlyHolding){
    if(!currentlyHolding) return false;

    if(day<50) return false;  //not enough data to get sma50

    double sma20 = stock->getSMA20(day);
    double sma50 = stock->getSMA50(day);
    double prevsma20 = stock->getSMA20(day-1);
    double prevsma50 = stock->getSMA50(day-1);

    if(sma20==0 || sma50==0) return false;

    // crossedBelow
    bool hasCrossed = (prevsma20 >= prevsma50) && (sma20<sma50); //already loss hoise, but now confirm that its actually going down
    return hasCrossed;
}

BuyHoldStrategy::BuyHoldStrategy() : Strategy("Buy and Hold"){}

bool BuyHoldStrategy::shouldBuy(Stock* stock, int day, bool currentlyHolding) {
    // Buy only on first valid day (after indicators calculated)
    return (!currentlyHolding && day >= 50);
}

bool BuyHoldStrategy::shouldSell(Stock* stock, int day, bool currentlyHolding) {
    // Never sell
    return false;
}