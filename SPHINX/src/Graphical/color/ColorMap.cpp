#include <assert.h>

#include "graphical/color/ColorMap.h"
#include "graphical/color/ColorMacros.h"
#include "graphical/color/ColorAssign.h"
#include "graphical/color/ColorRamp.h"
#include "graphical/color/Image.h"

//-------------------------------------------------------------------------
ColorMap::ColorMap(ColorRamp *ramp)
{
	m_colorramp = NULL;
	setColorRamp(ramp);
	m_checkImg = NULL;

    glyphAntecedentColor = QColor(Qt::blue);
    glyphConsequentColor = QColor(Qt::green);
    glyphMissingColor = QColor(Qt::gray);
    glyphConnectColor = QColor(Qt::gray);
}

//-------------------------------------------------------------------------
void ColorMap::initTex()
{
	m_checkImg = new Image(64,64);
	m_checkImg->clear(255,255,255);
	int i, j;
	unsigned char c;

	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			if ( ((i & 0x8) == 0) ^ ((j & 0x8) == 0) ) {
				c = (unsigned char)255;
			} else {
				c = (unsigned char)127;
			}
			m_checkImg->WritePixel(i,j,c,c,c);
		}
	}
	m_checkImg->CreateTexture();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ColorMap::ColorMap(const ColorMap& map)
{
	m_colorramp = new ColorRamp(*(map.m_colorramp));
    if (map.m_checkImg) {
		initTex();
	} else {
		m_checkImg = NULL;
	}
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void ColorMap::operator = (const ColorMap& map)
{
	if (&map == this)
		return;

	delete m_colorramp;
	m_colorramp = map.m_colorramp;
    if (map.m_checkImg) {
		initTex();
	} else {
		m_checkImg = NULL;
	}
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ColorMap::~ColorMap()
{
	SAFE_DELETE(m_checkImg);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void ColorMap::setColorRamp(ColorRamp *ramp)
{
	SAFE_DELETE(m_colorramp);
	m_colorramp = ramp;
    m_len = m_colorramp->getsize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned long ColorMap::operator[] (int index)
{
    while(index > m_len-1) index -= m_len;
    while(index < 0) index += m_len;
    if (m_colorramp->getColorModel() == ColorRamp::MODEL_RGB) {
        return (
                ((unsigned long) (255.0f * m_colorramp->evaluate(index, 2))) |
                (((unsigned long) (255.0f * m_colorramp->evaluate(index, 1))) << 8) |
                (((unsigned long) (255.0f * m_colorramp->evaluate(index, 0))) << 16) |
                (((unsigned long) (255.0f * m_colorramp->evaluate(index, 3))) << 24));
    } else if (m_colorramp->getColorModel() == ColorRamp::MODEL_HSI) {
        return HSIColor(6.0f * m_colorramp->evaluate(index, 0),
                m_colorramp->evaluate(index, 1),
                m_colorramp->evaluate(index, 2),
                m_colorramp->evaluate(index, 3)).getRGB();
    }
    return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned long ColorMap::getColor(float index)
{
	// If index is bigger or smaller than accepted range, normalize it
	// This will allow a programmer to loop through a ramp over and over
	while(index > 1.0f) index -= 1.0f;
	while(index < 0) index += 1.0f;
	if (m_colorramp->getColorModel() == ColorRamp::MODEL_RGB) {
		return (
				((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 2))) |
				(((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 1))) << 8) |
				(((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 0))) << 16) |
				(((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 3))) << 24));
	} else if (m_colorramp->getColorModel() == ColorRamp::MODEL_HSI) {
		return HSIColor(6.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 0),
				m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 1),
				m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 2),
				m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 3)).getRGB();
	}
	return 0;
}


//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void ColorMap::drawColorBar(float x1 /*= 0.0f*/, float y1 /*= 0.0f*/,
		float x2 /*= 1.0f*/, float y2 /*= 0.0f*/,
		float width /*= 5.0f*/)
{
	glPushMatrix();

	// Draw the textured quad first
	if (m_checkImg) {
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(x1+1.0f,y1-width+1.0f,0);
		glScalef(fabs(x2-x1),width+3.0f, 1.0f);
		m_checkImg->EnableTexturing();
		glBegin(GL_QUADS); {
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(3.0f, 0.0f); glVertex2f(1.0f, 0.0f);
			glTexCoord2f(3.0f, 1.0f); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
		}
		glEnd();
		m_checkImg->DisableTexturing();
		glPopMatrix();
	}
	// Draw the map
	glPushMatrix();
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(width);
	glBegin(GL_LINE_STRIP); {
		// initial x and y
		double x = x1, y = y1;
		// deltas
		double dx = x2 - x1, dy = y2 - y1;
		// step size
		double step = 1 / double(CMAPSIZE - 1);
		// for each delta step, draw a line of the
		// specified width with the color in the lut
		for (int i = 0; i < CMAPSIZE; i++) {
			unsigned long color = getColorForColorBar(float(i)/float(CMAPSIZE));
			glColor4ub((unsigned char) (color >> 16) & 0xff,
					(unsigned char) (color >> 8) & 0xff,
					(unsigned char) (color >> 0) & 0xff,
					(unsigned char) (color >> 24) & 0xff);
			x += step*dx; y += step*dy;
			glVertex2f((float)x, (float)y);
		}
	}
	glEnd();

	glLineWidth(1.0);
	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
	glPopMatrix();

	glPopMatrix();
}

unsigned long ColorMap::getColorForColorBar(float index) {
	if (m_colorramp->getColorModel() == ColorRamp::MODEL_RGB) {
		return (
				((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 2))) |
				(((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 1))) << 8) |
				(((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 0))) << 16) |
				(((unsigned long) (255.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 3))) << 24));
	} else if (m_colorramp->getColorModel() == ColorRamp::MODEL_HSI) {
		return HSIColor(6.0f * m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 0),
				m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 1),
				m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 2),
				m_colorramp->evaluate((int)(index*(CMAPSIZE-1)), 3)).getRGB();
	}
	return 0;
}

QPixmap ColorMap::drawColorBar2(int w, int h) {
	QImage image(w, h, QImage::Format_ARGB32);
	for(int i = 0; i < w; i++) {
		RGBt color(getColorForColorBar(float(i)/float(w)));
		QRgb value = qRgba(int(color.R*255), int(color.G*255), int(color.B*255), int(color.A*255));
		for(int j = 0; j < h; j++)
			image.setPixel(i, j, value);
	}

	QPixmap pix = QPixmap::fromImage(image);
	return pix;
}
