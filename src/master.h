#ifndef MASTER_H_B_AND_P
#define MASTER_H_B_AND_P

#include "data.h"
#include "node.h"
#include "price.h"

// CPLEX
#include <ilcplex/ilocplex.h>

// STL
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

class Master
{
public:
	Master(Data *input);
	void solve();
	void solveIP();
	friend class Tree;

	void updateBranchingRules(Node &no);

	// Returns a double value with the objective value from variable binPackingSolver
	double getObjValue();
	void debug();
	void getDual(Price &p,bool isRoot = false);

	// Returns the Feasibility status of binPackingSolver
	bool isFeasible();

	// Returns a string that contains information about the model status
	string getStatus();

	// Add column c to the model
	void addColumn(Price &p, bool isRoot = false);
	
	// Collection of itens inside the bin
	vector<vector<bool>> bin;

	// Set of columns that cannot enter the basis
	vector<int> forbidenColumn;

	// Overloading output operator
	friend ostream &operator<<(ostream &out, Master &m);
	
private:
	Data *in;
	IloModel masterBinPacking;
	IloNumVarArray Lambda;
	IloRangeArray Assignment;
	IloObjective binsUsed;
	IloCplex binPackingSolver;
	bool MIP;
};
#endif