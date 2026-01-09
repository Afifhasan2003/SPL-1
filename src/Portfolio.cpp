#include "../include/Portfolio.h" //have to use double quotes for user-defined header files
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//a constructor without an argument is by default provided by the compiler in C++, unlike Java
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

    cout << "\n=== Portfolio Summary: '" << name << "' ===" << endl;
    
    if (holdings.empty()) {
        cout << "No holdings yet." << endl;
        cout << "Cash Balance: $" << fixed << setprecision(2) << cashBalance << endl;
        return;
    }
    
    cout << fixed << setprecision(2);
    cout << "\nSymbol\tQty\tAvg Cost\tCurrent\t\tProfit/Loss" << endl;
    cout << "------------------------------------------------------------" << endl;

    double totalValue = 0.0;
    double totalCost = 0.0;
    
    for (const auto& pair : holdings) {
        const Holding& h = pair.second;
        double pricePerHolding = h.quantity * h.avgCost;
        totalCost += pricePerHolding;
        
        if (stockData.find(h.symbol) != stockData.end()) {
            // For now, we'll add a method to get latest price later
            cout << h.symbol << "\t" << h.quantity << "\t$" << h.avgCost 
                 << "\t\t(Load stock data)" << "\t$0.00" << endl;
        } else {
            cout << h.symbol << "\t" << h.quantity << "\t$" << h.avgCost 
                 << "\t\tN/A\t\tN/A" << endl;
        }
    }
    
    cout << "\nTotal Cost Basis: $" << totalCost << endl;
    cout << "Cash Balance: $" << cashBalance << endl;
    cout << "Total Portfolio Value: $" << (totalCost + cashBalance) << endl;


}

bool Portfolio::hasStock(string symbol){
    return holdings.find(symbol) != holdings.end();
}

int Portfolio::getQuanity(string symbol){
    if(hasStock(symbol))
        return holdings.at(symbol).quantity;
    
    return 0;
}   

bool Portfolio::saveToFile(string filename){
    ofstream file(filename);    //output file stream, to write to a file
    if(!file.is_open()){
        cout<<"could not save to "<<filename<<endl;
        return false;
    }
    file<< "Portfolio_name: "<<name<<endl;
    file<< "Cash: "<<cashBalance<<endl;

    file<< "Holdings: "<<endl;
    for(auto pair: holdings){
        Holding &h=pair.second;
        file<<h.symbol<<","<<h.quantity<<","<<h.avgCost<<","<<h.purchaseDate<<endl;
    }

    file<<"Transactions:"<<endl;
    for(auto &trans:transactions)       //transactions is vector of strings
        file<<trans<<endl;

    file.close();
    return true;
}
bool Portfolio::loadFromFile(string filename){
    
    ifstream file(filename);

    if (!file.is_open())
    {
        cout<<"could not load from "<<filename<<endl;
        return false;
    }

    string line, section="";

    while (getline(file,line))
    {  
        if (line.empty())
            continue;
        
        else if (line=="Holdings")
        {
            section="holding"; 
            continue;
        }
        else if(line=="Transactions"){
            section = "tran";
            continue;
        }
        

        //now
        if(section==""){     //name or cash
            size_t pos= line.find(':');         // find returns size_t type(unsigned numer)
            if(pos != string::npos)     //npos is the biggest number in size_t
            {
                string detail = line.substr(0,pos);
                string val = line.substr(pos+1);  //till end

                if(detail == "Portfolio_name" )
                    name = val;
                else if(detail == "Cash")
                    cashBalance = stod(val);
            }
        }
        else if (section == "holding")  
        {
        // each line has symbol,quantity,avgCost,date
        stringstream ss(line);
        string symbol,quantityStr,avgCostStr,dateStr;

        getline(ss, symbol, ',');
        getline(ss, quantityStr, ',');
        getline(ss, avgCostStr, ',');
        getline(ss, dateStr, ',');

        //now create an instance and save in map
        Holding h;
        h.symbol = symbol;
        h.quantity =  stoi(quantityStr);
        h.avgCost = stod(avgCostStr);
        h.purchaseDate = dateStr;

        holdings[symbol] = h;
 
        }
        else if (section== "tran")
        {
            transactions.push_back(line);
        }
        
    }
    
    file.close();
    return true;
}