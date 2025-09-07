#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <ctime>
#include <iomanip>
#include "events.h"
#include "util.h"
#include "stage.h"
using namespace std;

void eventMenu(const string& username);
void createEvent(vector<Event>& events, const vector<Stage>& stages, const string& username);
void viewEvents(const vector<Event>& events, const string& username);
void makePayment(vector<Event>& events, const string& username);
void saveEventsToFile(const vector<Event>& events);
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
		clearScreen();
		cout << "==== Event Menu ====\n";
		cout << "1. Create Event\n";
		cout << "2. Make Payment\n";
		cout << "3. Edit Events\n";
		cout << "4. View Events\n";
		cout << "5. Quit\n";
		cout << "Select an option: ";

		int selection;
		cin >> selection;
		inputCheck(selection, 1, 5, "Invalid Input! Please Retry [1-5]: ");

		switch (selection) {
		case 1:
			createEvent(events, stages, username);
			saveEventsToFile(events);
			break;

		case 2:
			makePayment(events, username);
			break;

		case 3:
			editEvents(events, username);
			saveEventsToFile(events);
			break;

		case 4:
			viewEvents(events, username);
			break;

		case 5:
			condition = false;
			clearScreen();
			return;
		default:
			pauseScreen();
		}
	} while (condition);
}

void createEvent(vector<Event>& events, const vector<Stage>& stages, const string& username) {
	clearScreen();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	string name, date, startTime, endTime;
	double ticketPrice;
	int availableTickets;
	int id = events.size() + 1;

	cout << "\n=== Create New Event (Enter Q anytime to cancel) ===\n";

	// Input Event Name
	do {
		cout << "Enter Event Name: ";
		getline(cin, name);
		if (name == "Q" || name == "q") return;
		if (name.empty()) cout << "Event name cannot be empty!\n";
	} while (name.empty());

	// Show Available Stages
	vector<int> operationalStages;
	cout << "\n=== Available Stages ===\n";
	for (size_t i = 0; i < stages.size(); ++i) {
		cout << i + 1 << ". " << stages[i].stageName
			<< " (Capacity: " << stages[i].capacity
			<< ", Price/Day: RM" << fixed << setprecision(2) << stages[i].pricePerDay
			<< ", Status: " << (stages[i].isOperational ? "Operational" : "Not Operational") << ")\n";
		if (stages[i].isOperational) operationalStages.push_back(i);
	}

	if (operationalStages.empty()) {
		cout << "\n No operational stages available. Please contact the admin.\n";
		return;
	}

	// Stage Selection
	int stageChoice;
	do {
		cout << "Select a stage by number (Q to cancel): ";
		string input;
		getline(cin, input);
		if (input == "Q" || input == "q") return;

		try {
			stageChoice = stoi(input);
		}
		catch (...) {
			stageChoice = -1;
		}

		if (stageChoice < 1 || stageChoice >(int)stages.size() || !stages[stageChoice - 1].isOperational) {
			cout << "Invalid selection! Please choose a valid stage.\n";
			stageChoice = -1;
		}
	} while (stageChoice < 1);

	string venue = stages[stageChoice - 1].stageName;
	availableTickets = stages[stageChoice - 1].capacity;
	double venueFee = stages[stageChoice - 1].pricePerDay;

	// Input Date
	do {
		cout << "\nEnter Date (DD-MM-YYYY, Q to cancel): ";
		getline(cin, date);
		if (date == "Q" || date == "q") return;
		if (!isValidDate(date)) cout << "Invalid date format or out-of-range date. Please retry.\n";
	} while (!isValidDate(date));

	// Input Start Time
	do {
		cout << "\nEnter Start Time (HH:MM, Q to cancel): ";
		getline(cin, startTime);
		if (startTime == "Q" || startTime == "q") return;
		if (!isValidTime(startTime)) cout << "Invalid start time. Use 24-hour format (HH:MM).\n";
	} while (!isValidTime(startTime));

	// Input End Time
	do {
		cout << "Enter End Time (HH:MM, Q to cancel): ";
		getline(cin, endTime);
		if (endTime == "Q" || endTime == "q") return;

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

	// Conflict check
	bool conflict = false;
	for (const auto& e : events) {
		if (e.venue == venue && e.date == date) {
			int eStartH, eStartM, eEndH, eEndM;
			sscanf_s(e.startTime.c_str(), "%d:%d", &eStartH, &eStartM);
			sscanf_s(e.endTime.c_str(), "%d:%d", &eEndH, &eEndM);
			int newStart = stoi(startTime.substr(0, 2)) * 60 + stoi(startTime.substr(3, 2));
			int newEnd = stoi(endTime.substr(0, 2)) * 60 + stoi(endTime.substr(3, 2));
			int existingStart = eStartH * 60 + eStartM;
			int existingEnd = eEndH * 60 + eEndM;
			if ((newStart < existingEnd) && (existingStart < newEnd)) {
				conflict = true;
				break;
			}
		}
	}

	if (conflict) {
		clearScreen();
		cout << "+------------------------------+\n";
		cout << "| Scheduling conflict detected |\n";
		cout << "+------------------------------+\n";
		pauseScreen();
		return;
	}

	// Ticket Price
	do {
		cout << "\nEnter Ticket Price (RM, Q to cancel): ";
		string input;
		getline(cin, input);
		if (input == "Q" || input == "q") return;

		try {
			ticketPrice = stod(input);
		}
		catch (...) {
			ticketPrice = -1;
		}

		if (ticketPrice <= 0) {
			cout << "Invalid price! Please enter a positive number.\n";
		}
	} while (ticketPrice <= 0);

	events.emplace_back(
		id, name, venue, date, startTime, endTime,
		"Upcoming", venueFee, username, ticketPrice, availableTickets
	);

	cout << "\nEvent created successfully!\n";
	pauseScreen();
}


void printLine(const vector<int>& widths) {
	cout << "+";
	for (int w : widths) {
		cout << string(w + 2, '-') << "+";
	}
	cout << "\n";
}

void viewEvents(const vector<Event>& events, const string& username) {
	clearScreen();
	cout << "\n=== Your Booking History ===\n";

	bool hasEvents = false;

	// Adjusted column widths (Organizer widened)
	vector<int> widths = { 5, 15, 15, 12, 12, 12, 10, 12, 12, 15 };
	vector<string> headers = {
		"ID", "Name", "Venue", "Date", "Time",
		"Status", "Payment", "Price(RM)", "Tickets", "Organizer"
	};

	// Print header
	printLine(widths);
	cout << "|";
	for (size_t i = 0; i < headers.size(); i++) {
		cout << " " << left << setw(widths[i]) << headers[i] << " |";
	}
	cout << "\n";
	printLine(widths);

	for (const auto& e : events) {
		if (e.organizerName == username) {
			hasEvents = true;
			cout << "| " << left << setw(widths[0]) << e.id
				<< " | " << setw(widths[1]) << e.name
				<< " | " << setw(widths[2]) << e.venue
				<< " | " << setw(widths[3]) << e.date
				<< " | " << setw(widths[4]) << (e.startTime + "-" + e.endTime)
				<< " | " << setw(widths[5]) << e.status
				<< " | " << setw(widths[6]) << e.paymentStatus
				<< " | " << setw(widths[7]) << fixed << setprecision(2) << e.ticketPrice
				<< " | " << setw(widths[8]) << e.availableTickets
				<< " | " << setw(widths[9]) << e.organizerName
				<< " |\n";
			printLine(widths);
		}
	}

	if (!hasEvents) {
		cout << "You haven't created any events yet.\n";
	}

	pauseScreen();
}

void makePayment(vector<Event>& events, const string& username) {
	clearScreen();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << "\n=== Make Payment ===\n";

	bool hasUnpaid = false;

	// Column widths aligned with headers
	vector<int> widths = { 5, 15, 15, 12, 15, 12, 12, 15 };
	vector<string> headers = {
		"ID", "Name", "Venue", "Date", "Time",
		"Status", "Payment", "Venue Fee(RM)"
	};

	// Print header
	printLine(widths);
	cout << "|";
	for (size_t i = 0; i < headers.size(); i++) {
		cout << " " << left << setw(widths[i]) << headers[i] << " |";
	}
	cout << "\n";
	printLine(widths);

	// List unpaid events
	for (const auto& e : events) {
		if (e.organizerName == username && e.paymentStatus == "Unpaid") {
			hasUnpaid = true;
			cout << "| " << left << setw(widths[0]) << e.id
				<< " | " << setw(widths[1]) << e.name
				<< " | " << setw(widths[2]) << e.venue
				<< " | " << setw(widths[3]) << e.date
				<< " | " << setw(widths[4]) << (e.startTime + "-" + e.endTime)
				<< " | " << setw(widths[5]) << e.status
				<< " | " << setw(widths[6]) << e.paymentStatus
				<< " | " << setw(widths[7]) << fixed << setprecision(2) << e.venueFee
				<< " |\n";
			printLine(widths);
		}
	}

	if (!hasUnpaid) {
		cout << "\nNo unpaid events found.\n";
		pauseScreen();
		return;
	}

	// Ask for event ID
	cout << "\nEnter the Event ID to pay for (Q to cancel): ";
	string input;
	getline(cin, input);
	if (input == "Q" || input == "q") return;

	int id;
	try {
		id = stoi(input);
	}
	catch (...) {
		cout << "Invalid input.\n";
		pauseScreen();
		return;
	}

	auto it = find_if(events.begin(), events.end(), [id, &username](Event& e) {
		return e.id == id && e.organizerName == username && e.paymentStatus == "Unpaid";
		});

	if (it == events.end()) {
		cout << "Invalid Event ID or payment already completed.\n";
		pauseScreen();
		return;
	}

	// Simulate payment
	it->paymentStatus = "Paid";
	cout << "\nPayment successful for Event ID " << id << " (" << it->name << ").\n";

	pauseScreen();
}

string to_string_fixed(double value, int precision) {
	ostringstream out;
	out << fixed << setprecision(precision) << value;
	return out.str();
}

void editEvents(vector<Event>& events, const string& username) {
	clearScreen();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	vector<int> editableIds;

	cout << "\n=== Your Upcoming Events ===\n";

	bool hasEvents = false;

	// Print table header
	cout << "+----+----------------------+------------+----------+-------------------+\n";
	cout << "| ID | Name                 | Date       | Status   | Ticket Price (RM) |\n";
	cout << "+----+----------------------+------------+----------+-------------------+\n";

	for (const auto& e : events) {
		if (e.status == "Upcoming" && e.organizerName == username) {
			hasEvents = true;
			editableIds.push_back(e.id);

			cout << "| " << setw(2) << left << e.id << " "
				<< "| " << setw(20) << left << e.name.substr(0, 20) << " "
				<< "| " << setw(10) << left << e.date << " "
				<< "| " << setw(8) << left << e.status << " "
				<< "| " << setw(17) << right << (to_string_fixed(e.ticketPrice, 2)) << " |\n";
		}
	}

	cout << "+----+----------------------+------------+----------+-------------------+\n";


	if (!hasEvents) {
		cout << "\nNo upcoming events available to edit.\n";
		pauseScreen();
		return;
	}

	int id;
	cout << "\nEnter Event ID to edit (Q to cancel): ";
	string input;
	getline(cin, input);
	if (input == "Q" || input == "q") return;

	try {
		id = stoi(input);
	}
	catch (...) {
		cout << "Invalid input.\n";
		pauseScreen();
		return;
	}

	auto it = find_if(events.begin(), events.end(), [id, &username](const Event& e) {
		return e.id == id && e.status == "Upcoming" && e.organizerName == username;
		});

	if (it == events.end()) {
		cout << "You cannot edit this event.\n";
		pauseScreen();
		return;
	}

	Event& e = *it;
	bool condition = true;
	do {
		cout << "\n=== Editing Event: " << e.name << " (ID: " << e.id << ") ===\n";
		cout << "1. Edit Name\n2. Edit Venue\n3. Edit Date\n4. Edit Start Time\n";
		cout << "5. Edit End Time\n6. Edit Status\n7. Edit Ticket Price\n8. Quit\n";
		cout << "Select an option (Q to cancel): ";

		string choice;
		getline(cin, choice);
		if (choice == "Q" || choice == "q") return;

		int selection;
		try { selection = stoi(choice); }
		catch (...) { selection = -1; }

		switch (selection) {
		case 1:
			cout << "Enter new name (Q to cancel): ";
			getline(cin, e.name);
			if (e.name == "Q" || e.name == "q") return;
			break;
		case 2:
			cout << "Enter new venue (Q to cancel): ";
			getline(cin, e.venue);
			if (e.venue == "Q" || e.venue == "q") return;
			break;
		case 3:
			do {
				cout << "Enter new date (DD-MM-YYYY, Q to cancel): ";
				getline(cin, e.date);
				if (e.date == "Q" || e.date == "q") return;
				if (!isValidDate(e.date)) cout << "Invalid date format!\n";
			} while (!isValidDate(e.date));
			break;
		case 4:
			do {
				cout << "Enter new start time (HH:MM, Q to cancel): ";
				getline(cin, e.startTime);
				if (e.startTime == "Q" || e.startTime == "q") return;
				if (!isValidTime(e.startTime)) cout << "Invalid time format!\n";
			} while (!isValidTime(e.startTime));
			break;
		case 5:
			do {
				cout << "Enter new end time (HH:MM, Q to cancel): ";
				getline(cin, e.endTime);
				if (e.endTime == "Q" || e.endTime == "q") return;
				if (!isValidTime(e.endTime)) cout << "Invalid time format!\n";
			} while (!isValidTime(e.endTime));
			break;
		case 6:
			do {
				cout << "Enter new status (Upcoming/Ongoing/Completed, Q to cancel): ";
				getline(cin, e.status);
				if (e.status == "Q" || e.status == "q") return;
				for (auto& c : e.status) c = tolower(c);
				if (e.status != "upcoming" && e.status != "ongoing" && e.status != "completed") {
					cout << "Invalid status!\n";
				}
				else {
					e.status[0] = toupper(e.status[0]);
					break;
				}
			} while (true);
			break;
		case 7:
			do {
				cout << "Enter new ticket price (Q to cancel): RM";
				string priceInput;
				getline(cin, priceInput);
				if (priceInput == "Q" || priceInput == "q") return;
				try {
					e.ticketPrice = stod(priceInput);
				}
				catch (...) {
					e.ticketPrice = -1;
				}
			} while (e.ticketPrice <= 0);
			break;

		case 8:
			condition = false;
			break;
		default:
			cout << "Invalid selection!\n";
			pauseScreen();
			break;
		}
	} while (condition);

	pauseScreen();
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
	ifstream inFile(EVENTS_FILE);
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
	// Expected format: DD-MM-YYYY
	if (date.size() != 10 || date[2] != '-' || date[5] != '-') {
		return false; // Wrong format
	}

	int day, month, year;
	char dash1, dash2;

	stringstream ss(date);
	ss >> day >> dash1 >> month >> dash2 >> year;

	if (ss.fail() || dash1 != '-' || dash2 != '-') {
		return false; // Invalid parsing
	}

	// Validate basic ranges
	if (month < 1 || month > 12 || day < 1 || year < 1900) {
		return false;
	}

	// Days in each month
	int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// Leap year check for February
	bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	if (isLeap && month == 2) {
		daysInMonth[1] = 29;
	}

	if (day > daysInMonth[month - 1]) {
		return false;
	}

	// Get today's date
	time_t t = time(0);
	tm today{};
	localtime_s(&today, &t);

	int currentYear = today.tm_year + 1900;
	int currentMonth = today.tm_mon + 1;
	int currentDay = today.tm_mday;

	// Convert both dates to "days since epoch" for comparison
	tm selectedDate{};
	selectedDate.tm_year = year - 1900;
	selectedDate.tm_mon = month - 1;
	selectedDate.tm_mday = day;

	time_t selectedTime = mktime(&selectedDate);
	time_t todayTime = mktime(&today);

	// Past dates are invalid
	if (difftime(selectedTime, todayTime) < 0) {
		return false;
	}

	// Limit booking up to 1 year ahead (customizable)
	tm oneYearLater = today;
	oneYearLater.tm_year += 1;
	time_t oneYearLaterTime = mktime(&oneYearLater);

	if (difftime(selectedTime, oneYearLaterTime) > 0) {
		return false;
	}

	return true;
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