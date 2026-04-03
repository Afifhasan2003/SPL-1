// DataFetcher.h
#ifndef DATAFETCHER_H
#define DATAFETCHER_H

#include <string>
#include <vector>

using namespace std;

class DataFetcher {
public:
    // Update all stocks in watchlist.txt
    static void updateWatchlist(string watchlistFile = "watchlist.txt");

    // Update a single stock by symbol
    // forceRedownload=true: always do full fresh download, overwriting existing CSV
    static bool updateStock(string symbol, bool forceRedownload = false);

private:
    // Core fetch: pulls data from Twelve Data from startDate to today
    // startDate format: YYYY-MM-DD. If empty, fetches full history.
    static bool fetchAndSave(string symbol, string startDate);

    //Make HTTP GET request, returns response body(json)
    static string makeRequest(string url);

    //Parse JSON response and save to CSV
    static bool parseAndSave(string& json,  string& filename, bool append);

    //Get today's date as YYYY-MM-DD string
    static string getTodayDate();

    //Add one day to a YYYY-MM-DD date string
    static string addOneDay(string& date);

    //Get the last date recorded in a CSV (returns "" if file missing/empty)
    static string getLastDate(string& symbol);

    static string loadApiKey(); // from config.txt
};

#endif