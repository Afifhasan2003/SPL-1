displaySummary,saveToFile, loadFromFile function in portfolio.cpp


## Regression working process
Sure. In the regression model we use three matrix operations to solve the OLS equation:

```
W = (X^T * X)^-1 * X^T * Y
```

Let me break down what each matrix actually is with real numbers.

---

**X — Feature Matrix** `(n × 6)`

Each row is one trading day, each column is one feature:

```
       bias  SMA20   SMA50   RSI    MACD   Mom
Day 1 [ 1,   150.2,  148.5,  45.2,  0.8,   2.1 ]
Day 2 [ 1,   151.0,  148.9,  47.1,  0.9,   2.3 ]
Day 3 [ 1,   152.1,  149.2,  49.0,  1.1,   2.5 ]
...
Day n [ 1,   ...                                ]
```

n = number of training days (could be 500, 1000, etc.)

---

**Y — Target Vector** `(n × 1)`

Next day's actual closing price for each day:

```
Day 1 [ 152.3 ]
Day 2 [ 153.1 ]
Day 3 [ 151.8 ]
...
Day n [ ...   ]
```

---

**W — Weight Vector** `(6 × 1)`

The result we're solving for — one weight per feature:

```
[ bias_weight  ]   →  base price offset
[ sma20_weight ]   →  how much SMA20 influences price
[ sma50_weight ]   →  how much SMA50 influences price
[ rsi_weight   ]   →  how much RSI influences price
[ macd_weight  ]   →  how much MACD influences price
[ mom_weight   ]   →  how much Momentum influences price
```

---

**The three intermediate matrices:**

**X^T** — just X flipped `(6 × n)` — needed for the math

**X^T * X** — `(6 × 6)` — captures how features correlate with each other

**X^T * X)^-1** — `(6 × 6)` — the inverse, this is the expensive operation (Gaussian elimination)

**X^T * Y** — `(6 × 1)` — captures how each feature correlates with the target price

---

**Final prediction for a new day:**

```
predicted_price = W[0]*1 + W[1]*SMA20 + W[2]*SMA50 + W[3]*RSI + W[4]*MACD + W[5]*Mom
```




## why normalize 
{
    Sure.

**Why normalize?**

Our input variables live on completely different scales — SMA20 might be 150, RSI is between 0-100, MACD is around 0.8, Momentum is 2.1. If we feed raw numbers into the matrix math, the large-scale features (SMA20, SMA50) dominate just because they're bigger numbers, not because they're more important. Normalization levels the playing field.

---

**The method — Z-score (Standardization)**

For every input variable, subtract its mean and divide by its standard deviation:
```
normalized = (value - mean) / std_dev
```

This forces every variable to have mean=0 and std=1, so they're all on the same scale regardless of original units.

---

**What the means actually are**

We store one mean per input variable:
```
featureMeans[0] = average SMA20 across all training days  (e.g. 150.5)
featureMeans[1] = average SMA50 across all training days  (e.g. 148.9)
featureMeans[2] = average RSI   across all training days  (e.g. 50.3)
featureMeans[3] = average MACD  across all training days  (e.g. 0.7)
featureMeans[4] = average Momentum across all training days (e.g. 1.8)
```

So `featureMeans[2]` is literally — if you took every RSI value from every training day and averaged them, that number. Same idea for the others.

The target (closing price) gets its own `targetMean` and `targetStd` for the same reason.

---

**Why means and stds are stored as member variables**

Because at prediction time you must use the exact same normalization parameters from training. The model learned its weights assuming that scale. If you recomputed the mean/std on new data, the scale would shift and the weights would give wrong answers. The model and its normalization parameters are inseparable.

---

**Denormalization at prediction time**

After the model outputs a normalized prediction, you reverse the process to get back to real price:
```
predicted_price = (normalized_prediction × targetStd) + targetMean
```

---

**Full flow in one picture:**
```
Raw indicators → subtract mean, divide by std → normalized X matrix
Raw prices     → subtract mean, divide by std → normalized Y vector
                          ↓
                 OLS solves for weights W
                          ↓
New day indicators → normalize with SAME mean/std → dot product with W → normalized prediction
                          ↓
                 × targetStd + targetMean → actual price
```
}








## some famous Stock names
    Tech / Internet
1. Apple — AAPL
2. Microsoft — MSFT
3. Alphabet (Google) — GOOGL
4. Amazon — AMZN
5. Meta Platforms — META
6. Tesla — TSLA
7. NVIDIA — NVDA
8. Netflix — NFLX
9. Adobe — ADBE
10. Intel — INTC
11. AMD — AMD
12. IBM — IBM
13. Oracle — ORCL
14. Salesforce — CRM
15. Cisco — CSCO
16. Qualcomm — QCOM
17. PayPal — PYPL
18. Uber — UBER
19. Airbnb — ABNB
20. Spotify — SPOT

Finance
21. JPMorgan Chase — JPM
22. Bank of America — BAC
23. Goldman Sachs — GS
24. Morgan Stanley — MS
25. Visa — V
26. Mastercard — MA
27. American Express — AXP
28. BlackRock — BLK
29. Citigroup — C
30. Wells Fargo — WFC

Consumer & Retail
31. Coca-Cola — KO
32. PepsiCo — PEP
33. McDonald’s — MCD
34. Starbucks — SBUX
35. Nike — NKE
36. Walmart — WMT
37. Costco — COST
38. Target — TGT
39. Home Depot — HD
40. Lowe’s — LOW

Automotive & Transport
41. Toyota — TM
42. Ford — F
43. General Motors — GM
44. Honda — HMC
45. BMW — BMWYY
46. Mercedes-Benz — MBGYY
47. Boeing — BA
48. Airbus — EADSY
49. Delta Airlines — DAL
50. FedEx — FDX

Energy
51. Exxon Mobil — XOM
52. Chevron — CVX
53. Shell — SHEL
54. BP — BP
55. TotalEnergies — TTE

Healthcare & Pharma
56. Johnson & Johnson — JNJ
57. Pfizer — PFE
58. Moderna — MRNA
59. AstraZeneca — AZN
60. Roche — RHHBY
61. Novartis — NVS
62. Merck — MRK
63. AbbVie — ABBV

Semiconductors / Hardware
64. TSMC — TSM
65. ASML — ASML
66. Broadcom — AVGO
67. Texas Instruments — TXN
68. Micron — MU

Media & Entertainment
69. Disney — DIS
70. Warner Bros Discovery — WBD
71. Comcast — CMCSA
72. Sony — SONY
73. Paramount — PARA

Industrial / Conglomerates
74. General Electric — GE
75. Siemens — SIEGY
76. Honeywell — HON
77. Caterpillar — CAT
78. 3M — MMM

Luxury / Fashion
79. LVMH — LVMUY
80. Gucci (Kering) — PPRUY
81. Adidas — ADDYY
82. Puma — PMMAF

Food & FMCG
83. Nestlé — NSRGY
84. Unilever — UL
85. Procter & Gamble — PG
86. Colgate-Palmolive — CL

Telecom
87. AT&T — T
88. Verizon — VZ
89. T-Mobile — TMUS

Other Giants
90. Berkshire Hathaway — BRK.B
91. Tencent — TCEHY
92. Alibaba — BABA
93. Samsung — SSNLF
94. SAP — SAP
95. Zoom — ZM
96. Square (Block) — SQ
97. Shopify — SHOP
98. Booking Holdings — BKNG
99. eBay — EBAY
100. ZoomInfo — ZI
##

