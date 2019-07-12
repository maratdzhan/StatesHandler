#pragma once


#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;

void ToUpperFunct(string & str)
{
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}

struct State
{
	// вектор, который для каждого тайм поинта содержит номер слоя
	map<int, vector<pair<string,string>>> time_point;
	
};

class StatesHandler
{
protected:
	string _path;
	vector<string> files_list;
	map<string, State> states;

	string currentState;
	string currentLayer;
	int end;
	int currentTimePointUpper;
	int currentTimePointLower;
	bool isInitialized;
	vector<string> handling;
	vector<int> tmpnts;
	int added;

	void Scan();
	void Parsing();
	void Commands(const string& input);
	void ExtractState();
	void ExtractCycle();
	void Store();
	void End();
	void Clear();
	void Merge();
	void Merging(int lhs, int rhs);
	void GetDataForMerging(int idx, vector<pair<double, double>>& arr);

public:
	StatesHandler();
	void Work();
	void PrintAvailableStates() const;
	void PrintAvailableTimePoints() const;
	void PrintAvailableLayers();
	void PrintLoaded() const;
	void PrintCommands() const;
};


StatesHandler::StatesHandler()
{
	added = 0;
	end = 1;
	currentTimePointUpper = 0;
	currentTimePointLower = 0;
	isInitialized = false;
	_path = "StatesHandle/";
	Scan();
	Parsing();
}

void StatesHandler::PrintAvailableStates() const
{
	int cntr = 1;
	printf("Available states: ");
	for (const auto& state : states) {
		cout << state.first;
		if (cntr++ % 10 == 0) cout << endl;
	}
	printf("\n");
}

void StatesHandler::PrintAvailableTimePoints()const
{
	int cntr = 1;
	
	string s;
	for (const auto& timePoint : states.at(currentState).time_point)
	{
		s+= to_string(timePoint.first) + " ";
		if ((cntr) % 11 == 0)
		{
			cout << "Cycle " << cntr / 11 << ": "+s;
			printf("\n");
			s.clear();
		}
		cntr++;
	}
	printf("\n");
}

void StatesHandler::PrintAvailableLayers()
{
	int commonq = 0, initializedq=0;

	for (const auto& layer : states.at(currentState).time_point)
	{
		if (layer.first >= currentTimePointLower && layer.first <= currentTimePointUpper)
		{
			commonq++;
			for (const auto& lay : layer.second)
			{ 
				if (lay.first == currentLayer)
				{
					tmpnts.push_back(layer.first);
					handling.push_back(lay.second);
					initializedq++;
					cout << "Point.. " << layer.first<<" ..OK!\n";
					break;
				}
				
			}
		}
	}
	if (commonq == initializedq)
	{
		isInitialized = true;
		system("cls");
	}
	cout << initializedq << " of " << commonq << " has been initialized\n";
	
}

void StatesHandler::End()
{
	end = 0;
}

void StatesHandler::Clear()
{
	cout << "clearing.. ";
	currentState.clear();
	currentLayer.clear();
	currentTimePointUpper=0;
	currentTimePointLower=0;
	isInitialized = false;;
	handling.clear();
	cout << "ready!\n";
}

void StatesHandler::PrintLoaded()const
{
	int cntr = 0;
	for (const auto& item : handling)
	{
		cout << ++cntr << ".  " << item << endl;
	}
	cout << endl;
}


template < typename T >
void GetParam(T& _value, const std::string& s, const size_t number)
{
	std::string resultString;
	size_t currentNumber = 1;
	T result = 0;

	bool breaking = false;
	try {
		for (unsigned int pos = 0; pos < s.size(); pos++)
		{
			if ((s[pos] != ',') && (currentNumber == number) && (s[pos] != 32))
			{
				resultString += s[pos];
			}
			else
			{
				if (!breaking)
				{
					if (currentNumber > number)
						breaking = true;
					if (s[pos] == ',')
						currentNumber++;
				}
				else
					break;
			}
		}

		if (!resultString.empty())
			_value = static_cast<T>(stod(resultString));
		else
			_value = -1;

	}
	catch (std::exception& exc)
	{
		std::string error_text = exc.what();
		error_text += +" at <" + s + ">";
		error_text += "at pos " + number;
		std::cerr << error_text << std::endl;
	}
}

void StatesHandler::PrintCommands() const
{
	printf("'extract' - extract state\n");
	printf("'merge' - merge states\n");
	printf("'store' - store extracted states\n");
	printf("'end' - quit\n");
	printf("------------------------------------\n");
}