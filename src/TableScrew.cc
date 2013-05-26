#include "interface/Table.h"

using namespace std;
using namespace theDoctor;

Table::Table()
{
}

Table::Table(vector<string> colNames_, vector<string> rowNames_, vector<string> colLabels_, vector<string> rowLabels_)
{
	if (colNames_.size() != colLabels.size()) return;
	if (rowNames_.size() != rowLabels.size()) return;
	
	colNames  = colNames_;
	rowNames  = rowNames_;
	colLabels = colLabels_;
	rowLabels = rowLabels_;

	nCol = (int) colNames.size();
	nRow = (int) rowNames.size();


	for (int i = 0 ; i < nCol ; i++)
    {
        vector<Figure> newVec;
        data.push_back(newVec);
	    for (int j = 0 ; j < nRow ; j++)
		    data[i].push_back(Figure(0.0,0.0));
    }

}

Table::Table(vector<string> colNames_, vector<string> rowNames_)
{
	
	colNames  = colNames_;
	rowNames  = rowNames_;
	colLabels = colNames;
	rowLabels = rowNames;

	nCol = (int) colNames.size();
	nRow = (int) rowNames.size();
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

bool Table::SetColLabel(int colId, string label)
{
	if ((colId >= nCol) || (colId < 0)) return false;
	colLabels[colId] = label;
	return true;
}

bool Table::SetColLabel(string colName, string label)
{
	for (int i = 0 ; i < nCol ; i++)
		if (colName == colNames[i]) 
		{
			colLabels[i] = label;
			return true;
		}
	return false;
}

bool Table::SetRowLabel(int rowId, string label)
{
	if ((rowId >= nRow) || (rowId < 0)) return false;
	rowLabels[rowId] = label;
	return true;
}
bool Table::SetRowLabel(string rowName, string label)
{
	for (int i = 0 ; i < nCol ; i++)
		if (rowName == rowNames[i]) 
		{
			rowLabels[i] = label;
			return true;
		}
	return false;
}
/*
bool Table::Fill(int colId, int rowId, float value)
{
	if ((colId >= nCol) || (rowId >= nRow))	return false;
	if ((colId < 0)     || (rowId < 0))	    return false;

	data[colId][nCol] += value;
	return true;
}
bool Table::Fill(string colName, string rowName, float value)
{
	int indexRow = -1;
	int indexCol = -1;

	for (int i = 0 ; i < nRow ; i++) if (rowName == rowNames[i]) indexRow = i;
	for (int i = 0 ; i < nCol ; i++) if (colName == colNames[i]) indexCol = i;

	return Fill(indexCol,indexRow,value);
}*/

bool Table::Set(int colId, int rowId, Figure value)
{
	if ((colId >= nCol) || (rowId >= nRow))	return false;
	if ((colId < 0)     || (rowId < 0))	    return false;

	data[colId][rowId] = value;
	return true;
}

bool Table::Set(string colName, string rowName, Figure value)
{

	int indexRow = -1;
	int indexCol = -1;

	for (int i = 0 ; i < nRow ; i++) if (rowName == rowNames[i]) indexRow = i;
	for (int i = 0 ; i < nCol ; i++) if (colName == colNames[i]) indexCol = i;

	return Set(indexCol,indexRow,value);
}

Figure Table::Get(int colId, int rowId)
{
	if ((colId >= nCol) || (rowId >= nRow))	return -1;
	if ((colId < 0)     || (rowId < 0))	    return -1;

	return data[colId][rowId];
}

Figure Table::Get(string colName, string rowName)
{

	int indexRow = -1;
	int indexCol = -1;

	for (int i = 0 ; i < nRow ; i++) if (rowName == rowNames[i]) indexRow = i;
	for (int i = 0 ; i < nCol ; i++) if (colName == colNames[i]) indexCol = i;

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
		cout << setw(width) << colLabels[i] << " |";
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
		cout << setw(width) << rowLabels[i];
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
	int width = 15;

	cout << left;


	// Header
	cout << setw(width) << " ";
	cout << " & ";
	for (int i = 0 ; i < nCol ; i++)
	{
		cout << setw(width) << "\\textbf{" << colLabels[i] << "} ";
		if (i < nCol - 1) cout << "& ";
        else cout << "\\\\";
	}
	cout << endl;

	// Line after header
	cout << "\\hline" << endl;

	// Rows
	for (int i = 0 ; i < nRow ; i++)
	{
		cout << setw(width) << "\\textbf{" << rowLabels[i] << "} ";
		cout << " & ";
		for (int j = 0 ; j < nCol ; j++)
		{
			cout << setw(width) << Get(j,i).Print() << " ";
			if (j < nCol - 1) cout << "& ";
            else cout << "\\\\";
		}
		cout << endl;
	}

}

/*
bool Table::MultiplyRow(string rowLabel,float factor)
{
	int rowIndex = -1;

	for (int i = 0 ; i < nRow ; i++) 
		if (rowLabel == rowNames[i]) rowIndex = i;

	if (rowIndex == -1) return false;

	for (int i = 0 ; i < nCol ; i++)
		Set(i,rowIndex,Get(i,rowIndex) * factor);

	return true;
}
*/
