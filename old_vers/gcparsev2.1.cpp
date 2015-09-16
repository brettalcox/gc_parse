#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include <boost/tuple/tuple.hpp>

#include "gnuplot-iostream.h"

using namespace std;

void psGen(string genName, string graphName, string delimit, int heapLength)
{
	const char *gen = (char*)genName.c_str();
 	FILE * logFile;
        char* heapSpace = new char[heapLength];
	char* heapTime = new char[heapLength];
        //char heapNumber[5];

        string delimiter2 = "K->";
        string delimiter3 = "K->";
        string delimiter4 = "K(";

        size_t pos = 0;
        size_t pos2 = 0;
        string token;
        int count = 0;
        int count2 = 1;
        int iteration = 1;
        int iteration2 = 1;

        string gcInput;
        string number;
	string nameOfGraph = "set output \"" + graphName + "\"\n";
	string titleOfGraph = "set title \"" + genName + " Garbage Collection\"\n";

        Gnuplot gp;
        gp << "set term png size 1920,1080\n";
        gp << nameOfGraph;
        gp << titleOfGraph;
	gp << "set xlabel \"Iterations * 2 (Before/After)\"\n";
	gp << "set ylabel \"Kilobytes\"\n";
        gp << "plot '-' with lines\n";

        logFile = fopen ("LocationGC.log", "r");
        while ( logFile != NULL && fgets(heapSpace, heapLength, logFile) != NULL && fgets(heapTime, heapLength, logFile) != NULL ) {
                if (strstr(heapSpace, gen)) {
                        string heap(heapSpace);
                        pos = heap.find(delimit);
                        pos2 = heap.find(delimiter2);
                        token = heap.substr(pos + 6, ((pos2 - pos) - 6));

                        //stringstream ss;
                        //ss << count;
                        //number = ss.str();

                        //gcInput = number + " " + token + "\n";
                        //gp << gcInput;

                        //count += 2;
                        //cout << gcInput << endl << heap << endl;
                        //iteration++;

			strstr(heapTime, "[PSYoungGen");
			string heapStamp(heapTime);
			number = heapStamp.substr(0, 5);
			//cout << number << endl;
			gcInput = number + " " + token + "\n";
			cout << gcInput << endl;
			gp << gcInput;
                }
                if (strstr(heapSpace, gen)) {
                        string heap(heapSpace);
                        pos = heap.find(delimiter3);
                        pos2 = heap.find(delimiter4);
                        token = heap.substr(pos + 3, ((pos2 - pos) - 3));

                        stringstream ss;
                        ss << count2;
                        number = ss.str();

                        gcInput = number + " " + token + "\n";
                        gp << gcInput;

                        count2 += 2;
                        cout << gcInput << endl << heap << endl;
                        iteration2++;
                }
        }
	gp << "e\n";
	fflush(logFile);
	fclose(logFile);
}

int main() {

	string delimitYoung = "gGen: ";
	string delimitOld = "dGen: ";
	string delimitPerm = "mGen: ";

	psGen("[PSYoungGen", "graph_young.png", delimitYoung, 50);
	sleep(1);
	psGen("[PSOldGen", "graph_old.png", delimitOld, 50);
	sleep(1);
	psGen("[PSPermGen", "graph_perm.png", delimitPerm, 55);
	sleep(1);

	return 0;
}
