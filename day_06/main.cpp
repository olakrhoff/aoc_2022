#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

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
    
    while (!file.eof())
    {
        getline(file, temp);
    }
    
    file.close();
    
    char occurences[256] {};
    
    //Task 1
    //#define SIZE 4
    //Task 2
    #define SIZE 14
    
    for (int i = 0; i < SIZE; ++i)
        occurences[temp[i]]++;
    
    int sum {};
    for (int i = 0; i < SIZE; ++i)
    {
        if (occurences[temp[i]] == 1)
            ++sum;
    }
    if (sum == SIZE)
    {
        cout << "Index: " << SIZE << endl;
    }
    
    for (int i = SIZE; i < temp.length(); ++i)
    {
        occurences[temp[i - SIZE]]--;
        occurences[temp[i]]++;
        int sum {};
        for (int j = 0; j < SIZE; ++j)
        {
            if (occurences[temp[i - j]] == 1)
                ++sum;
        }
        if (sum == SIZE)
        {
            cout << "Index: " << i + 1 << endl;
            break;
        }
    }
    
    
    return EXIT_SUCCESS;
}
