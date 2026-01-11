#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <string>

class MenuSystem {
public:
    static void displayMainMenu();
    static void displayPortfolioMenu();
    static void displaySelectedPortfolioMenu(std::string portfolioName);
};

#endif