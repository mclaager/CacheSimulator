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
Address address;
    std::vector<std::shared_ptr<Edge>> edges; // Adjacency list

    Node(Address addr);
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
    std::map<Address, std::shared_ptr<Node>> nodes; // Node storage

public:
    GraphLimitingQueue* graphQueue;  // Reference to the queue

    Graph(GraphLimitingQueue* queue);  // Constructor declaration

    void AddNode(Address address);
    void RemoveNode(Address address);

    void HandleCorrectPrediction(Address lastAddress, Address thisAddress);
    void HandleIncorrectPrediction(Address lastAddress, Address incorrectAddress);
    Address PrefetchAddress(Address currentAddress);
};

class GraphLimitingQueue
{
private:
    std::list<Node*> queue;
    std::unordered_map<Address, std::list<Node*>::iterator> nodeMap;

public:
    int maxSize;
    GraphLimitingQueue(int size);
    
    unsigned int GetCurrentSize();
    void Add(Node* node);
    void Promote(Address address);
    void Remove(Address address);
    Node* GetNode(Address currentAddress);
    Node* GetTail();
};

#endif // GRAPHCACHE_H
