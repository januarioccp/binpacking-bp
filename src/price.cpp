#include "price.h"

Price::Price(Data *input) : price(input->env(), input->nItems()),
							pricingModel(input->env()),
							x(input->env(), input->nItems()),
							priceSolver(input->env())
{
	this->in = input;
	pricingModel.setName("Knapsack");

	for (int i = 0; i < in->nItems(); i++)
		x[i].setName(("x_" + to_string(i)).c_str());

	ReducedCost = IloAdd(pricingModel, IloMinimize(in->env(), 1));
	ReducedCost.setName("Obj");
	pricingModel.add(IloScalProd(in->itemWeight(), x) <= in->binCapacity());

	priceSolver.extract(pricingModel);
	priceSolver.setOut(in->env().getNullStream());
	
	xFast = new int[in->nItems()];
    p = new int[in->nItems()];
    w = new int[in->nItems()];

	for (int i = 0; i < in->nItems(); i++)
         w[i] = in->itemWeight_[i];
}

Price::~Price(){
	delete [] p;
	delete [] w;
	delete [] xFast;
}

Price::Price(Data *input, Node &n):Price(input){
	
	// Force items to stay together...
	for (auto &i : n.together_)
		pricingModel.add(x[i.first] == x[i.second]);
		
	// Conflict constraint: at most one item can enter in this bin
	for (auto &i : n.conflict_)
		pricingModel.add(x[i.first] + x[i.second] <= 1);
}

IloNum Price::reducedCost(bool isRoot)
{
	if(isRoot)
		return rc;
	else
		return priceSolver.getValue(ReducedCost);
}

void Price::solve(bool isRoot)
{
	if(isRoot){
		rc = 1.0 - minknap(in->nItems(), p, w, xFast, in->binCapacity()) / in->factor;
	}
	else{
		ReducedCost.setExpr(1+IloScalProd(price, x));
		priceSolver.extract(pricingModel);
		priceSolver.solve();
	}
}

ostream &operator<<(ostream &out, Price &p)
{
	out << p.pricingModel << endl;
	return out;
}

string Price::getStatus()
{
	stringstream buffer;
	buffer << priceSolver.getStatus();
	return buffer.str();
}

bool Price::isFeasible(){
	return !(this->priceSolver.getCplexStatus() == IloCplex::Infeasible);
}

void Price::setDual(IloNum var, IloNum dual)
{
	price[var] = -dual;
}
