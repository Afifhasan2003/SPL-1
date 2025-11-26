#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Helper function to remove spaces
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    
    if (start == string::npos) return "";
    return str.substr(start, end - start + 1);
}

int main() {
    // Vectors to store data
    vector<string> dates;
    vector<double> openPrices;
    vector<double> closePrices;
    vector<long long> volumes;
    
    // Open the CSV file
    ifstream file("data/firstCSVfile.csv");
    
    if (!file.is_open()) {
        cout << "Error: Could not open file!" << endl;
        return 1;
    }
    
    string line;
    int lineNumber = 0;
    
    // Read file line by line
    while (getline(file, line)) {
        lineNumber++;
        
        // Skip the header
        if (lineNumber == 1) {
            continue;
        }
        
        // Parse the line
        stringstream ss(line);
        string date, open, high, low, close, volume;
        
        getline(ss, date, ',');
        getline(ss, open, ',');
        getline(ss, high, ',');
        getline(ss, low, ',');
        getline(ss, close, ',');
        getline(ss, volume, ',');
        
        // Trim spaces
        date = trim(date);
        open = trim(open);
        close = trim(close);
        volume = trim(volume);
        
        // Convert strings to numbers and store
        dates.push_back(date);
        openPrices.push_back(stod(open));      // string to double
        closePrices.push_back(stod(close));
        volumes.push_back(stoll(volume));      // string to long long
    }
    
    file.close();
    
    // Display what we loaded
    cout << "Loaded " << dates.size() << " days of data\n" << endl;
    
    // Show first 3 and last 3 entries
    cout << "First 3 days:" << endl;
    for (int i = 0; i < 3 && i < dates.size(); i++) {
        cout << dates[i] << " - Open: $" << openPrices[i] 
             << ", Close: $" << closePrices[i] << endl;
    }
    
    cout << "\nLast 3 days:" << endl;
    int start = dates.size() - 3;
    for (int i = start; i < dates.size(); i++) {
        cout << dates[i] << " - Open: $" << openPrices[i] 
             << ", Close: $" << closePrices[i] << endl;
    }
    
    return 0;
}