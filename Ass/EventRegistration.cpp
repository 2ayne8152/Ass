#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include "abc.h"
using namespace std;

void eventMenu();
void createEvent(vector<Event>& events);
void viewEvents(const vector<Event>& events);
void makePayment(vector<Event>& events);
void saveEventsToFile(const vector<Event>& events);
void loadEventsFromFile(vector<Event>& events);
void inputCheck(int& input, double min, double max, string errormsg);

const string FILE_NAME = "events.txt";

int main() {
	eventMenu();
	return 0;
}

void eventMenu() {
	vector<Event> events;
	loadEventsFromFile(events);
	bool condition = true;

	do {
		cout << "==== Venue Booking Menu ====" << endl;
		cout << "1. Book Venue" << endl;
		cout << "2. Make Payment" << endl;
		cout << "3. View Booking History" << endl;
		cout << "4. Quit" << endl;
	
		int selection;
		cin >> selection;
		inputCheck(selection, 1, 4, "Invalid Input! Please Retry [1-4]: ");

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
			viewEvents(events);
			break;
		}
		case 4: {
			condition = false;
			return;
		}
		}
	} while (condition);
}

void createEvent(vector<Event>& events) {
	cin.ignore();  // Clear leftover newline from previous input

	string name, venue, date, startTime, endTime, status, organizer;
	int id = events.size() + 1; // Auto-increment ID

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