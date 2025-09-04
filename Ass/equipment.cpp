#include "equipment.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

vector<Equipment> equipmentList;

void loadEquipmentFromFile() {
    ifstream file("equipment.txt");
    equipmentList.clear();
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string name, quantityStr, location, status;

            getline(ss, name, ',');
            getline(ss, quantityStr, ',');
            getline(ss, location, ',');
            getline(ss, status);

            // Trim whitespace
            auto trim = [](string& s) {
                s.erase(0, s.find_first_not_of(" \t"));
                s.erase(s.find_last_not_of(" \t") + 1);
                };

            trim(name);
            trim(quantityStr);
            trim(location);
            trim(status);

            try {
                int quantity = quantityStr.empty() ? 0 : stoi(quantityStr);
                if (status.empty()) status = "Available"; // default
                equipmentList.push_back(Equipment(name, quantity, location, status));
            }
            catch (...) {
                cout << "Warning: Invalid equipment data: " << line << endl;
            }
        }
        file.close();
    }
    else {
        cout << "No equipment file found. Starting with empty inventory.\n";
    }
}

void saveEquipmentToFile() {
    ofstream file("equipment.txt");
    if (file.is_open()) {
        for (const auto& eq : equipmentList) {
            file << eq.name << ", " << eq.quantity << ", " << eq.location << ", " << eq.status << endl;
        }
        file.close();
    }
}

void displayAllEquipment() {
    cout << "\n===== EQUIPMENT INVENTORY =====\n";
    cout << left << setw(5) << "No."
        << setw(20) << "Equipment Name"
        << setw(10) << "Quantity"
        << setw(15) << "Location"
        << "Status\n";
    cout << "-------------------------------------------------------------\n";

    if (equipmentList.empty()) {
        cout << "No equipment in inventory.\n";
        return;
    }

    int index = 1;
    for (const auto& eq : equipmentList) {
        cout << left << setw(5) << index++
            << setw(20) << eq.name
            << setw(10) << eq.quantity
            << setw(15) << eq.location
            << eq.status << endl;
    }
}

void addEquipment() {
    string name, location, quantityStr;
    int quantity;

    // validate name (only alphabets and spaces allowed)
    while (true) {
        cout << "\nEnter equipment name: ";
        getline(cin, name);

        bool valid = !name.empty();
        for (char c : name) {
            if (!isalpha(c) && c != ' ') { // allow letters and spaces only
                valid = false;
                break;
            }
        }

        if (!valid) {
            cout << "Invalid name! Equipment name must contain only alphabets and spaces.\n";
            continue;
        }
        break;
    }

    // validate quantity (must be a positive integer)
    while (true) {
        cout << "Enter quantity to add: ";
        getline(cin, quantityStr);

        bool valid = !quantityStr.empty();
        for (char c : quantityStr) {
            if (!isdigit(c)) {
                valid = false;
                break;
            }
        }

        if (!valid) {
            cout << "Invalid input! Quantity must be a positive number.\n";
            continue;
        }

        quantity = stoi(quantityStr);
        if (quantity <= 0) {
            cout << "Quantity must be greater than 0.\n";
            continue;
        }
        break;
    }

    // Validate location (must be Storage or StageX)
    while (true) {
        cout << "Enter location (Storage or Stage ID like Stage1): ";
        getline(cin, location);

        if (location.empty()) {
            location = "Storage"; // default
            break; // exit loop
        }

        // normalize short form S1 to Stage1
        if ((location[0] == 'S' || location[0] == 's') && location.size() > 1 && isdigit(location[1])) {
            location = "Stage" + location.substr(1);
        }

        // check if it's exactly "Storage"
        if (location == "Storage") {
            break; // valid then exit loop
        }

        // check if its StageX with numeric part
        if (location.rfind("Stage", 0) == 0) { // starts with "Stage"
            string numPart = location.substr(5);
            bool validNum = !numPart.empty();
            for (char c : numPart) {
                if (!isdigit(c)) { validNum = false; break; }
            }
            if (validNum) {
                break; // valid ? exit loop
            }
        }

        cout << "Invalid location! Please enter 'Storage' or 'StageX' (e.g., Stage1, Stage2).\n";
    }

    bool found = false;
    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == location) {
            eq.quantity += quantity;
            found = true;
            break;
        }
    }

    if (!found) {
        equipmentList.push_back(Equipment(name, quantity, location, "Available"));
    }

    saveEquipmentToFile();
    cout << "Equipment added successfully!\n";
}


void removeEquipment() {
    displayAllEquipment();

    if (equipmentList.empty()) {
        return;
    }

    string name, location;
    int quantity;

    cout << "\nEnter equipment name to remove: ";
    getline(cin, name);

    cout << "Enter location (Storage or stage ID): ";
    getline(cin, location);

    cout << "Enter quantity to remove: ";
    cin >> quantity;
    cin.ignore();

    for (auto it = equipmentList.begin(); it != equipmentList.end(); ++it) {
        if (it->name == name && it->location == location) {
            if (quantity >= it->quantity) {
                equipmentList.erase(it);
                cout << "Equipment removed completely.\n";
            }
            else {
                it->quantity -= quantity;
                cout << quantity << " units removed. Remaining: " << it->quantity << endl;
            }
            saveEquipmentToFile();
            return;
        }
    }

    cout << "Equipment not found at specified location!\n";
}

void transferEquipment() {
    displayAllEquipment();

    if (equipmentList.empty()) {
        return;
    }

    string name, fromLocation, toLocation;
    int quantity;

    cout << "\nEnter equipment name to transfer: ";
    getline(cin, name);

    cout << "Enter current location: ";
    getline(cin, fromLocation);

    cout << "Enter quantity to transfer: ";
    cin >> quantity;
    cin.ignore();

    cout << "Enter destination (Storage or stage ID): ";
    getline(cin, toLocation);

    Equipment* sourceEq = nullptr;
    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == fromLocation) {
            sourceEq = &eq;
            break;
        }
    }

    if (!sourceEq || sourceEq->quantity < quantity) {
        cout << "Not enough equipment available for transfer!\n";
        return;
    }

    Equipment* destEq = nullptr;
    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == toLocation) {
            destEq = &eq;
            break;
        }
    }

    sourceEq->quantity -= quantity;

    if (destEq) {
        destEq->quantity += quantity;
    }
    else {
        equipmentList.push_back(Equipment(name, quantity, toLocation, "Available"));
    }

    saveEquipmentToFile();
    cout << "Equipment transferred successfully!\n";
}

void updateEquipmentStatus() {
    displayAllEquipment();
    if (equipmentList.empty()) return;

    string name, location, newStatus;
    cout << "\nEnter equipment name to update: ";
    getline(cin, name);
    cout << "Enter location: ";
    getline(cin, location);
    cout << "Enter new status (Available / In Use / Damaged / Under Repair): ";
    getline(cin, newStatus);

    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == location) {
            eq.status = newStatus;
            saveEquipmentToFile();
            cout << "Status updated successfully!\n";
            return;
        }
    }
    cout << "Equipment not found!\n";
}

void equipmentMenu() {
    loadEquipmentFromFile();

    while (true) {
        cout << "\n===== EQUIPMENT MANAGEMENT =====\n";
        cout << "1. View All Equipment\n";
        cout << "2. Add Equipment\n";
        cout << "3. Remove Equipment\n";
        cout << "4. Transfer Equipment\n";
        cout << "5. Update Equipment Status\n";
        cout << "6. Back to Crisis Menu\n";
        cout << "Choose an option: ";

        string input;
        getline(cin, input);

        bool isValid = !input.empty();
        for (char c : input) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }

        if (!isValid) {
            cout << "Invalid input! Please enter a number between 1 and 6.\n";
            continue;
        }

        int choice = stoi(input);

        switch (choice) {
        case 1:
            displayAllEquipment();
            break;
        case 2:
            addEquipment();
            break;
        case 3:
            removeEquipment();
            break;
        case 4:
            transferEquipment();
            break;
        case 5:
            updateEquipmentStatus();
            break;
        case 6:
            saveEquipmentToFile();
            return;
        default:
            cout << "Invalid choice! Please select 1-6.\n";
        }
    }
}

