#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include "events.h"
using namespace std;

void eventMenu();
void createEvent(vector<Event>& events);
void viewEvents(const vector<Event>& events);
void makePayment(vector<Event>& events);
void saveEventsToFile(const vector<Event>& events);
void loadEventsFromFile(vector<Event>& events);
void inputCheck(int& input, double min, double max, string errormsg);
void editEvents(vector<Event>& events);

const string FILE_NAME = "events.txt";

void eventMenu() {
	vector<Event> events;
	loadEventsFromFile(events);
	bool condition = true;

	do {
		cout << "==== Venue Booking Menu ====" << endl;
		cout << "1. Book Venue" << endl;
		cout << "2. Make Payment" << endl;
		cout << "3. Edit Booking" << endl;
		cout << "4. View Booking History" << endl;
		cout << "5. Quit" << endl;
	
		int selection;
		cin >> selection;
		inputCheck(selection, 1, 5, "Invalid Input! Please Retry [1-5]: ");

		switch (selection) {
		case 1: {
			createEvent(events);
			saveEventsToFile(events);
			break;
		}
		case 2: {
			makePayment(events);
			break;
		}
		case 3: {
			editEvents(events);
			saveEventsToFile(events);
			break;
		}	  
		case 4: {
			viewEvents(events);
			break;
		}
		case 5: {
			condition = false;
			return;
		}
		}
	} while (condition);
}

void createEvent(vector<Event>& events) {
	cin.ignore();

	string name, venue, date, startTime, endTime, status, organizer;
	int id = events.size() + 1;

	cout << "\n=== Create New Event ===\n";
	cout << "Enter Event Name: ";
	getline(cin, name);

	cout << "Enter Venue: ";
	getline(cin, venue);

	cout << "Enter Date (DD-MM-YYYY): ";
	getline(cin, date);

	cout << "Enter Start Time (HH:MM): ";
	getline(cin, startTime);

	cout << "Enter End Time (HH:MM): ";
	getline(cin, endTime);

	cout << "Enter Organizer Name: ";
	getline(cin, organizer);

	cout << "Enter Status (Upcoming/Ongoing/Completed): ";
	getline(cin, status);

	events.emplace_back(id, name, venue, date, startTime, endTime, status, organizer);

	cout << "\n Event created successfully!\n";
}

void viewEvents(const vector<Event> &events) {
    cout << "\n=== Booking History ===\n";
    if (events.empty()) {
        cout << "No events have been booked yet.\n";
        return;
    }

    for (const auto &e : events) {
        cout << "\nEvent ID: " << e.id
             << "\nName: " << e.name
             << "\nVenue: " << e.venue
             << "\nDate: " << e.date
             << "\nTime: " << e.startTime << " - " << e.endTime
             << "\nOrganizer: " << e.organizerName
             << "\nStatus: " << e.status
             << "\n-----------------------------";
    }
    cout << "\n";
}

void makePayment(vector<Event>& events) {
	cout << "\n=== Unpaid Events ===\n";

	vector<int> unpaidIndexes;
	for (int i = 0; i < events.size(); i++) {
		if (events[i].paymentStatus == "Unpaid") {
			unpaidIndexes.push_back(i);
			cout << unpaidIndexes.size() << ". " // show 1-based selection
				<< "\nEvent ID: " << events[i].id
				<< "\nName: " << events[i].name
				<< "\nVenue: " << events[i].venue
				<< "\nDate: " << events[i].date
				<< "\nTime: " << events[i].startTime << " - " << events[i].endTime
				<< "\nOrganizer: " << events[i].organizerName
				<< "\nStatus: " << events[i].status
				<< "\nPayment Status: " << events[i].paymentStatus
				<< "\n-----------------------------";
		}
	}

	if (unpaidIndexes.empty()) {
		cout << "\nNo unpaid events available.\n";
		return;
	}

	int selection;
	cout << "\nSelect the number of the event to pay for: ";
	cin >> selection;

	// Validate user input
	while (cin.fail() || selection < 1 || selection > unpaidIndexes.size()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "Invalid selection. Please try again: ";
		cin >> selection;
	}

	int index = unpaidIndexes[selection - 1];
	events[index].paymentStatus = "Paid";

	cout << "\nPayment successful for event: " << events[index].name << "\n";
}

void editEvents(vector<Event>& events) {
	vector<Event> editableEvents;
	for (const auto& e : events) {
		if (e.status == "Upcoming") {
			editableEvents.push_back(e);
		}
	}
	if (events.empty()) {
		cout << "No events to edit.\n";
		return;
	}
	
	viewEvents(editableEvents);
	cout << "Enter Event ID to edit: ";
	int id;
	cin >> id;
	inputCheck(id, 1, events.size(), "Invalid Event ID. Please try again: ");

	//Find event by ID
	auto it = find_if(events.begin(), events.end(), 
		[id](const Event& e) { return e.id == id; });
	if (it == events.end()) {
		cout << "Event not found.\n";
		return;
	}

	Event& e = *it;
	cin.ignore();

	bool condition = true;

	cout << "Editing Event: " << e.name << "(ID: " << e.id << ")\n";
	do {
		cout << "1. Edit Name\n";
		cout << "2. Edit Venue\n";
		cout << "3. Edit Date\n";
		cout << "4. Edit Start Time\n";
		cout << "5. Edit End Time\n";
		cout << "6. Edit Status\n";
		cout << "7. Quit\n";

		int selection;
		cin >> selection;
		inputCheck(selection, 1, 7, "Invalid Input! Please Retry [1-7]: ");
		cin.ignore();
		switch (selection) {
			case 1: {
				cout << "Enter new name: ";
				getline(cin, e.name);
				break;
			}
			case 2: {
				cout << "Enter new venue: ";
				getline(cin, e.venue);
				break;
			}
			case 3: {
				cout << "Enter new date (DD-MM-YYYY): ";
				getline(cin, e.date);
				break;
			}
			case 4: {
				cout << "Enter new start time (HH:MM): ";
				getline(cin, e.startTime);
				break;
			}
			case 5: {
				cout << "Enter new end time (HH:MM): ";
				getline(cin, e.endTime);
				break;
			}
			case 6: {
				cout << "Enter new status (Upcoming/Ongoing/Completed): ";
				getline(cin, e.status);
				break;
			}
			case 7: {
				condition = false;
				break;
			}
		}
	} while (condition);
}

void saveEventsToFile(const vector<Event>& events) {
	ofstream outFile(FILE_NAME);
	if (!outFile) {
		cout << "Error: Unable to save events.\n";
		return;
	}

	for (const auto& e : events) {
		outFile << e.id << "|"
			<< e.name << "|"
			<< e.venue << "|"
			<< e.date << "|"
			<< e.startTime << "|"
			<< e.endTime << "|"
			<< e.status << "|"
			<< e.paymentStatus << "|"
			<< e.organizerName << "\n";
	}
	outFile.close();
}

void loadEventsFromFile(vector<Event>& events) {
	ifstream inFile(FILE_NAME);
	if (!inFile) return; // No file yet, skip loading

	string line;
	while (getline(inFile, line)) {
		if (line.empty()) continue; // skip empty lines

		stringstream ss(line);
		Event e;
		string token;

		if (!getline(ss, token, '|')) continue; // ID missing
		try { e.id = stoi(token); }
		catch (...) { continue; }

		getline(ss, e.name, '|');
		getline(ss, e.venue, '|');
		getline(ss, e.date, '|');
		getline(ss, e.startTime, '|');
		getline(ss, e.endTime, '|');
		getline(ss, e.status, '|');
		getline(ss, e.paymentStatus, '|');
		getline(ss, e.organizerName, '|');

		events.push_back(e);
	}
	inFile.close();
}

void inputCheck(int& input, double min, double max, string errormsg) {
	while (cin.fail() || input < min || input > max) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << '\n' << errormsg;
		cin >> input;
	}
}