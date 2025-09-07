#pragma once
#include <string>
using namespace std;

struct Equipment {
    string name;
    int quantity;
    string location; 
    string status;

    Equipment() = default;

    Equipment(string _name, int _quantity, string _location = "Storage", string _status = "Available")
        : name(_name), quantity(_quantity), location(_location), status(_status) {}
};

void equipmentMenu();
void saveEquipmentToFile();
void loadEquipmentFromFile();
void displayAllEquipment();
void addEquipment();
void removeEquipment();
void transferEquipment();
void updateEquipmentStatus();