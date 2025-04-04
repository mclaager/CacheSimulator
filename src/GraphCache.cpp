#include "GraphCache.h"

Node::Node(Block addr) : block(addr) {}

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

bool Graph::IsValid()
{
    return Graph::graphQueue->maxSize > 0;
}

void Graph::AddNode(Block Block) {
    if (nodes.find(Block) == nodes.end()) {
        // Handle queue full condition
        if (graphQueue->GetCurrentSize() >= graphQueue->maxSize) {
            std::shared_ptr<Node> tail = graphQueue->GetTail();
            if (tail) {
                RemoveNode(tail->block);
            }
        }

        // Create and add new node
        auto newNode = std::make_shared<Node>(Block);
        nodes[Block] = newNode;
        graphQueue->Add(newNode);

        // Add edge to previous node if not first node
        if (nodes.size() > 1) {
            std::shared_ptr<Node> lastNode = graphQueue->GetHead();
            auto newEdge = std::make_shared<Edge>(10, lastNode, newNode);
            lastNode->AddEdge(newEdge);
            newNode->AddEdge(newEdge);
        }
    } else {
        graphQueue->Promote(Block);
    }
}

void Graph::RemoveNode(Block Block) {
    auto it = nodes.find(Block);
    if (it != nodes.end()) {
        // Remove all edges pointing to this node
        it->second->ClearEdges();
        
        // Remove from queue
        graphQueue->Remove(Block);
        
        // Remove from graph
        nodes.erase(it);
    }
}

void Graph::HandleCorrectPrediction(Block lastBlock, Block correctBlock) {
    auto it = nodes.find(lastBlock);
    if (it != nodes.end()) {
        auto lastNode = it->second;

        for (auto& edge : lastNode->edges) {
            if (auto toNode = edge->to.lock()) {
                if (toNode->block == correctBlock) {
                    edge->weight += 3;
                    return;
                }
            }
        }
        throw std::runtime_error("Prediction error: Expected edge does not exist!");
    }
    throw std::runtime_error("Prediction error: Last Block node not found!");
}

void Graph::HandleIncorrectPrediction(Block lastBlock, Block incorrectBlock) 
{
    auto it = nodes.find(lastBlock);
    auto incIt = nodes.find(incorrectBlock);

    if (it != nodes.end() && incIt != nodes.end()) 
    {
        auto lastNode = it->second;
        auto wrongNode = incIt->second;

        for (auto e = lastNode->edges.begin(); e != lastNode->edges.end();) {
            if (auto toNode = (*e)->to.lock()) {
                if (toNode->block == incorrectBlock) {
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
            RemoveNode(incorrectBlock);
        }
    }
}

unsigned int Graph::getRelationship(Block currentBlock, Block victimBlock)
{
    auto currentNode = graphQueue->GetNode(currentBlock);
    auto victimNode = graphQueue->GetNode(victimBlock);


    for(auto e = currentNode->edges.begin(); e != currentNode->edges.end();)
    {
        if(auto subject = (*e)->to.lock())
        {
            if (subject->block == victimBlock)
                return (*e)->weight;
        }
    }

    return 0;

}

Block Graph::PrefetchBlock(Block currentBlock) {
    auto currentNode = graphQueue->GetNode(currentBlock);
    if (!currentNode || currentNode->edges.empty()) {
        return -1;
    }

    int maxWeight = 0;
    Block prefetchAddr = -1;
    
    for (const auto& edge : currentNode->edges) {
        if (auto toNode = edge->to.lock()) {
            if (edge->weight > maxWeight) {
                maxWeight = edge->weight;
                prefetchAddr = toNode->block;
            }
        }
    }

    return prefetchAddr;
}

GraphLimitingQueue::GraphLimitingQueue(int size) : maxSize(size) {}

void GraphLimitingQueue::Add(std::shared_ptr<Node> node) {
    if (nodeMap.find(node->block) != nodeMap.end()) {
        Promote(node->block);
    } else {
        queue.push_front(node);
        nodeMap[node->block] = queue.begin();
    }
}

void GraphLimitingQueue::Promote(Block Block) {
    auto it = nodeMap.find(Block);
    if (it != nodeMap.end()) {
        queue.splice(queue.begin(), queue, it->second);
    }
}

void GraphLimitingQueue::Remove(Block Block) {
    auto it = nodeMap.find(Block);
    if (it != nodeMap.end()) {
        queue.erase(it->second);
        nodeMap.erase(it);
    }
}

unsigned int GraphLimitingQueue::GetCurrentSize() {
    return queue.size();
}

std::shared_ptr<Node> GraphLimitingQueue::GetNode(Block currentBlock) {
    auto it = nodeMap.find(currentBlock);
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
            std::cout << std::hex << node->block << std::dec << std::endl;
        }
    }
}