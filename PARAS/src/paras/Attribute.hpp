#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP
#include <vector>

#include "util/json/json.h"
using namespace std;

namespace PARASProgram
{
    class Attribute
    {
        public:
            Attribute(string* name, vector<string> *membersOfAttribute, vector<string> *memberName);
            void convertToNamed(vector<string*> *toBeConverted, vector<string*> *putHere);
            string getNameForValue(string member);
            string getValueForName(string member);
            bool isMember(string possibleMember);
            string detailText(vector<string*> *fromVector, int count = 0);
            double getRelativePosition(string attribute);
            string* getAttributeName();
            Json::Value jsonify();
            bool *mapped;
        private:
            vector<string> *attributeMembers;
            vector<string> *memberNames;
            string* name;
            int maxAttr;
            int minAttr;
    };
}

#endif // ATTRIBUTE_HPP
