#include "../include/Portfolio.h" //have to use double quotes for user-defined header files
#include <iostream>

using namespace std;

// constructor
Portfolio::Portfolio(string portfolioName)
{
    name = portfolioName;
    cashBalance = 0;
}

void Portfolio::buyStock(string symbol, int quantity, double price, string date)
{
    double totalCost = quantity * price;

    if (totalCost > cashBalance)
    {
        cout << "not enough balance. Your balance is " << getCashBalance();
        return;
    }

    auto it = holdings.find(symbol);
    if (it != holdings.end())
    { // already owns some of this share
        Holding &h = holdings[symbol];
        double totalValue = (h.avgCost * h.quantity) + totalCost;
        h.quantity += quantity;
        h.avgCost = totalValue / h.quantity;
    }
    else
    { // does not own this share at all
        Holding h{symbol, quantity, price, date};
        holdings[symbol] = h;
    }

    // recording the transaction
    string transaction = "Buy " + to_string(quantity) + " " + symbol + " stocks at price " + to_string(price) + " on " + date;
    transactions.push_back(transaction);

    cout << "Transaction done! Bought " << quantity << " shares of " << name << " company" << endl;
    cashBalance -= totalCost;
}

void Portfolio::sellStock(string symbol, int quantity, double price, string date)
{
    auto it = holdings.find(symbol);
    if (it == holdings.end())
    {
        cout << "you dont own " << symbol << " " << endl;
        return;
    }
    Holding &h = holdings[symbol];

    if (h.quantity < quantity)
    {
        cout << "You dont have enough of this share" << endl;
        return;
    }

    double revenue = quantity * price;
    cashBalance += revenue;
    h.quantity -= quantity;

    if (h.quantity == 0)
        holdings.erase(symbol);

    string transaction = "sell " + to_string(quantity) + " shares of " + symbol + " at price " + to_string(price) + " on " + date;
    transactions.push_back(transaction);
    cout << "sold " << quantity << " shares from " << symbol << endl;
}

string Portfolio::getName() { return name; }
double Portfolio::getCashBalance() { return cashBalance; }
void Portfolio::addCash(double amount)
{
    cashBalance += amount;
    cout << "Successfully added " << amount << "tk on portfolio" << endl;
}


void Portfolio::displayHoldings(){

    if(holdings.empty()){
        cout<<"You have no holdings in this portfolio";
        return;
    }

    cout<<"\nHoldings in "<<name<< " portfolio"<<">>>"<<endl;
    cout<<"Symbol\t Quantity \t averageCost \t Total"<<endl;
    for(auto& pair:holdings){
        Holding& h = pair.second;
        cout<<h.symbol<<"\t"
            <<h.quantity<<"\t"
            <<"$"<<h.avgCost<<"\t"
            <<"$"<<h.avgCost*h.quantity
            <<endl;
    }
}
void Portfolio::displayTransactions(){
    if(!transactions.size()>0)
        cout<<"No transaction has been done yet"<<endl;
    
    else{
        cout<<"Transactions are--"<<endl;
        for (int i = 0; i < transactions.size(); i++)
        {
            cout<<i+1<<"."<<transactions[i]<<endl;        }
        
    }
}

void Portfolio::displaySummary(map<string, Stock *> &stockData){

}

bool Portfolio::hasStock(string symbol){
    return holdings.find(symbol) != holdings.end();
}

int Portfolio::getquanity(string symbol){
    if(hasStock(symbol))
        return holdings.at(symbol).quantity;
    
    return 0;
}   

bool Portfolio::saveToFile(string filename){
    cout<<"remainig yet";
    return false;
}
bool Portfolio::loadFromFile(string filename){
    cout<<"remaining yet";
    return false;
}