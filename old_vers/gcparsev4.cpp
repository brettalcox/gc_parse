#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include <boost/tuple/tuple.hpp>

#include "gnuplot-iostream.h"

using namespace std;

void psGen(string genName, string graphName, const char* nameOfFile, string delimit, int heapLength)
{
	const char *gen = (char*)genName.c_str();
 	FILE * logFile;
        char* heapSpace = new char[heapLength];

        string delimiter2 = "K->";
        string delimiter3 = "K->";
        string delimiter4 = "K(";

        size_t pos = 0;
        size_t pos2 = 0;
        string token;
	string time;
        int count = 0;
        int count2 = 1;
        int iteration = 0;
        int iteration2 = 0;

        string gcInput;
        string number;
	string nameOfGraph = "set output \"" + graphName + "\"\n";
	string titleOfGraph = "set title \"" + genName + " Garbage Collection\"\n";

        Gnuplot gp;
        gp << "set term png size 3840,1080\n";
        gp << nameOfGraph;
        gp << titleOfGraph;
	gp << "set xlabel \"Time (sec)(Before/After)\"\n";
	gp << "set ylabel \"Kilobytes\"\n";
        gp << "plot '-' with lines\n";

        logFile = fopen (nameOfFile, "r");
        while ( logFile != NULL && fgets(heapSpace, heapLength, logFile) != NULL) {
		if (strstr(heapSpace, "[PSYoungGen")) {
			string heap(heapSpace);
			pos = heap.find(":");
			time = heap.substr(0, pos - 1);
		}
                if (strstr(heapSpace, gen)) {
                        string heap(heapSpace);
                        pos = heap.find(delimit);
                        pos2 = heap.find(delimiter2);
                        token = heap.substr(pos + 6, ((pos2 - pos) - 6));

                        stringstream ss;
                        ss << count;
                        number = ss.str();

                        gcInput = time + " " + token + "\n";
                        gp << gcInput;

                        count += 2;
                        cout << gcInput << endl;
                }
                if (strstr(heapSpace, gen)) {
                        string heap(heapSpace);
                        pos = heap.find(delimiter3);
                        pos2 = heap.find(delimiter4);
                        token = heap.substr(pos + 3, ((pos2 - pos) - 3));

                        stringstream ss;
                        ss << count2;
                        number = ss.str();

                        gcInput = time + " " + token + "\n";
                        gp << gcInput;

                        count2 += 2;
                        cout << gcInput << endl;
                }
        }

	//gp << "e\n";
	fflush(logFile);
	fclose(logFile);
}

int main() {

	string delimitYoung = "gGen: ";
	string delimitOld = "dGen: ";
	string delimitPerm = "mGen: ";

	string nameOfLog;
	cout << "Enter the name of the log you would like to graph: ";
	cin >> nameOfLog;
	const char* nameOfFile = nameOfLog.c_str();

	psGen("[PSYoungGen", "graph_young.png", nameOfFile, delimitYoung, 50);
	sleep(1);
	psGen("[PSOldGen", "graph_old.png", nameOfFile, delimitOld, 50);
	sleep(1);
	psGen("[PSPermGen", "graph_perm.png", nameOfFile, delimitPerm, 55);
	sleep(1);

	return 0;
}
