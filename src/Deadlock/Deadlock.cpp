/**
    Deadlock.cpp
    Purpose: To create wait for graph and check deadlock
    @author Ishita Jaisia (ij2056)
    @version 1.1 24/11/21
*/

#include "Deadlock.h"

using namespace std;

Deadlock::Deadlock()
{
    this->vertices = number_of_transactions;
    adj = new list<int>[vertices];
}

/**
 * Add edge to the graph
 *
 * @param source vertex, set of transactions
 * @return void
 * @sideEffects - adjacency list of the graph
 */
void Deadlock::addEdge(int source, set<int> transactions)
{
    set<int>::iterator itr;
    transactions.erase(source);
    for (itr = transactions.begin(); itr != transactions.end(); itr++)
    {
        adj[source].push_back(*itr);
    }
}

/**
 * Remove all edges from and to the source
 *
 * @param source vertex
 * @return void
 * @sideEffects - adjacency list of the graph
 */
void Deadlock::removeAllEdges(int source)
{
    adj[source].clear();
    list<int>::iterator it;
    for (int i = 0; i < vertices; i++)
    {
        adj[i].remove(source);
    }
}

/**
 * Utility function to check for cycle in the graph
 *
 * @param vertices, visited array, recursion stack
 * @return bool
 * @sideEffects - None
 */
bool Deadlock::checkForCycleUtil(int vertices, bool visited[], bool *recStack)
{
    if (visited[vertices] == false)
    {
        visited[vertices] = true;
        recStack[vertices] = true;
        list<int>::iterator it;
        for (it = adj[vertices].begin(); it != adj[vertices].end(); it++)
        {
            if (!visited[*it] && checkForCycleUtil(*it, visited, recStack))
            {
                deadlock_transactions.insert(*it);
                return true;
            }
            else if (recStack[*it])
            {
                deadlock_transactions.insert(*it);
                return true;
            }
        }
    }
    recStack[vertices] = false;
    return false;
}

/**
 * Check for deadlock in the graph
 *
 * @param void
 * @return set of transactions taking part in the cycle
 * @sideEffects - None
 */
set<int> Deadlock::checkForDeadlock()
{
    deadlock_transactions.clear();
    bool *visited = new bool[vertices];
    bool *recStack = new bool[vertices];
    for (int i = 0; i < vertices; i++)
    {
        visited[i] = false;
        recStack[i] = false;
    }
    for (int i = 0; i < vertices; i++)
        if (checkForCycleUtil(i, visited, recStack))
            return deadlock_transactions;

    return deadlock_transactions;
}
