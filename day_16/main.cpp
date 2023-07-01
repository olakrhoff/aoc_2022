/**
 * Thanks to Korektur (github below), I could not for the life of me find my bug.
 * Then I found this persons implementation, which was very similar in concept to
 * my own, then I was able to eventually narrow the bug down to a implicit cast...
 *
 * Before I found the bug I began transforming my program into theirs, in hope of narrowing
 * down the possible bug-space. My code ended up quite similar, so I can't take full credit
 * for this solution. Part 2 I don't completely understand how works, but it was so fascinating
 * that I decided to keep it here to try and figure out. I tried (in another file) a approach
 * where I split the valves in two groups where the elephant and I got the two distinct sets.
 * The computation would then be split in half, but needs to be done twice, so if the sets
 * were correct it would take as long as task 1. However, I had to iterate through a lot of
 * combinations of sets. Whilst, Korektur's code has some magic dust that makes it work
 * significantly faster. (Though, the code don't solve the example correctly, very interesting).
 *
 * Github: https://github.com/korektur
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std;

#define DEBUG 0

vector<string> split(const string &input, const string &delimiter)
{
    vector<string> output {};
    string temp = input;
    
    while (temp.find(delimiter) != -1)
    {
        output.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    output.emplace_back(temp);
    
    return output;
}

unordered_map<char16_t, uint16_t> flows;
unordered_map<char16_t, vector<char16_t>> graph;
unordered_map<char16_t, unordered_map<char16_t, uint16_t>> tunnels;
vector<char16_t> valves;
vector<char16_t> valves_with_non_zero_flow;
unordered_map<char16_t, unordered_map<uint8_t, unordered_map<uint64_t, uint16_t>>> cache;
unordered_map<char16_t, uint8_t> mapping;

void floyd_warshall()
{
    // Initialize the paths matrix with large distances
    const uint16_t inf = INT16_MAX;
    for (const auto &u : valves)
        for (const auto &v : valves)
        {
            tunnels[u][v] = (u == v) ? 0 : inf;
            tunnels[v][u] = (u == v) ? 0 : inf;
        }
    
    // Update the paths with direct connections
    for (const auto &u : graph)
        for (const auto &v : u.second)
            tunnels[u.first][v] = 1;
    
    // Perform the Floyd-Warshall algorithm
    for (const auto &i : valves)
        for (const auto &j : valves)
            for (const auto &k : valves)
                tunnels[j][k] = min(tunnels[j][k], (uint16_t) (tunnels[j][i] + tunnels[i][k]));
}

uint16_t find_max_flow(const char16_t valve, uint8_t time_left, uint64_t state)
{
    // If cached, use that
    if (cache[valve][time_left][state] != 0)
        return cache[valve][time_left][state];
    if (time_left <= 2)
        return 0; // We don't have time to move and release more pressure
    static uint64_t all_opened = ((uint64_t) 1 << mapping.size()) - 1;
    if (state == all_opened)
        return 0; // All valves are open, can't increase value further
    
    uint16_t result_value {};
    
    for (const auto &to: valves_with_non_zero_flow)
    {
        // If node to potentially move to is opened, ignore it.
        if ((state >> mapping[to]) & 0b1)
            continue;
        // Can't go to itself
        if (to == valve)
            continue;
        // Should not be needed, but just a safeguard
        if (flows[to] == 0)
            continue;
        uint8_t travel_time = tunnels[valve][to];
        // If we run out of time moving this distance, there is no point to move
        if (travel_time + 1 >= time_left)
            continue;
        uint8_t remaining_time = time_left - travel_time - 1;
        uint16_t new_flow = flows[to] * remaining_time;
        uint16_t new_state = state | ((uint16_t) 1 << mapping[to]);
        result_value = max(result_value, static_cast<uint16_t>(new_flow + find_max_flow(to, remaining_time, new_state)));
    }
    cache[valve][time_left][state] = result_value;
    return result_value;
}

uint16_t find_max_flow_help(const char16_t valve, uint8_t time_left, uint64_t state)
{
    uint16_t result_value {};
    if (time_left > 2)
    {
        for (const auto &to: valves_with_non_zero_flow)
        {
            // If node to potentially move to is opened, ignore it.
            if ((state >> mapping[to]) & 0b1)
                continue;
            // Can't go to itself
            if (to == valve)
                continue;
            // Should not be needed, but just a safeguard
            if (flows[to] == 0)
                continue;
            uint8_t travel_time = tunnels[valve][to];
            // If we run out of time moving this distance, there is no point to move
            if (travel_time + 1 >= time_left)
                continue;
            uint8_t remaining_time = time_left - travel_time - 1;
            uint16_t new_flow = flows[to] * remaining_time;
            uint16_t new_state = state | ((uint16_t) 1 << mapping[to]);
            result_value = max(result_value, static_cast<uint16_t>(new_flow + find_max_flow_help(to, remaining_time, new_state)));
        }
    }
    if (time_left < 15)
        result_value = max(result_value, find_max_flow('AA', 26, state));
    return result_value;
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
    int num_valves {};
    while (!file.eof())
    {
        getline(file, temp);
        
        string node_name_string = temp.substr(6, 2);
        char16_t node_name = ((char16_t)node_name_string[0] << 8) | (char16_t)node_name_string[1];
        uint64_t flow_rate = stoi(temp.substr(23, temp.find(';')));
        flows[node_name] = flow_rate; // Set the flow rate
        if (flow_rate > 0)
        {
            mapping[node_name] = num_valves++;
            valves_with_non_zero_flow.push_back(node_name);
        }
        valves.push_back(node_name); // Add to list of all valves
        
        int extra {0};
        if (temp.find("valves") != -1)
            extra = 1;
        
        string to_nodes = temp.substr(temp.find("to valve") + 9 + extra);
        vector<string> nodes_string = split(to_nodes, ", ");
        vector<char16_t> nodes;
        for (auto n : nodes_string)
            nodes.emplace_back(((char16_t)n[0] << 8) | (char16_t)n[1]);
        graph[node_name] = nodes; // Add node and neighbours to graph
    }
    
    file.close();
    
    floyd_warshall();
    
    auto start = std::chrono::high_resolution_clock::now();
    //int64_t res = find_max_flow('AA', 30, 0);
    int64_t res = find_max_flow_help('AA', 26, 0);
    auto stop = std::chrono::high_resolution_clock::now();
    cout << "Res: " << res << "\tTime: " << duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms"
         << endl;
    
    return EXIT_SUCCESS;
}
