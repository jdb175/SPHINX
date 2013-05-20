#ifndef CONVERT_H_
#define CONVERT_H_

#include <string>

using namespace std;

namespace PARASProgram
{
	namespace Convert
	{
		string *doubleToString(double dbl);
		string *intToString(int in);
		string *longToString(long long in);

		int stringToInt(string* str);
		double stringToDouble(string* str);
		bool stringToBool(string* str);
	}

}


#endif /* CONVERT_H_ */
