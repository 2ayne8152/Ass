#include <iostream>
#include <string>
#include <sstream>
using namespace std;

void organizerMainMenu(const string& username) {
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
            // TODO: Add event management for organizers
            cout << "Event management not implemented yet.\n";
            break;
        case 2:
            // TODO: Add report viewing for organizers
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