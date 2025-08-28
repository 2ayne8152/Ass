#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include "event_staff.h"
#include "stage.h"
using namespace std;

void staffMainMenu(const string& username);
void userMainMenu(const string& username);
void stageMenu();
void crisisMenu();

void initializeStaffIdSystem() {
    ifstream file("last_staff_id.txt");
    if (!file.is_open()) {
        ofstream newFile("last_staff_id.txt");
        if (newFile.is_open()) {
            newFile << 0;
            newFile.close();
        }
    }
    else {
        file.close();
    }
}

bool isValidUsername(const string& username) {
    regex pattern("^[A-Za-z0-9_]{3,}$"); // letters, numbers, underscore, min 3 chars
    return regex_match(username, pattern);
}

bool isValidEmail(const string& email) {
    regex pattern(R"((\w+)(\.{0,1})(\w*)@(\w+)(\.(\w+))+)");
    return regex_match(email, pattern);
}

bool isValidPassword(const string& password) {
    if (password.length() < 6) 
        return false; 
    bool hasLetter = false, hasDigit = false;
    for (char c : password) {
        if (isalpha(c)) hasLetter = true;
        if (isdigit(c)) hasDigit = true;
    }
    return hasLetter && hasDigit;
}

bool isDuplicate(const string& username, const string& email, const string& role) {
    string filename = (role == "staff" ? "staff.txt" : "users.txt");
    ifstream file(filename);

    if (!file.is_open()) return false;

    string line, u, e, p;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> u >> e >> p;
        if (u == username || e == email) {
            return true;
        }
    }
    return false;
}

string getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}


void signUp(const string& role) {
    string username, email, password;

    cout << "\n===== " << (role == "staff" ? "STAFF" : "USER") << " SIGN UP =====" << endl;
    cout << "(Type 'exit' at any time to cancel)\n";
    
    // staff check passkey
    if (role == "staff") {
        string passkey;
        while (true) {
            cout << "Enter staff passkey: ";
            getline(cin, passkey);

            if (passkey == "exit") {
                cout << "Sign up cancelled.\n";
                return;
            }

            if (passkey == "1234") {
                cout << "Passkey accepted. Proceeding with staff registration...\n";
                break;
            }
            else {
                cout << "Invalid passkey. Try again or type 'exit' to cancel.\n";
            }
        }
    }

    while (true) {
        username = getInput("Enter username: ");
        if (username == "exit") {
            cout << "Sign up cancelled.\n";
            return;
        }
        if (!isValidUsername(username)) {
            cout << "Invalid username. Must be at least 3 characters (letters, numbers, underscore).\n";
            continue;
        }

        while (true) {
            email = getInput("Enter email: ");
            if (email == "exit") {
                cout << "Sign up cancelled.\n";
                return;
            }
            if (!isValidEmail(email)) {
                cout << "Invalid email format.\n";
                continue;
            }
            break;
        }

        while (true) {
            password = getInput("Enter password: ");
            if (password == "exit") {
                cout << "Sign up cancelled.\n";
                return;
            }
            if (!isValidPassword(password)) {
                cout << "Invalid password. Must be at least 6 characters, include a letter and a number.\n";
                continue;
            }
            break;
        }

        if (isDuplicate(username, email, role)) {
            cout << "Username or email already exists. Try again.\n";
            continue;
        }

        //save to correct file 
        string filename = (role == "staff" ? "staff.txt" : "users.txt");
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << username << " " << email << " " << password << endl;
            file.close();
            cout <<  (role == "staff" ? "Staff" : "User") << " sign up successful!" << ".\n";
        }
        else {
            cout << "Error: Could not open file.\n";
        }
        break;
    }
}


void userMainMenu(const string& username) {
    cout << "hi user";
}

void login(const string& role) {
    string usernameOrEmail, password;
    bool success = false;

    cout << "\n===== " << (role == "staff" ? "STAFF" : "USER") << " LOGIN =====" << endl;

    string filename = (role == "staff" ? "staff.txt" : "users.txt");

    while (true) {
        usernameOrEmail = getInput("Enter username or email: ");
        password = getInput("Enter password: ");

        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open " << filename << ".\n";
            return;
        }

        string line, u, e, p;
        success = false;
        while (getline(file, line)) {
            stringstream ss(line);
            ss >> u >> e >> p;
            //match username oe email
            if ((usernameOrEmail == u || usernameOrEmail == e) && password == p) {
                success = true;
                break;
            }
        }
        file.close();

        if (success) {
            cout << "\nLogin successful! Welcome, " << usernameOrEmail << ".\n";

            // go back to correct main menu
            if (role == "staff") {
                staffMainMenu(usernameOrEmail);
            }
            else {
                userMainMenu(usernameOrEmail);
            }
            break;
        }
        else {
            cout << "Invalid username/email or password. Try again.\n";
        }
    }
}

void homePageMenu() {
    while (true) {
        cout << "\n===== HOME PAGE =====\n";
        cout << "1. Sign Up (User)\n";
        cout << "2. Sign Up (Staff)\n";
        cout << "3. Log In (User)\n";
        cout << "4. Log In (Staff)\n";
        cout << "5. Exit\n";
        cout << "Choose an option: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            signUp("user");
        }
        else if (choice == "2") {
            signUp("staff");
        }
        else if (choice == "3") {
            login("user");
        }
        else if (choice == "4")
        {
            login("staff");
        }
        else if (choice == "5") {
            cout << "Exiting program. Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
}


void staffMainMenu(const string& username) {
    int choice;
    string input;

    while (true) {
        cout << "\n===== EVENT ORGANIZER STAFF MENU =====\n";
        cout << "Welcome, " << username << "!\n";
        cout << "1. Manage Event Staff\n";
        cout << "2. View Events\n";
        cout << "3. Crisis management\n";
        cout << "4. Logout\n";
        cout << "Choose an option: ";

        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            vector<EventStaff> staffList = loadEventStaff();
            manageEventStaffMenu(staffList);
            break;
        }
        case 2:
            cout << "Viewing events\n";
            break;
        case 3:
            crisisMenu();
            break;
        case 4:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}