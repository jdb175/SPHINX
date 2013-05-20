/*
 * ColorSelector.h
 *
 *  Created on: Jun 12, 2009
 *  Author: Jason Stasik
 *
 *  ColorSelector is a dialog that lets the user replace
 *  an existing color ramp with a new one. They are allowed
 *  to pick from any of the allowed types of ramps for the
 *  given color map. For each ramp type, they can pick any
 *  of the predefined ramps, and for RAMP_SINGLE, they can
 *  choose any color they want.
 */

#ifndef COLORSELECTOR_H_
#define COLORSELECTOR_H_

#include <vector>
#include <QWidget>
#include <QDialog>
#include <QSignalMapper>
#include <QLabel>
#include <QRadioButton>
#include "ColorManager.h"

class ColorMap;

class ColorSelector : public QDialog {
	Q_OBJECT

public:
	// Constructor, a reference to the ColorCustom class and the color to change
    ColorSelector(QWidget *colorCustom, ColorManager *manage, ColorMap* colorMappings);
	// Destructor
	virtual ~ColorSelector();
    void displayColorSelector(ColorMap *active);

private:
	int			m_selected; // the index of the currently selected ramp

    std::vector<QLabel*> seq_labels, div_labels, qual_labels; // widgets to draw on for each type

	QSignalMapper *m_map; // maps radio buttons to ColorManager::PredefinedColorRamps
    ColorManager *cm;
    ColorMap* activeColorMap;

    QRadioButton *radioButtonAr[ColorManager::TOTAL_RAMPS];
    QPushButton *anteColorButton;
    QColor anteColor;
    QPushButton *conseColorButton;
    QColor conseColor;
    QPushButton *connectColorButton;
    QColor connectColor;
    QPushButton *missingColorButton;
    QColor missingColor;


private:
	// Draws the color bars on the QGLWidgets
    void drawColorBars();
    void updateGlyphColors();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	// Applies the selected changes to the color
	void		apply();
	// Discards all changes made to the color
	void		cancel();
	// Updates the selected color
	void		setSelectedRadio(int button);
    void showColorChooser(int signal);
};

#endif /* COLORSELECTOR_H_ */
