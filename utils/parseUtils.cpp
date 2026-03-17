/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#include <complex>
#include "parseUtils.h"

using namespace std;

ParseUtils::ParseUtils()
{
}

ParseUtils::~ParseUtils()
{
}

vector<int> ParseUtils::split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<int> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(stoi(token));
    }

    res.push_back(stoi(s.substr(pos_start)));
    return res;
}

vector<complex<double>> ParseUtils::splitComplex(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<complex<double>> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {        
        token = s.substr (pos_start, pos_end - pos_start);        

        pos_start = pos_end + delim_len;
        res.push_back(parseComplex(token));
    }

    token = s.substr(pos_start);    
    res.push_back(parseComplex(token));

    return res;
}

complex<double> ParseUtils::parseComplex(string token)
{
    //token contains the complex number. complex number looks like the following:
    // a+bj, a-bj
    // a, -a
    // bj, -bj

    size_t plus_end = token.find("+", 0);
    size_t minus_end = token.find("-", 0);
    size_t j_end = token.find("j", 0);

    //check for imaginary part
    if(j_end != string::npos)
    {
        //we have an imaginary part:
        // a+bj, a-bj
        // bj, -bj

        if(plus_end == string::npos)
        {
            //contains no "+", so can be either:
            // a-bj
            // bj, -bj

            //checking if we have a "-"
            if(minus_end == string::npos)
            {
                //no minus: 
                // --> bj
                string imagString = token.substr(0, j_end);
                return complex<double>(0,stod(imagString));
            }
            else if(minus_end == 0)
            {
                //minus is at the beginning:
                // --> -bj
                string imagString = token.substr(0, j_end);
                return complex<double>(0,stod(imagString)); 
            }
            else
            {
                //we have a minus somewhere in the middle:
                // --> a-bj
                string realString = token.substr(0, minus_end);
                string imagString = token.substr(minus_end + 1, token.size() - 1);

                return complex<double>(stod(realString),(-1)*stod(imagString));
            }
        }
        else
        {
            //by convention, as bj != +bj for us, plus is somewhere in the middle:
            // --> a+bj
            string realString = token.substr(0, plus_end);
            string imagString = token.substr(plus_end + 1, token.size() - 1);

            return complex<double>(stod(realString),stod(imagString));
        }        
    }
    else
    {
        //no j found, so its a real number:
        // --> a, -a
        return complex<double>(stod(token), 0);
    }

    //should never occur, throw an error
    throw new invalid_argument("The complex number doesn't match any case" + token);

}