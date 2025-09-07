#include "equipment.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

vector<Equipment> equipmentList;

bool isValidStageLocation(const string& location);

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
    system("cls");
    cout << endl;

    string title = " EQUIPMENT INVENTORY ";
    int totalWidth = 85;
    int sideWidth = static_cast<int>((totalWidth - title.size()) / 2);

    cout << string(sideWidth, '=') << title << string(totalWidth - sideWidth - title.size(), '=') << endl;
    cout << endl;

    if (equipmentList.empty()) {
        cout << "No equipment in inventory.\n";
        return;
    }

    // Top border
    cout << "+" << string(5, '-') << "+" << string(20, '-') << "+" << string(10, '-')
        << "+" << string(27, '-') << "+" << string(15, '-') << "+" << endl;

    // Header row
    cout << "|" << left << setw(5) << "No." << "|"
        << setw(20) << "Equipment Name" << "|"
        << setw(10) << "Quantity" << "|"
        << setw(27) << "Location" << "|"
        << setw(15) << "Status" << "|" << endl;

    // Header separator
    cout << "+" << string(5, '-') << "+" << string(20, '-') << "+" << string(10, '-')
        << "+" << string(27, '-') << "+" << string(15, '-') << "+" << endl;

    // Data rows
    int index = 1;
    for (const auto& eq : equipmentList) {
        string displayStatus = eq.status;
        if (eq.location == "Store Room") {
            displayStatus = "In Storage";
        }

        cout << "|" << left << setw(5) << index++ << "|"
            << setw(20) << eq.name << "|"
            << setw(10) << eq.quantity << "|"
            << setw(27) << eq.location << "|"
            << setw(15) << displayStatus << "|" << endl;
    }

    // Bottom border
    cout << "+" << string(5, '-') << "+" << string(20, '-') << "+" << string(10, '-')
        << "+" << string(27, '-') << "+" << string(15, '-') << "+" << endl;
}

bool isValidStageLocation(const string& location) {
    if (location == "Store Room") return true;

    if (location.rfind("Stage", 0) == 0) {
        string numPart = location.substr(5);
        return !numPart.empty() && all_of(numPart.begin(), numPart.end(), ::isdigit);
    }
    return false;
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
            if (!isalpha(c) && c != ' ') {
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

    // Validate location (only StageX or Store Room allowed)
    while (true) {
        cout << "Enter location (Stage1, Stage2, ... or 'Store Room'): ";
        getline(cin, location);

        // Convert to proper case
        if (location == "store room" || location == "STOREROOM" || location == "storage") {
            location = "Store Room";
        }
        else if ((location[0] == 'S' || location[0] == 's') && location.size() > 1) {
            // Handle StageX format
            if (isdigit(location[1])) {
                location = "Stage" + location.substr(1);
            }
            else if (location.substr(0, 5) == "stage" || location.substr(0, 5) == "STAGE") {
                location[0] = 'S';
                for (size_t i = 1; i < location.size(); i++) {
                    location[i] = tolower(location[i]);
                }
            }
        }

        if (isValidStageLocation(location)) {
            break;
        }

        cout << "Invalid location! Please enter 'Store Room' or 'StageX' (e.g., Stage1, Stage2).\n";
    }

    bool found = false;
    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == location) {
            eq.quantity += quantity;
            // If adding to Store Room, set status to "In Storage"
            if (location == "Store Room") {
                eq.status = "In Storage";
            }
            found = true;
            break;
        }
    }

    if (!found) {
        string status = (location == "Store Room") ? "In Storage" : "Available";
        equipmentList.push_back(Equipment(name, quantity, location, status));
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

    cout << "Enter location (StageX or Store Room): ";
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

    // Validate quantity input
    while (true) {
        cout << "Enter quantity to transfer: ";
        string quantityStr;
        getline(cin, quantityStr);

        try {
            quantity = stoi(quantityStr);
            if (quantity <= 0) {
                cout << "Quantity must be greater than 0.\n";
                continue;
            }
            break;
        }
        catch (const exception& e) {
            cout << "Invalid input! Please enter a valid number.\n";
        }
    }

    // Validate destination location
    while (true) {
        cout << "Enter destination (StageX or Store Room): ";
        getline(cin, toLocation);

        // Convert to proper case
        if (toLocation == "store room" || toLocation == "STOREROOM" || toLocation == "storage") {
            toLocation = "Store Room";
        }
        else if ((toLocation[0] == 'S' || toLocation[0] == 's') && toLocation.size() > 1) {
            if (isdigit(toLocation[1])) {
                toLocation = "Stage" + toLocation.substr(1);
            }
            else if (toLocation.substr(0, 5) == "stage" || toLocation.substr(0, 5) == "STAGE") {
                toLocation[0] = 'S';
                for (size_t i = 1; i < toLocation.size(); i++) {
                    toLocation[i] = tolower(toLocation[i]);
                }
            }
        }

        if (isValidStageLocation(toLocation)) {
            break;
        }

        cout << "Invalid location! Please enter 'Store Room' or 'StageX' (e.g., Stage1, Stage2).\n";
    }

    Equipment* sourceEq = nullptr;
    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == fromLocation) {
            sourceEq = &eq;
            break;
        }
    }

    if (!sourceEq) {
        cout << "Equipment not found at the specified location!\n";
        return;
    }

    if (sourceEq->quantity < quantity) {
        cout << "Not enough equipment available for transfer! Only " << sourceEq->quantity << " available.\n";
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

    if (sourceEq->quantity == 0) {
        // Remove the source equipment if quantity becomes zero
        equipmentList.erase(remove_if(equipmentList.begin(), equipmentList.end(),
            [&](const Equipment& eq) {
                return eq.name == name && eq.location == fromLocation;
            }), equipmentList.end());
    }

    if (destEq) {
        destEq->quantity += quantity;
        // Update status based on destination
        if (toLocation == "Store Room") {
            destEq->status = "In Storage";
        }
        else if (destEq->status == "In Storage") {
            destEq->status = "Available";
        }
    }
    else {
        string status = (toLocation == "Store Room") ? "In Storage" : "Available";
        equipmentList.push_back(Equipment(name, quantity, toLocation, status));
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

    // Don't allow status change for equipment in Store Room
    if (location == "Store Room") {
        cout << "Cannot change status of equipment in Store Room. Status is always 'In Storage'.\n";
        return;
    }

    cout << "Enter new status (Available / In Use / Damaged / Under Repair): ";
    getline(cin, newStatus);

    for (auto& eq : equipmentList) {
        if (eq.name == name && eq.location == location) {
            if (location != "Store Room") {
                eq.status = newStatus;
                saveEquipmentToFile();
                cout << "Status updated successfully!\n";
            }
            else {
                cout << "Cannot change status of equipment in Store Room.\n";
            }
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