#include "interface/Table.h"

using namespace std;
using namespace theDoctor;

Table::Table()
{
}

Table::Table(vector<string> colTags_, vector<string> rowTags_, vector<string> colLabels_, vector<string> rowLabels_)
{
    Init(colTags_,rowTags_);
    SetLabels(colLabels_,rowLabels_);
}

Table::Table(vector<string> colTags_, vector<string> rowTags_)
{
    Init(colTags_,rowTags_);
    SetLabels(colTags_,rowTags_);
}
 
void Table::SetLabels(vector<string> colLabels_, vector<string> rowLabels_)
{
	if (colTags.size() != colLabels_.size()) return;
	if (rowTags.size() != rowLabels_.size()) return;

	colLabels = colLabels_;
	rowLabels = rowLabels_;
}

void Table::Init(vector<string> colTags_, vector<string> rowTags_)
{
	colTags  = colTags_;
	rowTags  = rowTags_;

	nCol = (int) colTags.size();
	nRow = (int) rowTags.size();
	for (int i = 0 ; i < nCol ; i++)
    {
        vector<Figure> newVec;
        data.push_back(newVec);
	    for (int j = 0 ; j < nRow ; j++)
		    data[i].push_back(Figure(0.0,0.0));
    }
}

Table:: ~Table() 
{ 
}

bool Table::Set(int colId, int rowId, Figure value)
{
	if ((colId >= nCol) || (rowId >= nRow))	return false;
	if ((colId < 0)     || (rowId < 0))	    return false;

	data[colId][rowId] = value;
	return true;
}

bool Table::Set(string colTag, string rowTag, Figure value)
{

	int indexRow = -1;
	int indexCol = -1;

	for (int i = 0 ; i < nRow ; i++) if (rowTag == rowTags[i]) indexRow = i;
	for (int i = 0 ; i < nCol ; i++) if (colTag == colTags[i]) indexCol = i;

	return Set(indexCol,indexRow,value);
}

Figure Table::Get(int colId, int rowId)
{
	if ((colId >= nCol) || (rowId >= nRow))	return -1;
	if ((colId < 0)     || (rowId < 0))	    return -1;

	return data[colId][rowId];
}

Figure Table::Get(string colTag, string rowTag)
{

	int indexRow = -1;
	int indexCol = -1;

	for (int i = 0 ; i < nRow ; i++) if (rowTag == rowTags[i]) indexRow = i;
	for (int i = 0 ; i < nCol ; i++) if (colTag == colTags[i]) indexCol = i;

	return Get(indexCol,indexRow);
}

void Table::PrintTable(string options)
{
	int width = 20;

	cout << left;

	// Line before header
	cout << " +";
	for (int i = 0 ; i < (width+3)*(nCol+1)-1 ; i++) cout << "-";
	cout << "+" << endl;

	// Header
	cout << " | ";
	cout << setw(width) << " ";
	cout << " | ";
	for (int i = 0 ; i < nCol ; i++)
	{
		cout << setw(width) << colTags[i] << " |";
		if (i < nCol - 1) cout << " ";
	}
	cout << endl;

	// Line after header
	cout << " +";
	for (int i = 0 ; i < (width+3)*(nCol+1)-1 ; i++) cout << "-";
	cout << "+" << endl;

	// Rows
	for (int i = 0 ; i < nRow ; i++)
	{
		cout << " | ";
		cout << setw(width) << rowTags[i];
		cout << " | ";
		for (int j = 0 ; j < nCol ; j++)
		{
			cout << setw(width) << Get(j,i).Print();
            
            //if (options == "withError")
			//    cout << " +/- " << setw(width) << Get(j,i).error();
                
            cout << " |";
			if (j < nCol - 1) cout << " ";
		}
		cout << endl;
	}

	// Line after rows
	cout << " +";
	for (int i = 0 ; i < (width+3)*(nCol+1)-1 ; i++) cout << "-";
	cout << "+" << endl;

}

void Table::PrintTableLatex(string options)
{
	cout << left;

    // Begin tabular
    cout << "\\begin{tabular}{|l|";
	for (int i = 0 ; i < nCol ; i++) cout << "c";
    cout << "|}" << endl;

    // Line before header
	cout << "\\hline" << endl;

	// Header
	cout << "&" << endl;
	for (int i = 0 ; i < nCol ; i++)
	{
		cout << "\\textbf{" << colLabels[i] << "} ";
		if (i < nCol - 1) cout << "\t&";
        else cout << "\t\\\\";
	    cout << endl;
	}
	
    // Line after header
	cout << "\\hline" << endl;

	// Rows
	for (int i = 0 ; i < nRow ; i++)
	{
		cout << "\\textbf{" << rowLabels[i] << "} ";
		cout << "\t & ";
		for (int j = 0 ; j < nCol ; j++)
		{
			cout << Get(j,i).PrintLatex() << " ";
			if (j < nCol - 1) cout << "\t & ";
            else cout << "\t \\\\";
		}
		cout << endl;
	}
	
    // Line after content
    cout << "\\hline" << endl;
    
    // End tabular
    cout << "\\end{tabular}";

}

