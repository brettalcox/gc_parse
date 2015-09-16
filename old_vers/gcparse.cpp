#include <iostream>
#include <string>
#include <stdio.h>
#include <cmath>
#include <boost/tuple/tuple.hpp>

#include "gnuplot-iostream.h"
using namespace std;

int main() {

	FILE * logFile;
	char heapSpace[60];
	char heapNumber[5];

	string delimiter = "n: ";
	string delimiter2 = "->";
	string delimiter3 = "->";
	string delimiter4 = "(";

	size_t pos = 0;
	size_t pos2 = 0;
	string token;
	int count = 0;
	int count2 = 1;
	int iteration = 1;
	int iteration2 = 1;

	string gcInput;
	string number;

	Gnuplot gp;
	gp << "set term png size 1920,1080\n";
	gp << "set output \"graph.png\"\n";
	gp << "set title \"PSYoungGen Garbage Collection\"\n";
	gp << "plot '-' with lines\n";

	logFile = fopen ("gc.log", "r");
	while ( logFile != NULL && fgets(heapSpace, sizeof(heapSpace), logFile) != NULL) {
		if (strstr(heapSpace, "[PSYoungGen")) {
			string heap(heapSpace);
			pos = heap.find(delimiter);
			pos2 = heap.find(delimiter2);
			token = heap.substr(pos + 3, ((pos2 - pos) - 4));

			stringstream ss;
			ss << count;
			number = ss.str();

			gcInput = number + " " + token + "\n";
			gp << gcInput;

			count += 2;
			//cout << "Logged \"before\" iteration #" << iteration << endl;
			cout << gcInput << endl;
			iteration++;
		}
		if (strstr(heapSpace, "[PSYoungGen")) {
			string heap(heapSpace);
			pos = heap.find(delimiter3);
			pos2 = heap.find(delimiter4);
			token = heap.substr(pos + 2, ((pos2 - pos) - 3));

			stringstream ss;
			ss << count2;
			number = ss.str();

			gcInput = number + " " + token + "\n";
			gp << gcInput;

			count2 += 2;
			cout << gcInput << endl;
			//cout << "Logged \"after\" iteration #" << iteration2 << endl;
			iteration2++;
		}
	}

	gp << "e\n";
	fclose(logFile);
	fflush(logFile);
	return 0;
}
