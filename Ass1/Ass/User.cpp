#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <format>
#include "events.h"
#include "user.h"
#include "util.h"
using namespace std;

void viewAvailableEvents(const vector<Event>& events);
void bookTickets(vector<Event>& events, vector<Ticket>& tickets, const string& username);
void viewUserBookings(const vector<Ticket>& tickets, const string& username);
void saveTicketsToFile(const vector<Ticket>& tickets);
void loadTicketsFromFile(vector<Ticket>& tickets);
void makePayment(vector<Event>& events, vector<Ticket>& tickets, const string& username);

const string EVENTS_FILE = "events.txt";
const string TICKETS_FILE = "tickets.txt";

void userMainMenu(const string& username) {
    vector<Event> events;
    vector<Ticket> tickets;

    loadEventsFromFile(events);
    loadTicketsFromFile(tickets);

    bool condition = true;

    do {
        clearScreen();
        cout << "\n==== Ticket Booking Menu ====\n";
        cout << "1. View Available Events\n";
        cout << "2. Book Tickets\n";
        cout << "3. View My Bookings\n";
        cout << "4. Make Payment\n";
        cout << "5. Quit\n";
        cout << "Select an option: ";

        int selection;
        cin >> selection;
        inputCheck(selection, 1, 5, "Invalid Input! Please Retry [1-5]: ");

        switch (selection) {
        case 1:
            viewAvailableEvents(events);
            break;
        case 2:
            bookTickets(events, tickets, username);
            saveEventsToFile(events);
            saveTicketsToFile(tickets);
            break;
        case 3:
            viewUserBookings(tickets, username);
            break;
        case 4:
            makePayment(events, tickets, username);
            saveEventsToFile(events);
            saveTicketsToFile(tickets);
            break;
        case 5:
            condition = false;
            clearScreen();
            return;
        }
    } while (condition);
}

void printTableLine() {
    cout << "+------+------------------------------+-----------------+------------+---------------+--------------------------+---------------+------------+\n";
}

void viewAvailableEvents(const vector<Event>& events) {
    clearScreen();

    // Table width for centering title
    const int tableWidth = 140;
    string title = "=== Available Events ===";
    int padding = (tableWidth - (int)title.size()) / 2;
    cout << string(padding, ' ') << title << "\n\n";

    // Table header
    printTableLine();
    cout << "| " << setw(4) << left << "ID"
        << " | " << setw(28) << left << "Name"
        << " | " << setw(15) << left << "Venue"
        << " | " << setw(10) << left << "Date"
        << " | " << setw(13) << left << "Time"
        << " | " << setw(24) << left << "Organizer"
        << " | " << setw(13) << right << "Price (RM)"
        << " | " << setw(10) << right << "Tickets" << " |\n";
    printTableLine();

    bool hasEvents = false;
    for (const auto& e : events) {
        if (e.status == "Upcoming" && e.availableTickets > 0) {
            hasEvents = true;

            string timeRange = e.startTime + " - " + e.endTime;

            cout << "| " << setw(4) << left << e.id
                << " | " << setw(28) << left << e.name.substr(0, 28)
                << " | " << setw(15) << left << e.venue.substr(0, 15)
                << " | " << setw(10) << left << e.date
                << " | " << setw(13) << left << timeRange
                << " | " << setw(24) << left << e.organizerName.substr(0, 24)
                << " | " << setw(13) << right << fixed << setprecision(2) << e.ticketPrice
                << " | " << setw(10) << right << e.availableTickets << " |\n";
        }
    }

    if (!hasEvents) {
        cout << "| " << setw(125) << left << "No upcoming events with available tickets."
            << " |\n";
    }

    printTableLine();
    cout << "\n";
    pauseScreen();
}

void printPaymentLine() {
    cout << "+-----+----------+------------------------------+-----+---------------+----------+\n";
}

void makePayment(vector<Event>& events, vector<Ticket>& tickets, const string& username) {
    clearScreen();
    cout << "\n=== Make Payment ===\n\n";
    vector<int> unpaidIndexes;

    // Print header
    printPaymentLine();
    cout << "| " << setw(3) << left << "No."
        << " | " << setw(8) << left << "TicketID"
        << " | " << setw(28) << left << "Event Name"
        << " | " << setw(3) << left << "Qty"
        << " | " << setw(13) << right << "Total (RM)"
        << " | " << setw(8) << left << "Status" << " |\n";
    printPaymentLine();

    // Show unpaid tickets
    for (int i = 0; i < tickets.size(); i++) {
        if (tickets[i].username == username && tickets[i].status == "Booked") {
            unpaidIndexes.push_back(i);
            cout << "| " << setw(3) << left << unpaidIndexes.size()
                << " | " << setw(8) << left << tickets[i].ticketID
                << " | " << setw(28) << left << tickets[i].eventName.substr(0, 28)
                << " | " << setw(3) << left << tickets[i].quantity
                << " | " << setw(13) << right << fixed << setprecision(2) << tickets[i].totalPrice
                << " | " << setw(8) << left << tickets[i].status
                << " |\n";
        }
    }

    if (unpaidIndexes.empty()) {
        cout << "| " << setw(78) << left << "You have no unpaid bookings."
            << " |\n";
        printPaymentLine();
        pauseScreen();
        return;
    }

    printPaymentLine();

    // Ask user to choose which ticket to pay for
    int selection;
    cout << "\nSelect the ticket number to make payment: ";
    cin >> selection;

    while (cin.fail() || selection < 1 || selection > unpaidIndexes.size()) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Invalid selection. Please try again: ";
        cin >> selection;
    }

    int ticketIndex = unpaidIndexes[selection - 1];

    cout << "\nProcessing payment for Ticket ID: " << tickets[ticketIndex].ticketID << "...\n";
    cout << "Amount to pay: RM" << fixed << setprecision(2) << tickets[ticketIndex].totalPrice << "\n";

    // Simulate payment confirmation
    char confirm;
    cout << "Confirm payment? (Y/N): ";
    cin >> confirm;

    if (confirm == 'Y' || confirm == 'y') {
        tickets[ticketIndex].status = "Paid";

        // Update the event's payment status if fully paid
        for (auto& e : events) {
            if (e.id == tickets[ticketIndex].eventID) {
                e.paymentStatus = "Paid";
                break;
            }
        }

        cout << "\n Payment successful! Ticket ID: " << tickets[ticketIndex].ticketID << " is now marked as PAID.\n";
    }
    else {
        cout << "\n Payment cancelled.\n";
    }
    pauseScreen();
}

void bookTickets(vector<Event>& events, vector<Ticket>& tickets, const string& username) {
    clearScreen();
    //cout << "\n=== Book Tickets ===\n\n";
    const int tableWidth = 140;
    string title = "=== Book Tickets ===";
    int padding = (tableWidth - (int)title.size()) / 2;
    cout << string(padding, ' ') << title << "\n\n";

    vector<int> availableIndexes;

    // Table header
    printTableLine();
    cout << "| " << setw(4) << left << "No."
        << " | " << setw(28) << left << "Name"
        << " | " << setw(15) << left << "Venue"
        << " | " << setw(10) << left << "Date"
        << " | " << setw(13) << left << "Time"
        << " | " << setw(24) << left << "Organizer"
        << " | " << setw(13) << right << "Price (RM)"
        << " | " << setw(10) << right << "Tickets" << " |\n";
    printTableLine();

    // Print all available events in table
    for (int i = 0; i < events.size(); i++) {
        if (events[i].status == "Upcoming" && events[i].availableTickets > 0) {
            availableIndexes.push_back(i);

            string timeRange = events[i].startTime + " - " + events[i].endTime;

            cout << "| " << setw(4) << left << availableIndexes.size()
                << " | " << setw(28) << left << events[i].name.substr(0, 28)
                << " | " << setw(15) << left << events[i].venue.substr(0, 15)
                << " | " << setw(10) << left << events[i].date
                << " | " << setw(13) << left << timeRange
                << " | " << setw(24) << left << events[i].organizerName.substr(0, 24)
                << " | " << setw(13) << right << fixed << setprecision(2) << events[i].ticketPrice
                << " | " << setw(10) << right << events[i].availableTickets << " |\n";
        }
    }

    if (availableIndexes.empty()) {
        cout << "| " << setw(125) << left << "No events available for booking."
            << " |\n";
        printTableLine();
        cout << "\n";
        return;
    }

    printTableLine();

    // Let user choose
    int selection;
    cout << "\nSelect the number of the event to book: ";
    cin >> selection;

    while (cin.fail() || selection < 1 || selection > availableIndexes.size()) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Invalid selection. Please try again: ";
        cin >> selection;
    }

    int index = availableIndexes[selection - 1];

    int ticketsToBook;
    cout << "Enter number of tickets to book: ";
    cin >> ticketsToBook;
    while (cin.fail() || ticketsToBook <= 0 || ticketsToBook > events[index].availableTickets) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "Invalid number! Please enter a value between 1 and "
            << events[index].availableTickets << ": ";
        cin >> ticketsToBook;
    }

    // Reduce available tickets
    events[index].availableTickets -= ticketsToBook;

    // Create Ticket
    Ticket t;
    t.ticketID = tickets.empty() ? 1 : tickets.back().ticketID + 1;
    t.eventID = events[index].id;
    t.username = username;
    t.eventName = events[index].name;
    t.quantity = ticketsToBook;
    t.price = events[index].ticketPrice;
    t.totalPrice = events[index].ticketPrice * ticketsToBook;
    t.status = "Booked";

    tickets.push_back(t);

    cout << "\n Booking successful for event: " << events[index].name
        << "\n   Tickets Booked: " << ticketsToBook
        << "\n   Total Price: RM" << fixed << setprecision(2) << t.totalPrice
        << "\n";
    pauseScreen();
}

void printBookingTableLine() {
    cout << "+----------+------------------------------+----------+---------------+---------------+----------+\n";
}

void viewUserBookings(const vector<Ticket>& tickets, const string& username) {
    clearScreen();
    cout << "\n=== My Bookings ===\n\n";

    bool hasBookings = false;

    // Table header
    printBookingTableLine();
    cout << "| " << setw(8) << left << "TicketID"
        << " | " << setw(28) << left << "Event Name"
        << " | " << setw(8) << left << "Qty"
        << " | " << setw(13) << right << "Price (RM)"
        << " | " << setw(13) << right << "Total (RM)"
        << " | " << setw(8) << left << "Status" << " |\n";
    printBookingTableLine();

    // Table rows
    for (const auto& t : tickets) {
        if (t.username == username) {
            hasBookings = true;
            cout << "| " << setw(8) << left << t.ticketID
                << " | " << setw(28) << left << t.eventName.substr(0, 28)
                << " | " << setw(8) << left << t.quantity
                << " | " << setw(13) << right << fixed << setprecision(2) << t.price
                << " | " << setw(13) << right << fixed << setprecision(2) << t.totalPrice
                << " | " << setw(8) << left << t.status
                << " |\n";
        }
    }

    if (!hasBookings) {
        cout << "| " << setw(86) << left << "You have no bookings yet."
            << " |\n";
    }

    printBookingTableLine();
    pauseScreen();
}

void saveTicketsToFile(const vector<Ticket>& tickets) {
    ofstream outFile(TICKETS_FILE);
    if (!outFile) {
        cout << "Error: Unable to save tickets.\n";
        return;
    }

    for (const auto& t : tickets) {
        outFile << t.ticketID << "|"
            << t.eventID << "|"
            << t.username << "|"
            << t.eventName << "|"
            << t.quantity << "|"
            << t.price << "|"
            << t.totalPrice << "|"
            << t.status << "\n";
    }
    outFile.close();
}

void loadTicketsFromFile(vector<Ticket>& tickets) {
    ifstream inFile(TICKETS_FILE);
    if (!inFile) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Ticket t;
        string token;

        getline(ss, token, '|'); t.ticketID = stoi(token);
        getline(ss, token, '|'); t.eventID = stoi(token);
        getline(ss, t.username, '|');
        getline(ss, t.eventName, '|');
        getline(ss, token, '|'); t.quantity = stoi(token);
        getline(ss, token, '|'); t.price = stod(token);
        getline(ss, token, '|'); t.totalPrice = stod(token);
        getline(ss, t.status, '|');

        tickets.push_back(t);
    }
    inFile.close();
}