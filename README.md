# Spanning-tree-with-a-minimum-number-of-branching-nodes
The goal is not only to get a tree but to have one that minimize the branching nodes. Thus, we define by a branch node, a vertex v with degree 3 or higher.

We propose two approach to solve the problem : linear programming and a heuristic. For the first approach, we adapt two possible models. One is with a polynomial number of constraints and the other with an exponential number.

# Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.
## Prerequisites
To run the codes, you must install the CPLEX solver.
## Running the tests
Once you download the project and install the CPLEX solver, you must follow the following instructions to start the tests :
### Step 1 
Open terminal and go to the project folder
### Step 2
To launch the resolution of the polynomial model, you have to tape :
  1. make model_poly
  2. ./model_poly
### Step 3 
To obtain the solution by applying constraint generation method, you have to tape :
  1. make model_exp
  2. ./model_exp
### Step 4
To obtain the solution by using the heuristic, you have to tape :
  1. make heuristic
  2. ./heuristic
  
Then, you have to answer the question displayed "Write the name of the method you want to test : Algo1 or Algo2 ?
" by typing the name of the algorithm (Algo1 or Algo2).

