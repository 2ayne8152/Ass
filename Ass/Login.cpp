#include "login.h"
#include "user.h"
#include "report.h"
#include "encryption.h"
#include <conio.h>  
#include "util.h"

void staffMainMenu(const string& username);
void organizerMainMenu(const string& username);
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
    string filename;
    if (role == "staff") filename = "staff.txt";
    else if (role == "user") filename = "users.txt";
    else if (role == "organizer") filename = "organizers.txt";
    else return false;

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

bool findUserFile(const string& usernameOrEmail, const string& role, string& filenameOut) {
    if (role == "staff") filenameOut = "staff.txt";
    else if (role == "user") filenameOut = "users.txt";
    else if (role == "organizer") filenameOut = "organizers.txt";
    else return false;

    ifstream file(filenameOut);
    if (!file.is_open()) return false;

    string line, u, e, p;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> u >> e >> p;
        if (usernameOrEmail == u || usernameOrEmail == e) {
            return true; // found
        }
    }
    return false; // not found
}

string getHiddenInput(const string& prompt) {
    cout << prompt;
    string password;
    char ch;

    while (true) {
        ch = _getch();

        if (ch == 13) { // Enter key
            cout << endl;
            break;
        }
        else if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; // erase last '*'
            }
        }
        else {
            password.push_back(ch);
            cout << '*'; // mask with '*'
        }
    }

    return password;
}

void signUp(const string& role) {
    string username, email, password, confirmPassword;

    cout << "\n===== "
        << (role == "staff" ? "STAFF" : (role == "organizer" ? "ORGANIZER" : "USER"))
        << " SIGN UP =====" << endl;
    cout << "(Type '0' at any time to cancel)\n";

    // Staff passkey check
    if (role == "staff") {
        string passkey;
        while (true) {
            passkey = getHiddenInput("Enter staff passkey: ");

            if (passkey == "0") {
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
        while (true) {
            username = getInput("Enter username: ");
            if (username == "0")
                return;

            if (!isValidUsername(username)) {
                cout << "Invalid username. Must be at least 3 characters (letters, numbers, underscore).\n";
                continue;
            }
            break;
        }

        while (true) {
            email = getInput("Enter email: ");
            if (email == "0")
                return;

            if (!isValidEmail(email)) {
                cout << "Invalid email format.\n";
                continue;
            }
            break;
        }

        while (true) {
            password = getHiddenInput("Enter password: ");
            if (password == "0")
                return;

            if (!isValidPassword(password)) {
                cout << "Invalid password. Must be at least 6 characters, include a letter and a number.\n";
                continue;
            }

            confirmPassword = getHiddenInput("Confirm password: ");
            if (confirmPassword == "0")
                return;

            if (confirmPassword != password) {
                cout << "Passwords do not match. Please try again.\n";
                continue;
            }
            break;
        }

        if (isDuplicate(username, email, role)) {
            cout << "Username or email already exists. Try again.\n";
            continue;
        }
        break;
    }
    
    // Hash password before saving
    string hashedPassword = SHA256::hash(password);

    string filename; 
    if (role == "staff") 
        filename = "staff.txt";
    else if 
        (role == "user") filename = "users.txt";
    else if 
        (role == "organizer") filename = "organizers.txt";

    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << username << " " << email << " " << hashedPassword << endl;
        file.close();
        cout << (role == "staff" ? "STAFF" : (role == "organizer" ? "ORGANIZER" : "USER")) << " sign up successful!\n";
    }
    else {
        cout << "Error: Could not open file.\n";
    }        
             
}


void login(const string& role) {
    string usernameOrEmail, password;
    bool success = false;

    cout << "\n===== "
        << (role == "staff" ? "STAFF" : (role == "organizer" ? "ORGANIZER" : "USER"))
        << " LOGIN =====" << endl;
    cout << "(Type '0' at any time to cancel)\n";

    string filename; 
    if (role == "staff") 
        filename = "staff.txt";
    else if (role == "user") 
        filename = "users.txt";
    else if (role == "organizer") 
        filename = "organizers.txt";

    while (true) {
        usernameOrEmail = getInput("Enter username or email: ");
        if (usernameOrEmail == "0")
            return;
        password = getHiddenInput("Enter password: ");
        if (password == "0")
            return;
        // Hash the input password
        string hashedPassword = SHA256::hash(password);

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
            if ((usernameOrEmail == u || usernameOrEmail == e) && hashedPassword == p) {
                success = true;
                break;
            }
        }
        file.close();

        if (success) {
            cout << "\nLogin successful! Welcome, " << usernameOrEmail << ".\n";
            if (role == "staff") {
                staffMainMenu(usernameOrEmail);
            }
            else if (role == "organizer") {
                organizerMainMenu(usernameOrEmail);
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

void forgetPassword() {
    while (true) {
        clearScreen();
        cout << "\n===== FORGOT PASSWORD =====\n";
        cout << "Select your role:\n";
        cout << "1. Staff\n";
        cout << "2. User\n";
        cout << "3. Organizer\n";
        cout << "4. Back to Home\n";
        cout << "Choose an option: ";
        string roleChoice, role, filename;
        getline(cin, roleChoice);

        if (roleChoice == "1") 
        { 
            role = "staff"; filename = "staff.txt"; }
        else if (roleChoice == "2") 
        { 
            role = "user"; filename = "users.txt"; 
        }
        else if (roleChoice == "3") 
        { 
            role = "organizer"; filename = "organizers.txt"; 
}
        else if (roleChoice == "4") 
            return;  // exit to home page
        else {
            cout << "Invalid choice! Please enter 1-4.\n";
            pauseInterface();
            continue; // back to role menu
        }

        while (true) {
            string usernameOrEmail = getInput("Enter your username or email (0 to go back): ");
            if (usernameOrEmail == "0") break; // back to role selection

            // --- check if user exists in file ---
            ifstream file(filename);
            if (!file.is_open()) {
                cout << "Error opening file.\n";
                pauseInterface();
                break;
            }

            bool found = false;
            vector<string> lines;
            string line, u, e, p;
            while (getline(file, line)) {
                stringstream ss(line);
                ss >> u >> e >> p;
                if (usernameOrEmail == u || usernameOrEmail == e) {
                    found = true;
                }
                lines.push_back(line);
            }
            file.close();

            if (!found) {
                cout << "Username/email not found in system.\n";
                pauseInterface();
                continue; // retry username/email input
            }

            // --- enter new password ---
            string newPassword, confirmPassword;
            while (true) {
                newPassword = getHiddenInput("Enter new password (0 to go back): ");
                if (newPassword == "0") break; // back to username/email

                if (!isValidPassword(newPassword)) {
                    cout << "Invalid password. Must be at least 6 chars, with a letter & number.\n";
                    continue;
                }

                confirmPassword = getHiddenInput("Confirm new password (0 to go back): ");
                if (confirmPassword == "0") break;

                if (confirmPassword != newPassword) {
                    cout << "Passwords do not match. Try again.\n";
                    continue;
                }

                // --- update password ---
                string hashedPassword = SHA256::hash(newPassword);
                ofstream outFile(filename);
                for (const string& l : lines) {
                    stringstream ss(l);
                    ss >> u >> e >> p;
                    if (usernameOrEmail == u || usernameOrEmail == e) {
                        outFile << u << " " << e << " " << hashedPassword << endl;
                    }
                    else {
                        outFile << l << endl;
                    }
                }
                outFile.close();

                cout << "Password reset successful! You can now log in.\n";
                pauseInterface();
                return; //back to home page after success
            }
        }
    }
}


void homePageMenu(const string& message) {
    while (true) {
        clearScreen();
        cout << R"ASCII(
     ██╗ █████╗ ███╗   ███╗    ██████╗  ██╗      █████╗ ███╗   ██╗
     ██║██╔══██╗████╗ ████║    ██╔══██╗ ██║     ██╔══██╗████╗  ██║
     ██║███████║██╔████╔██║    ██████╔╝ ██║     ███████║██╔██╗ ██║
██   ██║██╔══██║██║╚██╔╝██║    ██╔═══╝  ██║     ██╔══██║██║╚██╗██║
╚█████╔╝██║  ██║██║ ╚═╝ ██║    ██║      ███████╗██║  ██║██║ ╚████║
 ╚════╝ ╚═╝  ╚═╝╚═╝     ╚═╝    ╚═╝      ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝
    )ASCII" << endl;

        cout << "\n===== HOME PAGE =====\n";
        cout << "1. Log In\n";
        cout << "2. Sign Up\n";
        cout << "3. Forgot Password\n";
        cout << "4. Exit\n";

        string choiceStr;
        int choice = 0;

        // input loop (only re-asks on error)
        while (true) {
            cout << "Choose an option: ";
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
                continue; // stay in this loop
            }

            choice = stoi(choiceStr);
            if (choice < 1 || choice > 3) {
                cout << "Invalid choice! Enter 1-3.\n";
                continue;
            }
            break; // valid choice
        }

        // handle valid choice
        switch (choice) {
        case 1: { // login menu
            clearScreen();
            bool inLoginMenu = true;
            while (inLoginMenu) {
                cout << "\n===== LOG IN MENU =====\n";
                cout << "1. Log In (Staff)\n";
                cout << "2. Log In (User)\n";
                cout << "3. Log In (Organizer)\n";
                cout << "4. Back to Home\n";
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
                    cout << "Invalid input! Enter 1-4.\n";
                    continue;
                }

                int loginChoice = stoi(loginInput);
                switch (loginChoice) {
                case 1:
                    login("staff");
                    pauseInterface();
                    inLoginMenu = false;
                    break;
                case 2:
                    login("user");
                    pauseInterface();
                    inLoginMenu = false;
                    break;
                case 3:
                    login("organizer");
                    pauseInterface();
                    inLoginMenu = false;
                    break;
                case 4:
                    inLoginMenu = false;
                    break;
                default:
                    cout << "Invalid choice! Enter 1-4.\n";
                    break;
                }
            }
            break;
        }

        case 2: { // SIGN UP MENU
            clearScreen();
            bool inSignUpMenu = true;
            while (inSignUpMenu) {
                cout << "\n===== SIGN UP MENU =====\n";
                cout << "1. Sign Up (Staff)\n";
                cout << "2. Sign Up (User)\n";
                cout << "3. Sign Up (Organizer)\n";
                cout << "4. Back to Home\n";
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
                    cout << "Invalid input! Enter 1-4.\n";
                    continue;
                }

                int signUpChoice = stoi(signUpInput);
                switch (signUpChoice) {
                case 1:
                    signUp("staff");
                    pauseInterface();
                    inSignUpMenu = false;
                    break;
                case 2:
                    signUp("user");
                    pauseInterface();
                    inSignUpMenu = false;
                    break;
                case 3:
                    signUp("organizer");
                    pauseInterface();
                    inSignUpMenu = false;
                    break;
                case 4:
                    inSignUpMenu = false;
                    break;
                default:
                    cout << "Invalid choice! Enter 1-4.\n";
                    break;
                }
            }
            break;
        }
        case 3: 
            forgetPassword();
            break;
        case 4:
            cout << "Exiting program. Goodbye!\n";
            return;
        }
    }
}

void updatePassword(const string& role, const string& usernameOrEmail) {
    string filename;
    if (role == "staff") filename = "staff.txt";
    else if (role == "user") filename = "users.txt";
    else if (role == "organizer") filename = "organizers.txt";
    else return;

    vector<string> fileData;
    ifstream fileIn(filename);
    if (!fileIn.is_open()) {
        cout << "Error: Could not open file.\n";
        return;
    }

    string line;
    while (getline(fileIn, line)) {
        fileData.push_back(line);
    }
    fileIn.close();

    // Find user
    bool found = false;
    for (auto& record : fileData) {
        string u, e, p;
        stringstream ss(record);
        ss >> u >> e >> p;
        if (usernameOrEmail == u || usernameOrEmail == e) {
            found = true;
            string currentPassword;
            while (true) {
                currentPassword = getHiddenInput("Enter current password (0 to cancel): ");
                if (currentPassword == "0") {
                    cout << "Password update cancelled.\n";
                    return; // exit the function
                }

                string hashedCurrent = SHA256::hash(currentPassword);
                if (hashedCurrent != p) {
                    cout << "Incorrect current password. Try again.\n";
                    continue; // retry
                }
                break; // correct password, exit loop
            }
            
            // Ask new password
            string newPassword, confirmPassword;
            while (true) {
                newPassword = getHiddenInput("Enter new password: ");
                if (!isValidPassword(newPassword)) {
                    cout << "Invalid password. Must be at least 6 characters, include a letter and a number.\n";
                    continue;
                }
                confirmPassword = getHiddenInput("Confirm new password: ");
                if (newPassword != confirmPassword) {
                    cout << "Passwords do not match. Try again.\n";
                    continue;
                }
                break;
            }

            // Update password
            string hashedNew = SHA256::hash(newPassword);
            record = u + " " + e + " " + hashedNew;
            cout << "Password updated successfully!\n";
            cin.get();
            break;
        }
    }

    if (!found) {
        cout << "User not found.\n";
        return;
    }

    // Save updated file
    ofstream fileOut(filename);
    if (!fileOut.is_open()) {
        cout << "Error: Could not save file.\n";
        return;
    }

    for (const auto& record : fileData) {
        fileOut << record << endl;
    }
    fileOut.close();

    pauseInterface();
}


void staffMainMenu(const string& username) {

    vector<Event> events;
    vector<Ticket> tickets;
    loadEventsFromFile(events);
    loadTicketsFromFile(tickets);
    int choice;
    string input;

    while (true) {
        clearScreen();
        cout << "\n===== EVENT ORGANIZER STAFF MENU =====\n";
        cout << "Welcome, " << username << "!\n\n";
        cout << "1. Manage Event Staff\n";
        cout << "2. Crisis Management\n";
        cout << "3. Reports\n";
        cout << "4. Update Password\n";
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
            crisisMenu();
            break;
        case 3:
            reportMenu(events, tickets);
            break;
        case 4 : 
            updatePassword("staff", username);
            break;
        case 5:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

