#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define TASK_1 0
#define DECRYPTION_KEY 811589153

typedef struct node
{
    int64_t val;
    node *prev;
    node *next;
    
    node(int64_t val) : val(val)
    {}
    
} node_t;

typedef struct double_linked_list
{
    node_t *head;
    int size;
    
    void insert_back(node_t *node)
    {
        if (size == 0)
        {
            head = node;
            node->prev = node;
            node->next = node;
        }
        else
        {
            node->next = head;
            node->prev = head->prev;
            node->next->prev = node;
            node->prev->next = node;
        }
        size++;
    }
    
    void move_node(node_t *node, int64_t shifts)
    {
        if (size <= 0)
        {
            cout << "Tried to move node, when there are no nodes" << endl;
            exit(32); // Bad should not happen
        }
        shifts %= size - 1;
        while (shifts < 0)
            shifts += size - 1;
        if (shifts == 0)
            return;
        if (node == head)
            head = head->next;
        
        // Extract the current node
        node->prev->next = node->next;
        node->next->prev = node->prev;
        
        node_t *next_node = node->next;
        for (int i = 0; i < shifts; ++i)
            next_node = next_node->next;
        
        node->next = next_node;
        node->prev = next_node->prev;
        node->prev->next = node;
        node->next->prev = node;
    }
    
    int find_index_of_val(int val)
    {
        node_t *current = head;
        for (int i = 0; i < size; ++i)
        {
            if (current->val == val)
                return i;
            current = current->next;
        }
        cout << "No index for val found: " << val << endl;
        exit(12);
    }
    
    node_t *at(int index)
    {
        if (index > size - 1)
        {
            cout << "Index out of bound: " << index << endl;
            exit(123);
        }
        
        index %= size;
        
        node_t *itr = head;
        for (int i = 0; i < index; ++i)
            itr = itr->next;
        
        return itr;
    }
    
} double_linked_list_t;

int64_t find_coordinates(vector<node_t *> &order, double_linked_list_t &list)
{
    for (node_t *node : order)
        list.move_node(node, node->val);
    
    int offset = list.find_index_of_val(0);
    int64_t sum = 0;
    for (int i = 1; i < 4; ++i)
        sum += list.at((offset + (i * 1000)) % list.size)->val;
    
    return sum;
}

int64_t find_coordinates_with_key(vector<node_t *> &order, double_linked_list_t &list)
{
    for (node_t *node : order)
        node->val = node->val * DECRYPTION_KEY;
    
    for (int i = 0; i < 10; ++i)
    {
        cout << "starting " << i + 1 << endl;
        for (node_t *node: order)
            list.move_node(node, node->val);
        cout << "ending " << i + 1 << endl;
    }
    
    int offset = list.find_index_of_val(0);
    int64_t sum = 0;
    for (int i = 1; i < 4; ++i)
        sum += list.at((offset + (i * 1000)) % list.size)->val;
    
    return sum;
}

int main()
{
    auto start_time = chrono::high_resolution_clock::now();
    //Reading data from file
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    string temp {};
    
    vector<node_t *> order;
    double_linked_list_t list;
    while (!file.eof())
    {
        getline(file, temp);
        node_t *new_node = new node_t(stoi(temp));
        order.emplace_back(new_node);
        list.insert_back(new_node);
    }
    
    file.close();
    
    
    if (TASK_1)
    {
        int64_t result = find_coordinates(order, list);
        cout << "Result: " << result << endl;
    }
    else
    {
        int64_t result = find_coordinates_with_key(order, list);
        cout << "Result with key: " << result << endl;
    }
    
    // Cleanup the memory
    for (node_t *p : order)
        delete p;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    
    return EXIT_SUCCESS;
}
