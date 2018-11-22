
#include <ilcplex/ilocplex.h>
#include <vector>
#include <utility>
#include <algorithm> 
#include "getData.hpp"
#include"graph_class.hpp"

ILOSTLBEGIN

typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloIntArray> IntMatrix;
typedef IloArray<IloIntVarArray> IntVarMatrix;
IloInt nbNodes;
IntMatrix Adj;

// Copy the matrix MatriceAdj in Adj_res
void copyMatrix (const IloEnv env, IntMatrix MatriceAdj, IntMatrix &Adj_res, int n){
	// ALLOCATION
	Adj_res = IntMatrix(env,n);
	for(int i=0; i<n;i++){
   		Adj_res[i]=IloIntArray(env,nbNodes);
	}

	for(int i=0; i<n;i++ ){
	   		for(int j=0; j<n;j++ ){	
	   			Adj_res[i][j] = MatriceAdj[i][j];
			}	
	}
}

struct myclass {
  bool operator() (pair<int,int>& p1, pair<int,int>& p2) { return (p1.second > p2.second);}
} sort_depth;

int nb_branch(vector<int> Parents){
	int n=0;
	int nS;
	for(int i =0; i< Parents.size(); i++){
		nS=0;
		for(int j =0; j< Parents.size(); j++){		
			if((j!=i)&((Parents[j] == i)||(Parents[i] == j))){
				nS++;
			}
		}
		if(nS>=3){ n++;}
	}
	return(n);
}

void reduction(vector<int> Parents, IntMatrix MatriceAdj, int node, int** &Adj_res, int n){
	// ALLOCATION
	Adj_res = new int* [n];
	for(int i=0; i<n;i++){
   		Adj_res[i] = new int[n];
	}
	// COPY
	for(int i=0; i<n;i++ ){
   		for(int j=0; j<n;j++ ){	
   			Adj_res[i][j] = MatriceAdj[i][j];
		}	
	}	

	list<int> queue;
	queue.push_back(node);
	
	while(!queue.empty())
	{
		node = queue.front();
		queue.pop_front();
		for(int j=0; j<Parents.size(); j++){
			
			if((j!=node)&(Parents[j] == node)){
				queue.push_back(j);
				for(int k =0; k<n; k++){
					Adj_res[j][k] = 0;
					Adj_res[k][j] = 0;	
				}
			}
		}
	}
}

void Algorithm1 (IntMatrix MatriceAdj, vector<int> &ParentsOpt, int n, vector< pair<int,int> > prof){
	int s;
	int** Adj_induit;
	vector<int> ParentsCurrent;
	int CostOpt = nb_branch(ParentsOpt);
	
	for(int i=0; i<prof.size(); i++){

		s = prof[i].first;
		reduction(ParentsOpt,MatriceAdj,s,Adj_induit,n);

		for(int i =0; i<n; i++){
			ParentsCurrent.push_back(ParentsOpt[i]);
		}

		for(int v =0; v<n; v++){
			if((Adj_induit[v][s] ==1)&(v!=ParentsOpt[s])){
				ParentsCurrent[s] = v;
				if ( nb_branch(ParentsCurrent) <= CostOpt){
					ParentsOpt[s] = v;
					CostOpt = nb_branch(ParentsCurrent);
					
				} 
			} else{ 
				ParentsCurrent[s] = ParentsOpt[s];
			}
		}
	}

}
void construction_graph(Graph &g, IntMatrix MatriceAdj, int n){

	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			if (MatriceAdj[i][j] == 1){
				g.addEdge(i,j);
			}
		}
	}
}
int voisin(const IloEnv env, IntMatrix MatriceAdj, vector<int> &ParentsVoisin, IntMatrix &MatriceAdjVoisin, int n, pair<int,int> arc, vector<int> &ProfondeursVoisin, int s){
	
	copyMatrix (env, MatriceAdj,MatriceAdjVoisin,n);
	MatriceAdjVoisin[arc.first][arc.second] = 0;
	MatriceAdjVoisin[arc.second][arc.first] = 0;
	Graph g(n);
	construction_graph(g,MatriceAdjVoisin,n);
	g.BFS(s,ParentsVoisin,ProfondeursVoisin);
	int test =0;
	int i=0; // it is possible to creat a tree through the matrice Adj
	while(i<n){

		if((i!=s)&(ParentsVoisin[i] == n*n)){
			test = 1; // it isn't possible to creat a tree through the matrice Adj
		} 
		i++;
	}
	return(test);
}

void Algorithm2(const IloEnv env, IntMatrix MatriceAdj, vector<int> &ParentsOpt, int n, int s){

	IntMatrix MatriceAdjVoisin;
	int test;
	vector<int> ProfondeursVoisin;
	vector<int> ParentsVoisin;
	pair<int,int> arc;
	int costOpt;
	if(s==0){
		costOpt = n*n;
	} else{
		costOpt = nb_branch(ParentsOpt);
	}
	
	for (int i = 0; i<n; i++){
		for (int j = i+1; j<n; j++){
			if(MatriceAdj[i][j] ==1){
				arc = make_pair(i,j);
				copyMatrix (env, MatriceAdj,MatriceAdjVoisin,n);
				test = voisin(env, MatriceAdj, ParentsVoisin,MatriceAdjVoisin,n,arc, ProfondeursVoisin,s);
				if (test ==0){
					if(nb_branch(ParentsVoisin) <= costOpt){
						costOpt = nb_branch(ParentsVoisin);
						for(int k =0; k<n; k++){
							ParentsOpt[k] = ParentsVoisin[k];
						}
					}
				}
			}
		}
	}

}

int main(int, char**) {
   IloEnv env;
   try {

	string ch1;
	cout << "Write the name of the method you want to test : Algo1 or Algo2 ?" << endl;
	cin >> ch1;
 
	vector<int> Parents;
	vector<int> ParentsOpt;
	vector<int> Profondeurs;

	vector< pair<int,int> > prof;
	char data[] = "./data/test00.txt"; // file's name that contains the data
	getData(env,data,Adj,nbNodes); // get data
	int CostOpt = nbNodes;
	
	// First approach.......................................................
	if(ch1 == "Algo1"){
		Graph g((int) nbNodes);
		construction_graph(g,Adj,(int) nbNodes);
		// we try all the possibilty to creat the tree  
		for(int Sommet =0; Sommet<(int) nbNodes; Sommet++){
			g.BFS(Sommet,Parents,Profondeurs);
		
			int Nb_branche = nb_branch(Parents);
		
			for (int i = 0 ;i < (int) nbNodes ; i++) {
				prof.push_back (make_pair(i,Profondeurs[i])); 
			}

			sort(prof.begin(), prof.end(), sort_depth);
			Algorithm1 (Adj,Parents,(int) nbNodes,prof);
			if(CostOpt>nb_branch(Parents)){
				CostOpt = nb_branch(Parents);
				for(int i =0; i<(int) nbNodes; i++){
					ParentsOpt.push_back(Parents[i]);
				}
			}
		}
	}
	
	// Second approach.......................................................
	if(ch1 == "Algo2"){
		vector<int> ParentsVoisin;
		IntMatrix MatriceAdjVoisin;
		pair<int,int> arc = make_pair(2,3);
		vector<int> ProfondeursVoisin;
		int test = voisin(env, Adj, ParentsVoisin, MatriceAdjVoisin, (int) nbNodes, arc, ProfondeursVoisin, 0);

		for(int k=0; k < (int) nbNodes; k++){
			ParentsOpt.push_back((int) nbNodes* (int) nbNodes);
		}

		for (int s=0; s < (int) nbNodes ; s++){
			if ( s==0){
				for(int k =0; k<(int) nbNodes; k++){
					ParentsOpt[k] = (int) nbNodes * (int) nbNodes;
				}
			}
			Algorithm2(env, Adj, ParentsOpt, (int) nbNodes, s);
			CostOpt = nb_branch(ParentsOpt);
		}
	}
	// Final display........................................................

	cout << "___________END ALGORITHM______________"<< endl;
	cout << "optimal Branch nb   : " <<CostOpt << endl;
	cout << "Tree : " << endl;
	for(int i= 0; i<(int) nbNodes; i++){
		if (i+1 != ParentsOpt[i] + 1){
		env.out() << i+1 << " - " <<ParentsOpt[i] + 1<< endl;
		}
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
