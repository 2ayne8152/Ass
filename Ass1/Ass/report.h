#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "events.h"
#include "user.h"
using namespace std;

struct EventSales {
    int eventID;
    string name;
    int ticketsSold;
    double revenue;
};

void generateEventReport();
void generateStaffReport();
void generateEquipmentReport();
void generateCrisisReport();
bool isTxtFile(const string& filename);
void reportMenu(const vector<Event>& events, const vector<Ticket>& tickets);
