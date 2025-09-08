#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include <vector>
#include "event_staff.h"
#include "stage.h"
#include "events.h"
#include "user.h"
using namespace std;

void homePageMenu(const string& message = "");
void staffMainMenu(const string& username);
void updatePassword(const string& role, const string& usernameOrEmail);