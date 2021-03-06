//
//  BSB.cpp
//  uncertainVolatilityModel
//
//  Created by Nick Kostoulas on 02/04/16.
//  Copyright © 2016 Nick Kostoulas. All rights reserved.
//

#include "BSB.hpp"

BSB::BSB(int n_, double dt_, double smax, double smin, double r_, double** F_):n(n_),dt(dt_),sigmaMax(smax),sigmaMin(smin),r(r_),F(F_){
    
}

BSB::~BSB(){
}
//Calculate supremum for given probability range
double BSB::supremum(double U, double M, double D) const{
    double maxVal = 0, currVal = 0, p = 0;
    int maxind = 0;
    
    for(int i=0; i<=MRT_STEPS; i++){
        p = lowerP + i*step;
        currVal = probU(p)*U + probM(p)*M + probD(p)*D;
        if(currVal>maxVal){
            maxVal = currVal;
            maxind = i;
        }
    }
    return maxVal;
}
//Calculate infinum for given probability range
double BSB::infinum(double U, double M, double D) const{
    double minVal = 1e6, currVal = 0, p = 0;
    int minind = 0;
    
    for(int i=0; i<=MRT_STEPS; i++){
        p = lowerP + i*step;
        currVal = probU(p)*U + probM(p)*M + probD(p)*D;
        if(currVal<minVal){
            minVal = currVal;
            minind = i;
        }
    }
    return minVal;
}
//Calculate BSB upper bound based on a trinomial tree of cashflow dynamics
double BSB::upperBound() const{
    double Wp[2*n+1][n+1];
    
    for (int i=n; i>=0; i--){
        for (int j=0; j<2*i+1; j++){
            if (i==n){
                Wp[j][i] = F[i][j];
            }else{
                Wp[j][i] = F[i][j] + exp(-r*dt)*supremum(Wp[j+2][i+1],Wp[j+1][i+1],Wp[j][i+1]);
            }
        }
    }
    return Wp[0][0];
}
//Calculate BSB lower bound based on a trinomial tree of cashflow dynamics
double BSB::lowerBound() const{
    double Wm[2*n+1][n+1];
    
    for (int i=n; i>=0; i--){
        for (int j=0; j<2*i+1; j++){
            if (i==n){
                Wm[j][i] = F[i][j];
            }else{
                Wm[j][i] = F[i][j] + exp(-r*dt)*infinum(Wm[j+2][i+1],Wm[j+1][i+1],Wm[j][i+1]);
            }
        }
    }
    return Wm[0][0];
}
