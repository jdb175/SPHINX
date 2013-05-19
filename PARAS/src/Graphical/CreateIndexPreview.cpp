#include "CreateIndexPreview.h"
#include <QTableWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <QHeaderView>
#include <QStringList>
#include <sstream>


using namespace std;

CreateIndexPreview::CreateIndexPreview(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Imported Data Preview"));
    setWindowModality(Qt::NonModal);


    previewTable = new QTableWidget(10, 10, this);
    previewTable->verticalHeader()->hide();
    previewTable->setSelectionMode(QAbstractItemView::NoSelection);

    nothingLabel = new QLabel(this);
    nothingLabel->setText("No Data Found");
    nothingLabel->setStyleSheet("QLabel { margin: 70px }");
    nothingLabel->setAlignment(Qt::AlignHCenter);
    QFont f;
    f.setBold(true);
    f.setPointSize(12);
    nothingLabel->setFont(f);


    layout = new QHBoxLayout;
    layout->addWidget(previewTable);
    layout->addWidget(nothingLabel);


    this->setLayout(layout);
}

void CreateIndexPreview::updatePreview(string filePath, string delimiter, unsigned long precision, bool noQuotes) {
    string line; //the currently read in line
    QStringList headers;
    int numColumns = 0; //the number of columns in the data

    //Open stream
    ifstream cvsFile;
    cvsFile.open(filePath.c_str());

    //get column headers
    getline(cvsFile, line);
    char *cstr;
    char *pch;
    cstr = new char [line.length()+1];
    strcpy (cstr, line.c_str());
    pch = strtok (cstr,delimiter.c_str());

    while (pch != NULL) {

        //pch is the header
        string value(pch);

        //clip quotes
        if(noQuotes) {
            value.erase(
            remove( value.begin(), value.end(), '\"' ),
            value.end()
            );
        }

        //trim whitespace and enclosing quotes
        size_t startpos = value.find_first_not_of(" \t");
        if( string::npos != startpos )
        {
            value = value.substr( startpos );
        }

        size_t endpos = value.find_last_not_of(" \t");
        if( string::npos != endpos )
        {
            value = value.substr( 0, endpos+1 );
        }

        headers << value.c_str();
        numColumns++;
        pch = strtok (NULL, delimiter.c_str());
    }

    if(numColumns == 0){
        previewTable->setVisible(false);
        nothingLabel->setVisible(true);
        return;
    } else {
        previewTable->setVisible(true);
        nothingLabel->setVisible(false);
    }

    previewTable->setColumnCount(numColumns);
    previewTable->setHorizontalHeaderLabels(headers);

    //now fill in the data
    //Iterate through each line of the infile
    int i = 0;
    while(getline(cvsFile, line) && i < 10){
        //Tokenize it by delimiter
        cstr = new char [line.length()+1];
        strcpy (cstr, line.c_str());

        pch = strtok (cstr,delimiter.c_str());
        int j = 0;

        while (pch != NULL) {
            //and add it to the table
            string value (pch);

            //clip quotes
            if(noQuotes) {
                value.erase(
                remove( value.begin(), value.end(), '\"' ),
                value.end()
                );
            }

            //trim whitespace and enclosing quotes
            size_t startpos = value.find_first_not_of(" \t");
            if( string::npos != startpos )
            {
                value = value.substr( startpos );
            }

            size_t endpos = value.find_last_not_of(" \t");
            if( string::npos != endpos )
            {
                value = value.substr( 0, endpos+1 );
            }

            //Now we handle discretization based on precision level
            double numValue = atof(pch);
            if(numValue != 0.0){
                numValue = floor(numValue/precision +0.5)*precision;
                stringstream convert;
                convert << numValue;
                value = convert.str();
            }
            //output the item
            QTableWidgetItem *item = new QTableWidgetItem(value.c_str());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            previewTable->setItem(i, j, item);
            pch = strtok (NULL, delimiter.c_str());
            j++;
        }
        i++;
    }
    previewTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    resize(600,366);
}
