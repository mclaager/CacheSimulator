#include "GraphCache.h"

Node::Node(Address addr) : address(addr) {}

void Node::AddEdge(std::shared_ptr<Edge> edge) {
    edges.push_back(edge);
}

void Node::ClearEdges() {
    edges.clear();
}

Edge::Edge(int _weight, std::weak_ptr<Node> _from, std::weak_ptr<Node> _to) 
    : weight(_weight), from(_from), to(_to) {}

Graph::Graph(GraphLimitingQueue* queue) : graphQueue(queue) {}

Graph::~Graph() {
    // Clear all nodes and edges
    for (auto& pair : nodes) {
        pair.second->ClearEdges();
    }
    nodes.clear();
}

void Graph::AddNode(Address address) {
    if (nodes.find(address) == nodes.end()) {
        // Handle queue full condition
        if (graphQueue->GetCurrentSize() >= graphQueue->maxSize) {
            std::shared_ptr<Node> tail = graphQueue->GetTail();
            if (tail) {
                RemoveNode(tail->address);
            }
        }

        // Create and add new node
        auto newNode = std::make_shared<Node>(address);
        nodes[address] = newNode;
        graphQueue->Add(newNode);

        // Add edge to previous node if not first node
        if (nodes.size() > 1) {
            std::shared_ptr<Node> lastNode = graphQueue->GetHead();
            auto newEdge = std::make_shared<Edge>(10, lastNode, newNode);
            lastNode->AddEdge(newEdge);
            newNode->AddEdge(newEdge);
        }
    } else {
        graphQueue->Promote(address);
    }
}

void Graph::RemoveNode(Address address) {
    auto it = nodes.find(address);
    if (it != nodes.end()) {
        // Remove all edges pointing to this node
        it->second->ClearEdges();
        
        // Remove from queue
        graphQueue->Remove(address);
        
        // Remove from graph
        nodes.erase(it);
    }
}

void Graph::HandleCorrectPrediction(Address lastAddress, Address correctAddress) {
    auto it = nodes.find(lastAddress);
    if (it != nodes.end()) {
        auto lastNode = it->second;

        for (auto& edge : lastNode->edges) {
            if (auto toNode = edge->to.lock()) {
                if (toNode->address == correctAddress) {
                    edge->weight += 3;
                    return;
                }
            }
        }
        throw std::runtime_error("Prediction error: Expected edge does not exist!");
    }
    throw std::runtime_error("Prediction error: Last address node not found!");
}

void Graph::HandleIncorrectPrediction(Address lastAddress, Address incorrectAddress) 
{
    auto it = nodes.find(lastAddress);
    auto incIt = nodes.find(incorrectAddress);

    if (it != nodes.end() && incIt != nodes.end()) 
    {
        auto lastNode = it->second;
        auto wrongNode = incIt->second;

        for (auto e = lastNode->edges.begin(); e != lastNode->edges.end();) {
            if (auto toNode = (*e)->to.lock()) {
                if (toNode->address == incorrectAddress) {
                    (*e)->weight--;
                    if ((*e)->weight <= 0) {
                        e = lastNode->edges.erase(e);
                        continue;
                    }
                }
            }
            ++e;
        }

        if (wrongNode->edges.empty()) {
            RemoveNode(incorrectAddress);
        }
    }
}

unsigned int Graph::getRelationship(Address currentAddress, Address victimAddress)
{
    auto currentNode = graphQueue->GetNode(currentAddress);
    auto victimNode = graphQueue->GetNode(victimAddress);


    for(auto e = currentNode->edges.begin(); e != currentNode->edges.end();)
    {
        if(auto subject = (*e)->to.lock())
        {
            if (subject->address == victimAddress)
                return (*e)->weight;
        }
    }

    return 0;

}

Address Graph::PrefetchAddress(Address currentAddress) {
    auto currentNode = graphQueue->GetNode(currentAddress);
    if (!currentNode || currentNode->edges.empty()) {
        return -1;
    }

    int maxWeight = 0;
    Address prefetchAddr = -1;
    
    for (const auto& edge : currentNode->edges) {
        if (auto toNode = edge->to.lock()) {
            if (edge->weight > maxWeight) {
                maxWeight = edge->weight;
                prefetchAddr = toNode->address;
            }
        }
    }

    return prefetchAddr;
}

GraphLimitingQueue::GraphLimitingQueue(int size) : maxSize(size) {}

void GraphLimitingQueue::Add(std::shared_ptr<Node> node) {
    if (nodeMap.find(node->address) != nodeMap.end()) {
        Promote(node->address);
    } else {
        queue.push_front(node);
        nodeMap[node->address] = queue.begin();
    }
}

void GraphLimitingQueue::Promote(Address address) {
    auto it = nodeMap.find(address);
    if (it != nodeMap.end()) {
        queue.splice(queue.begin(), queue, it->second);
    }
}

void GraphLimitingQueue::Remove(Address address) {
    auto it = nodeMap.find(address);
    if (it != nodeMap.end()) {
        queue.erase(it->second);
        nodeMap.erase(it);
    }
}

unsigned int GraphLimitingQueue::GetCurrentSize() {
    return queue.size();
}

std::shared_ptr<Node> GraphLimitingQueue::GetNode(Address currentAddress) {
    auto it = nodeMap.find(currentAddress);
    if (it != nodeMap.end()) {
        return it->second->lock();
    }
    return nullptr;
}

std::shared_ptr<Node> GraphLimitingQueue::GetTail() {
    if (!queue.empty()) {
        return queue.back().lock();
    }
    return nullptr;
}

std::shared_ptr<Node> GraphLimitingQueue::GetHead() {
    if (!queue.empty()) {
        return queue.front().lock();
    }
    return nullptr;
}

void GraphLimitingQueue::PrintQueue() {
    std::cout << "Queue: " << std::endl;
    for (auto& weakNode : queue) {
        if (auto node = weakNode.lock()) {
            std::cout << std::hex << node->address << std::dec << std::endl;
        }
    }
}