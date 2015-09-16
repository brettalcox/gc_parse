#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include <boost/tuple/tuple.hpp>
#include <exception>
#include "gnuplot-iostream.h"

using namespace std;

class Gen {

	private:
		string genName;
		string nameOfLogFile;
		int charBuffer;
		vector<float> maxHeapValue;
		float maxHeap;
		float minHeap;
		vector<float> minHeapValue;
		float timeOf;
		float meanHeap;
		vector<float> timeValues;
	public:
		Gen(string, string, int);
		int getCharBuffer();
		string getGenName();
		string getNameOfLogFile();
		void setMaxHeapValue(float);
		float getMaxHeapValue();
		void setMinHeapValue(float);
		float getMinHeapValue();
		void setTimeValues(float);
		size_t getTimeCounts();
		float getMeanHeapBefore();
		float getMeanHeapAfter();
		float getTotalTime();
};

class Parse {
	private:
		FILE * logFile;
		const string delimiter2 = "K->";
                const string delimiter3 = "K(";
		size_t pos = 0;
		size_t pos2 = 0;
		string timeStamp;
		string token;
		const char *gen;
		vector<string> gpHold;
		string correctedTitle;
		string graphFileName;
	public:
		void parseLog(Gen&, string, string, int);
		vector<string> getGpData();
		void fileNameParse(string, string);
		string getCorrectedTitle();
		string getGraphFileName();

};

class Gplot {

	private:
		Gnuplot gp;
		string gpInput;
		string titleOfGraph;
		string nameOfGraph;
		const string xLabel = "set xlabel \"Time (sec}\"\n";
		const string yLabel = "set ylabel \"Kilobytes\"\n";;
		const string plot = "plot '-' with lines\n";
		const string term = "set term png size 853,480\n";
	public:
		void setGpInput(string);
		void setNameOfGraph(string);
		void setTitleOfGraph(string, string);
		void setXLabel();
		void setYLabel();
		void setPlot();
		void setTerm();
		void fileNameParse();
		void plotGraph(vector<string>, string, string);
};

class newException : public exception {

	virtual const char* what() const throw()
	{
		return "Error loading file.";
	} 
} newEx;

Gen::Gen(string nameOfLog, string nameGen, int buffer)
{
	nameOfLogFile = nameOfLog;
	genName = nameGen;
	charBuffer = buffer;

	maxHeap = 0;
	
}

string Gen::getGenName()
{
	return genName;
}

string Gen::getNameOfLogFile()
{
	return nameOfLogFile;
}

int Gen::getCharBuffer()
{
	return charBuffer;
}

void Gen::setTimeValues(float value)
{
	timeValues.push_back(value);
}

size_t Gen::getTimeCounts()
{
	return timeValues.size();
}

void Gen::setMaxHeapValue(float value)
{
	maxHeapValue.push_back(value);
}

void Gen::setMinHeapValue(float value)
{
	minHeapValue.push_back(value);
}

float Gen::getMinHeapValue()
{
	minHeap = 999999999999;
	for (int i = 0; i < minHeapValue.size(); i++) 	{
		if (minHeapValue.at(i) < minHeap) {
			minHeap = minHeapValue.at(i);
		}
	}
	return minHeap;
}

float Gen::getMaxHeapValue()
{
	for (int i = 0; i < maxHeapValue.size(); i++) {
		if (maxHeapValue.at(i) > maxHeap) {
			maxHeap = maxHeapValue.at(i);
		}
	}
	return maxHeap;
}

float Gen::getTotalTime()
{
	return timeValues.at(timeValues.size() - 1);
}

float Gen::getMeanHeapBefore()
{
	float total = 0;
	for (int i = 0; i < maxHeapValue.size(); i++) {
		total += maxHeapValue.at(i);
	}
	return total / maxHeapValue.size();
}

float Gen::getMeanHeapAfter()
{
	float total = 0;
	for (int i = 0; i < minHeapValue.size(); i++) {
		total += minHeapValue.at(i);
	}
	return total / minHeapValue.size();
}

void Parse::parseLog(Gen& object, string nameOfLogFile, string delimiter, int charBuffer)
{
	const char *nameOfFile = nameOfLogFile.c_str();
	string::size_type sz;	

	try {
		logFile = fopen(nameOfFile, "r");
		if (!logFile) {
			throw newEx;
			return;
			;
		}
		char* heapSpace = new char[object.getCharBuffer()];
		const char *gen = (char*)object.getGenName().c_str();

		while ( logFile != NULL && fgets(heapSpace, object.getCharBuffer(), logFile) != NULL ) {
			if (strstr(heapSpace, "[PSYoungGen")) {
				string heapStore(heapSpace);
				pos = heapStore.find(":");
				timeStamp = heapStore.substr(0, pos - 1);
			}
			if (strstr(heapSpace, gen)) {
				string heapStore(heapSpace);
				pos = heapStore.find(delimiter);
				pos2 = heapStore.find(delimiter2);
				token = heapStore.substr(pos + 6, ((pos2 - pos) - 6));

				float maxHeap = stof(token, &sz);
				object.setMaxHeapValue(maxHeap);
				if (maxHeap > object.getMaxHeapValue()) {
					object.setMaxHeapValue(maxHeap);
				} 

				token = timeStamp + " " + token + "\n";
				gpHold.push_back(token);

				float timeOf = stof(timeStamp, &sz);
                                object.setTimeValues(timeOf);

			}	
			if (strstr(heapSpace, gen)) {
				string heapStore(heapSpace);
				pos = heapStore.find(delimiter2);
				pos2 = heapStore.find(delimiter3);
				token = heapStore.substr(pos + 3, ((pos2 - pos) - 3));

				float minHeap = stof(token, &sz);
				object.setMinHeapValue(minHeap);

				token = timeStamp + " " + token + "\n";
				gpHold.push_back(token);
			}
		}
		delete heapSpace;
		fflush(logFile);
		fclose(logFile);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

vector<string> Parse::getGpData()
{
	return gpHold;
}

void Parse::fileNameParse(string logName, string nameGen)
{
        size_t pos = 0;
        size_t pos2 = 0;

        correctedTitle = nameGen.substr(pos + 3, nameGen.length() - 3);
	pos2 = logName.find(".log");
	graphFileName = logName.substr(pos, pos2);

}

string Parse::getGraphFileName()
{
	return graphFileName;
}

string Parse::getCorrectedTitle()
{
	return correctedTitle;
}

void Gplot::setPlot()
{
	gp << plot;
}

void Gplot::setTerm()
{
	gp << term;
}

void Gplot::setXLabel()
{
	gp << xLabel;
}

void Gplot::setYLabel()
{
	gp << yLabel;
}

void Gplot::setGpInput(string input)
{
	gp << input;
}

void Gplot::setNameOfGraph(string graphName)
{
	string nameOfGraphString;
	nameOfGraphString = "set output \"graph_" + graphName + "_" + nameOfGraph + ".png\"\n";
	gp << nameOfGraphString;
}

void Gplot::setTitleOfGraph(string graphName, string nameGen)
{
	titleOfGraph = "set title \"" + graphName + " " + nameGen + " Garbage Collection\"\n";
	gp << titleOfGraph;
	nameOfGraph = nameGen;
}

void Gplot::plotGraph(vector<string> plots, string title, string graphName)
{
	titleOfGraph = title;
	setTerm();
	setXLabel();
	setYLabel();
	setTitleOfGraph(graphName, title);
	setNameOfGraph(graphName);
	setPlot();
	for (int i = 0; i < plots.size(); i++) {
		gp << plots.at(i);
	}
	gp << "e\n";
}

int main()
{

	string inputHold;
	cout << "Enter the name of the log you would like to graph: ";
	cin >> inputHold;

	Gen Young(inputHold, "[PSYoungGen", 50);
	Gen Old(inputHold, "[PSOldGen", 50);
	Gen Perm(inputHold, "[PSPermGen", 55);

	Parse YoungGen;
	Parse OldGen;
	Parse PermGen;

	Gplot newplot;

	YoungGen.fileNameParse(Young.getNameOfLogFile(), Young.getGenName());
	OldGen.fileNameParse(Old.getNameOfLogFile(), Old.getGenName());
	PermGen.fileNameParse(Perm.getNameOfLogFile(), Perm.getGenName());

	YoungGen.parseLog(Young, Young.getNameOfLogFile(), "gGen: ", 50);
	OldGen.parseLog(Old, Old.getNameOfLogFile(), "dGen: ", 50);
	PermGen.parseLog(Perm, Perm.getNameOfLogFile(), "mGen: ", 55);

	if (!YoungGen.getGpData().empty()) {
		newplot.plotGraph(YoungGen.getGpData(), YoungGen.getCorrectedTitle(), YoungGen.getGraphFileName());
			if (!OldGen.getGpData().empty()) {
				newplot.plotGraph(OldGen.getGpData(), OldGen.getCorrectedTitle(), OldGen.getGraphFileName());
			}
			if (!PermGen.getGpData().empty()) {
				newplot.plotGraph(PermGen.getGpData(), PermGen.getCorrectedTitle(), PermGen.getGraphFileName());
			}
		cout << endl << "Plotting complete." << endl << endl;
	}
	else {}
	
	cout << "###################################################" << endl;
	cout << "# YOUNG_GEN " << endl << "#" << endl;
	cout << "# " << Young.getTimeCounts() << " minor and major YoungGen garbage collections" << endl;	
	cout << "# Max Heap of YoungGen: " << Young.getMaxHeapValue() << "K" << endl;
	cout << "# Min Heap of YoungGen: " << Young.getMinHeapValue() << "K" << endl;
	cout << "# Mean Heap of YoungGen Before: " << Young.getMeanHeapBefore() << "K" << endl;
	cout << "# Mean Heap of YoungGen After: " << Young.getMeanHeapAfter() << "K" << endl;
	cout << "###################################################" << endl << endl;

	cout << "###################################################" << endl;
	cout << "# OLD_GEN " << endl << "#" << endl;
	cout << "# " << Old.getTimeCounts() << " major OldGen garbage collections" << endl;
	cout << "# Max Heap of OldGen: " << Old.getMaxHeapValue() << "K" << endl;
	cout << "# Min Heap of OldGen: " << Old.getMinHeapValue() << "K" << endl;
	cout << "# Mean Heap of OldGen Before: " << Old.getMeanHeapBefore() << "K" << endl;
	cout << "# Mean Heap of OldGen After: " << Old.getMeanHeapAfter() << "K" << endl;	
	cout << "###################################################" << endl << endl;
	
	cout << "###################################################" << endl;
	cout << "# PERM_GEN " << endl << "#" << endl;
	cout << "# " << Perm.getTimeCounts() << " major PermGen garbage collections" << endl;
	cout << "# Max Heap of PermGen: " << Perm.getMaxHeapValue() << "K" << endl;
	cout << "# Min Heap of PermGen: " << Perm.getMinHeapValue() << "K" << endl;
	cout << "# Mean Heap of PermGen Before: " << Perm.getMeanHeapBefore() << "K" << endl;
	cout << "# Mean Heap of PermGen After: " << Perm.getMeanHeapAfter() << "K" << endl;
	cout << "###################################################" << endl << endl;

	cout << "Total Time from service start: " << Young.getTotalTime() << " seconds" << endl << endl;
}
