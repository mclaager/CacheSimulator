#ifndef GRAPHCACHE_H
#define GRAPHCACHE_H

#include "ICache.h"
#include "types/CacheLine.h"

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <list>
#include <iostream>
#include <algorithm>


class Edge;
class Node;
class GraphLimitingQueue;

class Node
{
public:
    int address;
    std::vector<std::shared_ptr<Edge>> edges; // Adjacency list

    Node(int addr);
    void AddEdge(std::shared_ptr<Edge> edge);
};

class Edge
{
public:
    int weight;
    std::shared_ptr<Node> from;
    std::shared_ptr<Node> to;

    Edge(int _weight, std::shared_ptr<Node> _from, std::shared_ptr<Node> _to);
};

class Graph
{
    std::map<int, std::shared_ptr<Node>> nodes; // Node storage

public:
    GraphLimitingQueue& graphQueue;  // Reference to the queue

    Graph(GraphLimitingQueue& queue);  // Constructor declaration

    void AddNode(int address);
    void RemoveNode(int address);

    void HandleCorrectPrediction(int lastAddress, int thisAddress);
    void HandleIncorrectPrediction(int lastAddress, int incorrectAddress);
    int PrefetchAddress(int currentAddress);
};

class GraphLimitingQueue
{
private:
    int maxSize;
    std::list<Node*> queue;
    std::unordered_map<int, std::list<Node*>::iterator> nodeMap;

public:
    GraphLimitingQueue(int size);
    
    void Add(Node* node);
    void Promote(int address);
    void Remove(int address);
    Node* GetNode(int currentAddress);
};

#endif // GRAPHCACHE_H
