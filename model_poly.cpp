
// -------------------------------------------------------------- -*- C++ -*-
// File : model1.cpp
// --------------------------------------------------------------------------

/* ------------------------------------------------------------

Problem Description
-------------------

Goal is to find the tree of graphs that minimizes the number of branch nodes.

Number of nodes nbNodes.
Adjacency matrix Adj.

Minimize the number of branch nodes (A branch node is a node of degree 3 or higher).

------------------------------------------------------------ */

#include "getData.hpp"
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloIntArray> IntMatrix;
typedef IloArray<IloIntVarArray> IntVarMatrix;
IloInt nbNodes;
IloInt M =1000;

IntMatrix Adj;

static void setmodel(const IloEnv env,IloModel& model, BoolVarMatrix& y, BoolVarMatrix& x, IloBoolVarArray &z, IloNumVarArray& p, IntMatrix Adj){
	// to set model
	
	// Objective function 
	IloExpr obj(env);
	z = IloBoolVarArray(env,nbNodes);
	for(IloInt i=0; i<nbNodes;i++ ){
		obj +=z[i]; 
	}
	model.add(IloMinimize(env, obj));
      	obj.end();

	// Constraints
	x = BoolVarMatrix(env,nbNodes); 
	for(int i=0;i<nbNodes;i++) {
		x[i]=IloBoolVarArray(env,nbNodes);
	}
	
	for(IloInt i=0; i<nbNodes; i++){
		IloExpr c(env);
		for(IloInt j=0; j<nbNodes; j++){
			c += x[i][j];
		}
		model.add(c <= M*z[i] + 2);
		c.end();
	}

	IloExpr c1(env);
	for(IloInt i=0; i<nbNodes; i++){
		for(IloInt j=i; j<nbNodes; j++){
			c1 += x[i][j];
		}
	}
	model.add(c1 == nbNodes-1);
	c1.end();

	// Respect the adjacency matrix
	for(IloInt i=0; i<nbNodes; i++){
		for(IloInt j=0; j<nbNodes; j++){
			model.add(x[i][j] <= Adj[i][j]);
		}
	}

	y = BoolVarMatrix(env,nbNodes); 
	for(int i=0;i<nbNodes;i++) {
		y[i]=IloBoolVarArray(env,nbNodes);
	}

	for(IloInt i=0; i<nbNodes; i++){
		for(IloInt j=0; j<nbNodes; j++){
			model.add(y[i][j] + y[j][i] == x[i][j]);
		}
	}

	// Respect the symmetry of x
	for(IloInt i=1; i<nbNodes; i++){
		for(IloInt j=0; j<nbNodes; j++){
			model.add(x[i][j] == x[j][i]);
		}
	}

	for(IloInt i=0; i<nbNodes; i++){
		IloExpr c2(env);
		for(IloInt j=0; j<nbNodes; j++){
			c2 += y[j][i];
		}
		model.add(c2 <= 1);
		c2.end();
	}

	p = IloNumVarArray(env,nbNodes,0, IloInfinity, ILOINT);

	model.add(p[0] == 1);

	for(IloInt i=0; i<nbNodes; i++){
		for(IloInt j=0; j<nbNodes; j++){
			model.add((p[i] + y[i][j] - M*(1-y[i][j])) <= p[j]);
		}
	}
}
int main(int, char**) {
   IloEnv env;
   try {

	char data[] = "./data/test03.txt"; // file's name that contains the data
	getData(env,data,Adj,nbNodes); // get data
	
	BoolVarMatrix y(env,nbNodes); // y's initialization
	BoolVarMatrix x(env,nbNodes); // x's initialization
	IloBoolVarArray z(env,nbNodes); // z's initialization
	IloNumVarArray p(env,nbNodes); // p's initialization
	IloModel model(env); // model's initialization

	setmodel(env,model,y,x,z,p,Adj); // set model

	IloCplex cplex(model); // solveur
	cplex.extract(model);
      	cplex.exportModel("model1.lp");
      	cplex.solve(); // model's resolution

	env.out() << "-Solution status = " << cplex.getStatus() << endl;
	env.out() << "------------------------------------------------------------"<< endl;
      	env.out() << "-Cost = " << cplex.getObjValue() << endl;
	env.out() << "------------------------------------------------------------"<< endl;

	       	
	if (nbNodes <= 15) {
		env.out() << "-Solution : \n" << endl;
		for(IloInt i=0; i<nbNodes; i++){
			for(IloInt j=0; j<nbNodes; j++){
				env.out() << cplex.getValue(x[i][j]) << "\t";	
			}
			env.out() << "\n";
		}    
		env.out() << "------------------------------------------------------------"<< endl;  
	}	 

	
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Error" << endl;
   }
   env.end();
   return 0;
}
