#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;
using namespace std;

void generateEventReport();
void generateStaffReport();
void generateEquipmentReport();
void generateCrisisReport();
bool isTxtFile(const string& filename);
void exportReport(const string& reportName, const string& filename);
void reportMenu();
