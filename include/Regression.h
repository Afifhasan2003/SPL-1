// Regression.h
#ifndef REGRESSION_H
#define REGRESSION_H

#include <vector>
#include <string>
#include "Stock.h"

using namespace std;

class Regression {
private:
    // Model weights (7 values: bias + 6 features)
    vector<double> weights; // {1, bita1, bita2, bita3, bita4, bita5}
    
    // Normalization parameters (mean and stddev for each feature)
    vector<double> featureMeans;    //for each indicator
    vector<double> featureStds;  //for each indicator 
    double targetMean;
    double targetStd;
    
    // Training info
    bool isTrained;
    int trainStart;
    int trainEnd;
    string stockSymbol;
    
    // Training metrics
    double rSquared;
    double meanAbsoluteError;
    
    // Signal threshold (loaded from config.txt)
    double signalThreshold;
    
    // ─── Matrix Operations ────────────────────────────────────────────────────
    
    // Multiply two matrices: result = A * B
    // A is (rowsA x colsA), B is (colsA x colsB)
    vector<vector<double>> matMul(
         vector<vector<double>>& A, int rowsA, int colsA,
         vector<vector<double>>& B, int rowsB, int colsB
    ) ;
    
    // Transpose a matrix: result = A^T
    // A is (rows x cols), result is (cols x rows)
    vector<vector<double>> matTranspose(
         vector<vector<double>>& A, int rows, int cols
    ) ;
    
    // Invert a square matrix using Gaussian elimination
    // Returns empty vector if matrix is singular
    vector<vector<double>> matInverse(
         vector<vector<double>>& A, int n
    ) ;
    
    // ─── Feature Helpers ─────────────────────────────────────────────────────
    
    // Extract raw features for a given day (uses day-1 indicators — lagged)
    // Returns false if any indicator is 0 (not enough history)
    bool extractFeatures(Stock* stock, int day, vector<double>& features) ;
    
    // Normalize features using stored mean/std
    vector<double> normalizeFeatures( vector<double>& raw) ;
    
    // Normalize a single target value
    double normalizeTarget(double value) ;
    
    // Denormalize a predicted target value back to price scale
    double denormalizeTarget(double value) ;
    
    // ─── Config ──────────────────────────────────────────────────────────────
    
    // Load REGRESSION_THRESHOLD from config.txt (defaults to 0.5)
    double loadThreshold() ;

public:
    // ructor
    Regression();
    
    // Train the model on stock data from startDay to endDay
    void train(Stock* stock, int startDay, int endDay);
    
    // Predict next-day closing price for given day
    // Returns -1.0 if model not trained or features unavailable
    double predict(Stock* stock, int day) ;
    
    // Get signal: "BUY", "HOLD", or "SELL" based on prediction vs today's close
    string getSignal(Stock* stock, int day) ;
    
    // Display full report: metrics, weights, last 10 predictions, next-day signal
    void displayReport(Stock* stock) ;
    
    // Check if model has been trained
    bool trained() ;
};

#endif