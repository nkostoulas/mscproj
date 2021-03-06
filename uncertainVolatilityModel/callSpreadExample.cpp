//
//  callSpreadExample.cpp
//  uncertainVolatilityModel
//
//  Created by Nick Kostoulas on 02/04/16.
//  Copyright © 2016 Nick Kostoulas. All rights reserved.
//
#include "callSpreadExample.hpp"

void callSpreadExample(){
    ofstream bsbAsk("./data/bsbAskCall.txt");
    ofstream bsbBid("./data/bsbBidCall.txt");
    ofstream bsAsk("./data/bsAskCall.txt");
    ofstream bsBid("./data/bsBidCall.txt");
    ofstream bsMidl("./data/bsMidCall.txt");
    ofstream bsLow("./data/bsLowCall.txt");
    ofstream bsHigh("./data/bsHighCall.txt");
    ofstream prices("./data/pricesCall.txt");
    
    cout<<"This program compares BS and BSB on a Bull Call Spread of 2 options. \n";
    cout<<"Price\tBSUpper\tBSMid\tBSLower\tBSBLower\tBSBUpper\n";

    //*********** Basic Definitions *************************//
    double r = 0.05;    //risk free interest rate
    double S = 85;      //current underlying asset price
    double smax = 0.4;  //maximum volatility
    double smin = 0.1;  //minimum volatility
    double smid = (smax+smin)/2;
    
    double bsUpper = 0.0, bsMid=0.0, bsLower=0.0, bsbUpper=0.0, bsbLower=0.0;
    
    double timeToExpiry = 0.5;  // time to expiry of call spread
    double buyStrike = 90;      // strike price of call option bought
    double sellStrike = 100;    // strike price of call option sold
    
    double T = 1; //years for trinomial tree
    double N = 252; //periods per year for trinomial tree
    
    int n = N*timeToExpiry; //number of periods
    double dt = T/N;        //percentage of year for each period
    double timeToEval = 0.0;//time at which black scholes is evaluated
    
    double** F = new double*[n+1];
    double price = 0;
    for(int i = 0; i < n+1; ++i)
        F[i] = new double[2*i+1];
    for (int i=n; i>=0; i--){
        for (int j=0; j<2*i+1; j++){
            F[i][j] = 0;
        }
    }
    
    array<int,40> smatr= {0, 10, 20, 30, 40, 50, 55, 60, 65, 70, 72, 75, 78, 80, 82, 85,
                    88, 90, 92, 95, 98, 100, 105, 110, 115, 120, 125, 130, 135, 140,
                    145, 150, 155, 160, 165, 170, 175, 180, 190, 200 };
    
    // Loop for all initial prices of underlying in smatr
    for (int sind=0; sind<smatr.size(); sind++){
        
        S = smatr[sind];

        //********************  BSB Spread pricing *************//
        
        TrinomialTree tree(T, N, smax, smin, r, S);
        //tree.printTree();
        
        //***************** Call Spread Payoff Structure ******//
        for (int j=0; j<2*n+1; j++){
            price = tree.nodePrice(n, j-n);
            F[n][j] = max(price - buyStrike, 0.0) - max(price - sellStrike, 0.0);
        }
        //****************** BSB spread pricing ********//
        BSB bsb(n, dt, smax, smin, r, F);
        bsbUpper = bsb.upperBound();
        bsbLower = bsb.lowerBound();
        
        //****************** Black Scholes spread pricing ********//

        BS upperBuy(buyStrike, S, timeToExpiry, timeToEval, r, smax);   //ASK
        BS lowerBuy(buyStrike, S, timeToExpiry, timeToEval, r, smin);   //BID
        BS midBuy(buyStrike, S, timeToExpiry, timeToEval, r, smid);

        BS upperSell(sellStrike, S, timeToExpiry, timeToEval, r, smax); //BID
        BS lowerSell(sellStrike, S, timeToExpiry, timeToEval, r, smin); //ASK
        BS midSell(sellStrike, S, timeToExpiry, timeToEval, r, smid);
        
        bsUpper = upperBuy.callOptionPrice() - lowerSell.callOptionPrice();
        bsLower = lowerBuy.callOptionPrice() - upperSell.callOptionPrice();
        bsMid = midBuy.callOptionPrice() - midSell.callOptionPrice();
        
        prices << S << "\n";
        bsbAsk << bsbUpper << "\n";
        bsbBid << bsbLower << "\n";
        bsAsk << bsUpper << "\n";
        bsBid << bsLower << "\n";
        bsMidl << bsMid << "\n";
        bsLow << lowerBuy.callOptionPrice() - lowerSell.callOptionPrice()<< "\n";
        bsHigh << upperBuy.callOptionPrice() - upperSell.callOptionPrice() << "\n";
        
        cout<<S<<"\t"<<bsUpper<<"\t"<<bsMid<<"\t"<<bsLower<<"\t"<<bsbLower<<"\t"<<bsbUpper<<"\n";
        
    }
    for(int i = 0; i < n+1; ++i)
        delete [] F[i];
    delete [] F;
    
    bsbAsk.close();
    bsbBid.close();
    bsAsk.close();
    bsBid.close();
    bsMidl.close();
    bsLow.close();
    bsHigh.close();
    prices.close();
}