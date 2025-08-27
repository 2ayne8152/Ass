#pragma once
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

	Event() = default;

	Event(int _id, string _name, string _venue, string _date, string _startTime, string _endTime,
		string _status, string _organizerName, string _paymentStatus = "Unpaid")
		: id(_id), name(_name), venue(_venue), date(_date),
		startTime(_startTime), endTime(_endTime),
		status(_status), organizerName(_organizerName),
		paymentStatus(_paymentStatus) {
	}
};

void eventMenu();