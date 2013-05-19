#ifndef CREATEINDEXDIALOG_H
#define CREATEINDEXDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QGroupBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QCheckBox>

#include "paras/PARAS.hpp"
#include "CreateIndexPreview.h"

using namespace PARASProgram;

class CreateIndexDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateIndexDialog(QWidget *parent = 0, PARAS *pInstance = NULL);
    string *GetFileName();

private:
    QLineEdit *iFile;
    QPushButton *iBtn;
    QGroupBox *iGroupBox;

    QLineEdit *oFile;    
    QPushButton *oBtn;
    QGroupBox *oGroupBox;

    QLineEdit *minSup;
    QLineEdit *minConf;
    QLineEdit *threshold;
    QLineEdit *precision;
    QLineEdit *delimiter;
    QCheckBox *compressed;
    QCheckBox *stripQuotes;
    QPushButton *save;
    QPushButton *preview;
    QPushButton *cancel;

    QGroupBox *buttonGroupBox;
    QGroupBox *aprioriGroupBox;
    QButtonGroup *saveCancel;

    string outFile;

    CreateIndexPreview *previewWindow;
    PARAS *pInstance;

private slots:
    void CreateIndex();
    void GetInputFile();
    void GetOutputFile();
    void ShowPreview();
    
};

#endif // CREATEINDEXDIALOG_H
