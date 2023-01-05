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
    vector<node*> nodes {};
    
    node(string label, uint64_t flowRate) : label(std::move(label)), flow_rate(flowRate)
    {}
    
} node_t;

typedef struct graph
{
    node_t *root {};
    vector<node_t *> nodes {};
    
    void add_new_node(const string &new_node, uint64_t flow_rate)
    {
        node_t *current_node {};
        for (node_t *n : nodes)
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
        }
        else
            current_node->flow_rate = flow_rate;
        
        if (root == nullptr)
            root = current_node;
    }
    
    void add_new_tunnel(const string &from_node, const string &to_node)
    {
        node_t *from {};
        for (node_t *n : nodes)
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
        for (node_t *n : nodes)
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
        
        for (string name : nodes)
            cave.add_new_tunnel(node_name, name);
    }
    
    file.close();
    
    return EXIT_SUCCESS;
}
