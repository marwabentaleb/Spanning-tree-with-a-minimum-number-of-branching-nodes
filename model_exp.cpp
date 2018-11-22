
// -------------------------------------------------------------- -*- C++ -*-
// File : model1.cpp
// --------------------------------------------------------------------------

/* ------------------------------------------------------------

Method Description
-------------------

Goal is to find the tree of graphs that minimizes the number of branch nodes
by applying constraint generation method. 

Number of nodes nbNodes.
Adjacency matrix Adj.

Minimize the number of branch nodes (A branch node is a node of degree 3 or higher).

------------------------------------------------------------ */

#include "getData.hpp"
#include <ilcplex/ilocplex.h>
#include <vector> 

ILOSTLBEGIN

typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloIntArray> IntMatrix;
typedef IloArray<IloBoolArray> BoolMatrix;
typedef IloArray<IloIntVarArray> IntVarMatrix;
IloInt nbNodes;
IloInt M =1000;
IntMatrix Adj;

static void set_master_problem(const IloEnv env,IloModel& model, BoolVarMatrix& x, IloBoolVarArray &z, IntMatrix Adj, vector< IloBoolArray > vec_A){
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
	// link between z and x
	x = BoolVarMatrix(env,nbNodes); 
	for(IloInt i=0;i<nbNodes;i++) {
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

	// escape from the trivial solution
	IloExpr c1(env);
	for(IloInt i=0; i<nbNodes-1; i++){
		for(IloInt j=i+1; j<nbNodes; j++){
			c1 += x[i][j];
		}
	}
	model.add(c1 >= nbNodes-1);
	c1.end();

	// Respect the adjacency matrix
	for(IloInt i=0; i<nbNodes; i++){
		for(IloInt j=0; j<nbNodes; j++){
			model.add(x[i][j] <= Adj[i][j]);
		}
	}

	// Respect the symmetry of x
	for(IloInt i=0; i<nbNodes; i++){
		for(IloInt j=0; j<nbNodes; j++){
			model.add(x[i][j] == x[j][i]);
		}
	}

	// Adding the constraints 1- sum_i_j (a_is - y_ijs)x_ij

	for(IloInt s=0; s<vec_A.size(); s++){
		IloExpr c2(env);
		for(IloInt i=0; i<nbNodes; i++){
			for(IloInt j=0; j<nbNodes; j++){
				c2+= vec_A[s][i]*(1-vec_A[s][j])*x[i][j];
			}
		}

		model.add(c2>=1);
	}

}

static void set_slave_problem(const IloEnv env,IloModel& model, BoolVarMatrix& y, IloBoolVarArray& a, BoolMatrix X, IntMatrix Adj){
	// to set model
	
	// Objective function 
	IloExpr obj(env);
	
	a = IloBoolVarArray(env,nbNodes);
	y = BoolVarMatrix(env,nbNodes); 
	for(IloInt i=0; i<nbNodes;i++) {
		y[i]=IloBoolVarArray(env,nbNodes);
	}

	for(IloInt i=0; i<nbNodes;i++ ){
		for(IloInt j=0; j<nbNodes; j++ ){
			
			obj -= (a[i] - y[i][j])*(X[i][j]);
		}
	}
	obj +=1;
	model.add(IloMaximize(env, obj));
      	obj.end();

	for(IloInt i=0; i<nbNodes;i++ ){
		for(IloInt j=0; j<nbNodes; j++ ){
			model.add(y[i][j] <= a[i]);
			model.add(y[i][j] <= a[j]);
			model.add(y[i][j] >= a[i] + a[j] - 1);
		}
	}
	

	IloExpr c(env);
	for(IloInt i=0; i<nbNodes; i++){
		c+= a[i];		
	}
	model.add(c >= 1);
	model.add(c <= nbNodes-1);
	c.end();

}


int main(int, char**) {
   IloEnv env;
   try {

	char data[] = "./data/test02.txt"; // file's name that contains the data
	getData(env,data,Adj,nbNodes); // get data
	
	// initialisation
	vector< IloBoolArray > vec_A; 
	
	BoolMatrix X(env,nbNodes);
	for(IloInt i=0; i<nbNodes; i++){
		X[i]=IloBoolArray(env,nbNodes);
	}
	IloBoolArray vec_tmp(env,nbNodes);
	bool is_positive = false;
	while (is_positive == false){

		// initialisation
		IloModel master_model(env); // model's initialization
		BoolVarMatrix x(env,nbNodes);
		IloBoolVarArray z(env,nbNodes); // z's initialization

		// set master model
		set_master_problem(env,master_model,x,z,Adj,vec_A);
	
		// solve master model
		IloCplex cplex_master(master_model); // solveur
		cplex_master.extract(master_model);
	      	cplex_master.exportModel("model_exp.lp");
	      	cplex_master.solve(); // model's resolution

		// filling the matrix which contains the solution founded by the master problem
		for(IloInt i=0; i<nbNodes; i++){
			for(IloInt j=0; j<nbNodes; j++){
				X[i][j] = cplex_master.getValue(x[i][j]);
			}
		}
		
		env.out() << "-Master solution status = " << cplex_master.getStatus() << endl;
		env.out() << "------------------------------------------------------------"<< endl;
	      	env.out() << "-master_model Cost = " << cplex_master.getObjValue() << endl;
		env.out() << "------------------------------------------------------------"<< endl;
		env.out() << "-Solution : \n" << endl;
		for(IloInt i=0; i<nbNodes; i++){
			for(IloInt j=0; j<nbNodes; j++){
				env.out() << (X[i][j]) << "\t";	
			}
			env.out() << "\n";
		}    
		env.out() << "------------------------------------------------------------"<< endl; 


		IloModel slave_model(env);
		BoolVarMatrix y(env,nbNodes);
		IloBoolVarArray a(env,nbNodes);

		// set slave model
		set_slave_problem(env,slave_model,y,a,X,Adj);
		// solve master model
		IloCplex cplex_slave(slave_model); // solveur
		cplex_slave.extract(slave_model);
	      	cplex_slave.exportModel("slave_model.lp");
	      	cplex_slave.solve(); // model's resolution
		

		env.out() << "-Slave solution status = " << cplex_slave.getStatus() << endl;
		env.out() << "------------------------------------------------------------"<< endl;
	      	env.out() << "-slave_model Cost = " << cplex_slave.getObjValue() << endl;
		env.out() << "------------------------------------------------------------"<< endl;
		if (cplex_slave.getObjValue() > 0){
			env.out() << "sub set obtained by solving the slave model" << endl;
			for(IloInt j=0; j<nbNodes; j++){
					env.out() << cplex_slave.getValue(a[j]) << "\t";	
			}
			env.out() << "\n";
			env.out() << "------------------------------------------------------------"<< endl; 
		}

		
		if(cplex_slave.getObjValue() <=0){
			env.out() << " \t\t\t **END** \n***********************************************************"<< endl;
			env.out() << "-Optimal cost = " << cplex_master.getObjValue() << endl;
			env.out() << "-Optimal solution : \n" << endl;
			for(IloInt i=0; i<nbNodes; i++){
				for(IloInt j=0; j<nbNodes; j++){
					env.out() << (X[i][j]) << "\t";	
				}
				env.out() << "\n";
			}    
			env.out() << "***********************************************************"<< endl;
			is_positive = true;
		}else{

		}
		for(IloInt j=0; j<nbNodes; j++){
			vec_tmp[j] = cplex_slave.getValue(a[j]);
		}
		vec_A.push_back(vec_tmp);
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



