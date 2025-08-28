#pragma once
#include <string>
using namespace std;

struct Equipment {
    string name;
    int quantity;
    string location;  

    Equipment() = default;

    Equipment(string _name, int _quantity, string _location = "Storage")
        : name(_name), quantity(_quantity), location(_location) {}
};

void equipmentMenu();
void saveEquipmentToFile();
void loadEquipmentFromFile();
void displayAllEquipment();
void addEquipment();
void removeEquipment();
void transferEquipment();