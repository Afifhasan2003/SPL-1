// Regression.cpp
#include "../include/Regression.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace std;

//  Constructor, default values before each training session
Regression::Regression() {
    isTrained = false;
    trainStart = 0;     //
    trainEnd = 0;
    rSquared = 0.0;
    meanAbsoluteError = 0.0;
    targetMean = 0.0;
    targetStd = 1.0;
    signalThreshold = loadThreshold();
    stockSymbol = "";
}

// ─── Load threshold from config.txt ──────────────────────────────────────────
double Regression::loadThreshold()  {
    ifstream file("config.txt");
    if (!file.is_open()) return 0.5;

    string line;
    while (getline(file, line)) {
        if (line.substr(0, 22) == "REGRESSION_THRESHOLD=") {
            try {
                return stod(line.substr(22));   //string to double
            } catch (...) {         //catch any conversion error
                return 0.5;
            }
        }
    }

    return 0.5;
}

// Matrix Multiply
vector<vector<double>> Regression::matMul(
    vector<vector<double>>& A, int rowsA, int colsA,
    vector<vector<double>>& B, int rowsB, int colsB
) {
    // Initialize result matrix with zeros
    vector<vector<double>> result(rowsA, vector<double>(colsB, 0.0));

    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            for (int k = 0; k < colsA; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

//Matrix Transpose
vector<vector<double>> Regression::matTranspose(
     vector<vector<double>>& A, int rows, int cols
)  {
    vector<vector<double>> result(cols, vector<double>(rows, 0.0));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[j][i] = A[i][j];
        }
    }

    return result;
}

// Matrix Inverse (Gaussian Elimination with partial pivoting)
vector<vector<double>> Regression::matInverse(
     vector<vector<double>>& A, int n
) {
    // Build augmented matrix [A | I]
    vector<vector<double>> aug(n, vector<double>(2 * n, 0.0));      //make columns double for augmented 

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug[i][j] = A[i][j];
        }
        aug[i][n + i] = 1.0;  // Identity
    }

    // Forward elimination with partial pivoting
    for (int col = 0; col < n; col++) {     //for each column 
        // Find pivot row
        int pivotRow = col;
        double maxVal = fabs(aug[col][col]);        //  fabs( -2.5 ) = 2.5

        for (int row = col + 1; row < n; row++) {
            if (fabs(aug[row][col]) > maxVal) {
                maxVal = fabs(aug[row][col]);
                pivotRow = row;
            }
        }

        // Swap rows
        if (pivotRow != col) {
            vector<double> temp = aug[col];
            aug[col] = aug[pivotRow];
            aug[pivotRow] = temp;
        }

        // Check for singularity
        if (fabs(aug[col][col]) < 1e-12) {
            cerr << "✗ Matrix is singular — cannot invert." << endl;
            return vector<vector<double>>();
        }

        // Scale pivot row
        double pivot = aug[col][col];
        for (int j = 0; j < 2 * n; j++) {
            aug[col][j] /= pivot;
        }

        // Eliminate column entries above and below pivot
        for (int row = 0; row < n; row++) {
            if (row == col) continue;
            double factor = aug[row][col];
            for (int j = 0; j < 2 * n; j++) {
                aug[row][j] -= factor * aug[col][j];
            }
        }
    }

    // Extract right half (the inverse)
    vector<vector<double>> inv(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inv[i][j] = aug[i][n + j];
        }
    }

    return inv;
}

//  Extract raw indicators of a day (sma20, sma50, rsi, macd, momentum) 
bool Regression::extractFeatures(Stock* stock, int day, vector<double>& features) {
//use info of yesterday for today's prediction
    int lagDay = day - 1;

    if (lagDay < 0) return false;

    double sma20  = stock->getSMA20(lagDay);
    double sma50  = stock->getSMA50(lagDay);
    double rsi    = stock->getRSI(lagDay);
    double macd   = stock->getMACD(lagDay);
    double mom    = stock->getMomentum(lagDay);
    // double bMid   = stock->getBollingerMiddle(lagDay);  // Disabled: duplicates SMA20

    // Skip if any indicator hasn't been calculated yet (0.0 placeholder)
    if (sma20 == 0.0 || sma50 == 0.0 || rsi == 0.0 ||
        macd == 0.0 || mom == 0.0) {
        return false;
    }

    features.clear();
    features.push_back(sma20);
    features.push_back(sma50);
    features.push_back(rsi);
    features.push_back(macd);
    features.push_back(mom);
    // features.push_back(bMid);  // Disabled: duplicates SMA20

    return true;
}

//  Normalize features using stored mean/std 
vector<double> Regression::normalizeFeatures( vector<double>& raw)  {
    vector<double> normalized(raw.size());

    for (int i = 0; i < (int)raw.size(); i++) {
        if (featureStds[i] < 1e-10) {       //if stddev is too small, avoid division by zero and set normalized value to 0, meaning no deviation from mean
            normalized[i] = 0.0;
        } else {
            normalized[i] = (raw[i] - featureMeans[i]) / featureStds[i];
        }
    }

    return normalized;
}

//  Normalize / Denormalize target 
double Regression::normalizeTarget(double value)  {     //During training, we normalize the target (closing prices) before solving for weights. 
    if (targetStd < 1e-10) return 0.0;                     
    return (value - targetMean) / targetStd;
}

double Regression::denormalizeTarget(double value){    // During prediction, the model outputs a normalized number 
                                                        //(because it learned in normalized scale). We need to convert it back to real price.
    return (value * targetStd) + targetMean;
}

//  Train (this is the real regression implementation, finding the value of weights))
void Regression::train(Stock* stock, int startDay, int endDay) {
    stockSymbol = stock->getSymbol();
    trainStart = startDay;
    trainEnd = endDay;
    isTrained = false;

    int NUM_FEATURES = 5;   // sma20, sma50, rsi, macd, momentum

    //  Step 1: Collect valid samples (meaning if a day doesn't have all indicators calculated, we skip it) 
    vector<vector<double>> rawX;   // raw features
    vector<double> rawY;           // raw targets (close prices)


    //creating the matrix X and vector Y (which is the closed price of the day)
    for (int day = startDay; day <= endDay; day++) {
        vector<double> features;
        if (!extractFeatures(stock, day, features)) continue;
            //now feature has {sma20, sma50, rsi, macd, momentum} of day-1

        double target = stock->getClosePrice(day);
        if (target == 0.0) continue;

        rawX.push_back(features);
        rawY.push_back(target);
    }

    int n = rawX.size();

    if (n < NUM_FEATURES + 2) { //matrix X will have 6 columns, so we need at least 7 samples to solve for weights, else multiply will be problem 
        cout << "✗ Not enough valid training samples (" << n << "). Need at least "
             << NUM_FEATURES + 2 << "." << endl;
        return;
    }

    cout << " Collected " << n << " valid training samples." << endl;

    //  Step 2: Compute feature means and stds for Z-score normalization 
    featureMeans.assign(NUM_FEATURES, 0.0);     //This is used for resetting and initializing a vector to a specific size and value, 
    featureStds.assign(NUM_FEATURES, 0.0);      //{0.0, 0.0, 0.0, 0.0, 0.0}

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            featureMeans[j] += rawX[i][j];
        }
    }
    for (int i = 0; i < NUM_FEATURES; i++) {
        featureMeans[i] /= n;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            double diff = rawX[i][j] - featureMeans[j];
            featureStds[j] += diff * diff;
        }
    }
    for (int i = 0; i < NUM_FEATURES; i++) {
        featureStds[i] = sqrt(featureStds[i] / n);
    }

    //  Step 3: Compute target mean and std 
    targetMean = 0.0;
    for (int i = 0; i < n; i++) targetMean += rawY[i];
    targetMean /= n;

    double targetVariance = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = rawY[i] - targetMean;
        targetVariance += diff * diff;
    }
    targetStd = sqrt(targetVariance / n);

    // ── Step 4: Build normalized X matrix (with bias column) and Y vector ────
    // X is (n x 6): [1, sma20_norm, sma50_norm, rsi_norm, macd_norm, mom_norm]
    // Y is (n x 1): normalized close prices

    int cols = NUM_FEATURES + 1;  // +1 for bias

    vector<vector<double>> X(n, vector<double>(cols, 0.0));
    vector<vector<double>> Y(n, vector<double>(1, 0.0));    //n by 1 matrix

    for (int i = 0; i < n; i++) {
        X[i][0] = 1.0;  // bias term

        vector<double> normFeatures = normalizeFeatures(rawX[i]);
        for (int j = 0; j < NUM_FEATURES; j++) {
            X[i][j + 1] = normFeatures[j];
        }

        Y[i][0] = normalizeTarget(rawY[i]);
    }

    //  Step 5: Compute weights using Normal Equation: W = (X^T * X)^(-1) * X^T * Y
    vector<vector<double>> Xt  = matTranspose(X, n, cols);
    vector<vector<double>> XtX = matMul(Xt, cols, n, X, n, cols);
    vector<vector<double>> XtX_inv = matInverse(XtX, cols);

    if (XtX_inv.empty()) {
        cout << " Training failed: matrix inversion error." << endl;
        return;
    }

    vector<vector<double>> XtY  = matMul(Xt, cols, n, Y, n, 1);
    vector<vector<double>> W    = matMul(XtX_inv, cols, cols, XtY, cols, 1);

    // Store weights
    weights.resize(cols);
    for (int i = 0; i < cols; i++) {
        weights[i] = W[i][0];   
    }   
    //this is the thing we were looking for







    //Now lets see how accurate the result is, using two common metrics: R-squared and Mean Absolute Error (MAE)
    //R^2 = 1 - (SS_res / SS_tot), 
    //MAE = (1/n) * sum(|actual - predicted|) : If the MAE is $\$1.50$,  on average, your model's prediction is off by a dollar and fifty cents.
                                                // This is vital for risk management—if you are trading for a $\$0.50$ profit margin but your model has an error of $\$1.50$, your strategy is too risky.

    //  Step 6: Compute training metrics
    double ssTot = 0.0;     // Total sum of squares (variance of actual prices)
    double ssRes = 0.0;     // Residual sum of squares (variance of prediction errors)
    double maeSum = 0.0;

    for (int i = 0; i < n; i++) {
        // Predicted (normalized)
        double predNorm = weights[0];
        for (int j = 0; j < NUM_FEATURES; j++) {
            predNorm += weights[j + 1] * X[i][j + 1];
        }

        // Denormalize
        double predPrice = denormalizeTarget(predNorm);
        double actualPrice = rawY[i];

        double diff = actualPrice - predPrice;
        ssRes += diff * diff;
        maeSum += fabs(diff);

        double diffFromMean = actualPrice - targetMean;
        ssTot += diffFromMean * diffFromMean;
    }

    if (ssTot > 1e-10) {
        rSquared = 1.0 - (ssRes / ssTot);
    } else {
        rSquared = 0.0;   // If actual prices have no variance, R² is not defined, we set it to 0
    }
    meanAbsoluteError = maeSum / n;

    isTrained = true;

    cout << " Model trained successfully!" << endl;
    cout << fixed << setprecision(4);
    cout << "  R² Score: " << rSquared << endl;     // R² near 1 means the model's predictions are very close
    cout << "  MAE: $" << meanAbsoluteError << endl;    // MAE of $1.50 means on average the model's predictions are off by $1.50, 
}

// Predict next-day closing price for given day (returns -1.0 if model not trained or features unavailable)
double Regression::predict(Stock* stock, int day)  {
    if (!isTrained) return -1.0;

    vector<double> features;
    if (!extractFeatures(stock, day, features)) return -1.0;

    vector<double> normFeatures = normalizeFeatures(features);

    // W^T * x
    double predNorm = weights[0];  // bias
    for (int i = 0; i < (int)normFeatures.size(); i++) {
        predNorm += weights[i + 1] * normFeatures[i];
    }

    return denormalizeTarget(predNorm);
}
 
string Regression::getSignal(Stock* stock, int day)  {
    if (!isTrained) return "N/A";

    double predictedPrice = predict(stock, day);
    if (predictedPrice < 0) return "N/A";       //something is wrong, so dont know

    double previousDay = stock->getClosePrice(day - 1);
    if (previousDay == 0.0) return "N/A";        //if dont know yesterday's price, then can not compare and say buy or sell

    double changePercentage = ((predictedPrice - previousDay) / previousDay) * 100.0;

    if (changePercentage > signalThreshold) return "BUY";
    if (changePercentage < -signalThreshold) return "SELL";
    return "HOLD";
}

//called by main.cpp to display the full report of the regression model, 
void Regression::displayReport(Stock* stock)  {
    if (!isTrained) {
        cout << "\n Model not trained yet. Please train first." << endl;
        return;
    }

    int NUM_FEATURES = 5;
    string featureNames[] = {
        "SMA20", "SMA50", "RSI", "MACD", "Momentum"
        // "Bollinger Mid" is Disabled coz it duplicates SMA20
    };

    cout << "\n========================================" << endl;
    cout << "    REGRESSION MODEL REPORT" << endl;
    cout << "========================================" << endl;
    cout << "Stock: " << stockSymbol << endl;
    cout << "Training Period: Day " << trainStart << " to Day " << trainEnd << endl;
    cout << "Signal Threshold: ±" << signalThreshold << "%" << endl;
    cout << "----------------------------------------" << endl;
    cout << fixed << setprecision(4);

  
    
    cout << "\nModel Performance:" << endl;
    cout << "  R² Score:              " << rSquared << endl; //how well the model fits the training data, closer to 1 is better, negative means worse than just predicting the mean
    cout << "  Mean Absolute Error:   $" << meanAbsoluteError << endl; //how far off the model's predictions are from actual prices on average, lower is better, gives a sense of expected error in dollars which is crucial for risk management



    // ── Feature Weights ───────────────────────────────────────────────────────
    cout << "\nFeature Weights (normalized scale):" << endl;
    cout << "  Bias:                  " << weights[0] << endl;
    for (int i = 0; i < NUM_FEATURES; i++) {
        cout << "  " << left << setw(20) << featureNames[i]
             << weights[i + 1] << endl;
    }

    // ── Last 10 Predictions vs Actual ─────────────────────────────────────────
    cout << "\nLast 10 Predictions vs Actual:" << endl;
    cout << "----------------------------------------" << endl;
    cout << right;
    cout << setw(6) << "Day"
         << setw(12) << "Actual"
         << setw(12) << "Predicted"
         << setw(10) << "Error"
         << setw(8) << "Err%" << endl;
    cout << "----------------------------------------" << endl;

    int dataSize = stock->getDataSize();
    int count = 0;
    int startSearch = max(trainStart, dataSize - 20);  // Search recent days

    for (int day = startSearch; day <= trainEnd && count < 10; day++) {
        double predicted = predict(stock, day);
        if (predicted < 0) continue;

        double actual = stock->getClosePrice(day);
        if (actual == 0.0) continue;

        double error = actual - predicted;
        double errorPct = (error / actual) * 100.0;

        cout << setw(6) << day
             << setw(12) << fixed << setprecision(2) << actual
             << setw(12) << predicted
             << setw(10) << error
             << setw(7) << errorPct << "%" << endl;

        count++;
    }

    if (count == 0) {
        cout << "  No predictions available in training range." << endl;
    }

    // ── Next Day Prediction ───────────────────────────────────────────────────
    cout << "\n----------------------------------------" << endl;
    cout << "Next-Day Prediction:" << endl;

    int lastDay = dataSize - 1;
    double nextPred = predict(stock, lastDay + 1);  // Predict day after last

    // For next-day we use last day's indicators (lagged), predicting tomorrow
    // extractFeatures uses lagDay = day-1, so we call predict with lastDay+1
    vector<double> features;
    bool hasFeatures = extractFeatures(stock, lastDay + 1, features);

    //print details 
    if (!hasFeatures || nextPred < 0) { 
        // if can not predict from lastDay+1, try to predict from lastDay (which uses indicators of day before yesterday, so less accurate but at least gives some info)
        nextPred = predict(stock, lastDay);
        double todayClose = stock->getClosePrice(lastDay - 1);
        double lastClose  = stock->getClosePrice(lastDay);

        if (nextPred > 0 && lastClose > 0) {
            double pctChange = ((nextPred - lastClose) / lastClose) * 100.0;
            string signal = getSignal(stock, lastDay);

            cout << "  Today's Close:    $" << lastClose << endl;
            cout << "  Predicted Next:   $" << nextPred << endl;
            cout << "  Expected Change:  ";
            if (pctChange >= 0) cout << "+";
            else cout << "-";
        
            cout << pctChange << "%" << endl;
            cout << "  Signal:           " << signal << endl;
        } else {
            cout << "  Not enough indicator data for next-day prediction." << endl;
        }
    }
    else { // nothing went wrong 

        double lastClose = stock->getClosePrice(lastDay);
        double pctChange = ((nextPred - lastClose) / lastClose) * 100.0;

        string signal;
        if (pctChange > signalThreshold)       signal = "BUY";
        else if (pctChange < -signalThreshold) signal = "SELL";
        else                                    signal = "HOLD";

        cout << "  Today's Close:    $" << lastClose << endl;
        cout << "  Predicted Next:   $" << nextPred << endl;
        cout << "  Expected Change:  ";
        if (pctChange >= 0) cout << "+";
        cout << pctChange << "%" << endl;
        cout << "  Signal:           " << signal << endl;
    }

    cout << "========================================" << endl;
}

//  Trained check 
bool Regression::trained() {
    return isTrained;
}