#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 1

#define NUM_ROCKS 2022

/**
 * Sequence of bits representing either left (0) or right (1)
 * stream.
 */
typedef struct stream
{
    vector<uint64_t> buffer {0};
    uint8_t index {};
    uint64_t byte_index {};
    uint8_t bit_index {};
    
    void push(int direction)
    {
        buffer.back() = (buffer.back() << 1) + direction;
        index++;
        if (index >= 64)
        {
            index = 0;
            buffer.emplace_back(0);
        }
    }
    
    bool next_stream() const
    {
    
    }
} stream_t;

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
   
    stream_t stream;
    while (!file.eof())
    {
        getline(file, temp);
        for (const char c : temp)
        {
            if (c == '<')
                stream.push(0);
            else
                stream.push(1);
        }
    }
    
    file.close();
    
    
    
    return EXIT_SUCCESS;
}
