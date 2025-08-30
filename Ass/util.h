#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <limits>
#include <string>
using namespace std;

inline int getValidatedInt(const string& prompt) {
    int num;
    while (true) {
        if (cin >> num && num > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return num;
        }
        cout << "Invalid input. " << prompt << "\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

inline void inputCheck(int& input, double min, double max, string errormsg) {
    while (cin.fail() || input < min || input > max) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << '\n' << errormsg;
        cin >> input;
    }
}

#endif