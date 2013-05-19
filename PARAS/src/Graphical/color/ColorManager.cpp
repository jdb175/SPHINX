/*
 * ColorManager.cpp
 *
 *  Created on: Jun 18, 2009
 *	Author: Jason Stasik
 *
 *	Handles storing and creation of colors
 */

#include "graphical/color/ColorManager.h"
#include "graphical/color/ColorRamp.h"
#include "graphical/color/ColorMacros.h"

#include <assert.h>

ColorManager::ColorManager() {

    createNames();

    // Make sure these are in the same order as in PredefinedTypes
    // These indicate the number of ramps for each ramp type
    setNumColorRamps(SEQUENTIAL, 7);
    setNumColorRamps(DIVERGING, 6);
    setNumColorRamps(QUALITATIVE, 5);
}

ColorManager::~ColorManager() {
    m_predefinedNumColorMaps.clear();
    m_predefinedColorRampNames.clear();
}

void ColorManager::createNames() {
    // PREDEFINED COLOR MAPS
    m_predefinedColorRampNames.resize(TOTAL_RAMPS);
    m_predefinedColorRampNames[SEQ_ORANGES] = "Oranges";
    m_predefinedColorRampNames[SEQ_GRAYSCALE] = "Grayscale";
    m_predefinedColorRampNames[SEQ_YLORRD] = "Yellow, Orange, Red";
    m_predefinedColorRampNames[SEQ_YLGNBU] = "Yellow, Green, Blue";
    m_predefinedColorRampNames[SEQ_PURD] = "Purple, Red";
    m_predefinedColorRampNames[SEQ_NOYELLOW] = "No Yellow";
    m_predefinedColorRampNames[SEQ_HSIFULL] = "All Colors";
    m_predefinedColorRampNames[DIV_RDBU] = "Red, Blue";
    m_predefinedColorRampNames[DIV_PUOR] = "Purple, Orange";
    m_predefinedColorRampNames[DIV_RDYLGN] = "Red, Yellow, Green";
    m_predefinedColorRampNames[DIV_SPECTRAL] = "Spectral";
    m_predefinedColorRampNames[DIV_RDGY] = "Red, Gray";
    m_predefinedColorRampNames[DIV_PUGN] = "Purple, Green";
    m_predefinedColorRampNames[QUAL_SET1] = "Set 1";
    m_predefinedColorRampNames[QUAL_SET2] = "Set 2 Pastel";
    m_predefinedColorRampNames[QUAL_PASTEL] = "Set 1 Pastel";
    m_predefinedColorRampNames[QUAL_DARK] = "Set 2";
    m_predefinedColorRampNames[QUAL_ACCENTS] = "Accents";
}

/**
 * Template:
   case : {
        float ramp[][5] = {
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
                { , , , , 255 },
        };
        return new ColorRamp(, ramp, , ColorRamp::RAMP_);
    }
 */
ColorRamp* ColorManager::getPredefinedColorRamp(int mapName) {
    switch(mapName) {
    case SEQ_ORANGES: {
        float ramp[7][5] = {
                { 0.0f,		254.0f, 237.0f, 222.0f, 255.0f },
                { 42.0f,    253.0f, 208.0f, 162.0f, 255.0f },
                { 84.0f,    253.0f, 174.0f, 107.0f, 255.0f },
                { 126.0f,   253.0f, 141.0f, 60.0f, 255.0f },
                { 168.0f,   241.0f, 105.0f, 19.0f, 255.0f },
                { 210.0f,   217.0f, 72.0f, 1.0f, 255.0f },
                { 255.0f,   140.0f, 45.0f, 4.0f, 255.0f }
        };
        return new ColorRamp(7, ramp, SEQ_ORANGES, ColorRamp::RAMP_SEQ);
    }
    case SEQ_GRAYSCALE: {
        float ramp[2][5] = {
                { 0,	  255, 255, 255, 255 },
                { 255,    0, 0, 0, 255 }
        };
        return new ColorRamp(2, ramp, SEQ_GRAYSCALE, ColorRamp::RAMP_SEQ);
    }
    case SEQ_YLORRD: {
        float ramp[7][5] = {
                { 0.0f,		255.0f, 255.0f, 178.0f, 255.0f },
                { 42.0f,    254.0f, 217.0f, 118.0f, 255.0f },
                { 84.0f,    254.0f, 178.0f, 76.0f, 255.0f },
                { 126.0f,   253.0f, 141.0f, 60.0f, 255.0f },
                { 168.0f,   252.0f, 78.0f, 42.0f, 255.0f },
                { 210.0f,   227.0f, 26.0f, 28.0f, 255.0f },
                { 255.0f,   177.0f, 0.0f, 38.0f, 255.0f }
        };
        return new ColorRamp(7, ramp, SEQ_YLORRD, ColorRamp::RAMP_SEQ);
    }
    case SEQ_YLGNBU: {
        float ramp[7][5] = {
                { 0.0f,		255.0f, 255.0f, 204.0f, 255.0f },
                { 42.0f,    199.0f, 233.0f, 180.0f, 255.0f },
                { 84.0f,    127.0f, 205.0f, 187.0f, 255.0f },
                { 126.0f,   65.0f, 182.0f, 196.0f, 255.0f },
                { 168.0f,   29.0f, 145.0f, 192.0f, 255.0f },
                { 210.0f,   34.0f, 94.0f, 168.0f, 255.0f },
                { 255.0f,   12.0f, 44.0f, 132.0f, 255.0f }
        };
        return new ColorRamp(7, ramp, SEQ_YLGNBU, ColorRamp::RAMP_SEQ);
    }
    case SEQ_PURD: {
        float ramp[7][5] = {
                { 0.0f,		241.0f, 238.0f, 246.0f, 255.0f },
                { 42.0f,    212.0f, 185.0f, 218.0f, 255.0f },
                { 84.0f,    201.0f, 148.0f, 199.0f, 255.0f },
                { 126.0f,   223.0f, 101.0f, 176.0f, 255.0f },
                { 168.0f,   231.0f, 41.0f, 138.0f, 255.0f },
                { 210.0f,   206.0f, 18.0f, 86.0f, 255.0f },
                { 255.0f,   145.0f, 0.0f, 63.0f, 255.0f }
        };
        return new ColorRamp(7, ramp, SEQ_PURD, ColorRamp::RAMP_SEQ);
    }
    case SEQ_NOYELLOW: {
        float ramp[2][5] = {
                { 0, 65, 255, 255, 255 },
                { 255, 255, 255, 160, 255 },
        };
        return new ColorRamp(2, ramp, SEQ_NOYELLOW, ColorRamp::RAMP_SEQ, ColorRamp::MODEL_HSI);
    }
    case SEQ_HSIFULL: {
        float ramp[2][5] = {
                { 0, 0, 255, 255, 255 },
                { 255, 255, 255, 255, 255 },
        };
        return new ColorRamp(2, ramp, SEQ_HSIFULL, ColorRamp::RAMP_SEQ, ColorRamp::MODEL_HSI);
    }
    case DIV_RDBU: {
        float ramp[11][5] = {
                { 0, 103, 0, 31, 255 },
                { 26, 178, 24, 43, 255 },
                { 51, 214, 96,77 , 255 },
                { 77, 244, 165, 130, 255 },
                { 102, 253, 219, 199, 255 },
                { 127, 247, 247, 247, 255 },
                { 153, 209, 229, 240, 255 },
                { 178, 146, 197, 222, 255 },
                { 204, 67, 147, 195, 255 },
                { 229, 33, 102, 172, 255 },
                { 255, 5, 48, 97, 255 },
        };
        return new ColorRamp(11, ramp, DIV_RDBU, ColorRamp::RAMP_DIV);
    }
    case DIV_PUOR: {
        float ramp[11][5] = {
                { 0, 127, 59, 8, 255 },
                { 26, 179, 88, 6, 255 },
                { 51, 224, 130, 20, 255 },
                { 77, 253, 184, 99, 255 },
                { 102, 254, 224, 182, 255 },
                { 127, 247, 247, 247, 255 },
                { 153, 216, 218, 235, 255 },
                { 178, 178, 171, 210, 255 },
                { 204, 128, 115, 172, 255 },
                { 229, 84, 39, 136, 255 },
                { 255, 45, 0, 75, 255 },
        };
        return new ColorRamp(11, ramp, DIV_PUOR, ColorRamp::RAMP_DIV);
    }
    case DIV_RDYLGN: {
        float ramp[11][5] = {
                { 0, 165, 0, 38, 255 },
                { 26, 215, 48, 39, 255 },
                { 51, 244, 109, 67, 255 },
                { 77, 253, 174, 97, 255 },
                { 102, 254, 224, 139, 255 },
                { 127, 255, 255, 191, 255 },
                { 153, 217, 239, 139, 255 },
                { 178, 166, 217, 106, 255 },
                { 204, 102, 189, 99, 255 },
                { 229, 26, 152, 80, 255 },
                { 255, 0, 104, 55, 255 },
        };
        return new ColorRamp(11, ramp, DIV_RDYLGN, ColorRamp::RAMP_DIV);
    }
    case DIV_SPECTRAL: {
        float ramp[11][5] = {
                { 0, 158, 1, 66, 255 },
                { 26, 213, 62, 79, 255 },
                { 51, 244, 109, 67, 255 },
                { 77, 253, 174, 97, 255 },
                { 102, 254, 224, 139, 255 },
                { 127, 255, 255, 191, 255 },
                { 153, 230, 245, 152, 255 },
                { 178, 171, 221, 164, 255 },
                { 204, 102, 194, 165, 255 },
                { 229, 50, 136, 189, 255 },
                { 255, 94, 79, 162, 255 },
        };
        return new ColorRamp(11, ramp, DIV_SPECTRAL, ColorRamp::RAMP_DIV);
    }
    case DIV_RDGY: {
        float ramp[11][5] = {
                { 0, 103, 0, 31, 255 },
                { 26, 178, 24, 43, 255 },
                { 51, 214, 96, 77, 255 },
                { 77, 244, 165, 130, 255 },
                { 102, 253, 219, 199, 255 },
                { 127, 255, 255, 255, 255 },
                { 153, 224, 224, 224, 255 },
                { 178, 186, 186, 186, 255 },
                { 204, 135, 135, 135, 255 },
                { 229, 77, 77, 77, 255 },
                { 255, 26, 26, 26, 255 },
        };
        return new ColorRamp(11, ramp, DIV_RDGY, ColorRamp::RAMP_DIV);
    }
    case DIV_PUGN: {
        float ramp[11][5] = {
                { 0, 64, 0, 75, 255 },
                { 26, 118, 42, 131, 255 },
                { 51, 153, 112, 171, 255 },
                { 77, 194, 165, 207, 255 },
                { 102, 231, 212, 232, 255 },
                { 127, 247, 247, 247, 255 },
                { 153, 217, 240, 211, 255 },
                { 178, 168, 216, 183, 255 },
                { 204, 90, 174, 97, 255 },
                { 229, 27, 120, 55, 255 },
                { 255, 0, 68, 27, 255 },
        };
        return new ColorRamp(11, ramp, DIV_PUGN, ColorRamp::RAMP_DIV);
    }
    case QUAL_SET1: {
        float ramp[8][5] = {
                { 0, 228, 26, 28, 255 },
                { 36, 55, 126, 184, 255 },
                { 73, 77, 175, 74, 255 },
                { 109, 152, 78, 163, 255 },
                { 146, 255, 127, 0, 255 },
                { 182, 255, 255, 51, 255 },
                { 219, 166, 86, 40, 255 },
                { 255, 247, 129, 191, 255 },
        };
        return new ColorRamp(8, ramp, QUAL_SET1, ColorRamp::RAMP_QUAL);
    }
    case QUAL_SET2: {
        float ramp[8][5] = {
                { 0, 102, 194, 165, 255 },
                { 36, 252, 141, 98, 255 },
                { 73, 141, 160, 203, 255 },
                { 109, 231, 138, 195, 255 },
                { 146, 166, 216, 84, 255 },
                { 182, 255, 217, 47, 255 },
                { 219, 229, 196, 148, 255 },
                { 255, 179, 179, 179, 255 },
        };
        return new ColorRamp(8, ramp, QUAL_SET2, ColorRamp::RAMP_QUAL);
    }
    case QUAL_PASTEL: {
        float ramp[8][5] = {
                { 0, 251, 180, 174, 255 },
                { 36, 179, 205, 227, 255 },
                { 73, 204, 235, 197, 255 },
                { 109, 222, 203, 228, 255 },
                { 146, 254, 217, 166, 255 },
                { 182, 255, 255, 204, 255 },
                { 219, 229, 216, 189, 255 },
                { 255, 253, 218, 236, 255 },
        };
        return new ColorRamp(8, ramp, QUAL_PASTEL, ColorRamp::RAMP_QUAL);
    }
    case QUAL_DARK: {
        float ramp[8][5] = {
                { 0, 27, 158, 119, 255 },
                { 36, 217, 95, 2, 255 },
                { 73, 117, 112, 179, 255 },
                { 109, 231, 41, 138, 255 },
                { 146, 102, 166, 30, 255 },
                { 182, 230, 171, 2, 255 },
                { 219, 166, 118, 29, 255 },
                { 255, 102, 102, 102, 255 },
        };
        return new ColorRamp(8, ramp, QUAL_DARK, ColorRamp::RAMP_QUAL);
    }
    case QUAL_ACCENTS: {
        float ramp[8][5] = {
                { 0, 127, 201, 127, 255 },
                { 36, 190, 174, 212, 255 },
                { 73, 253, 192, 134, 255 },
                { 109, 255, 255, 153, 255 },
                { 146, 56, 108, 176, 255 },
                { 182, 240, 2, 127, 255 },
                { 219, 191, 91, 23, 255 },
                { 255, 102, 102, 102, 255 },
        };
        return new ColorRamp(8, ramp, QUAL_ACCENTS, ColorRamp::RAMP_QUAL);
    }
    default:
        return new ColorRamp();
    }
}

char* ColorManager::getPredefinedColorRampName(int mapName) {
    assert(mapName >= 0 && mapName <= TOTAL_RAMPS);
    return m_predefinedColorRampNames[mapName];
}

int ColorManager::getPredefinedTotal(int index) {
    assert(index >= 0 && index < TOTAL_TYPES);
    if(index == SEQUENTIAL) return m_predefinedNumColorMaps[index];
    else return m_predefinedNumColorMaps[index] - m_predefinedNumColorMaps[index-1];
}

int ColorManager::getPredefinedStart(int index) {
    assert(index >= 0 && index < TOTAL_TYPES);
    if(index == SEQUENTIAL) return 0;
    return m_predefinedNumColorMaps[index-1];
}

int ColorManager::getPredefinedEnd(int index) {
    assert(index >= 0 && index < TOTAL_TYPES);
    return m_predefinedNumColorMaps[index];
}

void ColorManager::setNumColorRamps(PredefinedTypes type, int num) {
    if(m_predefinedNumColorMaps.size() == 0)
        m_predefinedNumColorMaps.assign(TOTAL_TYPES, 0);
    if(type == SEQUENTIAL)
        m_predefinedNumColorMaps[type] = num;
    else
        m_predefinedNumColorMaps[type] = m_predefinedNumColorMaps[type-1] + num;
}
