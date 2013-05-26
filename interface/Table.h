#ifndef Table_h
#define Table_h

#include <iostream>
#include <vector>
#include <iomanip>

#include "interface/Figure.h"

using namespace std;

namespace theDoctor 
{

    class Table 
    {
      
     public:
      
      Table();
      Table(vector<string> colNames_, vector<string> rowNames_, vector<string> colLabels_, vector<string> rowLabels_);
      Table(vector<string> colNames_, vector<string> rowNames_);
      
      ~Table();

      bool SetColLabel(int colId, string label);
      bool SetColLabel(string colName, string label);

      bool SetRowLabel(int rowId, string label);
      bool SetRowLabel(string rowName, string label);

    //  bool Fill(int colId, int rowId, float value);  
    //  bool Fill(string colName, string rowName, float value);  
      
      bool Set(int colId, int rowId, Figure value);  
      bool Set(string colName, string rowName, Figure value);  
     
      Figure Get(int colId, int rowId);  
      Figure Get(string colName, string rowName);  

    //  bool MultiplyRow(string rowLabel,float factor);
     
      void PrintTable(string options = "");
      void PrintTableLatex(string options = "");
      
     private:

      // Data management
      vector<string> colNames;
      vector<string> rowNames;
      vector<string> colLabels;
      vector<string> rowLabels;

      int nCol;
      int nRow;
      vector< vector<Figure> > data;
      
    };

}

#endif
