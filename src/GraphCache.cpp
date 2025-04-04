#include "GraphCache.h"




Node::Node(Address addr) : address(addr) {}
void Node::AddEdge(std::shared_ptr<Edge> edge)
{
   // std::cout<<"adding edge"<<std::endl;
    edges.push_back(edge);
}

Edge::Edge(int _weight, std::shared_ptr<Node> _from, std::shared_ptr<Node> _to) : weight(_weight), from(_from), to(_to) {}

Graph::Graph(GraphLimitingQueue* queue) : graphQueue(queue) {}
void Graph::AddNode(Address address)
{
    // First, add the node to the graph
    if (nodes.find(address) == nodes.end()) {
        // if queue full remove tail address from queue, and its refrence in graph and map
        if (graphQueue->GetCurrentSize() >= graphQueue->maxSize)
        {
           // std::cout<<"Limit reached"<<std::endl;
            Node* tail = graphQueue->GetTail();
            if (tail != nullptr)
            {
                graphQueue->Remove(tail->address);
                nodes.erase(tail->address);
            }
        }

       // std::cout << "Adding " << std::hex <<  address << std::dec << std::endl;
        // Create a new node and insert it into the graph
        // std::cout<<"before new node made"<<std::endl;
        // graphQueue->PrintQueue();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(address);
        // std::cout<<"after newNode made"<<std::endl;
        // graphQueue->PrintQueue();
        nodes[address] = newNode;
       // graphQueue->PrintQueue();
        // Also add the node to the queue (LRU mechanism)
        graphQueue->Add(newNode.get());

        // If this is not the first node, add an edge to the previous node
        if (nodes.size() > 1) {
            //std::cout<<"Need to make new edge"<<std::endl;
            // Get the last added node (the previous node)
            std::shared_ptr<Node> lastNode = graphQueue->GetHead();

            // Create a new edge (you may need to determine the weight and direction)
            std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(20, lastNode, newNode);

            // Add the edge to both nodes' adjacency list
            lastNode->AddEdge(newEdge);
            newNode->AddEdge(newEdge);
        }


    }
    else
    {

        // std::cout << "Promoting " << std::hex <<  address << std::dec << std::endl;
        graphQueue->Promote(address);

    }
}

void Graph::RemoveNode(Address address)
{
    Graph::nodes.erase(address);
}


GraphLimitingQueue::GraphLimitingQueue(int size) : maxSize(size) {}

void Graph::HandleCorrectPrediction(Address lastAddress, Address thisAddress)
{
    auto it = nodes.find(lastAddress);
    if (it != nodes.end()) // Ensure lastAddress node exists
    {
        std::shared_ptr<Node> lastNode = it->second;

        for (std::shared_ptr<Edge> e : lastNode->edges)
        {
            if (e->to->address == thisAddress)
            {
                e->weight+=3; // Increase edge weight
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

void Graph::HandleIncorrectPrediction(Address lastAddress, Address incorrectAddress)
{
   // std::cout << "Handling Incorrect Prediction" << std::endl;

    // Find the nodes in the graph
    auto it = nodes.find(lastAddress);
    auto IncIt = nodes.find(incorrectAddress);

    if (it != nodes.end() && IncIt != nodes.end()) // Ensure nodes exist
    {
        std::shared_ptr<Node> lastNode = it->second;
        std::shared_ptr<Node> wrongNode = IncIt->second;
        
        // Iterate through the edges of the lastNode
        for (auto e = lastNode->edges.begin(); e != lastNode->edges.end();)
        {
            // Check if this edge connects to the wrongNode
            if ((*e)->to->address == incorrectAddress)
            {
                // Decrement the weight of the edge
                (*e)->weight--;

                // If weight is 0 or negative, erase this edge
                if ((*e)->weight <= 0)
                {
                   // std::cout << "erasing edge" << std::endl;
                    // Erase the edge from lastNode's edges
                    e = lastNode->edges.erase(e); // This safely updates the iterator
                    break;  // Skip incrementing the iterator since erase returns the next valid iterator
                }
            }
            else
            {
                ++e;  // Move to the next edge if no deletion occurred
            }
        }

        // If wrongNode has no edges left, remove it from the graph
        if (wrongNode->edges.empty())
        {
            std::cout<<"removing node"<<std::endl;
            nodes.erase(incorrectAddress);
        }
    }
}



Address Graph::PrefetchAddress(Address currentAddress)
{
    //std::cout<<"prefetching for: " << std::hex<<currentAddress<<std::endl;
    Node* currentNode = graphQueue->GetNode(currentAddress);
    if (!currentNode || currentNode->edges.empty()) {
        return -1; // Indicate no prefetch candidate exists
    }
    //std::cout<<"edges to check: "<< currentNode->edges.size()<<std::endl;
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

   // std::cout<< "returning address: " << std::hex<<address <<" weight: " << currentWeight<<std::endl;
    return address;
}


void GraphLimitingQueue::Add(Node* node)
{
    if (nodeMap.find(node->address) != nodeMap.end())
    {
        if(node->address == 0x40e180a0)
            std::cout<<"Trying to promote that address"<<std::endl;
        Promote(node->address);
    }
    else
    {

         if(node->address == 0x40e180a0)
         {
            std::cout<<"before: "<< std::endl;
            for (auto kvp : queue)
            {
                if(kvp->address == 0x40e180a0)
                {
                    if(nodeMap.find(0x40e180a0) != nodeMap.end())
                        std::cout<<"foudn in map too!"<<std::endl;
                   // std::cout <<"found: " << std::hex << kvp->address << std::dec << std::endl;
                    //throw;
                }
            }
         }

        queue.push_front(node);
        nodeMap[node->address] = queue.begin();


        if(node->address == 0x40e180a0)
        {
            std::cout<<"after: "<< std::endl;
            for (auto kvp : queue)
            {
                if(kvp->address == 0x40e180a0)
                {
                   // std::cout <<"found: " << std::hex << kvp->address << std::dec << std::endl;
                }
            }

            //throw;
        }

    }
}

void GraphLimitingQueue::Promote(Address address)
{
    //std::cout<< "ToPromote: "<<std::endl;
    //std::cout << std::hex<<address<<std::endl;
    // if(address == 0x40e180a0)
    // {
        for (auto kvp : queue)
        {
            if(kvp->address == 0x40e180a0)
            {
                //std::cout <<"found: " << std::hex << kvp->address << std::dec << std::endl;
            }
        }
    // }
    if (nodeMap.find(address) == nodeMap.end())
    {
        throw std::out_of_range("Node to promote not found in queue");
    }

    auto it = nodeMap[address];
    queue.splice(queue.begin(), queue, it);
}

void GraphLimitingQueue::Remove(Address address)
{
    int count = 0;
    
    if (nodeMap.find(address) == nodeMap.end())
    {
        throw std::out_of_range("Node to remove not found in queue");
    }

    auto it = nodeMap[address];
    queue.erase(it);
    nodeMap.erase(address);
}

unsigned int GraphLimitingQueue::GetCurrentSize()
{
    return queue.size();
}

Node* GraphLimitingQueue::GetNode(Address currentAddress)
{
    if (nodeMap.find(currentAddress) == nodeMap.end()) {
        throw std::out_of_range("Node to get not found");
    }

    auto it = nodeMap[currentAddress];

    return *it; // Correctly return the Node pointer
}

Node* GraphLimitingQueue::GetTail()
{
    return queue.back();
}

std::shared_ptr<Node> GraphLimitingQueue::GetHead()
{
    return std::shared_ptr<Node>(queue.front());

}

void GraphLimitingQueue::PrintQueue()
{
    std::cout<<"Queue: "<<std::endl;
    for (auto kvp : queue)
    {
        if(kvp->address == 0x40007a48)
        {
            //std::cout <<"found in print queue: " << std::hex << kvp->address << std::dec << std::endl;
            //if(nodeMap.find(0x40007a48) == nodeMap.end())
                //std::cout<<"not found in nodeMap"<<std::endl;
            //else
            //std::cout<<" found in nodeMap"<<std::endl;
        }
    }
}

