/*
 * ColorSelector.cpp
 *
 *  Created on: Jun 12, 2009
 *  Author: Jason Stasik
 */

#include "graphical/color/ColorSelector.h"
#include "graphical/color/ColorManager.h"
#include "graphical/color/RGBt.h"
#include "graphical/color/ColorMacros.h"
#include "graphical/color/ColorMap.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QColorDialog>
#include <QPushButton>
#include <QColor>
#include <QButtonGroup>
#include <QColor>
#include <QSpinBox>
#include <QCheckBox>
#include <QPixmap>
#include <QCloseEvent>


ColorSelector::ColorSelector(QWidget *main, ColorManager* manage, ColorMap* activeMapName) : QDialog(main) {
	setWindowTitle(tr("Color Selection"));
    setWindowModality(Qt::WindowModal);

	int index;

	// A ButtonGroup is a container used to make sure only 1 radio button is selected at a time
	// A GroupBox is merely a container used to graphically link related radio buttons
	QButtonGroup *buttongroup = new QButtonGroup(this);

    cm = manage;
    this->activeColorMap = activeMapName;

	m_map = new QSignalMapper(this);

	QVBoxLayout *layout = new QVBoxLayout();

    QGroupBox *seq, *div, *qual;

	m_selected = -1;


    //---------------------------------------------------------------
    seq = new QGroupBox("Sequential Ramps");

    QGridLayout *seqGrid = new QGridLayout();
    seqGrid->setColumnMinimumWidth(2, 20);

    seq_labels.resize(cm->getPredefinedTotal(ColorManager::SEQUENTIAL));

    index = 0;
    for(int i = cm->getPredefinedStart(ColorManager::SEQUENTIAL); i < cm->getPredefinedEnd(ColorManager::SEQUENTIAL) - 1; i++, index++) {
        QRadioButton *button = new QRadioButton();
        buttongroup->addButton(button);
        radioButtonAr[i] = button;
        connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
        m_map->setMapping(button, i);
        seq_labels[index] = new QLabel();
        seq_labels[index]->setFixedSize(120,15);
        if(i == (activeColorMap)->getRampID()) button->click();
        QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

        if(index % 2 == 0) {
            seqGrid->addWidget(label, index, 1);
            seqGrid->addWidget(button, index+1, 0);
            seqGrid->addWidget(seq_labels[index], index+1, 1);
        } else {
            seqGrid->addWidget(label, index-1, 4);
            seqGrid->addWidget(button, index, 3);
            seqGrid->addWidget(seq_labels[index], index, 4);
        }
    }

    seq->setLayout(seqGrid);
    layout->addWidget(seq);

    //---------------------------------------------------------------

	//---------------------------------------------------------------
    div = new QGroupBox("Diverging Ramps");

    QGridLayout *divGrid = new QGridLayout();
    divGrid->setColumnMinimumWidth(2, 20);

    div_labels.resize(cm->getPredefinedTotal(ColorManager::DIVERGING));

    index = 0;
    for(int i = cm->getPredefinedStart(ColorManager::DIVERGING); i < cm->getPredefinedEnd(ColorManager::DIVERGING); i++, index++) {
        QRadioButton *button = new QRadioButton();
        buttongroup->addButton(button);
        radioButtonAr[i] = button;
        connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
        m_map->setMapping(button, i);
        div_labels[index] = new QLabel();
        div_labels[index]->setFixedSize(120,15);
        if(i == (activeColorMap)->getRampID()) button->click();

        QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

        if(index % 2 == 0) {
            divGrid->addWidget(label, index, 1);
            divGrid->addWidget(button, index+1, 0);
            divGrid->addWidget(div_labels[index], index+1, 1);
        } else {
            divGrid->addWidget(label, index-1, 4);
            divGrid->addWidget(button, index, 3);
            divGrid->addWidget(div_labels[index], index, 4);
        }
    }

    div->setLayout(divGrid);
    layout->addWidget(div);

	//---------------------------------------------------------------

	//---------------------------------------------------------------
    qual = new QGroupBox("Qualitative Ramps");

    QGridLayout *qualGrid = new QGridLayout();
    qualGrid->setColumnMinimumWidth(2, 20);

    qual_labels.resize(cm->getPredefinedTotal(ColorManager::QUALITATIVE));

    index = 0;
    for(int i = cm->getPredefinedStart(ColorManager::QUALITATIVE); i < cm->getPredefinedEnd(ColorManager::QUALITATIVE) - 1; i++, index++) {
        QRadioButton *button = new QRadioButton();
        buttongroup->addButton(button);
        radioButtonAr[i] = button;
        connect(button, SIGNAL(clicked()), m_map, SLOT(map()));
        m_map->setMapping(button, i);
        qual_labels[index] = new QLabel();
        qual_labels[index]->setFixedSize(120,15);
        if(i == (activeColorMap)->getRampID()) button->click();

        QLabel *label = new QLabel(cm->getPredefinedColorRampName(i));

        if(index % 2 == 0) {
            qualGrid->addWidget(label, index, 1);
            qualGrid->addWidget(button, index+1, 0);
            qualGrid->addWidget(qual_labels[index], index+1, 1);
        } else {
            qualGrid->addWidget(label, index-1, 4);
            qualGrid->addWidget(button, index, 3);
            qualGrid->addWidget(qual_labels[index], index, 4);
        }
    }

    qual->setLayout(qualGrid);
    layout->addWidget(qual);

    QSignalMapper *glyphSignalMapper = new QSignalMapper(this);
    QGroupBox *glyphBox = new QGroupBox("Glyph Coloring ");
    QLabel *anteLabel = new QLabel("Antecedent: ");
    anteColorButton = new QPushButton(this);
    anteColorButton->setFlat(true);
    anteColorButton->setAutoFillBackground(true);
    anteColorButton->setFixedSize(60,15);
    glyphSignalMapper->setMapping(anteColorButton, 1);
    anteColorButton->connect(anteColorButton, SIGNAL(clicked()), glyphSignalMapper, SLOT(map()));

    QLabel *consLabel = new QLabel("Consequent: ");
    conseColorButton = new QPushButton(this);
    conseColorButton->setFlat(true);
    conseColorButton->setAutoFillBackground(true);
    conseColorButton->setFixedSize(60,15);
    glyphSignalMapper->setMapping(conseColorButton, 2);
    conseColorButton->connect(conseColorButton, SIGNAL(clicked()), glyphSignalMapper, SLOT(map()));

    QLabel *connectLabel = new QLabel("Connections: ");
    connectColorButton = new QPushButton(this);
    connectColorButton->setFlat(true);
    connectColorButton->setAutoFillBackground(true);
    connectColorButton->setFixedSize(60,15);
    glyphSignalMapper->setMapping(connectColorButton, 3);
    connectColorButton->connect(connectColorButton, SIGNAL(clicked()), glyphSignalMapper, SLOT(map()));

    QLabel *missingLabel = new QLabel("Missing: ");
    missingColorButton = new QPushButton(this);
    missingColorButton->setFlat(true);
    missingColorButton->setAutoFillBackground(true);
    missingColorButton->setFixedSize(60,15);
    glyphSignalMapper->setMapping(missingColorButton, 4);
    missingColorButton->connect(missingColorButton, SIGNAL(clicked()), glyphSignalMapper, SLOT(map()));

    glyphSignalMapper->connect(glyphSignalMapper, SIGNAL(mapped(int)), this, SLOT(showColorChooser(int)));

    QGridLayout *glyphColors = new QGridLayout();
    glyphColors->addWidget(anteLabel,0,0);
    glyphColors->addWidget(anteColorButton,0,1);
    glyphColors->addWidget(consLabel,0,2);
    glyphColors->addWidget(conseColorButton,0,3);
    glyphColors->addWidget(connectLabel,1,0);
    glyphColors->addWidget(connectColorButton,1,1);
    glyphColors->addWidget(missingLabel,1,2);
    glyphColors->addWidget(missingColorButton,1,3);
    glyphBox->setLayout(glyphColors);
    layout->addWidget(glyphBox);

	QPushButton *ok = new QPushButton("OK");
	connect(ok, SIGNAL(clicked()), this, SLOT(apply()));
	QPushButton *cancel = new QPushButton("Cancel");
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));

	QHBoxLayout *buttons = new QHBoxLayout();
	buttons->addWidget(ok);
	buttons->addWidget(cancel);
	layout->addLayout(buttons);

	setLayout(layout);

	drawColorBars();

	connect(m_map, SIGNAL(mapped(int)), this, SLOT(setSelectedRadio(int)));

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

}

ColorSelector::~ColorSelector() {
}

void ColorSelector::drawColorBars() {
    int index, i;

    //sequential
    index = 0;
    for(i = cm->getPredefinedStart(ColorManager::SEQUENTIAL); i < cm->getPredefinedEnd(ColorManager::SEQUENTIAL) - 1; i++, index++) {
        ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
        QPixmap p = c->drawColorBar2(120, 20);
        seq_labels[index]->setPixmap(p);
        delete c;
    }

    //divergent
    index = 0;
    for(i = cm->getPredefinedStart(ColorManager::DIVERGING); i < cm->getPredefinedEnd(ColorManager::DIVERGING); i++, index++) {
        ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
        QPixmap p = c->drawColorBar2(120, 20);
        div_labels[index]->setPixmap(p);
        delete c;
    }

    //qualitative
    index = 0;
    for(i = cm->getPredefinedStart(ColorManager::QUALITATIVE); i < cm->getPredefinedEnd(ColorManager::QUALITATIVE) - 1; i++, index++) {
        ColorMap *c = new ColorMap(cm->getPredefinedColorRamp(i));
        QPixmap p = c->drawColorBar2(120, 20);
        qual_labels[index]->setPixmap(p);
        delete c;
    }
}

void ColorSelector::setSelectedRadio(int button) {
	m_selected = button;
}

void ColorSelector::apply() {
    ColorRamp *c = cm->getPredefinedColorRamp(m_selected);
    activeColorMap->setColorRamp(c);
    activeColorMap->glyphAntecedentColor = anteColor;
    activeColorMap->glyphConsequentColor = conseColor;
    activeColorMap->glyphConnectColor = connectColor;
    activeColorMap->glyphMissingColor = missingColor;
    close();
}

void ColorSelector::cancel()
{
	close();
}

void ColorSelector::closeEvent(QCloseEvent *event)
{
	event->accept();
}

void ColorSelector::displayColorSelector(ColorMap* colorMap)
{
    this->activeColorMap = colorMap;
    radioButtonAr[(activeColorMap)->getRampID()]->click();

    anteColor = activeColorMap->glyphAntecedentColor;
    conseColor = activeColorMap->glyphConsequentColor;
    connectColor = activeColorMap->glyphConnectColor;
    missingColor = activeColorMap->glyphMissingColor;


    this->updateGlyphColors();

    this->show();
}

void ColorSelector::updateGlyphColors()
{
    QPalette p;
    p.setColor(QPalette::Button, anteColor);
    anteColorButton->setPalette(p);
    p.setColor(QPalette::Button, conseColor);
    conseColorButton->setPalette(p);
    p.setColor(QPalette::Button, connectColor);
    connectColorButton->setPalette(p);
    p.setColor(QPalette::Button, missingColor);
    missingColorButton->setPalette(p);
}

void ColorSelector::showColorChooser(int signal)
{
    QColor *initColor;
    QString colorTitle;

    switch(signal)
    {
        case 1:
        {
            initColor = &anteColor;
            colorTitle = QString("Choose Antecedent Color");
            break;
        }
        case 2:
        {
            initColor = &conseColor;
            colorTitle = QString("Choose Consequent Color");
            break;
        }
        case 3:
        {
            initColor = &connectColor;
            colorTitle = QString("Choose Connection Color");
            break;
        }
        case 4:
        {
            initColor = &missingColor;
            colorTitle = QString("Choose Missing Color");
            break;
        }
        default:
            break;
    }

    QColor newColor = QColorDialog::getColor(*initColor, this, colorTitle);

    if(newColor.isValid())
    {
        *initColor = newColor;
    }

    updateGlyphColors();
}
