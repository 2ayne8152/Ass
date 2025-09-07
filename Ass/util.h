#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <limits>
#include <string>
#include <cstdlib>  // For system()

using namespace std;

inline void clearScreen() {
    system("cls");   
}

inline void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

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

inline void inputCheck(int& input, int min, int max, string errormsg) {
    while (cin.fail() || input < min || input > max) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << '\n' << errormsg;
        cin >> input;
    }
}

inline string getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

inline void pauseInterface() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

#endif
