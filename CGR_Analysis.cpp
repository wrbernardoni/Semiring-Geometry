#include <iostream>
#include <fstream>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>

#include <Semirings.h>
#include <Monoids.h>
#include <ShortestPath.h>

using namespace std;
using namespace Semiring;

// Speed of light in km/s
const double speedOfLight = 299792.458;
const int NumberOfNodes = 10; // TODO make this dynamic. Unfortunately so much of our code assumes its not.
const int MaxCumulant = 50;
const int StorageN = 1;

#include <omp.h>

struct ContactFrame
{
	string from;
	string to;
	string contact;
	double rise;
	double set;
};

int main(int argc, char* argv[])
{
	omp_set_num_threads(4);

	if ((argc != 4) && (argc != 3))
	{
		cerr << "Invalid number of arguments, should be called as: CGRAnalysis ContactPlan.csv Distances.csv OutputPrefix" << endl;
		return 1;
	}

	string ContactPath = argv[1];
	string DistancePath = argv[2];
	string OutputPath;
	if (argc == 4)
		OutputPath = argv[3];
	else
		OutputPath = "./out";

	cout << "---- Contact Analysis ----" << endl;
	cout << "Contact Plan Path:\t" << ContactPath << endl;
	cout << "Distance Path:\t\t" << DistancePath << endl;
	cout << "Output Prefix:\t\t" << OutputPath << endl;
	cout << "Max Number of Nodes:\t" << NumberOfNodes << endl;
	cout << "Maximum Cumulant:\t" << MaxCumulant << endl;
	cout << "---- Contact Analysis ----" << endl << endl;

	cout << "Reading contact plan from " << ContactPath << endl;
	ifstream contactFile(ContactPath);

	if (!contactFile)
	{
		cerr << ContactPath << " could not be opened." << endl;
		return 1;
	}

	for (int i = 0; i < 8; i++)
	{
		string temp;
		getline(contactFile, temp);
	}

	vector<ContactFrame> contacts;
	set<string> nodes;
	while (contactFile)
	{
		ContactFrame frame;

		string temp;
		getline(contactFile, temp);

		stringstream tStream(temp);

		string id;
		getline(tStream, id, ',');

		if (id == "Analysis")
			break;

		stringstream idStream(id);
		string id1, id2;
		getline(idStream, id1, ' ');
		getline(idStream, id2, ' ');
		getline(idStream, id2, ' ');
		
		frame.from = id1;
		frame.to = id2;
		nodes.insert(id1);
		nodes.insert(id2);

		frame.contact = id1 + " - " + id2;

		string riseStr;
		getline(tStream, riseStr, ',');
		string setStr;
		getline(tStream, setStr, ',');

		frame.rise = stod(riseStr);
		frame.set = stod(setStr);
		contacts.push_back(frame);
	}

	cout << "\t"<< contacts.size() << " contacts loaded" << endl;


	cout << "Reading distances from " << DistancePath << endl;
	ifstream distanceFile(DistancePath);
	if (!distanceFile)
	{
		cerr << DistancePath << " could not be opened." << endl;
		return 1;
	}

	for (int i = 0; i < 5; i++)
	{
		string temp;
		getline(distanceFile, temp);
	}

	string distanceFileHeading;
	getline(distanceFile, distanceFileHeading);
	vector<string> headers;
	stringstream distanceHeadSS(distanceFileHeading);
	while (distanceHeadSS)
	{
		string dH;
		getline(distanceHeadSS, dH, ',');
		if (dH == "TIME_UNITS")
			continue;

		string dH1;
		string dH2;
		stringstream dHSS(dH);
		getline(dHSS, dH1, ' ');
		if (dH1 != "Distance")
			break;

		getline(dHSS, dH1, ' ');
		getline(dHSS, dH2, ' ');
		getline(dHSS, dH2, ' ');

		headers.push_back(dH1 + " - " + dH2);
	}


	map<string,vector<double>> delays;
	vector<double> timesteps;

	// Skip the units line
	getline(distanceFile, distanceFileHeading);

	while (distanceFile)
	{
		string line;
		getline(distanceFile, line);
		stringstream lineSS(line);

		string tsStr;
		getline(lineSS, tsStr, ',');

		if (tsStr == "")
			break;

		timesteps.push_back(stod(tsStr));

		for (int i = 0; i < headers.size(); i++)
		{
			string hDistStr;
			getline(lineSS, hDistStr, ',');
			delays[headers[i]].push_back(stod(hDistStr) / speedOfLight);
		}
	}

	cout << "Distances loaded" << endl;

	cout << "There are " << nodes.size() << " nodes active in the contact plan. Numbering them" << endl;
	cout << "\tOutputting node names to " << (OutputPath + "_Names.csv") << endl;

	ofstream outf((OutputPath + "_Names.csv"));
		
	map<string, int> NodetoID;
	map<int, string> IDtoNode;

	int UID = 0;
	for (auto n : nodes)
	{
		NodetoID[n] = UID;
		IDtoNode[UID] = n;
		if (UID < NumberOfNodes)
		outf << UID << "," << n << endl;
		UID++;
	}

	outf.close();


	cout << "Generating CGR Matrix" << endl;
	cout << "\tGiving storage to named nodes" << endl;
	Matrix<CGRSemiring, NumberOfNodes, NumberOfNodes> CGRMatrix;
	for (auto n : nodes)
	{
		if ((NodetoID[n] < NumberOfNodes) && (NodetoID[n] % StorageN == 0))
		{
			CGRMatrix(NodetoID[n],NodetoID[n]) = CGRSemiring::Storage();
		}
		else if (NodetoID[n] < NumberOfNodes)
		{
			CGRMatrix(NodetoID[n],NodetoID[n]) = CGRSemiring::One();
		}
	}

	cout << "\tAdding contacts" << endl;
	int nContacts = 0;
	for(int i = 0; i < contacts.size(); i++)
	{
		int f = NodetoID[contacts[i].from];
		int t = NodetoID[contacts[i].to];
		string c = contacts[i].contact;

		if ((f >= NumberOfNodes) || (t >= NumberOfNodes))
			continue;

		double startTime = contacts[i].rise;
		double tracker = contacts[i].rise;
		int trackerStart = 0;

		while (tracker > timesteps[trackerStart])
		{
			trackerStart++;

			if ((trackerStart + 1) >= timesteps.size())
			{
				break;
			}
		}

		while (tracker != contacts[i].set)
		{
			double cutOff;
			if ((trackerStart + 1) >= timesteps.size())
			{
				cutOff = contacts[i].set;
			}
			else
			{
				cutOff = min(contacts[i].set,timesteps[trackerStart + 1]);
			}

			CGRMatrix(f,t) = CGRMatrix(f,t) + CGRSemiring(Contact(tracker, cutOff, delays[c][trackerStart]));
			CGRMatrix(t,f) = CGRMatrix(t,f) + CGRSemiring(Contact(tracker, cutOff, delays[c][trackerStart]));
			nContacts += 2;

			tracker = cutOff;
			trackerStart++;
		}
	}

	cout << "\t" << nContacts << " contacts added" << endl;

	// TODO generate contact matrix
	cout << "Matrix generated" << endl;

	cout << "Generating diagonal submatrix" << endl;
	auto diagonal = Matrix<CGRSemiring, NumberOfNodes, NumberOfNodes>::Zero();
	for (int i = 0; i < NumberOfNodes; i++)
	{
		diagonal(i,i) = CGRMatrix(i,i);
	}

	cout << "Generating one-step matrix" << endl;
	auto oneStep = CGRMatrix;
	for (int i = 0; i < NumberOfNodes; i++)
	{
		oneStep(i,i) = CGRSemiring::Zero();
	}

	auto nsStep = oneStep + diagonal;

	cout << "Saving one step matrix to " << (OutputPath + "_step_1.CSG") << endl;
	ofstream oneStepSave((OutputPath + "_step_1.CSG"));
	for (int j = 0; j < min(NumberOfNodes, (int)nodes.size()); j++)
	{
		for (int k = 0; k < min(NumberOfNodes, (int)nodes.size()); k++)
		{
			oneStepSave << j << "," << k << "," << nsStep(j,k) << endl;
		}
	}
	oneStepSave.close();
	cout << "\tStep matrix saved" << endl;

	cout << "Generating two step matrix" << endl;

	nsStep = nsStep + oneStep * oneStep;
	cout << "Saving two step matrix to " << (OutputPath + "_step_2.CSG") << endl;
	oneStepSave.open((OutputPath + "_step_2.CSG"));
	for (int j = 0; j < min(NumberOfNodes, (int)nodes.size()); j++)
	{
		for (int k = 0; k < min(NumberOfNodes, (int)nodes.size()); k++)
		{
			oneStepSave << j << "," << k << "," << nsStep(j,k) << endl;
		}
	}
	oneStepSave.close();
	cout << "\tStep matrix saved" << endl;

	// TODO find Kleene star and output each cumulant

	cout << "Creating incrementing matrix" << endl;
	auto step = CGRMatrix + Matrix<CGRSemiring, NumberOfNodes, NumberOfNodes>::One();

	cout << "Beginning cumulant generation" << endl;
	auto nC = step;
	for (int i = 1; i < MaxCumulant; i++)
	{
		cout << "\t[" << i << "] cumulant generated " << endl;
		// cout << nC << endl;

		cout << "\tOutputting the " << i << " cumulant to " << (OutputPath + "_cumulant_" + to_string(i) + ".CSG") << endl;
		ofstream outf((OutputPath + "_cumulant_" + to_string(i) + ".CSG"));

		for (int j = 0; j < min(NumberOfNodes, (int)nodes.size()); j++)
		{
			for (int k = 0; k < min(NumberOfNodes, (int)nodes.size()); k++)
			{
				outf << j << "," << k << "," << nC(j,k) << endl;
			}
		}
		outf.close();
		cout << "\tCumulant saved" << endl;

		cout << "Generating the " << i+2 << " step matrix" << endl;
		nsStep = nsStep + Conjugate(nC, oneStep); //// oneStep * nC * oneStep;
 
		cout << "Saving the " << i+2 << " step matrix to " << (OutputPath + "_step_" + to_string(i+2) + ".CSG") << endl;
		outf.open((OutputPath + "_step_" + to_string(i+2) + ".CSG"));

		for (int j = 0; j < min(NumberOfNodes, (int)nodes.size()); j++)
		{
			for (int k = 0; k < min(NumberOfNodes, (int)nodes.size()); k++)
			{
				outf << j << "," << k << "," << nsStep(j,k) << endl;
			}
		}
		outf.close();
		cout << "\tStep matrix saved" << endl;

		cout << "Generating the " << i + 1 << " cumulant" << endl;
		auto next = nC * step;

		if (next == nC)
		{
			cout << "Matrix stabilized at " << i << " cumulant" << endl;
			break;
		}

		nC = next;
	}
	
	return 0;
}
