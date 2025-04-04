#ifndef GRAPHCACHE_H
#define GRAPHCACHE_H

#include <memory>
#include <unordered_map>
#include <list>
#include <vector>
#include <stdexcept>
#include <iostream>

typedef unsigned int Address;

class Edge;
class Node;

class GraphLimitingQueue {
public:
    GraphLimitingQueue(int size);
    void Add(std::shared_ptr<Node> node);
    void Promote(Address address);
    void Remove(Address address);
    unsigned int GetCurrentSize();
    std::shared_ptr<Node> GetNode(Address currentAddress);
    std::shared_ptr<Node> GetTail();
    std::shared_ptr<Node> GetHead();
    void PrintQueue();

    int maxSize;

private:
    std::list<std::weak_ptr<Node>> queue;
    std::unordered_map<Address, std::list<std::weak_ptr<Node>>::iterator> nodeMap;
};

class Node {
public:
    Node(Address addr);
    void AddEdge(std::shared_ptr<Edge> edge);
    void ClearEdges();
    
    Address address;
    std::vector<std::shared_ptr<Edge>> edges;
};

class Edge {
public:
    Edge(int weight, std::weak_ptr<Node> from, std::weak_ptr<Node> to);
    
    int weight;
    std::weak_ptr<Node> from;
    std::weak_ptr<Node> to;
};

class Graph {
public:
    Graph(GraphLimitingQueue* queue);
    ~Graph();
    bool IsValid();
    void AddNode(Address address);
    void RemoveNode(Address address);
    void HandleCorrectPrediction(Address lastAddress, Address correctAddress);
    void HandleIncorrectPrediction(Address lastAddress, Address incorrectAddress);
    unsigned int getRelationship(Address currentAddress, Address victimAddress);
    Address PrefetchAddress(Address currentAddress);

private:
    std::unordered_map<Address, std::shared_ptr<Node>> nodes;
    GraphLimitingQueue* graphQueue;
};

#endif // GRAPHCACHE_H