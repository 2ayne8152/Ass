#include "event_staff.h"
#include "events.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iomanip>
using namespace std;

bool isValidContact(const string& contact) {
    regex pattern(R"(\d{3}-\d{7})"); // exactly 3 digits, dash, 7 digits
    return regex_match(contact, pattern);
}

bool validateShiftFormat(const string& shiftTime) {
    regex pattern("^([01]?[0-9]|2[0-3]):([0-5][0-9])->([01]?[0-9]|2[0-3]):([0-5][0-9])$");
    return regex_match(shiftTime, pattern);
}

vector<EventStaff> loadEventStaff() {
    vector<EventStaff> staffList;
    ifstream file("event_staff.txt");

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, role, contact, event, shift;

            // Read until the end of line
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, role, ',');
            getline(ss, contact, ',');
            getline(ss, event, ',');
            getline(ss, shift);

            staffList.emplace_back(id, name, role, contact, event, shift);
        }
        file.close();
    }

    return staffList;
}

void saveEventStaff(const vector<EventStaff>& staffList) {
    ofstream file("event_staff.txt");

    if (file.is_open()) {
        for (const auto& staff : staffList) {
            file << staff.staffId << "," << staff.name << "," << staff.role << ","
                << staff.contactInfo << "," << staff.assignedEvent << "," << staff.shiftTime << endl;
        }
        file.close();
    }
}

void addEventStaff(vector<EventStaff>& staffList, const EventStaff& newStaff) {
    staffList.push_back(newStaff);
}

bool removeEventStaff(vector<EventStaff>& staffList, const string& staffId) {
    auto it = remove_if(staffList.begin(), staffList.end(),
        [&staffId](const EventStaff& staff) { return staff.staffId == staffId; });

    if (it != staffList.end()) {
        staffList.erase(it, staffList.end());
        return true;
    }

    return false;
}

EventStaff* findEventStaffById(vector<EventStaff>& staffList, const string& staffId) {
    for (auto& staff : staffList) {
        if (staff.staffId == staffId) {
            return &staff;
        }
    }
    return nullptr;
}

vector<EventStaff> findEventStaffByEvent(const vector<EventStaff>& staffList, const string& eventName) {
    vector<EventStaff> result;

    for (const auto& staff : staffList) {
        if (staff.assignedEvent == eventName) {
            result.push_back(staff);
        }
    }

    return result;
}

int getLastStaffId() {
    ifstream file("last_staff_id.txt");
    int lastId = 0;

    if (file.is_open()) {
        file >> lastId;
        file.close();
    }

    return lastId;
}

void saveLastStaffId(int id) {
    ofstream file("last_staff_id.txt");

    if (file.is_open()) {
        file << id;
        file.close();
    }
}

string generateNextStaffId(const vector<EventStaff>& staffList) {
    int maxId = 0;

    for (const auto& staff : staffList) {
        if (staff.staffId.length() >= 2 && staff.staffId.substr(0, 2) == "ES") {
            try {
                int idNum = stoi(staff.staffId.substr(2));
                if (idNum > maxId) {
                    maxId = idNum;
                }
            }
            catch (const exception& e) {
                // If conversion fails, continue with next ID
                continue;
            }
        }
    }

    int nextId = maxId + 1;
    stringstream ss;
    ss << "ES" << setw(4) << setfill('0') << nextId;
    return ss.str();
}

void displayAllEventStaff(const vector<EventStaff>& staffList) {
    cout << "\n===== ALL EVENT STAFF =====\n";
    if (staffList.empty()) {
        cout << "No event staff members found.\n";
        return;
    }

    for (size_t i = 0; i < staffList.size(); i++) {
        cout << i + 1 << ". ID: " << staffList[i].staffId
            << ", Name: " << staffList[i].name
            << ", Role: " << staffList[i].role
            << ", Contact: " << staffList[i].contactInfo
            << ", Event: " << staffList[i].assignedEvent
            << ", Shift: " << staffList[i].shiftTime << "\n";
    }
}

void displayEventStaffForEvent(const vector<EventStaff>& staffList, const string& eventName) {
    cout << "\n===== STAFF FOR EVENT: " << eventName << " =====\n";

    vector<EventStaff> eventStaff = findEventStaffByEvent(staffList, eventName);

    if (eventStaff.empty()) {
        cout << "No staff assigned to this event.\n";
        return;
    }

    for (size_t i = 0; i < eventStaff.size(); i++) {
        cout << i + 1 << ". ID: " << eventStaff[i].staffId
            << ", Name: " << eventStaff[i].name
            << ", Role: " << eventStaff[i].role
            << ", Shift: " << eventStaff[i].shiftTime
            << ", Contact: " << eventStaff[i].contactInfo << "\n";
    }
}

void addEventStaffFromInput(vector<EventStaff>& staffList) {
    string id, name, role, contact, event, shift;

    cout << "\n===== ADD EVENT STAFF =====\n";

    id = generateNextStaffId(staffList);
    cout << "Automatically generated Staff ID: " << id << endl;

    if (findEventStaffById(staffList, id) != nullptr) {
        cout << "Error: Staff ID already exists!\n";
        return;
    }

    cout << "Enter Name: ";
    getline(cin, name);

    cout << "Enter Role: ";
    getline(cin, role);

    while (true) {
        cout << "Enter Contact Info (XXX-XXXXXXX): ";
        getline(cin, contact);

        if (contact == "exit") {
            cout << "Cancelled.\n";
            return;
        }

        if (!isValidContact(contact)) {
            cout << "Invalid format. Please use XXX-XXXXXXX (e.g., 012-3456789).\n";
            continue;
        }
        break; 
    }

    cout << "Enter Assigned Event: ";
    getline(cin, event);

    while (true) {
        cout << "Enter Shift Time (Format: HH:MM->HH:MM, e.g., 09:00->17:30): ";
        getline(cin, shift);

        if (validateShiftFormat(shift)) {
            cout << "Valid shift format!" << endl;
            break;
        }
        else {
            cout << "Invalid format! Please use HH:MM->HH:MM in 24-hour format (e.g., 09:00->17:30)" << endl;
        }
    };

    EventStaff newStaff(id, name, role, contact, event, shift);
    addEventStaff(staffList, newStaff);

    cout << "Event staff added successfully!\n";
}

void removeEventStaffFromInput(vector<EventStaff>& staffList) {
    string id;

    cout << "\n===== REMOVE EVENT STAFF =====\n";
    cout << "Enter Staff ID to remove: ";
    getline(cin, id);

    if (removeEventStaff(staffList, id)) {
        cout << "Event staff removed successfully!\n";
    }
    else {
        cout << "Staff ID not found.\n";
    }
}

void viewStaffForEvent(const vector<EventStaff>& staffList) {
    string eventName;

    cout << "\n===== VIEW STAFF FOR EVENT =====\n";
    cout << "Enter Event Name: ";
    getline(cin, eventName);

    displayEventStaffForEvent(staffList, eventName);
}

void searchStaffById(const vector<EventStaff>& staffList) {
    string id;

    cout << "\n===== SEARCH STAFF BY ID =====\n";
    cout << "Enter Staff ID to search: ";
    getline(cin, id);

    vector<EventStaff> nonConstList = staffList;
    EventStaff* staff = findEventStaffById(nonConstList, id);

    if (staff != nullptr) {
        cout << "\nStaff Found:\n";
        cout << "ID: " << staff->staffId << "\n";
        cout << "Name: " << staff->name << "\n";
        cout << "Role: " << staff->role << "\n";
        cout << "Contact: " << staff->contactInfo << "\n";
        cout << "Event: " << staff->assignedEvent << "\n";
        cout << "Shift: " << staff->shiftTime << "\n";
    }
    else {
        cout << "Staff ID not found.\n";
    }
}

void manageEventStaffMenu(const vector<EventStaff>& staffList) {
    int choice;
    string input;

    vector<EventStaff> workingList = staffList;

    while (true) {
        cout << "\n===== EVENT STAFF MANAGEMENT =====\n";
        cout << "1. View All Event Staff\n";
        cout << "2. Add Event Staff\n";
        cout << "3. Remove Event Staff\n";
        cout << "4. View Staff for Specific Event\n";
        cout << "5. Search Staff by ID\n";
        cout << "6. Back to Main Menu\n";
        cout << "Choose an option: ";

        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1:
            displayAllEventStaff(workingList);
            break;
        case 2:
            addEventStaffFromInput(workingList);
            saveEventStaff(workingList);
            break;
        case 3:
            removeEventStaffFromInput(workingList);
            saveEventStaff(workingList);
            break;
        case 4:
            viewStaffForEvent(workingList);
            break;
        case 5:
            searchStaffById(workingList);
            break;
        case 6:
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}