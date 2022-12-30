#include <iostream>
#include <fstream>
#include <set>

using namespace std;

#define DEBUG 0

typedef struct coord
{
    int32_t x {}, y {};
    
    coord(int32_t x, int32_t y) : x(x), y(y)
    {}
    
    coord() = default;
    
    bool operator<(const coord &rhs) const
    {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return y < rhs.y;
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
    
    bool adjecent(const coord &rhs) const
    {
        return abs(x - rhs.x) <= 1 && abs(y - rhs.y) <= 1;
    }
    
    enum DIRECTION
    {
        UP,
        DOWN,
        RIGHT,
        LEFT,
        UP_RIGHT,
        DOWN_RIGHT,
        UP_LEFT,
        DOWN_LEFT,
        EQUAL,
        ERROR
    };
    
    DIRECTION direction(const coord &rhs) const
    {
        if (*this == rhs)
            return EQUAL;
        if (y - rhs.y == 0)
        {
            if (x - rhs.x >= 2)
                return UP;
            if (x - rhs.x <= -2)
                return DOWN;
        }
        else if (x - rhs.x == 0)
        {
            if (y - rhs.y >= 2 && x - rhs.x == 0)
                return RIGHT;
            if (y - rhs.y <= -2 && x - rhs.x == 0)
                return LEFT;
        }
        else if (x - rhs.x >= 1 && y - rhs.y >= 1)
            return UP_RIGHT;
        else if (x - rhs.x <= -1 && y - rhs.y >= 1)
            return UP_LEFT;
        else if (x - rhs.x >= 1 && y - rhs.y <= -1)
            return DOWN_RIGHT;
        else if (x - rhs.x <= -1 && y - rhs.y <= -1)
            return DOWN_LEFT;
        
        return ERROR;
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
    
    //Task 1
    //coord_t head {}, prev_head {}, tail {};
    //Task 2
    #define LENGTH 10
    coord_t rope[LENGTH] {};
    set<coord_t> visited {};
    visited.insert(rope[LENGTH - 1]);
    
    while (!file.eof())
    {
        getline(file, temp);
        
        char direction = temp[0];
        int16_t x {}, y {};
        switch (direction)
        {
            case 'R':
                x = 1;
                break;
            case 'D':
                y = -1;
                break;
            case 'L':
                x = -1;
                break;
            case 'U':
                y = 1;
                break;
            default:
                exit(EXIT_FAILURE);
        }
        
        int itr = stoi(temp.substr(2));
        
        //Task 1
        /*
        for (int i = 0; i < itr; ++i)
        {
            prev_head = head;
            head.x += x;
            head.y += y;
            
            if (!head.adjecent(tail))
            {
                tail = prev_head;
                visited.insert(tail);
            }
        }
        */
        
        //Task 2
        for (int i = 0; i < itr; ++i)
        {
            rope[0].x += x;
            rope[0].y += y;
            
            for (int r = 0; r < LENGTH - 1; ++r)
            {
                if (!rope[r].adjecent(rope[r + 1]))
                {
                    switch (rope[r].direction(rope[r + 1]))
                    {
                        case coord::UP:
                            rope[r + 1].x++;
                            break;
                        case coord::DOWN:
                            rope[r + 1].x--;
                            break;
                        case coord::RIGHT:
                            rope[r + 1].y++;
                            break;
                        case coord::LEFT:
                            rope[r + 1].y--;
                            break;
                        case coord::UP_RIGHT:
                            rope[r + 1].x++;
                            rope[r + 1].y++;
                            break;
                        case coord::DOWN_RIGHT:
                            rope[r + 1].x++;
                            rope[r + 1].y--;
                            break;
                        case coord::UP_LEFT:
                            rope[r + 1].x--;
                            rope[r + 1].y++;
                            break;
                        case coord::DOWN_LEFT:
                            rope[r + 1].x--;
                            rope[r + 1].y--;
                            break;
                        case coord::EQUAL:
                            break;
                        case coord::ERROR:
                            exit(EXIT_FAILURE);
                    }
                    
                    if (r + 1 == LENGTH - 1)
                        visited.insert(rope[LENGTH - 1]);
                }
            }
        }
        for (int i = 21; i >= -16; --i)
        {
            for (int j = -16; j < 26; ++j)
            {
                bool printed = false;
                for (int r = 0; r < LENGTH; ++r)
                    if (rope[r] == coord_t(j, i))
                    {
                        cout << r;
                        printed = true;
                        break;
                    }
                if (!printed)
                    cout << ".";
            }
            cout << endl;
        }
        
        cout << endl;
    }
    
    file.close();
    
    cout << visited.size() << endl;
    
    return EXIT_SUCCESS;
}
