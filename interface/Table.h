#ifndef Table_h
#define Table_h

#include <iostream>
#include <vector>
#include <iomanip>

#include "Figure.h"

using namespace std;

namespace theDoctor
{

    class Table
    {

     public:

      Table();
      Table(vector<string> colTags_, vector<string> rowTags_, vector<string> colLabels_, vector<string> rowLabels_);
      Table(vector<string> colTags_, vector<string> rowTags_);
      Table(string inputFile);

      void SetLabels(vector<string> colLabels_, vector<string> rowLabels_);
      void Init(vector<string> colTags_, vector<string> rowTags_, bool keepNegative = false);

      ~Table();

      bool Set(int colId, int rowId, Figure value);
      bool Set(string colTag, string rowTag, Figure value);

      Figure Get(int colId, int rowId);
      Figure Get(string colTag, string rowTag);

      void Print(int prec = 2, string options = "", std::ostream& output = cout);
      void PrintLatex(int prec = 2, string options = "", std::ostream& output = cout);

      void Print(string fileName, int prec = 2, string options = "");
      void PrintLatex(string fileName, int prec = 2, string options = "");

      public:

      // Data management
      vector<string> colTags;
      vector<string> rowTags;
      vector<string> colLabels;
      vector<string> rowLabels;

      protected:
      unsigned int nCol;
      unsigned int nRow;
      vector< vector<Figure> > data;
      bool negativeValues;
    };

}

#endif
