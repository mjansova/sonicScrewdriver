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

Table::Table(string inputFile)
{
    ifstream input(inputFile.c_str(),std::ifstream::in);

    string s;
    vector<string> colTags_;
    vector<string> rowTags_;
    vector<Figure> figures;

    input >> s;
    if (s[0] != '+') 
    { WARNING_MSG << "Problem reading file " << inputFile << ": expected to find '+', found '" << s[0] << "'" << endl; exit(-1); } 
    input >> s; 
    if (s    != "|") 
    { WARNING_MSG << "Problem reading file " << inputFile << ": expected to find '|', found '" << s << "'" << endl; exit(-1); } 

    while (true)
    {
        input >> s; 
        if (s != "|") 
        { WARNING_MSG << "Problem reading file " << inputFile << ": expected to find '|', found '" << s << "'" << endl; exit(-1); } 
        input >> s;
        if (s[0] == '+') break;
        else colTags_.push_back(s);
    }

    while (true)
    {
        input >> s; 
        if (s[0] == '+') break;

        input >> s; 
        rowTags_.push_back(s);

        for (unsigned int c = 0 ; c < colTags_.size() ; c++)
        {
            input >> s; 
            if (s != "|") 
            { WARNING_MSG << "Problem reading file " << inputFile << ": expected to find '|', found '" << s << "'" << endl; exit(-1); } 
           
            float value, error;
            input >> value >> s >> error;
            figures.push_back(Figure(value,error));
        }

        input >> s; 
        if (s != "|") 
        { WARNING_MSG << "Problem reading file " << inputFile << ": expected to find '|', found '" << s << "'" << endl; exit(-1); } 
    }

    Init(colTags_,rowTags_);
    for (unsigned int c = 0 ; c < colTags_.size() ; c++)
    for (unsigned int r = 0 ; r < rowTags_.size() ; r++)
    {
        Set(c,r,figures[c + r*colTags_.size()]);
    }
}

void Table::SetLabels(vector<string> colLabels_, vector<string> rowLabels_)
{
	if (colTags.size() != colLabels_.size()) return;
	if (rowTags.size() != rowLabels_.size()) return;

	colLabels = colLabels_;
	rowLabels = rowLabels_;
}

void Table::Init(vector<string> colTags_, vector<string> rowTags_, bool keepNegative)
{
        negativeValues = keepNegative;
	colTags  = colTags_;
	rowTags  = rowTags_;

        SetLabels(colTags,rowTags); //@MJ@ TODO for the case of resaving latex tab  

	nCol = (unsigned int) colTags.size();
	nRow = (unsigned int) rowTags.size();
	for (unsigned int i = 0 ; i < nCol ; i++)
    {
        vector<Figure> newVec;
        data.push_back(newVec);
	    for (unsigned int j = 0 ; j < nRow ; j++)
		    data[i].push_back(Figure(0.0,0.0,0.0));
    }
}

Table:: ~Table() 
{ 
}

bool Table::Set(int colId, int rowId, Figure value)
{
	if ((((unsigned int) colId) >= nCol) || (((unsigned int) rowId) >= nRow))	return false;
	if ((colId < 0)     || (rowId < 0))	    return false;

        if(value.value()<0 && !negativeValues)
        {
             value = Figure(0,0,0);
        }
	data[colId][rowId] = value;
	return true;
}

bool Table::Set(string colTag, string rowTag, Figure value)
{

	int indexRow = -1;
	int indexCol = -1;

	for (unsigned int i = 0 ; i < nRow ; i++) if (rowTag == rowTags[i]) indexRow = i;
	for (unsigned int i = 0 ; i < nCol ; i++) if (colTag == colTags[i]) indexCol = i;

    if (indexRow == -1) { WARNING_MSG << "Could not find row named "    << rowTag << " to set " << endl; return false; }
    if (indexCol == -1) { WARNING_MSG << "Could not find column named " << colTag << " to set " << endl; return false; }

	return Set(indexCol,indexRow,value);
}

Figure Table::Get(int colId, int rowId)
{
	if ((((unsigned int) colId) >= nCol) || (((unsigned int) rowId) >= nRow))	return -1;
	if ((colId < 0)     || (rowId < 0))	    return -1;

	return data[colId][rowId];
}

Figure Table::Get(string colTag, string rowTag)
{

	int indexRow = -1;
	int indexCol = -1;

	for (unsigned int i = 0 ; i < nRow ; i++) 
        {
           //cout << "row " << i << " name " << rowTags[i] << endl; 
           if (rowTag == rowTags[i]) indexRow = i;
        }
	for (unsigned int i = 0 ; i < nCol ; i++)
        { 
            //cout << "col " << i << " name " << colTags[i] << endl; 
            if (colTag == colTags[i]) indexCol = i;
        }

    if (indexRow == -1) { WARNING_MSG << "Could not find row named "    << rowTag << " to get " << endl; return Figure(0,0); }
    if (indexCol == -1) { WARNING_MSG << "Could not find column named " << colTag << " to get " << endl; return Figure(0,0); }

	return Get(indexCol,indexRow);
}

void Table::Print(int prec, string options, std::ostream& output)
{
	int width = 16 + 2*prec;

    // Find max width for row tags
    unsigned int widthRowTags = 10;
    for (unsigned int i = 0 ; i < nRow ; i++)
    {
        if (widthRowTags < rowTags[i].size())
            widthRowTags = rowTags[i].size();
    }

	output << left;

	// Line before header
	output << " +";
	for (unsigned int i = 0 ; i < (width+3)*(nCol)+2+widthRowTags ; i++) output << "-";
	output << "+" << endl;

	// Header
	output << " | ";
	output << setw(widthRowTags) << " ";
	output << " | ";
	for (unsigned int i = 0 ; i < nCol ; i++)
	{
		output << setw(width) << colTags[i] << " |";
		if (i < nCol - 1) output << " ";
	}
	output << endl;

	// Line after header
	output << " +";
	for (unsigned int i = 0 ; i < (width+3)*(nCol)+2+widthRowTags ; i++) output << "-";
	output << "+" << endl;


	// Rows
	for (unsigned int i = 0 ; i < nRow ; i++)
	{
		output << " | ";
		output << setw(widthRowTags) << rowTags[i];
		output << " | ";
		for (unsigned int j = 0 ; j < nCol ; j++)
		{
			output << setw(width) << Get(j,i).Print(prec, options);
            
            output << " |";
			if (j < nCol - 1) output << " ";
		}
		output << endl;
	}

	// Line after rows
	output << " +";
	for (unsigned int i = 0 ; i < (width+3)*(nCol)+2+widthRowTags ; i++) output << "-";
	output << "+" << endl;
}

void Table::PrintLatex(int prec, string options, std::ostream& output)
{
	output << left;

    // Begin tabular
    output << "\\documentclass{article}" << endl;
    output << "\\usepackage{graphicx}" << endl;
    output << "\\usepackage{pdflscape}" << endl;
    output << "\\begin{document}" << endl;
    output << "\\begin{landscape}" << endl;

    output << "\\noindent\\hrulefill" << endl;
    output << "\\smallskip\\noindent" << endl;
    output << "\\resizebox{\\linewidth}{!}{%" << endl;
    output << "\\begin{tabular}{|l|";
	for (unsigned int i = 0 ; i < nCol ; i++) output << "c";
    output << "|}" << endl;

    // Line before header
	output << "\\hline" << endl;

	// Header
	output << "&" << endl;
	for (unsigned int i = 0 ; i < nCol ; i++)
	{
		output << "\\textbf{" << colLabels[i] << "} ";
		if (i < nCol - 1) output << "\t&";
        else output << "\t\\\\";
	    output << endl;
	}
	
    // Line after header
	output << "\\hline" << endl;

	// Rows
	for (unsigned int i = 0 ; i < nRow ; i++)
	{
		output << "\\textbf{" << rowLabels[i] << "} ";
		output << "\t & ";
		for (unsigned int j = 0 ; j < nCol ; j++)
		{
		    output << Get(j,i).PrintLatex(prec,options) << " ";

			if (j < nCol - 1) output << "\t & ";
            else output << "\t \\\\";
		}
		output << endl;
	}
	
    // Line after content
    output << "\\hline" << endl;
    
    // End tabular
    output << "\\end{tabular}}" << endl;
    output << "\\end{landscape}" << endl;
    output << "\\end{document}" << endl;


}

void Table::Print(string fileName, int prec, string options)
{
    ofstream output(fileName.c_str(), ofstream::out);
    Print(prec,options,output);
    output.close();
}
void Table::PrintLatex(string fileName, int prec, string options)
{
    ofstream output(fileName.c_str(), ofstream::out);
    PrintLatex(prec,options,output);
    output.close();
}
