#include "user.h"
#include "events.h"

void viewEvents(const vector<Event>& events);
void bookTickets(const string& username);
void viewBookings(const string& username);
void cancelBooking(const string& username);

int generateTicketID() {
    static int lastTicketID = 0;
    return ++lastTicketID;
}

void userMainMenu(const string& username) {
    while (true) {
        cout << "\n===== USER MAIN MENU =====\n";
        cout << "1. View Events\n";
        cout << "2. Book Tickets\n";
        cout << "3. View My Bookings\n";
        cout << "4. Cancel Booking\n";
        cout << "5. Logout\n";
        cout << "Choose an option: ";
        string choiceStr;
        getline(cin, choiceStr);
        bool isValid = !choiceStr.empty();
        for (char c : choiceStr) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }
        if (!isValid) {
            cout << "Invalid input! Please enter a number between 1 and 5.\n";
            continue;
        }
        int choice = stoi(choiceStr);
        switch (choice) {
        case 1:
            viewEvents();
            break;
        case 2:
            bookTickets(username);
            break;
        case 3:
            viewBookings(username);
            break;
        case 4:
            cancelBooking(username);
            break;
        case 5:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice! Please select 1-5.\n";
            break;
        }
    }
}

void viewEvents(const vector<Event>& events) {
    cout << "\n=== Available Events ===\n";
    bool hasEvents = false;

    for (const auto& e : events) {
        if (e.status == "Upcoming") {
            hasEvents = true;
            cout << "\nEvent ID: " << e.id
                << "\nName: " << e.name
                << "\nVenue: " << e.venue
                << "\nDate: " << e.date
                << "\nTime: " << e.startTime << " - " << e.endTime
                << "\nOrganizer: " << e.organizerName
                << "\n-----------------------------";
        }
    }

    if (!hasEvents) {
        cout << "No events available\n";
    }
    cout << "\n";
}

void bookTickets(const string& username) {
    int eventID, quantity;
    cout << "Enter Event ID to book: ";
    cin >> eventID;
    cout << "Enter number of tickets: ";
    cin >> quantity;
    cin.ignore(); // clear newline
    ifstream file("events.txt");
    if (!file.is_open()) {
        cout << "No events available.\n";
        return;
    }
    string line;
    bool eventFound = false;
    string eventName;
    double price;
    int availableTickets;
    while (getline(file, line)) {
        stringstream ss(line);
        int id;
        string name, date, time, venue;
        ss >> id >> name >> date >> time >> venue >> price >> availableTickets;
        if (id == eventID) {
            eventFound = true;
            eventName = name;
            break;
        }
    }
    file.close();
    if (!eventFound) {
        cout << "Event not found.\n";
        return;
    }
    if (quantity > availableTickets) {
        cout << "Not enough tickets available.\n";
        return;
    }
    double totalPrice = price * quantity;
    int ticketID = generateTicketID();
    ofstream outFile("tickets.txt", ios::app);
    if (!outFile.is_open()) {
        cout << "Error booking tickets.\n";
        return;
    }
    outFile << ticketID << " " << eventID << " " << username << " " << eventName << " " << quantity << " " << price << " " << totalPrice << " Booked\n";
    outFile.close();
    // Update available tickets in events.txt
    ifstream inFile("events.txt");
    ofstream tempFile("temp.txt");
    while (getline(inFile, line)) {
        stringstream ss(line);
        int id, availTickets;
        string name, date, time, venue;
        double prc;
        ss >> id >> name >> date >> time >> venue >> prc >> availTickets;
        if (id == eventID) {
            availTickets -= quantity;
        }
        tempFile << id << " " << name << " " << date << " " << time << " " << venue << " " << prc << " " << availTickets << "\n";
    }
    inFile.close();
    tempFile.close();
	remove("events.txt");
    if (rename("temp.txt", "events.txt") != 0) {
        perror("Error renaming temp.txt to events.txt");
    }
	cout << "Successfully booked " << quantity << " tickets for " << eventName << ". Total price: $" << totalPrice << "\n";
}

void viewBookings(const string& username) {
    ifstream file("tickets.txt");
    if (!file.is_open()) {
        cout << "No bookings found.\n";
        return;
    }
    string line;
    cout << "\n===== MY BOOKINGS =====\n";
    cout << "TicketID\tEventID\tEventName\tQuantity\tPrice\tTotalPrice\tStatus\n";
    bool hasBookings = false;
    while (getline(file, line)) {
        stringstream ss(line);
        int ticketID, eventID, quantity;
        string user, eventName, status;
        double price, totalPrice;
        ss >> ticketID >> eventID >> user >> eventName >> quantity >> price >> totalPrice >> status;
        if (user == username) {
            hasBookings = true;
            cout << ticketID << "\t" << eventID << "\t" << eventName << "\t" << quantity << "\t" << price << "\t" << totalPrice << "\t" << status << "\n";
        }
    }
    file.close();
    if (!hasBookings) {
        cout << "No bookings found.\n";
    }
}

void cancelBooking(const string& username) {
    int ticketID;
    cout << "Enter Ticket ID to cancel: ";
    cin >> ticketID;
    cin.ignore(); // clear newline
    ifstream file("tickets.txt");
    if (!file.is_open()) {
        cout << "No bookings found.\n";
        return;
    }
    string line;
    vector<string> bookings;
    bool bookingFound = false;
    int eventID, quantity;
    while (getline(file, line)) {
        stringstream ss(line);
        int tID, eID, qty;
        string user, eventName, status;
        double price, totalPrice;
        ss >> tID >> eID >> user >> eventName >> qty >> price >> totalPrice >> status;
        if (tID == ticketID && user == username && status == "Booked") {
            bookingFound = true;
            eventID = eID;
            quantity = qty;
            continue; // skip this booking to effectively delete it
        }
        bookings.push_back(line);
    }
    file.close();
    if (!bookingFound) {
        cout << "Booking not found or already cancelled.\n";
        return;
    }
    ofstream outFile("tickets.txt");
    for (const auto& b : bookings) {
        outFile << b << "\n";
    }
    outFile.close();
    // Update available tickets in events.txt
    ifstream inFile("events.txt");
    ofstream tempFile("temp.txt");
    while (getline(inFile, line)) {
        stringstream ss(line);
        int id, availTickets;
        string name, date, time, venue;
        double prc;
        ss >> id >> name >> date >> time >> venue >> prc >> availTickets;
        if (id == eventID) {
            availTickets += quantity; // return tickets to availability
        }
        tempFile << id << " " << name << " " << date << " " << time << " " << venue << " " << prc << " " << availTickets << "\n";
    }
    inFile.close();
    tempFile.close();
    remove("events.txt");
    if (rename("temp.txt", "events.txt") != 0) {
        perror("Error renaming temp.txt to events.txt");
    }
    cout << "Successfully cancelled booking for Ticket ID: " << ticketID << "\n";
}