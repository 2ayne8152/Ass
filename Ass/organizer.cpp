#include <iostream>
#include <string>
#include <sstream>
#include "util.h"
using namespace std;


void eventMenu(const string& username);

void organizerMainMenu(const string& username) {
    clearScreen();
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
    int choice;
    string input;

    while (true) {
        cout << "\n===== ORGANIZER MENU =====\n";
        cout << "Welcome, " << username << "!\n\n";
        cout << "1. Manage My Events\n";
        cout << "2. View Reports\n";
        cout << "3. Logout\n";
        cout << "Choose an option: ";

        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1:
            eventMenu(username); 
            break;
        case 2:
            cout << "Report viewing not implemented yet.\n";
            break;
        case 3:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}