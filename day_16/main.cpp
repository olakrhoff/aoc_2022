#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0

typedef struct node
{
    string label {};
    uint64_t flow_rate {};
    vector<node *> nodes {};
    
    node(string label, uint64_t flowRate) : label(std::move(label)), flow_rate(flowRate)
    {}
    
} node_t;

typedef struct graph
{
    node_t *root {};
    vector<node_t *> nodes {};
    static uint64_t total_flow_capacity;
    
    void add_new_node(const string &new_node, uint64_t flow_rate)
    {
        node_t *current_node {};
        for (node_t *n: nodes)
        {
            if (n->label == new_node)
            {
                current_node = n;
                break;
            }
        }
        if (current_node == nullptr)
        {
            current_node = new node_t(new_node, flow_rate);
            nodes.emplace_back(current_node);
            total_flow_capacity += current_node->flow_rate;
        }
        else
            current_node->flow_rate = flow_rate;
        
        if (root == nullptr)
            root = current_node;
    }
    
    void add_new_tunnel(const string &from_node, const string &to_node)
    {
        node_t *from {};
        for (node_t *n: nodes)
        {
            if (n->label == from_node)
            {
                from = n;
                break;
            }
        }
        
        if (from == nullptr)
            exit(2);
        
        bool added {false};
        for (node_t *n: nodes)
        {
            if (n->label == to_node)
            {
                added = true;
                from->nodes.emplace_back(n);
                break;
            }
        }
        
        if (!added)
        {
            add_new_node(to_node, 0);
            from->nodes.emplace_back(nodes.back());
        }
    }
    
    struct path
    {
        node_t *current_node;
        vector<node_t *> open_valves {};
        uint8_t time_left {};
        uint64_t flow {};
        
        path(node_t *currentNode, const vector<node_t *> &openValves, uint8_t timeLeft, uint64_t flow) : current_node(
                currentNode),
                                                                                                         open_valves(
                                                                                                                 openValves),
                                                                                                         time_left(
                                                                                                                 timeLeft),
                                                                                                         flow(flow)
        {}
        
        path(const path &other) : current_node(other.current_node),
                                  open_valves(other.open_valves),
                                  time_left(other.time_left),
                                  flow(other.flow)
        {}
        
        bool operator<(const path &rhs) const
        {
            return (flow - rhs.flow) < ((rhs.time_left - time_left) * total_flow_capacity);
        }
        
        bool operator>(const path &rhs) const
        {
            return rhs < *this;
        }
        
        bool operator<=(const path &rhs) const
        {
            return !(rhs < *this);
        }
        
        bool operator>=(const path &rhs) const
        {
            return !(*this < rhs);
        }
    };
    
    uint64_t find_best_route()
    {
        uint64_t total_flow_capacity {};
        for (node_t *n: nodes)
            total_flow_capacity += n->flow_rate;
        
        
        vector<path> heap {};
        
        heap.push_back(path(root, {}, 30, 0));
        std::push_heap(heap.begin(), heap.end());
        
        while (!heap.empty())
        {
            path current = heap.front();
            std::pop_heap(heap.begin(), heap.end());
            heap.pop_back();
            
            //Continue to implement A star algorithm to find the best path
        }
        
    }
    
} graph_t;

vector<string> split(const string &input, const string &delimiter)
{
    vector<string> output {};
    string temp = input;
    
    while (temp.find(delimiter) != -1)
    {
        output.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    output.emplace_back(temp);
    
    return output;
}

int main()
{
    //Reading data from file
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    string temp {};
    
    graph_t cave {};
    
    while (!file.eof())
    {
        getline(file, temp);
        
        string node_name = temp.substr(6, 2);
        uint64_t flow_rate = stoi(temp.substr(23, temp.find(';')));
        
        int extra {0};
        if (temp.find("valves") != -1)
            extra = 1;
        
        string to_nodes = temp.substr(temp.find("to valve") + 9 + extra);
        vector<string> nodes = split(to_nodes, ", ");
        
        cave.add_new_node(node_name, flow_rate);
        
        for (string name: nodes)
            cave.add_new_tunnel(node_name, name);
    }
    
    file.close();
    
    
    uint64_t score = cave.find_best_route();
    
    cout << score << endl;
    
    return EXIT_SUCCESS;
}
