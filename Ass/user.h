#pragma once

#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "events.h"
using namespace std;

struct Ticket {
    int ticketID;
    int eventID;
    string username;
    string eventName;
    int quantity;
    double price;
    double totalPrice;
    string status;
};

void userMainMenu(const string& username);
void loadTicketsFromFile(vector<Ticket>& tickets);