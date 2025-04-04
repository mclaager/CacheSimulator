#ifndef GRAPHCACHE_H
#define GRAPHCACHE_H

#include "types/CacheLine.h"

#include <memory>
#include <unordered_map>
#include <list>
#include <vector>
#include <stdexcept>
#include <iostream>

class Edge;
class Node;

class GraphLimitingQueue {
public:
    GraphLimitingQueue(int size);
    void Add(std::shared_ptr<Node> node);
    void Promote(Block block);
    void Remove(Block block);
    unsigned int GetCurrentSize();
    std::shared_ptr<Node> GetNode(Block currentBlock);
    std::shared_ptr<Node> GetTail();
    std::shared_ptr<Node> GetHead();
    void PrintQueue();

    int maxSize;

private:
    std::list<std::weak_ptr<Node>> queue;
    std::unordered_map<Block, std::list<std::weak_ptr<Node>>::iterator> nodeMap;
};

class Node {
public:
    Node(Block block);
    void AddEdge(std::shared_ptr<Edge> edge);
    void ClearEdges();
    
    Block block;
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
    void AddNode(Block block);
    void RemoveNode(Block block);
    void HandleCorrectPrediction(Block lastBlock, Block correctBlock);
    void HandleIncorrectPrediction(Block lastBlock, Block incorrectBlock);
    unsigned int getRelationship(Block currentBlock, Block victimBlock);
    Block PrefetchBlock(Block currentBlock);

private:
    std::unordered_map<Block, std::shared_ptr<Node>> nodes;
    GraphLimitingQueue* graphQueue;
};

#endif // GRAPHCACHE_H