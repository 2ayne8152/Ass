#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
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

void viewAvailableEvents(const vector<Event>& events) {
	clearScreen();
    cout << "\n=== Available Events ===\n";
    bool hasEvents = false;

    for (const auto& e : events) {
        if (e.status == "Upcoming" && e.availableTickets > 0) {
            hasEvents = true;
            cout << "\nEvent ID: " << e.id
                << "\nName: " << e.name
                << "\nVenue: " << e.venue
                << "\nDate: " << e.date
                << "\nTime: " << e.startTime << " - " << e.endTime
                << "\nOrganizer: " << e.organizerName
                << "\nTicket Price: RM" << fixed << setprecision(2) << e.ticketPrice
                << "\nAvailable Tickets: " << e.availableTickets
                << "\n-----------------------------";
        }
    }

    if (!hasEvents) {
        cout << "No upcoming events with available tickets.\n";
    }
    cout << "\n";
    pauseScreen();
}

void makePayment(vector<Event>& events, vector<Ticket>& tickets, const string& username) {
    clearScreen();
    cout << "\n=== Make Payment ===\n";
    vector<int> unpaidIndexes;

    // Show unpaid tickets
    for (int i = 0; i < tickets.size(); i++) {
        if (tickets[i].username == username && tickets[i].status == "Booked") {
            unpaidIndexes.push_back(i);
            cout << unpaidIndexes.size() << ". Ticket ID: " << tickets[i].ticketID
                << " | Event: " << tickets[i].eventName
                << " | Qty: " << tickets[i].quantity
                << " | Total: RM" << fixed << setprecision(2) << tickets[i].totalPrice
                << " | Status: " << tickets[i].status << "\n";
        }
    }

    if (unpaidIndexes.empty()) {
        cout << "You have no unpaid bookings.\n";
        return;
    }

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

        cout << "Payment successful! Ticket ID: " << tickets[ticketIndex].ticketID << " is now marked as PAID.\n";
    }
    else {
        cout << "Payment cancelled.\n";
    }
    pauseScreen();
}

void bookTickets(vector<Event>& events, vector<Ticket>& tickets, const string& username) {
    clearScreen();
    cout << "\n=== Book Tickets ===\n";

    vector<int> availableIndexes;
    for (int i = 0; i < events.size(); i++) {
        if (events[i].status == "Upcoming" && events[i].availableTickets > 0) {
            availableIndexes.push_back(i);
            cout << availableIndexes.size() << ". "
                << events[i].name << " (ID: " << events[i].id << ") | Date: " << events[i].date
                << " | Venue: " << events[i].venue
                << " | Price: RM" << fixed << setprecision(2) << events[i].ticketPrice
                << " | Tickets Left: " << events[i].availableTickets << "\n";
        }
    }

    if (availableIndexes.empty()) {
        cout << "No events available for booking.\n";
        return;
    }

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

    cout << "\nBooking successful for event: " << events[index].name
        << "\nTickets Booked: " << ticketsToBook
        << "\nTotal Price: RM" << fixed << setprecision(2) << t.totalPrice
        << "\n";
    pauseScreen();
}

void viewUserBookings(const vector<Ticket>& tickets, const string& username) {
    clearScreen();
    cout << "\n=== My Bookings ===\n";
    bool hasBookings = false;

    for (const auto& t : tickets) {
        if (t.username == username) {
            hasBookings = true;
            cout << "\nTicket ID: " << t.ticketID
                << "\nEvent Name: " << t.eventName
                << "\nQuantity: " << t.quantity
                << "\nPrice per Ticket: RM" << fixed << setprecision(2) << t.price
                << "\nTotal Price: RM" << fixed << setprecision(2) << t.totalPrice
                << "\nStatus: " << t.status
                << "\n-----------------------------";
        }
    }

    if (!hasBookings) {
        cout << "You have no bookings yet.\n";
    }
    cout << "\n";
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