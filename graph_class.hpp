#ifndef GRAPH_CLASS_H
#define GRAPH_CLASS_H

#include <list>
#include <vector>
using namespace std;
 
// This class represents a directed graph using
// adjacency list representation
class Graph
{
    int V;    // No. of vertices
 
    // Pointer to an array containing adjacency
    // lists
    list<int> *adj;   
public:
    Graph(int V);  // Constructor
 
    // function to add an edge to graph
    void addEdge(int v, int w); 
 
    // prints BFS traversal from a given source s
    void BFS(int s, vector<int> &Parents,vector<int> &Profondeurs); 
    void DFSUtil(int v, bool visited[], vector<int> &Parents, vector<int> &Profondeurs);
    void DFS(int v, vector<int> &Parents, vector<int> &Profondeurs);
};
 
Graph::Graph(int V)
{
    this->V = V;
    adj = new list<int>[V];
}
 
void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
}

void Graph::BFS(int s, vector<int> &Parents, vector<int> &Profondeurs)
{

    // Mark all the vertices as not visited
    bool *visited = new bool[V];
    for(int i = 0; i < V; i++){
	Parents.push_back(V*V);
	Profondeurs.push_back(1);
        visited[i] = false;
    }
    // Create a queue for BFS
    list<int> queue;
 
    // Mark the current node as visited and enqueue it
    visited[s] = true;
    queue.push_back(s);
    Parents[s] = s;
    // 'i' will be used to get all adjacent
    // vertices of a vertex
    list<int>::iterator i;
 
    while(!queue.empty())
    {
        // Dequeue a vertex from queue 
        s = queue.front();

        queue.pop_front();
 
        // Get all adjacent vertices of the dequeued
        // vertex s. If a adjacent has not been visited, 
        // then mark it visited and enqueue it
        for (i = adj[s].begin(); i != adj[s].end(); ++i)
        {
            if (!visited[*i])
            {   Parents[*i] = s;
                visited[*i] = true;
		Profondeurs[*i] = Profondeurs[s]+1;
                queue.push_back(*i);
            }
        }
    }
}

void Graph::DFSUtil(int v, bool visited[], vector<int> &Parents, vector<int> &Profondeurs)
{
    // Mark the current node as visited and
    // print it
    visited[v] = true;
    //cout << v << " ";
 
    // Recur for all the vertices adjacent
    // to this vertex
    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i]){
            Parents[*i] = v;
            Profondeurs[*i] = Profondeurs[v]+1;
            DFSUtil(*i, visited,Parents,Profondeurs);
	}
}
 
// DFS traversal of the vertices reachable from v.
// It uses recursive DFSUtil()
void Graph::DFS(int v, vector<int> &Parents, vector<int> &Profondeurs)
{
    // Mark all the vertices as not visited
    bool *visited = new bool[V];
    for (int i = 0; i < V; i++){
	Parents.push_back(V*V);
	Profondeurs.push_back(1);
        visited[i] = false;
    }
    Parents[v] = v;
    // Call the recursive helper function
    // to print DFS traversal
    DFSUtil(v, visited, Parents, Profondeurs);
}

#endif
