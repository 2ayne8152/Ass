#ifndef EVENT_STAFF_H
#define EVENT_STAFF_H

#include <string>
#include <vector>

using namespace std; 


struct EventStaff {
    string staffId;
    string name;
    string role;
    string contactInfo;
    string assignedEvent;
    string shiftTime;

    EventStaff(const string& id, const string& n, const string& r,
        const string& contact, const string& event, const string& shift)
        : staffId(id), name(n), role(r), contactInfo(contact),
        assignedEvent(event), shiftTime(shift) {}
};

vector<EventStaff> loadEventStaff();

void saveEventStaff(const vector<EventStaff>& staffList);

void addEventStaff(vector<EventStaff>& staffList, const EventStaff& newStaff);

bool removeEventStaff(vector<EventStaff>& staffList, const string& staffId);

EventStaff* findEventStaffById(vector<EventStaff>& staffList, const string& staffId);

vector<EventStaff> findEventStaffByEvent(const vector<EventStaff>& staffList, const string& eventName);

void displayAllEventStaff(const vector<EventStaff>& staffList);
void displayEventStaffForEvent(const vector<EventStaff>& staffList, const string& eventName);

void manageEventStaffMenu(const vector<EventStaff>& staffList);

string generateNextStaffId(const std::vector<EventStaff>& staffList);
void addEventStaffFromInput(vector<EventStaff>& staffList);

#endif 
