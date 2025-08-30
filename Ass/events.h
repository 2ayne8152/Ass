#ifndef EVENTS_H
#define EVENTS_H

#include <string>
using namespace std;

struct Event {
    int id;
    string name;
    string venue;
    string date;
    string startTime;
    string endTime;
    string status;
    string paymentStatus;
    string organizerName;
    double ticketPrice;         // <-- Added
    int availableTickets;       // <-- Added

    Event() = default;

    Event(int _id, string _name, string _venue, string _date, string _startTime, string _endTime,
        string _status, string _organizerName, double _ticketPrice, int _availableTickets,
        string _paymentStatus = "Unpaid")
        : id(_id), name(_name), venue(_venue), date(_date),
        startTime(_startTime), endTime(_endTime),
        status(_status), organizerName(_organizerName),
        ticketPrice(_ticketPrice), availableTickets(_availableTickets),
        paymentStatus(_paymentStatus) {
    }
};

void eventMenu(const string& username);
void loadEventsFromFile(vector<Event>& events);
void saveEventsToFile(const vector<Event>& events);

#endif
