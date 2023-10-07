#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0

typedef struct coord
{
    int x, y;
    
    coord(int x, int y) : x(x), y(y)
    {}
    
    coord()
    {}
    
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
    vector<blizzard_t> next_blizzards;
    
    for (auto b : blizzards)
    {
        blizzard_t n = b;
        switch (n.dir)
        {
            case UP:
                n.pos.y--;
                if (n.pos.y <= top_left.y)
                    n.pos.y = bottom_right.y - 1;
                break;
            case DOWN:
                n.pos.y++;
                if (n.pos.y >= bottom_right.y)
                    n.pos.y = top_left.y + 1;
                break;
            case LEFT:
                n.pos.x--;
                if (n.pos.x <= top_left.x)
                    n.pos.x = bottom_right.x - 1;
                break;
            case RIGHT:
                n.pos.x++;
                if (n.pos.x >= bottom_right.x)
                    n.pos.x = top_left.x + 1;
                break;
        }
        next_blizzards.emplace_back(n);
    }
    
    blizzards = next_blizzards;
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

/*
 * The blizzards argument is the blizzard instance we are comparing to.
 */
vector<coord_t> get_moves(const coord_t current, const vector<blizzard_t> &blizzards, const coord_t top_left, const coord_t bottom_right)
{
    // Find all possible coords to move to
    vector<coord_t> all_moves = {coord_t(current.x, current.y + 1),
                                 coord_t(current.x - 1, current.y),
                                 coord_t(current.x + 1, current.y),
                                 coord_t(current.x , current.y - 1),
                                 coord_t(current.x, current.y)};
    // Filter out the ones that is not inside the walls
    vector<coord_t> possible_moves;
    for (auto m : all_moves)
    {
        // If it is the start spot
        if (m.x == top_left.x + 1 && m.y == top_left.y)
        {
            possible_moves.emplace_back(m);
            continue;
        }
        // If it is the end spot
        if (m.x == bottom_right.x - 1 && m.y == bottom_right.y)
        {
            possible_moves.emplace_back(m);
            continue;
        }
        
        // If it is within the four walls
        if (m.x > top_left.x && m.x < bottom_right.x && m.y > top_left.y && m.y < bottom_right.y)
        {
            possible_moves.emplace_back(m);
            continue;
        }
    }
    
    // Check that the coord doesn't have a blizzard
    vector<coord_t> feasible_moves;
    for (auto m : possible_moves)
    {
        bool collision = false;
        for (auto b : blizzards) //TODO: Would be much quicker with a ordered set instead of vector
        {
            if (m.x == b.pos.x && m.y == b.pos.y)
            {
                collision = true;
                break;
            }
        }
        if (!collision)
            feasible_moves.emplace_back(m);
    }
    
    // Return feasible moves
    return feasible_moves;
}

typedef struct state
{
    coord_t pos;
    int itr;
    
    state(const coord_t &pos, int itr) : pos(pos), itr(itr)
    {}
    
    bool operator<(const state &rhs) const
    {
        if (itr < rhs.itr)
            return true;
        if (itr > rhs.itr)
            return false;
        
        return pos < rhs.pos;
    }
    
    bool operator>(const state &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const state &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const state &rhs) const
    {
        return !(*this < rhs);
    }
} state_t;

int find_fastest_route(const coord_t start, const coord_t end, const vector<blizzard_t> &blizzards, const coord_t top_left, const coord_t bottom_right, int iteration_offset)
{
    
    int WIDTH = bottom_right.x;
    int HEIGHT = bottom_right.y;
    int INSTANCES = WIDTH * HEIGHT;
    
    iteration_offset = iteration_offset % INSTANCES;
    
    vector<vector<blizzard_t>> blizzards_instances = create_blizzard_instance(blizzards, INSTANCES, top_left, bottom_right);
    rotate(blizzards_instances.begin(), blizzards_instances.begin() + iteration_offset, blizzards_instances.end());
    set<state_t> states;
    states.insert(state_t(start, 0));
    
    while (!states.empty())
    {
        state_t current = *states.begin(); // Get state with the shortest path
        //cout << current.itr << endl;
        states.erase(states.begin()); // Remove first element
        
        if (current.pos.x == end.x && current.pos.y == end.y)
            return current.itr;
        
        vector<coord_t> new_states = get_moves(current.pos, blizzards_instances.at((current.itr + 1) % blizzards_instances.size()), top_left, bottom_right);
        
        for (auto c : new_states)
            states.insert(state_t(c, current.itr + 1));
    }
    
    return -1; // Did not find a path to the end
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
    
    // Part 1
    //cout << "Fastest route: " << find_fastest_route(start, end, blizzards, coord_t(-1, -1), coord_t(end.x + 1, end.y)) << endl;
    
    //Part 2
    int start_to_end = find_fastest_route(start, end, blizzards, coord_t(-1, -1), coord_t(end.x + 1, end.y), 0);
    int end_to_start = find_fastest_route(end, start, blizzards, coord_t(-1, -1), coord_t(end.x + 1, end.y), start_to_end);
    int start_to_end_again = find_fastest_route(start, end, blizzards, coord_t(-1, -1), coord_t(end.x + 1, end.y), start_to_end + end_to_start);
    
    cout << "Fastest route: " << start_to_end + end_to_start + start_to_end_again << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
