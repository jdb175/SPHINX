#include <string>
#include <cstdlib>
#include <sstream>
#include "util/Utility.hpp"
using namespace std;

namespace SPHINXProgram
{
	namespace Convert
	{
		/**
		 * Converts the given double to a string
		 */
		string *doubleToString(double dbl)
		{
			ostringstream convert;
			convert << dbl;
			string *ret = new string(convert.str());
			return ret;
		}


		/**
		 * Converts the given int to a string
		 */
		string *intToString(int in)
		{
			stringstream convert;
			convert << in;
			string *ret = new string(convert.str());
			return ret;
		}


		/**
		 * Converts the given long to a string
		 */
		string *longToString(long long in)
		{
			stringstream convert;
			convert << in;
			string *ret = new string(convert.str());
			return ret;
		}


		/**
		 * Converts the given string to an int
		 */
		int stringToInt(string* str)
		{
			return atoi(str->c_str());
		}

		/**
		 * Converts the given string to a double
		 */
		double stringToDouble(string* str)
		{
			stringstream convert;
			double dbl;
			convert << *str;
			convert >> dbl;

			return dbl;
		}

		/**
		 * Converts the given string to a bool
		 */
		bool stringToBool(string* str)
		{
			return Utility::bothTrim(str)->compare("true") == 0;
		}
	}

}
