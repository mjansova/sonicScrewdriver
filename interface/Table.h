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
      Table(vector<string> colTags_, vector<string> rowTags_, vector<string> colLabels_, vector<string> rowLabels_);
      Table(vector<string> colTags_, vector<string> rowTags_);

      void SetLabels(vector<string> colLabels_, vector<string> rowLabels_);
      void Init(vector<string> colTags_, vector<string> rowTags_);
      
      ~Table();

      bool Set(int colId, int rowId, Figure value);  
      bool Set(string colTag, string rowTag, Figure value);  
     
      Figure Get(int colId, int rowId);  
      Figure Get(string colTag, string rowTag);  

      void PrintTable(string options = "");
      void PrintTableLatex(string options = "");
      
     private:

      // Data management
      vector<string> colTags;
      vector<string> rowTags;
      vector<string> colLabels;
      vector<string> rowLabels;

      int nCol;
      int nRow;
      vector< vector<Figure> > data;
      
    };

}

#endif
