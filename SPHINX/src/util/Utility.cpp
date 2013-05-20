#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <functional>
#include <cctype>
#include "util/Utility.hpp"
#include "paras/Rule.hpp"
#include "paras/XYPair.hpp"
#include "util/Convert.hpp"

using namespace std;

namespace PARASProgram
{
	namespace Utility 
	{
        struct compXY {
            bool operator() (XYPair* i,XYPair* j) {
                return (i->getX() < j->getX() || (i->getY() < j->getY() && i->getX() == j->getX()));
            }
        } comp;

		// trim from start
		string* startTrim(string *s) {
		        s->erase(s->begin(), std::find_if(s->begin(), s->end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		        return s;
		}

		// trim from end
		string* endTrim(string *s) {
		        s->erase(std::find_if(s->rbegin(), s->rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s->end());
		        return s;
		}

		// trim from both ends
		string* bothTrim(string *s) {
		        return startTrim(endTrim(s));
		}

		/**
		 * Takes the contents of an vector of strings and concatenates them into a single string.
		 * @param arr the vector of strings.
		 * @return a single string with all of the contents of the vector.
		 */
		string *arrayToString(vector<string*> *arr, string* buf)
		{
			string delim = ",";
			buf->clear();
			buf->reserve(arr->size() * 3); //try to allocate as much memory as needed at first.
			for(unsigned int i = 0; i != arr->size(); i++){
				string *item = arr->at(i);
				buf->append(item->c_str());
                if(i != arr->size() - 1) buf->append(delim);
			}

			return buf;
		}

		/**
		 * Checks if the given string is present in a given string vector
		 * @param strChild the possible substring
		 * @param strParent the possible parent vector.
		 * @return whether strChild is a substring of strParent.
		 */
		bool isSubstring(string *strChild, vector<string*> *strParent)
		{
			for(std::vector<string*>::size_type i = 0; i != strParent->size(); i++){
				string *p = strParent->at(i);
				if(p->compare(*strChild) == 0) return true;
			}

			return false;
		}

		/**
		 * Alphabetizes the contents of an vector of strings.
		 * @param strToken the vector to alphabetize.
		 * @return the alphabetized vector.
		 */
		vector<string*> *numericalOrder(vector<string*> *strToken, bool forItemSets)
		{
			int offsetForItemSets = 0;
			if(forItemSets) offsetForItemSets = 1; //dont want to sort last item if for item sets.
		
			std::sort(strToken->begin(), strToken->end() - offsetForItemSets, strSort);

			return strToken;
		}

		/**
		 * String sort function.  Used for std::sort in above function.
		 */
		bool strSort(string* a, string* b)
		{
			return atoi(a->c_str()) < atoi(b->c_str());
		}

		/**
		 * Prints the list of rules, along with a designated message.
		 * @param listOfRules the list of rules to print
		 * @param msg the message to print along with the list
		 */
		void printRulesVector(vector<Rule*> *listOfRules, string msg)
		{
			cout << msg << endl;

			for(std::vector<Rule*>::size_type i = 0; i != listOfRules->size(); i++){
				Rule* ar = listOfRules->at(i);
				cout << endl;
				cout << i << endl;
				cout << "Antecedent: " << *ar->XToListString() << "\n";
				cout << "Consequent: " << *ar->YToListString() << "\n";
				cout << "Support: " << ar->getSupport() << ", Confidence: " << ar->getConfidence() << endl;
			}
		}

		/**
		 * Finds the distinct points within a list of points.
		 * @param listOfPoints the list of points to go through.  This list must be sorted.
		 * @return a list of the distinct points within the original list
		 */
		vector<XYPair*> *distinctPoints(vector<XYPair*> *listOfPoints)
		{
			double lastX = 0.0;
			double lastY = 0.0;
			vector<XYPair*>* distinctPoints = new vector<XYPair*>;
	
			for(unsigned int i = 0; i < listOfPoints->size(); i++){
				XYPair* item = listOfPoints->at(i);
				if (item->getX() != lastX || item->getY() != lastY)
				{
					distinctPoints->push_back(item);
				}
				lastX = item->getX();
				lastY = item->getY();
			}
			return distinctPoints;
		}

		/**
		 * Checks if the inputted string begins with any digit.
		 * @param str the string to be checked
		 * @return a boolean of whether or not the string begins with a digit.
		 */
		bool startsWithDigit(string str)
		{
			if(str.size() == 0) return false;
			return isdigit(str.at(0));
		}

        /**
         * Builds intersections for the stable region points, returning a list of points with intersections added
         */
        vector<XYPair*> *buildIntersections(vector<XYPair*> *pspacePoints)
        {
            vector <XYPair*> *listOfNewPoints = new vector <XYPair*>;
            vector< pair<double,double> > presenceMap;

            for(size_t i = 0; i < pspacePoints->size(); i++)
            {
                XYPair* first = pspacePoints->at(i);
                presenceMap.push_back(pair<double,double>(first->getX(), first->getY()));
                for(size_t j = i+1; j < pspacePoints->size(); j++)
                {
                    if(j == i) continue;

                    XYPair* sec = pspacePoints->at(j);

                    if(first->getX() == sec->getX() != first->getY() == sec->getY()) continue;

                    if(first->getX() < sec->getX() != first->getY() < sec->getY())
                    {
                        double x = std::min(first->getX(), sec->getX());
                        double y = std::min(first->getY(), sec->getY());

                        XYPair* newPair = new XYPair(x,y);
                        listOfNewPoints->push_back(newPair);
                    }
                }
            }

            std::sort(listOfNewPoints->begin(), listOfNewPoints->end(), comp);

            double lastX = 0.0;
            double lastY = 0.0;
            XYPair *lastPair;
            vector<XYPair*>* distinctPoints = new vector<XYPair*>;

            for(unsigned int i = 0; i < listOfNewPoints->size(); i++){
                XYPair* item = listOfNewPoints->at(i);
                bool noExists = std::find(presenceMap.begin(), presenceMap.end(), pair<double, double>(item->getX(), item->getY())) == presenceMap.end();
                if ((item->getX() != lastX || item->getY() != lastY))
                {
                    if(noExists)
                    {
                        distinctPoints->push_back(item);
                        lastPair = item;
                    }
                }
                lastX = item->getX();
                lastY = item->getY();

                if(!noExists) delete item;
            }

            return distinctPoints;
        }
	}
}
