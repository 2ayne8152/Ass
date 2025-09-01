#include "report.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

// --- Helper: Validate export file extension ---
bool isTxtFile(const string& filename) {
    if (filename.size() < 4) return false;
    return filename.substr(filename.size() - 4) == ".txt";
}

// --- Event Report ---
void generateEventReport() {
    ifstream file("events.txt");
    if (!file.is_open()) {
        cout << "No events found.\n";
        return;
    }

    cout << "\n===== EVENT REPORT =====\n";
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

// --- Staff Report ---
void generateStaffReport() {
    ifstream file("staff.txt");
    if (!file.is_open()) {
        cout << "No staff found.\n";
        return;
    }

    cout << "\n===== STAFF REPORT =====\n";
    string username, email, password;
    while (file >> username >> email >> password) {
        cout << "Staff: " << username << " | Email: " << email << endl;
    }
    file.close();
}

// --- Equipment Report ---
void generateEquipmentReport() {
    ifstream file("equipment.txt");
    if (!file.is_open()) {
        cout << "No equipment found.\n";
        return;
    }

    cout << "\n===== EQUIPMENT REPORT =====\n";
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

// --- Crisis Report ---
void generateCrisisReport() {
    ifstream file("crisis.txt");
    if (!file.is_open()) {
        cout << "No crisis records found.\n";
        return;
    }

    cout << "\n===== CRISIS REPORT =====\n";
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

// --- Export Report ---
void exportReport(const string& reportName, const string& filename) {
    if (!isTxtFile(filename)) {
        cout << "Error: Reports can only be exported as .txt files!\n";
        return;
    }

    ifstream src(reportName + ".txt");
    if (!src.is_open()) {
        cout << "Error: Could not find " << reportName << ".txt to export.\n";
        return;
    }

    ofstream dest(filename);
    if (!dest.is_open()) {
        cout << "Error: Could not create " << filename << ".\n";
        return;
    }

    dest << src.rdbuf();
    cout << "Report exported successfully to " << filename << "!\n";

    src.close();
    dest.close();
}

// --- Menu ---
void reportMenu() {
    int choice;
    string input;

    while (true) {
        cout << "\n===== REPORT MENU =====\n";
        cout << "1. Generate Event Report\n";
        cout << "2. Generate Staff Report\n";
        cout << "3. Generate Equipment Report\n";
        cout << "4. Generate Crisis Report\n";
        cout << "5. Back to Main Menu\n";
        cout << "Choose an option: ";

        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1:
            generateEventReport();  
            break;
        case 2:
            generateStaffReport();  
            break;
        case 3:
            generateEquipmentReport();  
            break;
        case 4:
            generateCrisisReport();  
            break;
        case 5:
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

