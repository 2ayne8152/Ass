#include "report.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "util.h"
#include "events.h"
#include "user.h"
using namespace std;


void monthlySalesReport(const vector<Event>& events, const vector<Ticket>& tickets) {
    clearScreen();
    cout << "\n=== Monthly Ticket Sales & Revenue Report ===\n";

    string monthInput;
    regex monthPattern(R"(^\d{4}-(0[1-9]|1[0-2])$)");

    while (true) {
        monthInput = getInput("Enter month (format YYYY-MM, e.g. 2025-09): ");
        if (regex_match(monthInput, monthPattern)) {
            break; // valid input
        }
        else {
            cout << "Invalid format. Please use YYYY-MM with a valid month (01–12).\n";
        }
    }

    int totalTickets = 0;
    double totalRevenue = 0.0;
    int totalEvents = 0;

    // Print header
    cout << "\nReport for " << monthInput << "\n";
    cout << "+---------------------------------------------------------------------+\n";
    cout << "| " << setw(5) << left << "ID"
        << " | " << setw(28) << left << "Event Name"
        << " | " << setw(10) << right << "Sold"
        << " | " << setw(12) << right << "Revenue (RM)" << setw(6) << right << "|\n";
    cout << "+---------------------------------------------------------------------+\n";

    for (const auto& e : events) {
        if (e.date.substr(0, 7) == monthInput) {
            int sold = 0;
            double revenue = 0.0;

            // Calculate sales from tickets
            for (const auto& t : tickets) {
                if (t.eventID == e.id && t.status == "Paid") {
                    sold += t.quantity;
                    revenue += t.totalPrice;
                }
            }

            if (sold > 0) {
                totalTickets += sold;
                totalRevenue += revenue;
                totalEvents++;

                cout << "| " << setw(5) << left << e.id
                    << " | " << setw(28) << left << e.name.substr(0, 28)
                    << " | " << setw(10) << right << sold
                    << " | " << setw(12) << right << fixed << setprecision(2) << revenue
                     << setw(6) << right << " |\n";
            }
        }
    }

    cout << "+---------------------------------------------------------------------+\n";

    if (totalEvents == 0) {
        cout << "No sales found for this month.\n";
    }
    else {
        cout << "Summary: " << totalEvents << " events | "
            << totalTickets << " tickets sold | RM"
            << fixed << setprecision(2) << totalRevenue << " revenue\n";
    }

    pauseInterface();
}

void topEventsReport(const vector<Event>& events, const vector<Ticket>& tickets) {
    clearScreen();
    cout << "\n=== Top Events by Tickets Sold ===\n";

    vector<EventSales> salesData;

    // Collect sales data for each event
    for (const auto& e : events) {
        int sold = 0;
        double revenue = 0.0;

        for (const auto& t : tickets) {
            if (t.eventID == e.id && t.status == "Paid") {
                sold += t.quantity;
                revenue += t.totalPrice;
            }
        }

        if (sold > 0) {
            salesData.push_back({ e.id, e.name, sold, revenue });
        }
    }

    if (salesData.empty()) {
        cout << "No ticket sales found.\n";
        pauseInterface();
        return;
    }

    // Sort by tickets sold (descending)
    sort(salesData.begin(), salesData.end(), [](const EventSales& a, const EventSales& b) {
        return a.ticketsSold > b.ticketsSold;
        });

    // Print header
    cout << "+--------------------------------------------------------------------------+\n";
    cout << "| " << setw(5) << left << "Rank"
        << " | " << setw(5) << left << "ID"
        << " | " << setw(28) << left << "Event Name"
        << " | " << setw(10) << right << "Tickets"
        << " | " << setw(12) << right << "Revenue (RM)" << " |\n";
    cout << "+--------------------------------------------------------------------------+\n";

    // Show top 5 (or fewer if less data)
    int rank = 1;
    for (const auto& s : salesData) {
        if (rank > 5) break; // limit to top 5
        cout << "| " << setw(5) << left << rank
            << " | " << setw(5) << left << s.eventID
            << " | " << setw(28) << left << s.name.substr(0, 28)
            << " | " << setw(10) << right << s.ticketsSold
            << " | " << setw(12) << right << fixed << setprecision(2) << s.revenue
            << " |\n";
        rank++;
    }

    cout << "+--------------------------------------------------------------------------+\n";
    pauseInterface();
}

// --- Menu ---
void reportMenu(const vector<Event>& events, const vector<Ticket>& tickets) {
    int choice;
    string input;

    while (true) {
        cout << "\n===== REPORT MENU =====\n";
        cout << "1. Generate Sales Report\n";
        cout << "2. Generate Staff Report\n";
        cout << "3. Back to Main Menu\n";
        cout << "Choose an option: ";

        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1:
            monthlySalesReport(events, tickets);
            break;
        case 2:
            topEventsReport(events, tickets);
            break;
        case 3:
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

