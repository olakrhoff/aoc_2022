#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 1

typedef struct coord
{
    int x, y;
    
    coord(int x, int y) : x(x), y(y)
    {}
    
    coord()
    {}
} coord_t;

typedef enum DIRECTION
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} DIRECTION_T;

typedef struct blizzard
{
    coord_t pos;
    DIRECTION_T dir;
    
    blizzard(const coord_t &pos, DIRECTION_T dir) : pos(pos), dir(dir)
    {}
} blizzard_t;

void next_minute_of_blizzards(vector<blizzard_t> &blizzards, const coord_t top_left, const coord_t bottom_right)
{
    // Do one minute of simulation to the blizzard
}

vector<vector<blizzard_t>> create_blizzard_instance(const vector<blizzard_t> &blizzards, int instances, const coord_t top_left, const coord_t bottom_right)
{
    vector<vector<blizzard_t>> res;
    vector<blizzard_t> bliz = blizzards;
    for (int i = 0; i < instances; ++i)
    {
        res.emplace_back(bliz);
        next_minute_of_blizzards(bliz, top_left, bottom_right);
    }
    
    return res;
}

bool has_blizzard(const vector<blizzard_t> &blizzards, coord_t pos)
{
    for (auto b : blizzards)
        if (b.pos.x == pos.x && b.pos.y == pos.y)
            return true;
    
    return false;
}

int find_fastest_route(const coord_t start, const coord_t end, const vector<blizzard_t> &blizzards, const coord_t top_left, const coord_t bottom_right)
{
    
    int WIDTH = bottom_right.x;
    int HEIGHT = bottom_right.y;
    int INSTANCES = WIDTH * HEIGHT;
    int64_t table[WIDTH * HEIGHT * INSTANCES];
    
    vector<vector<blizzard_t>> blizzards_instances = create_blizzard_instance(blizzards, INSTANCES, top_left, bottom_right);
    
    // Init the table
    for (int ins = 0; ins < INSTANCES; ++ins)
    {
        int page_index = WIDTH * HEIGHT * ins;
        if (has_blizzard(blizzards_instances.at(ins), coord_t(WIDTH - 1, HEIGHT - 1)))
            table[page_index + WIDTH * HEIGHT - 1] = 1;
        else
            table[page_index + WIDTH * HEIGHT - 1] = 0;
    }
    
    // Loop over table pages and fill in the adjacent cells through the instances and repeat until we find the distance from the cell below the start coord
    
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
    coord_t start = coord_t(0, -1), end;
    vector<blizzard_t> blizzards;
    int y = -1;
    while (!file.eof())
    {
        getline(file, temp);
        
        for (int x = 0; x < temp.size(); ++x)
        {
            switch (temp.at(x))
            {
                case '#':
                    // Ignore, continue
                    break;
                case '^':
                    blizzards.emplace_back(blizzard_t(coord_t(x - 1, y), DIRECTION_T::UP));
                    break;
                case 'v':
                    blizzards.emplace_back(blizzard_t(coord_t(x - 1, y), DIRECTION_T::DOWN));
                    break;
                case '<':
                    blizzards.emplace_back(blizzard_t(coord_t(x - 1, y), DIRECTION_T::LEFT));
                    break;
                case '>':
                    blizzards.emplace_back(blizzard_t(coord_t(x - 1, y), DIRECTION_T::RIGHT));
                    break;
                case '.':
                    if (temp.at(x - 1) == '#' && temp.at(x + 1) == '#' && y > 0)
                        end = coord_t(x - 1, y);
                    break;
                default:
                    cout << "Could not classify the input char: " << temp.at(x) << endl;
                    exit(123);
            }
        }
        ++y;
    }
    
    file.close();
    
    cout << "Fastest route: " << find_fastest_route(start, end, blizzards, coord_t(0, 0), coord_t(end.x + 1, end.y)) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
