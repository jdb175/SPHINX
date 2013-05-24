#include "paras/XYPair.hpp"
#include "util/json/json.h"
#include "util/Convert.hpp"
#include <sstream>


using namespace SPHINXProgram;
using namespace std;

XYPair::XYPair(double first, double second)
{
	this->x = first;
	this->y = second;
}

XYPair::~XYPair()
{
}

double XYPair::getX()
{
	return x;
}

void XYPair::setX(double value)
{
	x = value;
}

double XYPair::getY()
{
	return y;
}

void XYPair::setY(double value)
{
	y = value;
}


Json::Value XYPair::jsonify()
{
    Json::Value xyRoot;
    xyRoot["x"] = x;
    xyRoot["y"] = y;
    return xyRoot;
}
