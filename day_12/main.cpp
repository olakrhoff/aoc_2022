#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

typedef struct node
{
    char value {};
    bool visited {};
    uint32_t distance {};
    vector<node *> nodes;
    
    node(char value) : value(value)
    {}
    
    bool can_traval(const node &rhs)
    {
        return (rhs.get_height() - get_height()) <= 1;
    }
    
    int16_t get_height() const
    {
        if (value == 'S')
            return (int16_t)(0);
        if (value == 'E')
            return (int16_t)('z' - 'a');
        return (int16_t)(value - 'a');
    }
    
    bool operator<(const node &rhs) const
    {
        return distance < rhs.distance;
    }
    
    bool operator>(const node &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const node &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const node &rhs) const
    {
        return !(*this < rhs);
    }
    
} node_t;


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
    
    vector<string> map {};
    
    node_t *start, *end;
    vector<node_t *> nodes;
    
    
    while (!file.eof())
    {
        getline(file, temp);
        
        map.emplace_back(temp);
    }
    
    const int ROWS = map.size();
    const int COLS = map.at(0).length();
    
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            node_t *new_node = new node_t(map.at(y).at(x));
            nodes.emplace_back(new_node);
        }
    }
    
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            node_t *current = nodes.at(y * COLS + x);
         
            if (current->value == 'S')
                start = current;
            else if (current->value == 'E')
                end = current;
            
            if (y > 0 && current->can_traval(*nodes.at((y - 1) * COLS + x))) //ADD TOP
            {
                //Task 1
                //current->nodes.emplace_back(nodes.at((y - 1) * COLS + x));
                //Task 2
                nodes.at((y - 1) * COLS + x)->nodes.emplace_back(current);
            }
            if (y < ROWS - 1 && current->can_traval(*nodes.at((y + 1) * COLS + x))) //ADD DOWN
            {
                //Task 1
                //current->nodes.emplace_back(nodes.at((y + 1) * COLS + x));
                //Task 2
                nodes.at((y + 1) * COLS + x)->nodes.emplace_back(current);
            }
            if (x > 0 && current->can_traval(*nodes.at(y * COLS + x - 1))) //ADD LEFT
            {
                //Task 1
                //current->nodes.emplace_back(nodes.at(y * COLS + x - 1));
                //Task 2
                nodes.at(y * COLS + x - 1)->nodes.emplace_back(current);
            }
            if (x < COLS - 1 && current->can_traval(*nodes.at(y * COLS + x + 1))) //ADD RIGHT
            {
                //TasK 1
                //current->nodes.emplace_back(nodes.at(y * COLS + x + 1));
                //Task 2
                nodes.at(y * COLS + x + 1)->nodes.emplace_back(current);
            }
        }
    }
    
    
    vector<node_t*> heap;
    std::make_heap(heap.begin(), heap.end());
    
    //Task 1
    //heap.emplace_back(start);
    //Task 2
    heap.emplace_back(end);
    std::sort_heap(heap.begin(), heap.end());
    
    while (!heap.empty())
    {
        node_t *current = heap.front();
        
        std::pop_heap(heap.begin(), heap.end());
        heap.pop_back();
        
        current->visited = true;
        
        //Task 1
        //if (current->value == 'E')
        //Task 2
        if (current->get_height() == 0)
        {
            cout << current->distance << endl;
            break;
        }
        
        for (node_t *n : current->nodes)
        {
            if (!n->visited)
            {
                n->visited = true;
                n->distance = current->distance + 1;
                
                heap.push_back(n);
                std::push_heap(heap.begin(), heap.end());
            }
        }
        std::sort(heap.begin(), heap.end(), [](node_t* a, node_t *b){ return a->distance < b->distance; });
    }
    
    file.close();
    
    return EXIT_SUCCESS;
}
