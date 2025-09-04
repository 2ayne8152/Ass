#include "stage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "equipment.h"
using namespace std;

vector<Stage> stages;
void equipmentMenu();

void loadStagesFromFile() {
    ifstream file("stages.txt");
    stages.clear();

    if (file.is_open()) {
        string line;
        int lineNumber = 0;

        while (getline(file, line)) {
            lineNumber++;

            if (line.empty() || line.find_first_not_of(" \t\n\r") == string::npos) {
                continue;
            }

            stringstream ss(line);
            string stageNum, stageName, capacity, operational, currentEvent, pricePerDayStr;

            if (!getline(ss, stageNum, ',') ||
                !getline(ss, stageName, ',') ||
                !getline(ss, capacity, ',') ||
                !getline(ss, operational, ',') ||
                !getline(ss, currentEvent, ',') ||
                !getline(ss, pricePerDayStr)) {

                cerr << "Warning: Invalid format on line " << lineNumber << ": " << line << endl;
                continue;
            }

            auto trim = [](string& s) {
                s.erase(0, s.find_first_not_of(" \t\n\r"));
                s.erase(s.find_last_not_of(" \t\n\r") + 1);
                };

            trim(stageNum);
            trim(stageName);
            trim(capacity);
            trim(operational);
            trim(currentEvent);
            trim(pricePerDayStr);

            Stage stage;
            stage.stageNumber = stageNum;
            stage.stageName = stageName;

            try {
                stage.capacity = capacity.empty() ? 0 : stoi(capacity);
            }
            catch (...) {
                stage.capacity = 0;
                cerr << "Warning: Invalid capacity on line " << lineNumber << endl;
            }

            stage.isOperational = (operational == "true" || operational == "1");

            if (currentEvent.empty() || currentEvent == "\"\"" || currentEvent == "\"\"") {
                stage.currentEvent = "";
            }
            else {
                if (currentEvent.front() == '"' && currentEvent.back() == '"') {
                    stage.currentEvent = currentEvent.substr(1, currentEvent.length() - 2);
                }
                else {
                    stage.currentEvent = currentEvent;
                }
            }

            try {
                stage.pricePerDay = pricePerDayStr.empty() ? 0.0 : stod(pricePerDayStr);
            }
            catch (...) {
                stage.pricePerDay = 0.0;
                cerr << "Warning: Invalid pricePerDay on line " << lineNumber << endl;
            }

            stages.push_back(stage);
        }
        file.close();
    }
    else {
        cout << "Error! Cannot open stages.txt file." << endl;
    }
}

void saveStagesToFile() {
    ofstream file("stages.txt");
    if (file.is_open()) {
        for (const auto& stage : stages) {
            file << stage.stageNumber << ", " << stage.stageName << ", " << stage.capacity << ", "
                << (stage.isOperational ? "true" : "false") << ", "
                << (stage.currentEvent.empty() ? "\"\"" : stage.currentEvent) << ", "
                << fixed << setprecision(2) << stage.pricePerDay << endl;
        }
        file.close();
    }
}

void displayAllStages() {
    system("cls");
    cout << endl;

    string title = " ALL VENUE STAGES ";
    int totalWidth = 85;
    int sideWidth = (totalWidth - title.size()) / 2;

    cout << string(sideWidth, '=') << title << string(totalWidth - sideWidth - title.size(), '=') << endl;
    cout << endl;

    if (stages.empty()) {
        cout << "No stages found.\n";
        return;
    }

    cout << left
        << setw(5) << "No."
        << setw(8) << "ID"
        << setw(18) << "Stage Name"
        << setw(8) << "Capacity"
        << setw(12) << "Status"
        << setw(15) << "Current Event"
        << setw(15) << "Price/Day" << endl;

    cout << string(totalWidth, '-') << endl;

    for (size_t i = 0; i < stages.size(); i++) {
        cout << left
            << setw(5) << (i + 1)
            << setw(8) << stages[i].stageNumber
            << setw(18) << stages[i].stageName
            << setw(8) << stages[i].capacity
            << setw(12) << (stages[i].isOperational ? "Operational" : "BROKEN")
            << setw(15) << (stages[i].currentEvent.empty() ? "Available" : stages[i].currentEvent)
            << setw(15) << fixed << setprecision(2) << stages[i].pricePerDay
            << endl;
    }
}

void assignEventToStage() {
    displayAllStages();

    string stageId;
    string eventName;

    cout << "\nEnter stage ID (e.g., S1, S2): ";
    getline(cin, stageId);

    Stage* selectedStage = nullptr;
    for (auto& stage : stages) {
        if (stage.stageNumber == stageId) {
            selectedStage = &stage;
            break;
        }
    }

    if (!selectedStage) {
        cout << "Invalid stage ID!\n";
        return;
    }

    if (!selectedStage->isOperational) {
        cout << "This stage is currently broken and cannot be assigned events!\n";
        return;
    }

    if (!selectedStage->currentEvent.empty()) {
        cout << "This stage already has an event: " << selectedStage->currentEvent << endl;
        cout << "Do you want to replace it? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();

        if (choice != 'y' && choice != 'Y') {
            return;
        }
    }

    cout << "Enter event name to assign: ";
    getline(cin, eventName);

    selectedStage->currentEvent = eventName;
    saveStagesToFile();

    cout << "Event '" << eventName << "' assigned to " << selectedStage->stageName << " successfully!\n";
}

void updateStageStatus() {
    displayAllStages();

    string stageId;
    cout << "\nEnter stage ID to update status (e.g., S1, S2): ";
    getline(cin, stageId);

    Stage* selectedStage = nullptr;
    for (auto& stage : stages) {
        if (stage.stageNumber == stageId) {
            selectedStage = &stage;
            break;
        }
    }

    if (!selectedStage) {
        cout << "Invalid stage ID!\n";
        return;
    }

    cout << "Current status: " << (selectedStage->isOperational ? "Operational" : "BROKEN") << endl;
    cout << "Set new status (1 for Operational, 0 for Broken): ";
    int status;
    cin >> status;
    cin.ignore();

    selectedStage->isOperational = (status == 1);

    if (!selectedStage->isOperational && !selectedStage->currentEvent.empty()) {
        cout << "WARNING: This stage has an ongoing event: " << selectedStage->currentEvent << endl;
        cout << "The event will be cancelled due to stage being out of service.\n";
        selectedStage->currentEvent = "";
    }

    saveStagesToFile();
    cout << "Stage status updated successfully!\n";
}

void stageMenu() {
    loadStagesFromFile();
    int choice;

    while (true) {
        cout << "\n===== STAGE MANAGEMENT =====\n";
        cout << "1. View All Stages\n";
        cout << "2. Assign Event to Stage\n";
        cout << "3. Update Stage Status\n";
        cout << "4. Back to Main Menu\n";
        cout << "Choose an option: ";

        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            displayAllStages();
            break;
        case 2:
            assignEventToStage();
            break;
        case 3:
            updateStageStatus();
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice!\n";
        }
    }
}

void crisisMenu() {
    int choice;

    while (true) {
        cout << "\n===== CRISIS MANAGEMENT =====\n";
        cout << "1. Stage Management\n";
        cout << "2. Equipment Management\n";
        cout << "3. Back to Staff Main Menu\n";
        cout << "Choose an option: ";

        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            stageMenu(); 
            break;
        case 2:
            equipmentMenu();
            break;
        case 3:
            return; 
        default:
            cout << "Invalid choice!\n";
        }
    }
}