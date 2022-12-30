#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

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
    int64_t register_X {1};
    uint64_t cycle {};
    int64_t sum {};
    
    vector<int64_t> histroy_X {};
    
    while (!file.eof())
    {
        getline(file, temp);
        ++cycle;
        histroy_X.emplace_back(register_X);
        if (cycle % 40 == 20)
            sum += register_X * (int64_t)cycle;
        if (temp != "noop")
        {
            ++cycle;
            histroy_X.emplace_back(register_X);
            if (cycle % 40 == 20)
                sum += register_X * (int64_t)cycle;
            register_X += stoi(temp.substr(temp.find(' ')));
        }
        
    }
    
    cout << sum << endl;
    
    //Task 2
    
    #define WIDTH 40
    #define HEIGHT 6
    
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            uint32_t index = y * WIDTH + x;
            int64_t value = histroy_X.at(index);
            if (abs(value - x) <= 1)
                cout << "#";
            else
                cout << ".";
        }
        cout << endl;
    }
    
    file.close();
    
    return EXIT_SUCCESS;
}
