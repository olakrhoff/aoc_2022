#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

using namespace std;

#define DEBUG 0

vector<string> split(string val, char criteria)
{
    vector<string> values {};
    string temp {};
    for (char c : val)
    {
        if (c == criteria)
        {
            if (temp.length() != 0)
                values.emplace_back(temp);
            temp = "";
        }
        else
        {
            temp += c;
        }
    }
    
    if (temp.length() != 0)
        values.emplace_back(temp);
    
    return values;
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
    
    vector<string> start_lines {};
    
    while (!file.eof())
    {
        getline(file, temp);
        
        if (temp.empty())
            break;
        start_lines.emplace_back(temp);
    }
    
    uint8_t num_stacks = split(start_lines.back(), ' ').size();
    
    vector<stack<char>> stacks(num_stacks);
    
    for (int i = start_lines.size() - 2; i >= 0; --i)
    {
        for (int j = 0; j < num_stacks; ++j)
        {
            if (start_lines.at(i).length() <= 1 + j * 4)
                continue;
            char value = start_lines.at(i).at(1 + j * 4);
            if (value != ' ')
                stacks.at(j).push(value);
        }
    }
    
    stack<char> temp_stack {};
    
    while (!file.eof())
    {
        getline(file, temp);
        vector<string> instruction = split(temp, ' ');
        uint8_t number = stoi(instruction[1]);
        uint8_t from = stoi(instruction[3]);
        uint8_t to = stoi(instruction[5]);
        
        //Task 1
        /*
        for (int i = 0; i < number; ++i)
        {
            stacks[to - 1].push(stacks[from - 1].top());
            stacks[from - 1].pop();
        }
        */
        
        //Task 2
        for (int i = 0; i < number; ++i)
        {
            temp_stack.push(stacks[from - 1].top());
            stacks[from - 1].pop();
        }
        
        while (!temp_stack.empty())
        {
            stacks[to - 1].push(temp_stack.top());
            temp_stack.pop();
        }
        
    }
    
    file.close();
    
    
    for (auto s : stacks)
        cout << s.top();
    cout << endl;
    
    
    return EXIT_SUCCESS;
}
