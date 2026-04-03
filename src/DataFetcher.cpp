// DataFetcher.cpp
#include "../include/DataFetcher.h"
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

using namespace std;

// ─── libcurl write callback ───────────────────────────────────────────────────
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// ─── Load API key from config.txt ────────────────────────────────────────────
string DataFetcher::loadApiKey() {
    ifstream file("config.txt");
    if (!file.is_open()) {
        cerr << " config.txt not found." << endl;
        return "";
    }

    string line;    //format is API_KEY=actual_key_here
    while (getline(file, line)) {
        if (line.substr(0, 8) == "API_KEY=") {
            return line.substr(8);
        }
    }

    cerr << "API_KEY not found in config.txt" << endl;   //cerr shows in red in terminal, good for errors
    return "";
}


void DataFetcher::updateWatchlist(string watchlistFileName) {
    ifstream file(watchlistFileName);
    if (!file.is_open()) {
        cout << " Could not open watchlist: " << watchlistFileName << endl;
        return;
    }

    vector<string> symbols;
    string line;
    while (getline(file, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
            line.pop_back(); //removes last char(in this case \n, \r, or space). This is to handle different OS newline formats and any trailing spaces.
        if (!line.empty())
            symbols.push_back(line);
    }
    file.close();

    if (symbols.empty()) {
        cout << "# Watchlist is empty." << endl; 
        return;
    }

    cout << "\n=== Updating Watchlist (" << symbols.size() << " stocks) ===" << endl;

    int success = 0;
    for ( string& sym : symbols) {
        if (updateStock(sym)) success++;
    }

    cout << "\n✓ Updated " << success << "/" << symbols.size() << " stocks." << endl;
}

// update a single stock of given symbol
bool DataFetcher::updateStock(string symbol, bool forceRedownload) {

    // to avoid same stock with diff name, like AAPL and aapl 
    for (char& c : symbol) c = toupper(c);
    
    if (forceRedownload) {
        cout << "[" << symbol << "] Re-downloading fresh from Twelve Data..." << endl;
        return fetchAndSave(symbol, "");    //fetchAndSave with empty startDate will download full history and overwrite existing CSV
    }

    string lastDate = getLastDate(symbol);  //last data in CSV, format YYYY-MM-DD. If file missing/empty, returns ""

    if (lastDate.empty()) {
        cout << "[" << symbol << "] No existing data. Fetching full history..." << endl;
        return fetchAndSave(symbol, "");
    } 
    else {
        string fromDate = addOneDay(lastDate);      //start fetching from the day after last recorded date to avoid duplicates
        string today = getTodayDate();

        if (fromDate >= today) {
            cout << "[" << symbol << "] Already up to date (last: " << lastDate << ")" << endl;
            return true;
        }

        cout << "[" << symbol << "] Fetching from " << fromDate << " onwards..." << endl;
        return fetchAndSave(symbol, fromDate);
    }
}


// this is the main data fetching function 
bool DataFetcher::fetchAndSave(string symbol, string startDate) {
    string apiKey = loadApiKey();
    if (apiKey.empty()) return false;

    string today = getTodayDate();


    string url = "https://api.twelvedata.com/time_series"
                 "?symbol=" + symbol +
                 "&interval=1day" +   
                 "&outputsize=5000" +   //request max rows to ensure we get all data from startDate to today, even for stocks with long history
                 "&apikey=" + apiKey;

    // If we have a start date, add it
    if (!startDate.empty()) {
        url += "&start_date=" + startDate;
        url += "&end_date=" + today;
    }

    string response = makeRequest(url); //response is the JSON string returned by the API

    if (response.empty()) {
        cout << "✗ [" << symbol << "] No response from server." << endl;
        return false;
    }

    // Check for API errors
    if (response.find("\"status\":\"error\"") != string::npos ||        //npos= not found
        response.find("\"code\":404") != string::npos) {
        cout << "✗ [" << symbol << "] API error (bad symbol or invalid request)." << endl;
        return false;
    }

    // Check for rate limit
    if (response.find("API credits") != string::npos ||
        response.find("rate limit") != string::npos) {
        cout << "✗ [" << symbol << "] Rate limit reached. Try again later." << endl;
        return false;
    }

    string filename = "data/" + symbol + ".csv";
    bool append = !startDate.empty();

    return parseAndSave(response, filename, append);    //if appent= false then parseAndSave will write from scratch with header, if true it will append new rows to existing CSV while avoiding duplicates
}

// responsde is json
bool DataFetcher::parseAndSave( string& json, string& filename, bool append) {
    // Twelve Data returns newest first —So we collect all rows then reverse

    // JSON structure: 
    // "values":[
    //   {"datetime":"2023-09-01","open":"...","high":"...","low":"...","close":"...","volume":"..."},
    //   {"datetime":"2023-08-31","open":"...","high":"...","low":"...","close":"...","volume":"..."},
    //   ...
    // ]    


    vector<string> rows;

    // Find "values" array
    size_t valuesPos = json.find("\"values\":");
    if (valuesPos == string::npos) {
        cout << "✗ Could not find values in response." << endl;
        return false;
    }

    // Extract each object {...} from the array
    size_t pos = valuesPos;
    while (true) {  //loop through each object {...} in the "values" array

        size_t objStart = json.find('{', pos + 1);  //returns integer position of first occurrence of '{' after pos. If not found, returns string::npos
        if (objStart == string::npos) break;
        size_t objEnd = json.find('}', objStart);
        if (objEnd == string::npos) break;

        string obj = json.substr(objStart, objEnd - objStart + 1);

        // Helper lambda to extract a field value from the object
        auto extract = [&](const string& key) -> string {
            string search = "\"" + key + "\":\"";
            size_t start = obj.find(search);
            if (start == string::npos) return "";
            start += search.size();
            size_t end = obj.find("\"", start);
            if (end == string::npos) return "";
            return obj.substr(start, end - start);
        };

        string date   = extract("datetime");
        string open   = extract("open");
        string high   = extract("high");
        string low    = extract("low");
        string close  = extract("close");
        string volume = extract("volume");

        if (!date.empty() && !close.empty()) {
            rows.push_back(date + "," + open + "," + high + "," + low + "," + close + "," + volume);
        }

        pos = objEnd;
    }

    if (rows.empty()) {
        cout << "  (no new data returned)" << endl;
        return true;
    }

    // Reverse so oldest is first (chronological order)
    reverse(rows.begin(), rows.end());

    if (append) {   //if append=true, we need to read existing CSV to avoid duplicates, 
                    //then append only new rows. If false, we write from scratch with header

        // Read existing dates to avoid duplicates
        set<string> existingDates;
        {
            ifstream readFile(filename);
            if (readFile.is_open()) {
                string line;
                getline(readFile, line); // skip header
                while (getline(readFile, line)) {
                    size_t comma = line.find(',');  //position of first comma 
                    if (comma != string::npos)
                        existingDates.insert(line.substr(0, comma));
                }
            }
        }

        // Append only new rows
        ofstream outFile(filename, ios::app);  //input-output-stream:append mode. This will create the file if it doesn't exist, or open it for appending if it does exist.
        if (!outFile.is_open()) {
            cout << "✗ Could not open file for appending: " << filename << endl;
            return false;
        }

        int added = 0;
        for (const string& row : rows) {
            size_t comma = row.find(',');  
            string date = row.substr(0, comma);
            if (existingDates.find(date) == existingDates.end()) { //date doesnt exist 
                outFile << row << "\n";
                added++;
            }
        }
        outFile.close();
        cout << "✓ [" << filename << "] Appended " << added << " new rows." << endl;

    } else {
        // Full download — write from scratch with header
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cout << "✗ Could not create file: " << filename << endl;
            return false;
        }

        outFile << "Date,Open,High,Low,Close,Volume\n";
        for (const string& row : rows) {
            outFile << row << "\n";
        }
        outFile.close();
        cout << "✓ [" << filename << "] Saved " << rows.size() << " rows." << endl;
    }

    return true;
}

// recieves api url, returns Json, or empty string on error
string DataFetcher::makeRequest(string url) {
    CURL* curl = curl_easy_init();
    string buffer;

    if (!curl) return "";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible)");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);//performs the HTTP request and stores the response in buffer via the WriteCallback function
                                            //res is just a status code(integer )

    if (res != CURLE_OK) {
        cerr << "✗ curl error: " << curl_easy_strerror(res) << endl;
        buffer = "";
    }

    curl_easy_cleanup(curl);
    return buffer;  //butter is jsob string
}

// get last recorded date from CSV
string DataFetcher::getLastDate( string& symbol) {
    string filename = "data/" + symbol + ".csv";
    ifstream file(filename);
    if (!file.is_open()) return "";

    string lastLine, line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (!line.empty()) lastLine = line;
    }
    file.close();

    if (lastLine.empty()) return "";

    size_t comma = lastLine.find(',');
    if (comma == string::npos) return "";

    return lastLine.substr(0, comma); // returns YYYY-MM-DD
}


string DataFetcher::getTodayDate() {
    time_t now = time(nullptr); //in seconds since 1970-01-01
    tm* t = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t); //format time as YYYY-MM-DD

    return string(buf);
}

// ─── Private: add one day to YYYY-MM-DD ──────────────────────────────────────
string DataFetcher::addOneDay( string& date) {
    int year  = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day   = stoi(date.substr(8, 2));

    tm t = {};
    t.tm_year = year - 1900;    //tm_year is: years since 1900
    t.tm_mon  = month - 1;      //tm_mon is 0-based (0=Jan, 1=Feb, ..., 11=Dec)
    t.tm_mday = day + 1;        //add one day. 
    mktime(&t);                 //if day jan 32, mktime will normalize it to feb 1, etc.

    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
    return string(buf);
}