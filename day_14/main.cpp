#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0

vector<string> split(const string val, const string delimiter)
{
    vector<string> values {};
    string temp = val;
    
    while (temp.find(delimiter) != -1)
    {
        values.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    
    values.emplace_back(temp);
    
    return values;
}

typedef struct coord
{
    int32_t x, y;
    
    coord(int32_t x, int32_t y) : x(x), y(y)
    {}
    
    coord(const string &s)
    {
        vector<string> values = split(s, ",");
        x = stoi(values.at(0));
        y = stoi(values.at(1));
    }
    
    bool operator<(const coord &rhs) const
    {
        if (y < rhs.y)
            return true;
        if (rhs.y < y)
            return false;
        return x < rhs.x;
    }
    
    bool operator>(const coord &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const coord &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const coord &rhs) const
    {
        return !(*this < rhs);
    }
    
    bool operator==(const coord &rhs) const
    {
        return x == rhs.x &&
               y == rhs.y;
    }
    
    bool operator!=(const coord &rhs) const
    {
        return !(rhs == *this);
    }
    
    vector<coord> line(const coord &rhs) const
    {
        vector<coord> coords {};
        
        if (x == rhs.x)
        {
            for (int i = 0; i <= abs((int)y - (int)rhs.y); ++i)
            {
                int sign = (int)y - (int)rhs.y < 0 ? 1 : -1;
                coords.emplace_back(coord(x, sign * i + y));
            }
        }
        else
        {
            for (int i = 0; i <= abs((int)x - (int)rhs.x); ++i)
            {
                int sign = (int)x - (int)rhs.x < 0 ? 1 : -1;
                coords.emplace_back(coord(sign * i + x, y));
            }
        }
        
        return coords;
    }
    
} coord_t;

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
    set<coord_t> rocks {};
    coord_t sand_entry(500, 0);
    
    while (!file.eof())
    {
        getline(file, temp);
    
        vector<string> points = split(temp, " -> ");
        vector<coord_t> coords {};
        
        for (string s : points)
            coords.emplace_back(coord_t(s));
        
        
        for (int i = 0; i < coords.size() - 1; ++i)
        {
            vector<coord_t> line = coords.at(i).line(coords.at(i + 1));
            for (coord_t c : line)
                rocks.insert(c);
        }
    }
    
    int max_y = max_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.y < b.y; })->y;
    int min_y = min_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.y < b.y; })->y;
    int max_x = max_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.x < b.x; })->x;
    int min_x = min_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.x < b.x; })->x;
    
    for (int y = min_y; y <= max_y; ++y)
    {
        for (int x = min_x; x <= max_x; ++x)
        {
            if (rocks.find(coord_t(x, y)) != rocks.end())
            {
                cout << "#";
            }
            else
            {
                cout << ".";
            }
        }
        cout << endl;
    }
    
    file.close();
    
    
    //Task 2
    vector<coord_t> floor = coord_t(-(max_y + 3) + 500, max_y + 2).line(coord_t(max_y + 3 + 500, max_y + 2));
    for (coord_t c : floor)
        rocks.insert(c);
    
    max_y = max_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.y < b.y; })->y;
    
    bool done = false;
    int count;
    for (count = 0; !done; ++count)
    {
        coord_t sand = sand_entry;
        bool valid = false;
        do
        {
            coord_t next = sand;
            next.y += 1;
            
            if (next.y > max_y)
            {
                done = true;
                break;
            }
            
            if (rocks.find(next) == rocks.end())
            {
                sand = next;
                valid = true;
                continue;
            }
            next.x -= 1;
            if (rocks.find(next) == rocks.end())
            {
                sand = next;
                valid = true;
                continue;
            }
            
            next.x += 2;
            if (rocks.find(next) == rocks.end())
            {
                sand = next;
                valid = true;
                continue;
            }
            
            if (sand == sand_entry)
            {
                done = true;
                break;
            }
            
            valid = false;
        } while (valid);
        if (!done)
            rocks.insert(sand);
    }
    
    //Task 1
    //cout << count - 1 << endl;
    //Task 2
    cout << count << endl;
    
    
    max_y = max_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.y < b.y; })->y;
    min_y = min_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.y < b.y; })->y;
    max_x = max_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.x < b.x; })->x;
    min_x = min_element(rocks.begin(), rocks.end(), [](coord_t a, coord_t b){ return a.x < b.x; })->x;
    
    
    
    for (int y = min_y; y <= max_y; ++y)
    {
        for (int x = min_x; x <= max_x; ++x)
        {
            if (rocks.find(coord_t(x, y)) != rocks.end())
            {
                cout << "#";
            }
            else
            {
                cout << ".";
            }
        }
        cout << endl;
    }
    
    return EXIT_SUCCESS;
}
