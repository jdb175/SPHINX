#ifndef CREATEINDEXPREVIEW_H
#define CREATEINDEXPREVIEW_H

#include <QDialog>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <string>

using namespace std;

class CreateIndexPreview : public QDialog
{
    Q_OBJECT
public:
    explicit CreateIndexPreview(QWidget *parent = 0);
    void updatePreview(string filePath, string delimiter, unsigned long precision, bool noQuotes);

private:
    QTableWidget *previewTable;
    QLabel *nothingLabel;
    QHBoxLayout *layout;
};

#endif // CREATEINDEXPREVIEW_H
