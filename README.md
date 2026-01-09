# QuantLab: Stock Portfolio Management System

## Project Description

A C++ application for managing investment portfolios and analyzing stock performance. Users can create multiple portfolios, track investments in different companies, and analyze how their holdings perform over time. The system calculates financial metrics like returns, volatility, and risk-adjusted performance measures. It includes a backtesting engine to test trading strategies on historical data and uses regression models to predict future stock trends and provide buy/hold/sell recommendations.

## Features

- Portfolio management (create, track, buy/sell stocks)
- Financial analytics (returns, volatility, Sharpe ratio, drawdown)
- Technical indicators (Moving Averages, RSI, MACD, Bollinger Bands)
- Strategy backtesting engine
- Predictive analytics using regression models
- Buy/sell recommendation system
- Console-based user interface

## Technical Stack

- **Language**: C++
- **Data Structures**: STL (vector, map etc)
- **Storage**: File-based (CSV for stock data)
- **Interface**: Console/Terminal

## Timeline

- **Phase 1** (Weeks 1-8): Core system and portfolio management
- **Phase 2** (Weeks 9-14): Analytics and backtesting
- **Phase 3** (Weeks 15-18): Predictive models and recommendations



## How to Run
g++ -std=c++17 main.cpp src/Stock.cpp src/Portfolio.cpp src/Analytics.cpp src/Strategy.cpp src/Backtester.cpp -o quantlab
./quantlab