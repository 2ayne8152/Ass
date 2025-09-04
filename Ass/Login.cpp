#include "login.h"
#include "user.h"
#include "report.h"

void staffMainMenu(const string& username);
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
            cout << (role == "staff" ? "Staff" : "User") << " sign up successful!" << ".\n";
        }
        else {
            cout << "Error: Could not open file.\n";
        }
        break;
    }
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
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear leftover input
            }
            break;
        }
        else {
            cout << "Invalid username/email or password. Try again.\n";
        }
    }
}

void homePageMenu() {


    cout << R"ASCII(
     ██╗ █████╗ ███╗   ███╗    ██████╗  ██╗      █████╗ ███╗   ██╗
     ██║██╔══██╗████╗ ████║    ██╔══██╗ ██║     ██╔══██╗████╗  ██║
     ██║███████║██╔████╔██║    ██████╔╝ ██║     ███████║██╔██╗ ██║
██   ██║██╔══██║██║╚██╔╝██║    ██╔═══╝  ██║     ██╔══██║██║╚██╗██║
╚█████╔╝██║  ██║██║ ╚═╝ ██║    ██║      ███████╗██║  ██║██║ ╚████║
 ╚════╝ ╚═╝  ╚═╝╚═╝     ╚═╝    ╚═╝      ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝
)ASCII" << endl;


    while (true) {
        cout << "\n===== HOME PAGE =====\n";
        cout << "1. Log In\n";
        cout << "2. Sign Up\n";
        cout << "3. Exit\n";
        cout << "Choose an option: ";

        string choiceStr;
        getline(cin, choiceStr);

        bool isValid = !choiceStr.empty();
        for (char c : choiceStr) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }

        if (!isValid) {
            cout << "Invalid input! Please enter 1-3.\n";
            continue;
        }

        int choice = stoi(choiceStr);

        switch (choice) {
        case 1: { // login menu
            bool inLoginMenu = true;
            while (inLoginMenu) {
                cout << "\n===== LOG IN MENU =====\n";
                cout << "1. Log In (Staff)\n";
                cout << "2. Log In (User)\n";
                cout << "3. Back to Home\n";
                cout << "Choose an option: ";

                string loginInput;
                getline(cin, loginInput);

                bool isValidInput = !loginInput.empty();

                for (char ch : loginInput) {
                    if (!isdigit(ch)) {
                        isValidInput = false;
                        break;
                    }
                }

                if (!isValidInput) {
                    cout << "Invalid input! Enter 1-3.\n";
                    continue;
                }

                int loginChoice = stoi(loginInput);
                switch (loginChoice) {
                case 1:
                    login("staff");
                    inLoginMenu = false;
                    break;
                case 2:
                    login("user");
                    inLoginMenu = false;
                    break;
                case 3:
                    inLoginMenu = false;
                    break;
                default:
                    cout << "Invalid choice! Enter 1-3.\n";
                    break;
                }
            }
            break;
        }

        case 2: { // SIGN UP MENU
            bool inSignUpMenu = true;
            while (inSignUpMenu) {
                cout << "\n===== SIGN UP MENU =====\n";
                cout << "1. Sign Up (Staff)\n";
                cout << "2. Sign Up (User)\n";
                cout << "3. Back to Home\n";
                cout << "Choose an option: ";

                string signUpInput;
                getline(cin, signUpInput);

                bool isValidInput = !signUpInput.empty();
                for (char ch : signUpInput) {
                    if (!isdigit(ch)) {
                        isValidInput = false;
                        break;
                    }
                }

                if (!isValidInput) {
                    cout << "Invalid input! Enter 1-3.\n";
                    continue;
                }

                int signUpChoice = stoi(signUpInput);
                switch (signUpChoice) {
                case 1:
                    signUp("staff");
                    inSignUpMenu = false;
                    break;
                case 2:
                    signUp("user");
                    inSignUpMenu = false;
                    break;
                case 3:
                    inSignUpMenu = false;
                    break;
                default:
                    cout << "Invalid choice! Enter 1-3.\n";
                    break;
                }
            }
            break;
        }


        case 3:
            cout << "Exiting program. Goodbye!\n";
            return;

        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}

void staffMainMenu(const string& username) {
    int choice;
    string input;

    while (true) {
        cout << "\n===== EVENT ORGANIZER STAFF MENU =====\n";
        cout << "Welcome, " << username << "!\n\n";
        cout << "1. Manage Event Staff\n";
        cout << "2. Event Menu\n";
        cout << "3. Crisis Management\n";
        cout << "4. Reports\n";
        cout << "5. Logout\n";
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
            eventMenu(username);
            break;
        case 3:
            crisisMenu();
            break;
        case 4:
            reportMenu();
            break;
        case 5:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

