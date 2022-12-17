#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

uint8_t get_int_from_char(char c)
{
    if (c >= *"A" && c <= *"Z")
        return c - *"A" + 26;
    return c - *"a";
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
    
    uint64_t total_score {};
    
    while (!file.eof())
    {
        //Task 1
        /*
        getline(file, temp);
        
        uint8_t num_items = temp.size();
        bool occur[26*2] {false};
        string first = temp.substr(0, num_items / 2), second = temp.substr(num_items / 2, num_items / 2);
        
        for (char c : first)
            occur[get_int_from_char(c)] = true;
        char found;
        for (char c : second)
            if (occur[get_int_from_char(c)])
            {
                found = c;
                break;
            }
        total_score += get_int_from_char(found) + 1;
        */
        
        //Task 2
        uint8_t occurences[26*2] {0};
        for (int i = 0; i < 3; ++i)
        {
            bool found[26*2] {false};
            getline(file, temp);
            
            for (char c : temp)
                found[get_int_from_char(c)] = true;
            
            for (int j = 0; j < 26*2; ++j)
                if (found[j])
                    occurences[j]++;
        }
        
        //for (uint8_t c : occurences)
        for (int i = 0; i < 26*2; ++i)
            if (occurences[i] == (uint8_t)3)
            {
                total_score += i + 1;
                break;
            }
    }
    file.close();
    
    
    cout << total_score << endl;
    
    return EXIT_SUCCESS;
}
