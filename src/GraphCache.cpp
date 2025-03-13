#include "GraphCache.h"




Node::Node(int addr) : address(addr) {}
void Node::AddEdge(std::shared_ptr<Edge> edge)
{
    edges.push_back(edge);
}

Edge::Edge(int _weight, std::shared_ptr<Node> _from, std::shared_ptr<Node> _to) : weight(_weight), from(_from), to(_to) {}

Graph::Graph(GraphLimitingQueue& queue) : graphQueue(queue) {}
void Graph::AddNode(int address)
{
    // First, add the node to the graph
    if (nodes.find(address) == nodes.end()) {
        // Create a new node and insert it into the graph
        std::shared_ptr<Node> newNode = std::make_shared<Node>(address);
        nodes[address] = newNode;

        // Also add the node to the queue (LRU mechanism)
        graphQueue.Add(newNode.get());

        // If this is not the first node, add an edge to the previous node
        if (nodes.size() > 1) {
            // Get the last added node (the previous node)
            auto it = std::prev(nodes.end());
            std::shared_ptr<Node> prevNode = it->second;

            // Create a new edge (you may need to determine the weight and direction)
            std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(1, prevNode, newNode);

            // Add the edge to both nodes' adjacency list
            prevNode->AddEdge(newEdge);
            newNode->AddEdge(newEdge);
        }
    }
}


GraphLimitingQueue::GraphLimitingQueue(int size) : maxSize(size) {}

void Graph::HandleCorrectPrediction(int lastAddress, int thisAddress)
{
    auto it = nodes.find(lastAddress);
    if (it != nodes.end()) // Ensure lastAddress node exists
    {
        std::shared_ptr<Node> lastNode = it->second;

        for (std::shared_ptr<Edge> e : lastNode->edges)
        {
            if (e->to->address == thisAddress)
            {
                e->weight++; // Increase edge weight
                return; // Edge found and updated, exit function
            }
        }

        // If no edge is found, this is an error condition
        throw std::runtime_error("Prediction error: Expected edge does not exist!");
    }
    else
    {
        throw std::runtime_error("Prediction error: Last address node not found!");
    }
}

void Graph::HandleIncorrectPrediction(int lastAddress, int incorrectAddress)
{
    auto lastIt = nodes.find(lastAddress);
    auto incorrectIt = nodes.find(incorrectAddress);

    if (lastIt != nodes.end())
    {
        std::shared_ptr<Node> lastNode = lastIt->second;

        lastNode->edges.erase(
            std::remove_if(lastNode->edges.begin(), lastNode->edges.end(),
                [incorrectAddress](std::shared_ptr<Edge> e) {
                    return e->to->address == incorrectAddress;
                }),
            lastNode->edges.end()
        );

        if (lastNode->edges.empty())
            nodes.erase(lastIt);
    }

    if (incorrectIt != nodes.end())
    {
        std::shared_ptr<Node> incorrectNode = incorrectIt->second;

        incorrectNode->edges.erase(
            std::remove_if(incorrectNode->edges.begin(), incorrectNode->edges.end(),
                [lastAddress](std::shared_ptr<Edge> e) {
                    return e->to->address == lastAddress;
                }),
            incorrectNode->edges.end()
        );

        if (incorrectNode->edges.empty()) {
            nodes.erase(incorrectIt);
        }
    }
}

int Graph::PrefetchAddress(int currentAddress)
{
    Node* currentNode = graphQueue.GetNode(currentAddress);
    if (!currentNode || currentNode->edges.empty()) {
        return -1; // Indicate no prefetch candidate exists
    }

    int currentWeight = 0;
    int address = -1;
    
    for (const auto& edge : currentNode->edges)
    {
        if (edge->weight > currentWeight)
        {
            currentWeight = edge->weight;
            address = edge->to->address;
        }
    }

    return address;
}


void GraphLimitingQueue::Add(Node* node)
{
    if (nodeMap.find(node->address) != nodeMap.end())
    {
        Promote(node->address);
    }
    else
    {
        if (queue.size() >= maxSize)
        {
            Node* tailNode = queue.back();
            nodeMap.erase(tailNode->address);
            queue.pop_back();
        }

        queue.push_front(node);
        nodeMap[node->address] = queue.begin();
    }
}

void GraphLimitingQueue::Promote(int address)
{
    if (nodeMap.find(address) == nodeMap.end())
    {
        throw std::out_of_range("Node to promote not found in queue");
    }

    auto it = nodeMap[address];
    queue.splice(queue.begin(), queue, it);
}

void GraphLimitingQueue::Remove(int address)
{
    if (nodeMap.find(address) == nodeMap.end())
    {
        throw std::out_of_range("Node to remove not found in queue");
    }

    auto it = nodeMap[address];
    queue.erase(it);
    nodeMap.erase(address);
}

Node* GraphLimitingQueue::GetNode(int currentAddress)
{
    if (nodeMap.find(currentAddress) == nodeMap.end()) {
        throw std::out_of_range("Node to get not found");
    }

    auto it = nodeMap[currentAddress];
    return *it; // Correctly return the Node pointer
}

