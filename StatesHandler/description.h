#pragma once

void StatesHandler::Scan()
{

	for (const auto& entry : std::filesystem::recursive_directory_iterator(_path)) {
//		string entire = entry.path().string();
		files_list.push_back(entry.path().string());
	}
	cout << "Founded " << files_list.size() << " files\n";
}

void StatesHandler::Parsing()
{
	for (const auto& item : files_list)
	{
		string _state;
		string _layer;
		string _time;
		int elem = 0;
		for (int i = _path.size(); i < (int)item.size(); i++)
		{
			if (item[i] == '_')
			{
				elem++;
				continue;
			}

			switch (elem)
			{
			case 0:
				_state += item[i];
				break;
			case 1:
				_time += item[i];
				break;
			case 2:
				_layer += item[i];
				break;
			}


		}
		try {
			states[_state].time_point[stoi(_time)].push_back({ _layer, item });
		}
		catch (exception& a)
		{
			cerr << __FUNCTION__ << " " << a.what();
		}
	}

	cout << "Parsed to " << states.size() << " states\n\n";

}

void StatesHandler::Commands(const string & input)
{
	if (input == "EXTRACT") ExtractState();
	if (input == "STORE") Store();
	if (input == "CLEAR") Clear();
	if (input == "MERGE") Merge();
	if (input == "END") End();
}

void StatesHandler::Work()
{
	string command;
	while (end) 
	{
		PrintCommands();
		printf("select command:\n>>> ");
		cin >> command;
		ToUpperFunct(command);
		Commands(command);
	}
}


void StatesHandler::ExtractState()
{
	PrintAvailableStates();
	cout << ">>> ";
	cin >> (currentState);
	try
	{
		if (states.count(currentState)) {
			PrintAvailableTimePoints();
			ExtractCycle();
		}
		else
		{
			system("cls");
			cerr << "State " << currentState << " not found\n";
		}
	}
	catch (exception & exc)
	{
		cerr << "Error: " << exc.what() << endl;
	}
}

void StatesHandler::ExtractCycle()
{
	string val1, val2;
	cout << "First time point\n>>> ";
	cin >> (val1);
	cout << "Second time point\n>>> ";
	cin >> val2;
	cout << "Select layer\n>>> ";
	cin >> currentLayer;

	try
	{
		currentTimePointLower = stoi(val1);
		currentTimePointUpper = stoi(val2);

		if (currentTimePointLower<=currentTimePointUpper) 
		{
			PrintAvailableLayers();
			
		}
		else
		{
			system("cls");
			cerr << "Wrong input data " << val1 << " " << val2 << " " << currentLayer << endl;
		}
	}
	catch (exception& exc)
	{
		cerr << "Error: " << exc.what() << endl;
	}

	cout << endl;
}

void StatesHandler::Store()
{
	if (!isInitialized) return;

	ofstream ofs("coords.pvm");
	ofs << "CARTESIAN\nSHIFT\n";
	for (const auto& file : handling)
	{
		cout << file<<" ";
		ifstream ifs(file);
		if (!ifs.is_open()) { cout << " false..\n"; break; }
		string temp;
		while (!ifs.eof())
		{
			getline(ifs, temp);
			if (!temp.empty())
				ofs << temp << endl;
		}
		cout << " Ok!\n";
		ifs.close();
	}
	ofs.close();
	cout << "Completed\n";
}

void StatesHandler::Merge()
{
	if (handling.empty()) { system("cls"); cout << "Nothing to merge\n"; return; }
	PrintLoaded();
	cout << "Wich would you merge?\n>>> ";
	string v1, v2;
	cin >> v1; cout << ">>> "; cin >> v2;
	int idx1 = -1, idx2 = -1;
	try {
		idx1 = stoi(v1)-1;
		idx2 = stoi(v2)-1;
		Merging(idx1, idx2);
	}
	catch (exception& mex)
	{
		cout << __FUNCTION__ << mex.what() << endl;
	}
	
}

void StatesHandler::GetDataForMerging(int idx, vector<pair<double, double>>& arr)
{
	cout << "file " << handling[idx] << "... ";
	ifstream ifs(handling[idx]);
	string tmp;
	double x = 0, y = 0;
	if (!ifs.is_open()) return;
	while (!ifs.eof())
	{
		getline(ifs, tmp);
		if (!tmp.empty())
		{
			GetParam(x, tmp, 2);
			GetParam(y, tmp, 3);
			arr.push_back({ x,y });
		}
	}

	cout << "ready\n";
	ifs.close();
}

void StatesHandler::Merging(int lhs, int rhs)
{
	pair<vector<pair<double, double>>, vector<pair<double, double>>> dataForMerging;

	GetDataForMerging(lhs, dataForMerging.first);
	GetDataForMerging(rhs, dataForMerging.second);

	// Now make interface that allows make point between two seleted points
	if (dataForMerging.first.size() != dataForMerging.second.size()) { cout << "Error data size" << __FUNCTION__ << endl; return; }
	cout << "Your chooise: " << tmpnts[lhs] << " && " << tmpnts[rhs] << endl;
	string v;
	int k=0;
	while (true)
	{
		cout << "Select intermediate point(int only). -1 to exit.\n>>> ";
		cin >> v;
		if (v == "-1") {
			system("cls"); return;
		}
		try {
			k = stoi(v);
			if (k > tmpnts[lhs] && k < tmpnts[rhs])
			{
				cout << "You select point " << (k - tmpnts[lhs]) << "/" << (tmpnts[rhs] - tmpnts[lhs]) << " from range. (1 - yes, any other - no)\n>>> ";
				cin >> v;
				if (v == "1")
				{
					break;
				}
			}
		}
		catch(exception & exc)
		{
			cout << __FUNCTION__ << exc.what() << "::wrong value-> " << v << endl;
		}
	}

	string	name = "temp_" + to_string(++added);
	ofstream ofs(name);
	double nx = 0, ny = 0;
	for (int i = 0; i < (int)dataForMerging.first.size(); i++)
	{
		double x1 = dataForMerging.first[i].first;
		double x2 = dataForMerging.second[i].first;
		double y1 = dataForMerging.first[i].second;
		double y2 = dataForMerging.second[i].second;
		double range_x = x2 - x1;
		double range_y = y2 - y1;
		double part = ((double)k - (double)tmpnts[lhs]) / ((double)tmpnts[rhs] - (double)tmpnts[lhs]);
		nx = (x1 + part * range_x);
		ny = (y1 + part * range_y);
		ofs << i + 1 << "," << nx << "," << ny << "," << endl;
	}
	ofs.close();

	handling.insert(handling.begin()+rhs, name);
	tmpnts.insert(tmpnts.begin() + rhs, k);
	system("cls");
	cout << "Merged!\n";
}