#ifndef PAIR_H
#define PAIR_H

#include "util/json/json.h"
#include <string>

using namespace std;
namespace PARASProgram
{
	class XYPair
	{
		public:
			XYPair(double first, double second);
            ~XYPair();
			double getX();
			void setX(double value);

			double getY();
			void setY(double value);

            Json::Value jsonify();

        private:
			double x, y;
	};
}
#endif
