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
    
    
    struct top_three
    {
        uint64_t a {}, b {}, c {};
    
        bool operator<(const uint64_t &rhs) const
        {
            if (a < rhs)
                return true;
            if (b < rhs)
                return true;
            return c < rhs;
        }
        
        bool operator>(const uint64_t &rhs) const
        {
            return *this < rhs;
        }
        
        top_three& operator=(const uint64_t &val)
        {
            if (a < b)
            {
                if (a < c)
                    a = val;
                else
                    c = val;
            }
            else
            {
                if (b < c)
                    b = val;
                else
                    c = val;
            }
    
            return *this;
        }
        
        uint64_t sum() const
        {
            return a + b + c;
        }
    };
    
    top_three most_cals {0};
    uint64_t current_cals {};
    //uint64_t current_cals {0};
    while (!file.eof())
    {
        //file >> temp;
        getline(file, temp);
        
        if (temp == "")
        {
            if (most_cals < current_cals)
                most_cals = current_cals;
            current_cals = 0;
            continue;
        }
        current_cals += stoi(temp);
    }
    file.close();
    
    cout << most_cals.sum() << endl;
    
    return EXIT_SUCCESS;
}
