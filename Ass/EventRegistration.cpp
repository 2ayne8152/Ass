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
		cout << "Select an option: ";
	
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

	do {
		cout << "Enter Event Name: ";
		getline(cin, name);
		if (name.empty()) cout << "Event name cannot be empty!\n";
	} while (name.empty());

	do {
		cout << "Enter Venue: ";
		getline(cin, venue);
		if (venue.empty()) cout << "Venue cannot be empty!\n";
	} while (venue.empty());

	do {
		cout << "Enter Date (DD-MM-YYYY): ";
		getline(cin, date);
		if (!isValidDate(date)) cout << "Invalid date format or out-of-range date. Please retry.\n";
	} while (!isValidDate(date));

	do {
		cout << "Enter Start Time (HH:MM): ";
		getline(cin, startTime);
		if (!isValidTime(startTime)) cout << "Invalid start time. Use 24-hour format (HH:MM).\n";
	} while (!isValidTime(startTime));

	do {
		cout << "Enter End Time (HH:MM): ";
		getline(cin, endTime);
		if (!isValidTime(endTime)) {
			cout << "Invalid end time. Use 24-hour format (HH:MM).\n";
			continue;
		}

		int startH, startM, endH, endM;
		sscanf(startTime.c_str(), "%d:%d", &startH, &startM);
		sscanf(endTime.c_str(), "%d:%d", &endH, &endM);

		if ((endH * 60 + endM) <= (startH * 60 + startM)) {
			cout << "End time must be after start time.\n";
			continue;
		}
		break;
	} while (true);

	do {
		cout << "Enter Organizer Name: ";
		getline(cin, organizer);
		if (organizer.empty()) cout << "Organizer name cannot be empty!\n";
	} while (organizer.empty());

	do {
		cout << "Enter Status (Upcoming/Ongoing/Completed): ";
		getline(cin, status);
		for (auto& c : status) c = tolower(c);
		if (status != "upcoming" && status != "ongoing" && status != "completed") {
			cout << "Invalid status! Please enter Upcoming, Ongoing, or Completed.\n";
			continue;
		}
		break;
	} while (true);

	events.emplace_back(id, name, venue, date, startTime, endTime, status, organizer);
	cout << "\nEvent created successfully!\n";
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
	vector<Event*> editableEvents;
	for (auto& e : events) {
		if (e.status == "Upcoming") {
			editableEvents.push_back(&e);
		}
	}

	if (editableEvents.empty()) {
		cout << "\nNo events to edit.\n";
		return;
	}

	cout << "\n=== Editable Events (Upcoming) ===\n";
	for (const auto& e : editableEvents) {
		cout << "ID: " << e->id << " | " << e->name << " | " << e->date << " | " << e->status << "\n";
	}

	int id;
	cout << "\nEnter Event ID to edit: ";
	cin >> id;
	inputCheck(id, 1, events.size(), "Invalid Event ID. Please try again: ");

	Event* eventToEdit = nullptr;
	for (auto& e : editableEvents) {
		if (e->id == id) {
			eventToEdit = e;
			break;
		}
	}

	if (!eventToEdit) {
		cout << "Event ID not editable or does not exist.\n";
		return;
	}

	Event& e = *eventToEdit;
	cin.ignore();

	bool condition = true;
	do {
		cout << "\n=== Editing Event: " << e.name << " (ID: " << e.id << ") ===\n";
		cout << "1. Edit Name\n";
		cout << "2. Edit Venue\n";
		cout << "3. Edit Date\n";
		cout << "4. Edit Start Time\n";
		cout << "5. Edit End Time\n";
		cout << "6. Edit Status\n";
		cout << "7. Quit\n";
		cout << "Select an option: ";

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
			do {
				cout << "Enter new date (DD-MM-YYYY): ";
				getline(cin, e.date);
				if (!isValidDate(e.date)) cout << "Invalid date format! Please retry.\n";
			} while (!isValidDate(e.date));
			break;
		}
		case 4: {
			do {
				cout << "Enter new start time (HH:MM): ";
				getline(cin, e.startTime);
				if (!isValidTime(e.startTime)) cout << "Invalid time format! Please retry.\n";
			} while (!isValidTime(e.startTime));
			break;
		}
		case 5: {
			do {
				cout << "Enter new end time (HH:MM): ";
				getline(cin, e.endTime);
				if (!isValidTime(e.endTime)) cout << "Invalid time format! Please retry.\n";
			} while (!isValidTime(e.endTime));
			break;
		}
		case 6: {
			do {
				cout << "Enter new status (Upcoming/Ongoing/Completed): ";
				getline(cin, e.status);
				for (auto& c : e.status) c = tolower(c);
				if (e.status != "upcoming" && e.status != "ongoing" && e.status != "completed") {
					cout << "Invalid status! Please retry.\n";
				}
				else break;
			} while (true);
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

bool isValidDate(const string& date) {
	regex datePattern(R"(^([0-2][0-9]|3[01])-(0[1-9]|1[0-2])-(\d{4})$)");
	if (!regex_match(date, datePattern)) return false;

	// Extract day, month, year for logical validation
	int day, month, year;
	sscanf(date.c_str(), "%d-%d-%d", &day, &month, &year);

	// Basic range checks
	if (year < 1900 || year > 2100) return false;

	// Days per month check (ignoring leap year first)
	int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	// Leap year adjustment
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
		daysInMonth[1] = 29;
	}

	return (day >= 1 && day <= daysInMonth[month - 1]);
}

bool isValidTime(const string& time) {
	regex timePattern(R"(^([01]\d|2[0-3]):([0-5]\d)$)");
	return regex_match(time, timePattern);
}
