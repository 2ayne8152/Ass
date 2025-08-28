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
            string name, quantityStr, location;

            getline(ss, name, ',');
            getline(ss, quantityStr, ',');
            getline(ss, location);

            // Trim whitespace
            auto trim = [](string& s) {
                s.erase(0, s.find_first_not_of(" \t"));
                s.erase(s.find_last_not_of(" \t") + 1);
                };

            trim(name);
            trim(quantityStr);
            trim(location);

            // Convert and add to list
            try {
                int quantity = quantityStr.empty() ? 0 : stoi(quantityStr);
                equipmentList.push_back(Equipment(name, quantity, location));
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
            file << eq.name << ", " << eq.quantity << ", " << eq.location << endl;
        }
        file.close();
    }
}

void displayAllEquipment() {
    cout << "\n===== EQUIPMENT INVENTORY =====\n";
    cout << "Equipment Name         Quantity  Location\n";
    cout << "-----------------------------------------\n";

    if (equipmentList.empty()) {
        cout << "No equipment in inventory.\n";
        return;
    }

    for (const auto& eq : equipmentList) {
        cout << setw(20) << left << eq.name << " "
            << setw(8) << eq.quantity << " "
            << eq.location << endl;
    }
}

void addEquipment() {
    string name, location;
    int quantity;

    cout << "\nEnter equipment name: ";
    getline(cin, name);

    cout << "Enter quantity to add: ";
    cin >> quantity;
    cin.ignore();

    cout << "Enter location (Storage or stage ID like S1): ";
    getline(cin, location);

    if (location.empty()) {
        location = "Storage";
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
        equipmentList.push_back(Equipment(name, quantity, location));
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
        equipmentList.push_back(Equipment(name, quantity, toLocation));
    }

    saveEquipmentToFile();
}

void equipmentMenu() {
    loadEquipmentFromFile();
    int choice;

    while (true) {
        cout << "\n===== EQUIPMENT MANAGEMENT =====\n";
        cout << "1. View All Equipment\n";
        cout << "2. Add Equipment\n";
        cout << "3. Remove Equipment\n";
        cout << "4. Transfer Equipment\n";
        cout << "5. Back to Crisis Menu\n";
        cout << "Choose an option: ";

        cin >> choice;
        cin.ignore();

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
            saveEquipmentToFile();
            return;
        default:
            cout << "Invalid choice! PLease try again.\n";
        }
    }
}