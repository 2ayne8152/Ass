﻿#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include "events.h"
#include "util.h"
#include "stage.h"
#include <iomanip>
using namespace std;

void eventMenu(const string& username);
void createEvent(vector<Event>& events, const vector<Stage>& stages, const string& username);
void viewEvents(const vector<Event>& events, const string& username);
void makePayment(vector<Event>& events, const string& username);
void saveEventsToFile(const vector<Event>& events);
void inputCheck(int& input, double min, double max, string errormsg);
void editEvents(vector<Event>& events, const string& username);
bool isValidTime(const string& time);
bool isValidDate(const string& date);
void loadStagesFromFile(vector<Stage>& stages);

const string EVENTS_FILE = "events.txt";
const string STAGES_FILE = "stages.txt";

void eventMenu(const string& username) {
	vector<Event> events;
	vector<Stage> stages;
	loadEventsFromFile(events);
	loadStagesFromFile(stages);
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
				createEvent(events, username);
				saveEventsToFile(events);
				break;
			}
			case 2: {
				makePayment(events, username);
				break;
			}
			case 3: {
				editEvents(events);
				saveEventsToFile(events);
				break;
			}	  
			case 4: {
				viewEvents(events, username);
				break;
			}
			case 5: {
				condition = false;
				return;
			}
		}
	} while (condition);
}

void createEvent(vector<Event>& events, const vector<Stage>& stages, const string& username) {
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover input

	string name, date, startTime, endTime;
	double ticketPrice;
	int availableTickets;
	int id = events.size() + 1;

	cout << "\n=== Create New Event ===\n";

	// Input Event Name
	do {
		cout << "Enter Event Name: ";
		getline(cin, name);
		if (name.empty()) cout << "Event name cannot be empty!\n";
	} while (name.empty());

	// Choose Venue from Available Stages
	if (stages.empty()) {
		cout << " No stages available. Please add stages first!\n";
		return;
	}

	cout << "\n=== Available Stages ===\n";
	for (size_t i = 0; i < stages.size(); ++i) {
		cout << i + 1 << ". " << stages[i].stageName
			<< " (Capacity: " << stages[i].capacity
			<< ", Status: " << (stages[i].isOperational ? "Operational" : "Not Operational") << ")\n";
	}

	int stageChoice;
	do {
		cout << "Select a stage by number: ";
		cin >> stageChoice;

		if (cin.fail() || stageChoice < 1 || stageChoice >(int)stages.size()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid selection! Please choose a valid stage.\n";
		}
		else if (!stages[stageChoice - 1].isOperational) {
			cout << " This stage is not operational. Please select another one.\n";
			stageChoice = -1;
		}
	} while (stageChoice < 1 || stageChoice >(int)stages.size());

	string venue = stages[stageChoice - 1].stageName;
	availableTickets = stages[stageChoice - 1].capacity;

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	// Input Date
	do {
		cout << "Enter Date (DD-MM-YYYY): ";
		getline(cin, date);
		if (!isValidDate(date)) cout << "Invalid date format or out-of-range date. Please retry.\n";
	} while (!isValidDate(date));

	// Input Start Time
	do {
		cout << "Enter Start Time (HH:MM): ";
		getline(cin, startTime);
		if (!isValidTime(startTime)) cout << "Invalid start time. Use 24-hour format (HH:MM).\n";
	} while (!isValidTime(startTime));

	// Input End Time
	do {
		cout << "Enter End Time (HH:MM): ";
		getline(cin, endTime);
		if (!isValidTime(endTime)) {
			cout << "Invalid end time. Use 24-hour format (HH:MM).\n";
			continue;
		}

		int startH, startM, endH, endM;
		sscanf_s(startTime.c_str(), "%d:%d", &startH, &startM);
		sscanf_s(endTime.c_str(), "%d:%d", &endH, &endM);

		if ((endH * 60 + endM) <= (startH * 60 + startM)) {
			cout << "End time must be after start time.\n";
			continue;
		}
		break;
	} while (true);

	// Input Ticket Price
	do {
		cout << "Enter Ticket Price (RM): ";
		cin >> ticketPrice;
		if (cin.fail() || ticketPrice <= 0) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid price! Please enter a positive number.\n";
		}
	} while (ticketPrice <= 0);

	// Save Event
	events.emplace_back(
		id,
		name,
		venue,
		date,
		startTime,
		endTime,
		"Upcoming",       // Default status
		username,         // Organizer
		ticketPrice,
		availableTickets
	);

	cout << "\n Event created successfully!\n";
	cout << "   Venue: " << venue << " | Tickets Available: " << availableTickets << "\n";
}

void viewEvents(const vector<Event>& events, const string& username) {
	cout << "\n=== Your Booking History ===\n";
	bool hasEvents = false;

	for (const auto& e : events) {
		if (e.organizerName == username) {
			hasEvents = true;
			cout << "\nEvent ID: " << e.id
				<< "\nName: " << e.name
				<< "\nVenue: " << e.venue
				<< "\nDate: " << e.date
				<< "\nTime: " << e.startTime << " - " << e.endTime
				<< "\nOrganizer: " << e.organizerName
				<< "\nStatus: " << e.status
				<< "\nPayment Status: " << e.paymentStatus
				<< "\nTicket Price: RM" << fixed << setprecision(2) << e.ticketPrice
				<< "\nAvailable Tickets: " << e.availableTickets
				<< "\n-----------------------------";
		}
	}

	if (!hasEvents) {
		cout << "You haven't created any events yet.\n";
	}
	cout << "\n";
}

void makePayment(vector<Event>& events, const string& username) {
	cout << "\n=== Unpaid Events ===\n";

	vector<int> unpaidIndexes;
	for (int i = 0; i < events.size(); i++) {
		if (events[i].paymentStatus == "Unpaid" && events[i].organizerName == username) {
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

void editEvents(vector<Event>& events, const string& username) {
	vector<int> editableIds;

	cout << "\n=== Your Upcoming Events ===\n";
	for (const auto& e : events) {
		if (e.status == "Upcoming" && e.organizerName == username) {
			editableIds.push_back(e.id);
			cout << "ID: " << e.id << " | " << e.name << " | " << e.date
				<< " | " << e.status << " | Ticket Price: RM" << e.ticketPrice << "\n";
		}
	}

	if (editableIds.empty()) {
		cout << "\nNo upcoming events available to edit for your account.\n";
		return;
	}

	int id;
	cout << "\nEnter Event ID to edit: ";
	cin >> id;
	inputCheck(id, 1, events.size(), "Invalid Event ID. Please try again: ");

	auto it = find_if(events.begin(), events.end(), [id, &username](const Event& e) {
		return e.id == id && e.status == "Upcoming" && e.organizerName == username;
		});

	if (it == events.end()) {
		cout << "You cannot edit this event. It may belong to another user or is not upcoming.\n";
		return;
	}

	Event& e = *it;
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
		cout << "8. Edit Ticket Price\n";
		cout << "Select an option: ";

		int selection;
		cin >> selection;
		inputCheck(selection, 1, 8, "Invalid Input! Please Retry [1-8]: ");
		cin.ignore();

		switch (selection) {
		case 1:
			cout << "Enter new name: ";
			getline(cin, e.name);
			break;
		case 2:
			cout << "Enter new venue: ";
			getline(cin, e.venue);
			break;
		case 3:
			do {
				cout << "Enter new date (DD-MM-YYYY): ";
				getline(cin, e.date);
				if (!isValidDate(e.date)) cout << "Invalid date format! Please retry.\n";
			} while (!isValidDate(e.date));
			break;
		case 4:
			do {
				cout << "Enter new start time (HH:MM): ";
				getline(cin, e.startTime);
				if (!isValidTime(e.startTime)) cout << "Invalid time format! Please retry.\n";
			} while (!isValidTime(e.startTime));
			break;
		case 5:
			do {
				cout << "Enter new end time (HH:MM): ";
				getline(cin, e.endTime);
				if (!isValidTime(e.endTime)) cout << "Invalid time format! Please retry.\n";
			} while (!isValidTime(e.endTime));
			break;
		case 6:
			do {
				cout << "Enter new status (Upcoming/Ongoing/Completed): ";
				getline(cin, e.status);
				for (auto& c : e.status) c = tolower(c);
				if (e.status != "upcoming" && e.status != "ongoing" && e.status != "completed") {
					cout << "Invalid status! Please retry.\n";
				}
				else {
					e.status[0] = toupper(e.status[0]);
					break;
				}
			} while (true);
			break;
		case 7:
			condition = false;
			break;
		case 8:
			cout << "Enter new ticket price: RM";
			cin >> e.ticketPrice;
			while (cin.fail() || e.ticketPrice <= 0) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "Invalid price! Please enter a positive number: RM";
				cin >> e.ticketPrice;
			}
			cout << "Ticket price updated successfully!\n";
			break;
		}
	} while (condition);
}

void saveEventsToFile(const vector<Event>& events) {
	ofstream outFile(EVENTS_FILE);
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
			<< e.organizerName << "|"
			<< e.ticketPrice << "|"
			<< e.availableTickets << "\n";
	}

	outFile.close();
}

void loadEventsFromFile(vector<Event>& events) {
	ifstream inFile(FILE_NAME);
	if (!inFile) return; // No file yet, skip loading

	events.clear();
	string line;

	while (getline(inFile, line)) {
		if (line.empty()) continue;

		stringstream ss(line);
		Event e;
		string token;

		// Read ID safely
		if (!getline(ss, token, '|')) continue;
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

		// Ticket price
		getline(ss, token, '|');
		try { e.ticketPrice = stod(token); }
		catch (...) { e.ticketPrice = 0.0; }

		// Available tickets
		getline(ss, token, '|');
		try { e.availableTickets = stoi(token); }
		catch (...) { e.availableTickets = 0; }

		events.push_back(e);
	}

	inFile.close();
}

bool isValidDate(const string& date) {
	regex datePattern(R"(^([0-2][0-9]|3[01])-(0[1-9]|1[0-2])-(\d{4})$)");
	if (!regex_match(date, datePattern)) return false;

	int day = stoi(date.substr(0, 2));
	int month = stoi(date.substr(3, 2));
	int year = stoi(date.substr(6, 4));

	if (year < 1900 || year > 2100) return false;

	int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		daysInMonth[1] = 29;

	return day >= 1 && day <= daysInMonth[month - 1];
}

bool isValidTime(const string& time) {
	regex timePattern(R"(^([01]\d|2[0-3]):([0-5]\d)$)");
	return regex_match(time, timePattern);
}

void loadStagesFromFile(vector<Stage>& stages) {
	ifstream inFile(STAGES_FILE);
	if (!inFile) {
		cout << "Warning: No stages file found. Starting with an empty list.\n";
		return;
	}

	stages.clear();
	string line;

	while (getline(inFile, line)) {
		if (line.empty()) continue;

		stringstream ss(line);
		Stage s;
		string token;

		// Stage Number
		getline(ss, s.stageNumber, ',');
		s.stageNumber.erase(0, s.stageNumber.find_first_not_of(" \t"));
		s.stageNumber.erase(s.stageNumber.find_last_not_of(" \t") + 1);

		// Stage Name
		getline(ss, s.stageName, ',');
		s.stageName.erase(0, s.stageName.find_first_not_of(" \t"));
		s.stageName.erase(s.stageName.find_last_not_of(" \t") + 1);

		// Capacity
		getline(ss, token, ',');
		token.erase(0, token.find_first_not_of(" \t"));
		token.erase(token.find_last_not_of(" \t") + 1);
		s.capacity = stoi(token);

		// Is Operational (true/false)
		getline(ss, token, ',');
		token.erase(0, token.find_first_not_of(" \t"));
		token.erase(token.find_last_not_of(" \t") + 1);
		for (auto& c : token) c = tolower(c);
		s.isOperational = (token == "true" || token == "1" || token == "yes");

		// Current Event
		getline(ss, s.currentEvent, ',');
		s.currentEvent.erase(0, s.currentEvent.find_first_not_of(" \t\""));
		s.currentEvent.erase(s.currentEvent.find_last_not_of(" \t\"") + 1);

		// Price Per Day
		getline(ss, token, ',');
		token.erase(0, token.find_first_not_of(" \t"));
		token.erase(token.find_last_not_of(" \t") + 1);
		s.pricePerDay = stod(token);

		stages.push_back(s);
	}

	inFile.close();
}