#ifndef ADJLIST_H
#define ADJLIST_H

#include <vector>
#include <iostream>
using namespace std;

/** 
 * Class representing an adjacency list, with the possibility of marking a node as visited
 */
class AdjList {
private:
  /// Adjacency list
  vector<vector<int> > m_nodes;
  /// Visited vector
  vector<bool> m_visited;
  typedef vector<vector<int> >::iterator nodeiter;
  typedef vector<bool>::iterator visititer;

public: 

  /**
   * Constructor.
   * Initialize an adjacency list and initialize all nodes to unvisited with no neighbours
   * @param nodes Number of nodes in graph
   */
  AdjList(int nodes):m_nodes(nodes),  m_visited(nodes)
  {
    for(visititer i = m_visited.begin(); i != m_visited.end();++i)
    {
      *i = false;
    }
    for(nodeiter i = m_nodes.begin(); i != m_nodes.end(); ++i)
    {
      (*i) = vector<int>();
    }
  }

  /**
   * Mark two nodes as adjacent to each other (undirected)
   * @param n1 Node 1
   * @param n2 Node 2
   */
  inline void 
  adjacent(int n1, int n2)
  {
    m_nodes[n1].push_back(n2);
    m_nodes[n2].push_back(n1);
  }

  /**
   * Mark a node as visited
   * @param node Node to mark as visited 
   */
  inline void 
  visit(int node)
  {
    m_visited[node] = true;
  }
  
  /**
   * Check if node is visited
   * @param node node to check if visited
   * @return true if node is visited
   */
  inline 
  bool isVisited(int node) const
  {
    return m_visited[node];
  }
  
  /**
   * Find an adjacent node that has not been visited or found yet
   * @param node Node to find unvisited adjacent node for
   * @ret Unvisited neighbouring node if larger than 0, -1 if no such node exist
   */
  int findNext(int node) const
  {
    for(unsigned int i = 0; i < m_nodes[node].size(); i++)
    {
      if(!m_visited[m_nodes[node][i]])
      {
	return m_nodes[node][i];
      }
    }
    return -1;
  }
  
  /** 
   * Find all adjacent nodes that has not been visited yet
   * @param node to find all unvisited adjacent nodes for
   * @ret vector of adjacent unvisited nodes, empty vector if no such nodes exists.
   */
  vector<int> findAllNext(int node) const
  {
    vector<int> ret = vector<int>();

    for(unsigned int i = 0; i < m_nodes[node].size(); i++)
    {
      if(!m_visited[m_nodes[node][i]] )
      {
	ret.push_back(m_nodes[node][i]);
      }
    }
    return ret;
    
  }

  /**
   * Find the first node that has only 1 neighbour
   * @return A node with degree 1
   */
  int findFirst() const
  {
    for(unsigned int i = 0; i < m_nodes.size(); i++)
    {
      if(m_nodes[i].size() == 1) 
      {
	return i;
      }
    }
    return -1;
  }

  /**
   * Find all nodes with only one neighbour
   * @return Vector of all nodes in graph with degree 1
   */
  vector<int> findAllFirst() const
  {
    vector<int> ret;
    for(unsigned int i = 0; i < m_nodes.size(); i++)
    {
      if(m_nodes[i].size() == 1)
      {
	ret.push_back(i);
      }
    }
    return ret;
  }
	  
  /**
   * Print the adjacency list
   */
  void print()
  {
    for(unsigned int i = 0; i < m_nodes.size(); i++)
    {
      cerr << " Node " << i << ":\t";
      for(unsigned int j = 0; j < m_nodes[i].size(); j++)
      {
	cerr << m_nodes[i][j] << " "; 
      }
      cerr << endl;
    }
  }
};

#endif
