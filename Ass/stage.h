#pragma once
#include <string>
using namespace std;

struct Stage {
    string stageNumber;
    string stageName;
    int capacity = 0;
    bool isOperational = true;
    string currentEvent;
    double pricePerHour = 0.0;

    Stage() = default;

    Stage(string _stageNumber, string _stageName, int _capacity,
        bool _isOperational = true, string _currentEvent = "", double _pricePerHour = 0.0)
        : stageNumber(_stageNumber), stageName(_stageName),
        capacity(_capacity), isOperational(_isOperational),
        currentEvent(_currentEvent), pricePerHour(_pricePerHour) {
    }
};

void crisisMenu();
void stageMenu();
void saveStagesToFile();
void loadStagesFromFile();
void displayAllStages();
void assignEventToStage();
void updateStageStatus();