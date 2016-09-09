#ifndef __PRINT_MESSAGE__
#define __PRINT_MESSAGE__

// System includes

#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <time.h>
using namespace std;

// Root includes

#include <TFile.h>
#include <TMarker.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLorentzVector.h>
#include "TRandom.h"

void printProgressBar(int current, int max, string name = "Progress")
{
    std::string bar;
    int percent = 100 * (float) current / (float) max;

    for(int i = 0; i < 50; i++)
    {
        if( i < (percent/2))       bar.replace(i,1,"=");
        else if( i == (percent/2)) bar.replace(i,1,">");
        else                       bar.replace(i,1," ");
    }

    std::cout << "  [" << name << "]  \t";
    std::cout << "[" << bar << "] ";
    std::cout.width( 3 );
    std::cout << percent << "%     ";
    std::cout << "(" << current << " / " << max << ")" << "\r" << std::flush;
}

void printBoxedMessage(string message)
{
    cout << endl;

    cout << "   ┌──";
    for(unsigned int i = 0 ; i <= message.size() ; i++) cout << "─";
    cout << "─┐  " << endl;

    cout << "   │  " << message << "  │  " << endl;

    cout << "   └──";
    for(unsigned int i = 0 ; i <= message.size() ; i++) cout << "─";
    cout << "─┘  " << endl;

    cout << endl;

}

#endif
