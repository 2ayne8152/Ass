#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include "abc.h"
using namespace std;

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
            cout << (role == "staff" ? "Staff" : "User") << " sign up successful!" << ".\n";
        }
        else {
            cout << "Error: Could not open file.\n";
        }
        break;
    }
}

void staffMainMenu(const string& username) {
    cout << "hi staff";
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
        cout << "4. Exit\n";
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

void mainMenu() {
    int choice;
    string input;

    while (true) {
        cout << setw(27) << right << "BLABLABLA MENU\n";
        cout << "=========================================\n\n";
        cout << "1. Register account" << endl;
        cout << "2. Register for Event" << endl;
        cout << "3. Make Payment & Checkout" << endl;
        cout << "4. Book Event Date" << endl;
        cout << "5. View Event Marketing Info" << endl;
        cout << "6. Monitor Ongoing Events" << endl;
        cout << "7. Generate Event Reports" << endl;
        cout << "8. Exit" << endl;
        cout << "\n=========================================\n";
        cout << "Enter a choice: ";

        getline(cin, input); // get input as string

        stringstream ss(input);
        if (!(ss >> choice) || (ss >> ws && !ss.eof())) {
            cout << "Invalid input. Please enter a number between 1 and 7.\n\n";
            continue; // skip the rest of the loop and ask again
        }

        switch (choice) {
        case 1:
            signUp("staff");
            break;
        case 2:
            signUp("users");
            //cout << "Processing Payment & Checkout...\n";
            // paymentAndCheckout();
            break;
        case 3:
            cout << "Booking Event Date & Logistics...\n";
            // bookEventDate();
            break;
        case 4:
            cout << "Viewing Event Marketing Info...\n";
            // viewMarketing();
            break;
        case 5:
            cout << "Monitoring Ongoing Events...\n";
            // monitorEvents();
            break;
        case 6:
            cout << "Generating Event Reports...\n";
            // generateReports();
            break;
        case 7:
            cout << "Exiting program. Goodbye!\n";
            return; // exits the mainMenu() function
        default:
            cout << "Invalid choice. Please select a number between 1 and 7.\n";
        }

    }
}

void main() {
    homePageMenu();
}