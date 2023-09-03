#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <memory.h>

using namespace std;

#define DEBUG 0

#define MINUTES 24
#define MINUTES_2 32

typedef struct cost
{
    int ore;
    int clay;
    int obsidian;
    int geode;
    
    cost operator+(const cost &rhs)
    {
        return {this->ore + rhs.ore, this->clay + rhs.clay, this->obsidian + rhs.obsidian, this->geode + rhs.geode};
    }
    
    cost operator-(const cost &rhs)
    {
        return {this->ore - rhs.ore, this->clay - rhs.clay, this->obsidian - rhs.obsidian, this->geode - rhs.geode};
    }
    
    bool operator==(const cost &rhs) const
    {
        return ore == rhs.ore &&
               clay == rhs.clay &&
               obsidian == rhs.obsidian &&
               geode == rhs.geode;
    }
    
    bool operator!=(const cost &rhs) const
    {
        return !(rhs == *this);
    }
    
    bool operator<(const cost &rhs) const
    {
        if (ore < rhs.ore)
            return true;
        if (rhs.ore < ore)
            return false;
        if (clay < rhs.clay)
            return true;
        if (rhs.clay < clay)
            return false;
        if (obsidian < rhs.obsidian)
            return true;
        if (rhs.obsidian < obsidian)
            return false;
        return geode < rhs.geode;
    }
    
    bool operator>(const cost &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const cost &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const cost &rhs) const
    {
        return !(*this < rhs);
    }
    
} cost_t;

typedef enum
{
    ORE,
    CLAY,
    OBSIDIAN,
    GEODE
} ROBOTS_T;

typedef struct
{
    int id;
    const cost_t robot_costs[4];
    /*
    const cost_t ore_robot_cost;
    const cost_t clay_robot_cost;
    const cost_t obsidian_robot_cost;
    const cost_t geode_robot_cost;
    */
    
    bool can_make_robot(cost_t budget, cost_t price)
    {
        return price.ore <= budget.ore &&
               price.clay <= budget.clay &&
               price.obsidian <= budget.obsidian &&
               price.geode <= budget.obsidian;
    }
    
    bool should_make_robot(cost_t storage, cost_t production, ROBOTS_T type, int time_left)
    {
        switch (type)
        {
            case ORE:
            {
                int max_ore_cost = 0;
                for (auto robot_cost: robot_costs)
                    if (robot_cost.ore > max_ore_cost)
                        max_ore_cost = robot_cost.ore;
    
                if (max_ore_cost * time_left > storage.ore + production.ore * time_left)
                    return true;
                break;
            }
            case CLAY:
            {
                int max_clay_cost = 0;
                for (auto robot_cost: robot_costs)
                    if (robot_cost.clay > max_clay_cost)
                        max_clay_cost = robot_cost.clay;
    
                if (max_clay_cost * time_left > storage.clay + production.clay * time_left)
                    return true;
                break;
            }
            case OBSIDIAN:
            {
                int max_obsidian_cost = 0;
                for (auto robot_cost: robot_costs)
                    if (robot_cost.obsidian > max_obsidian_cost)
                        max_obsidian_cost = robot_cost.obsidian;
    
                if (max_obsidian_cost * time_left > storage.obsidian + production.obsidian * time_left)
                    return true;
                break;
            }
            case GEODE:
                return true;
        }
        
        return false;
    }
    
    int get_id() const
    {
        return id;
    }
    
} blueprint_t;

blueprint_t parse_to_blueprint(string input)
{
    input = input.substr(10);
    int id = stoi(input.substr(0, input.find(':')));
    input = input.substr(input.find("ore robot") + 16);
    cost_t ore = {stoi(input.substr(0, input.find(' '))), 0, 0, 0};
    input = input.substr(input.find("clay robot") + 17);
    cost_t clay = {stoi(input.substr(0, input.find(' '))), 0, 0, 0};
    input = input.substr(input.find("obsidian robot") + 21);
    cost_t obsidian = {stoi(input.substr(0, input.find(' '))), stoi(input.substr(input.find("and ") + 4, input.find(" clay"))), 0, 0};;
    input = input.substr(input.find("geode robot") + 18);
    cost_t geode = {stoi(input.substr(0, input.find(' '))), 0, stoi(input.substr(input.find("and ") + 4, input.find(" obsidian"))), 0};;
    
    
    
   return {id, ore, clay, obsidian, geode};
}

const cost_t PRODUCTION[4] =
{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
};

typedef struct state
{
    cost_t storage;
    cost_t production;
    int time_left;
    
    bool operator==(const state &rhs) const
    {
        return storage == rhs.storage &&
               production == rhs.production &&
               time_left == rhs.time_left;
    }
    
    bool operator!=(const state &rhs) const
    {
        return !(rhs == *this);
    }
    
    bool operator<(const state &rhs) const
    {
        if (storage < rhs.storage)
            return true;
        if (rhs.storage < storage)
            return false;
        if (production < rhs.production)
            return true;
        if (rhs.production < production)
            return false;
        return time_left < rhs.time_left;
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

typedef set<state_t> cache_t;

bool in_cache(cache_t &cache, state_t val)
{
    return cache.find(val) != cache.end();
}

int max_geode_collected(blueprint_t &blueprint, state_t start_state)
{
    cache_t cache;
    int max_state_geode = 0;
    queue<state_t> states;
    
    states.push(start_state);
    
    while (!states.empty())
    {
        state_t current_state = states.front();
        states.pop();
        
        // This is a final state, check if it is the best yet
        if (current_state.time_left == 0)
        {
            if (current_state.storage.geode > max_state_geode)
                max_state_geode = current_state.storage.geode;
            continue;
        }
        
        // We have a production that should make a geode each turn from now on
        if (blueprint.can_make_robot(current_state.production, blueprint.robot_costs[ROBOTS_T::GEODE]))
        {
            cout << "Hurray" << endl;
            int time_left = current_state.time_left;
            int rest_of_geode_production = (time_left * time_left + time_left) / 2;
            
            
            state_t new_state = {current_state.storage + (cost_t){0, 0, 0, rest_of_geode_production}, current_state.production, 0};
            states.push(new_state);
            continue;
        }
        
        // Manually loop over all robot types
        int possible = 0;
        for (int i = 3; i >= 0; --i)
        {
            bool was_possible;
            if ((was_possible = blueprint.can_make_robot(current_state.storage, blueprint.robot_costs[i])) && blueprint.should_make_robot(current_state.storage, current_state.production, (ROBOTS_T)i, current_state.time_left))
            {
                state_t new_state = {(current_state.storage - blueprint.robot_costs[i]) + current_state.production, current_state.production + PRODUCTION[i], current_state.time_left - 1};
                if (!in_cache(cache, new_state)) // If not cached
                {
                    states.push(new_state);
                    cache.insert(new_state);
                }
                // If we can make a geode, make one
                if (i == 3)
                {
                    possible = 4;
                    break;
                }
            }
            if (was_possible)
                ++possible;
        }
        
        // If we could make all four robots, then there is no reason to not make one
        if (possible >= 4)
            continue;
        
        state_t new_state = {current_state.storage + current_state.production, current_state.production, current_state.time_left - 1};
        if (!in_cache(cache, new_state)) // If not cached
        {
            states.push(new_state);
            cache.insert(new_state);
        }
    }
    return max_state_geode;
}

//#define TASK_1

int main()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    //Reading data from file
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    string temp {};
    #ifdef TASK_1
        uint64_t quality_level = 0;
    #else
        uint64_t quality_level = 1;
        int counter = 0;
    #endif
    while (!file.eof())
    {
        getline(file, temp);
        
        blueprint_t blueprint = parse_to_blueprint(temp);
        #ifdef TASK_1
        uint64_t quality = max_geode_collected(blueprint, {{0, 0, 0, 0}, {1, 0, 0, 0}, MINUTES});
        #else
        uint64_t quality = max_geode_collected(blueprint, {{0, 0, 0, 0}, {1, 0, 0, 0}, MINUTES_2});
        #endif
        cout << "Quaility: " << quality << endl;
        #ifdef TASK_1
        quality_level += quality * blueprint.get_id();
        #else
        quality_level *= quality;
        if (++counter >= 3)
            break;
        #endif
    }
    
    file.close();
    
    cout << "Quality level is: " << quality_level << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    
    return EXIT_SUCCESS;
}
