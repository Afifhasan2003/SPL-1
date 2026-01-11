#include "../include/MenuSystem.h"
#include <iostream>

using namespace std;

void MenuSystem::displayMainMenu() {
    cout << "\n======================================" << endl;
    cout << "      QuantLab - Main Menu" << endl;
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

void MenuSystem::displayPortfolioMenu() {
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

void MenuSystem::displaySelectedPortfolioMenu(string portfolioName) {
    cout << "\n======================================" << endl;
    cout << "  Portfolio: " << portfolioName << endl;
    cout << "======================================" << endl;
    cout << "1. Add cash" << endl;
    cout << "2. Buy stock" << endl;
    cout << "3. Sell stock" << endl;
    cout << "4. View holdings" << endl;
    cout << "5. View transactions" << endl;
    cout << "6. View summary" << endl;
    cout << "7. View performance analytics" << endl;
    cout << "8. Back" << endl;
    cout << "======================================" << endl;
    cout << "Enter choice: ";
}