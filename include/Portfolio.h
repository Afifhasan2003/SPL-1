#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include<iostream>
#include<string>
#include<map>
#include<vector>
#include"Stock.h"


using namespace std;

struct Holding
{
    string symbol;
    int quantity;
    double avgCost;
    string purchaseDate;
};

class Portfolio{
private:
    string name;
    map<string,Holding> holdings;    //symbol ->holding
    vector<string> transactions;
    double cashBalance;

public:
    Portfolio(string portfolioName);    //constructor

    void buyStock(string symbol, int quantity,double price, string date);
    void sellStock(string symbol, int quantity, double price, string date);

    string getName();
    double getCashBalance();
    void addCash(double amount);

    void displayHoldings();
    void displayTransactions();
    void displaySummary(map<string,Stock*>& stockData);


    bool hasStock(string symbol);
    int getquanity(string symbol);

    bool saveToFile(string filename);
    bool loadFromFile(string filename);
};


#endif