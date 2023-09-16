#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;

#define DEBUG 1

//#define SHOW_MAP

typedef enum
{
    RIGHT,
    DOWN,
    LEFT,
    UP
} DIRECTION_T;

typedef struct
{
    uint64_t row;
    uint64_t col;
    bool is_wall;
} coord_t;


typedef struct
{
    coord_t coord;
    DIRECTION_T dir;
} marker_t;

vector<coord_t> parse_input(string &input, uint64_t row_index)
{
    vector<coord_t> row;
    
    for (int i = 0; i < input.length(); ++i)
    {
        if (input.at(i) == '.')
            row.emplace_back((coord_t){row_index, (uint64_t)i, false});
        else if (input.at(i) == '#')
            row.emplace_back((coord_t){row_index, (uint64_t)i, true});
    }
    
    return row;
}

typedef enum
{
    MOVE,
    ROTATE
} INSTRUCTION_TYPE_T;

typedef enum
{
    CLOCKWISE,
    COUNTER_CLOCKWISE
} ROTATION_T;

typedef struct
{
    INSTRUCTION_TYPE_T type;
    union
    {
        ROTATION_T rotation;
        uint8_t steps; // This might be a possible error
    };
} instruction_t;

vector<instruction_t> parse_instructions(string &input)
{
    vector<instruction_t> result;
    
    while (!input.empty())
    {
        int next_r = input.find("R");
        next_r = next_r == -1 ? 0 : next_r;
        int next_l = input.find("L");
        next_l = next_l == -1 ? 0 : next_l;
    
        int next;
        if (next_r && next_l)
            next = next_r < next_l ? next_r : next_l;
        else if (next_r)
            next = next_r;
        else if (next_l)
            next = next_l;
        else
            next = input.length();
        instruction_t ins;
        ins.type = INSTRUCTION_TYPE_T::MOVE;
        ins.steps = stoi(input.substr(0, next));
        result.emplace_back(ins);
        input = input.substr(next);
        if (input.empty())
            break;
        ins.type = INSTRUCTION_TYPE_T::ROTATE;
        if (input.at(0) == 'R')
            ins.rotation = ROTATION_T::CLOCKWISE;
        else
            ins.rotation = ROTATION_T::COUNTER_CLOCKWISE;
        result.emplace_back(ins);
        input = input.substr(1);
    }
    
    return result;
}
#ifdef SHOW_MAP
    vector<marker_t> moves;
#endif

vector<coord_t> get_column(vector<vector<coord_t>> &map, uint64_t column)
{
    vector<coord_t> col;
    
    for (vector<coord_t> row : map)
    {
        for (coord_t coord : row)
        {
            if (coord.col == column)
            {
                col.emplace_back(coord);
                break;
            }
        }
    }
    
    return col;
}

#ifdef SHOW_MAP
void add_move(marker_t marker)
{
    for (auto itr = moves.begin(); itr != moves.end(); ++itr)
        if (itr->coord.col == marker.coord.col && itr->coord.row == marker.coord.row)
        {
            moves.erase(itr);
            break;
        }
    moves.emplace_back(marker);
}
#endif

bool move_marker(marker_t &marker, vector<vector<coord_t>> &map)
{
    // Check if there is a wall directly adjacent to us in the direction we are moving
    // If not, move, else stop
    switch (marker.dir)
    {
        case RIGHT:
        {
            vector<coord_t> row = map.at(marker.coord.row);
            uint64_t col = (marker.coord.col - row.at(0).col + 1) % row.size();
            if (row.at(col).is_wall)
                return false;
            
            marker.coord.col++;
            if (marker.coord.col - row.at(0).col >= row.size())
                marker.coord.col = row.at(0).col;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
        case DOWN:
        {
            vector<coord_t> col = get_column(map, marker.coord.col);
            uint64_t row = (marker.coord.row - col.at(0).row + 1) % col.size();
            if (col.at(row).is_wall)
                return false;
    
            marker.coord.row++;
            if (marker.coord.row - col.at(0).row >= col.size())
                marker.coord.row = col.at(0).row;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
        case LEFT:
        {
            vector<coord_t> row = map.at(marker.coord.row);
            uint64_t col = (marker.coord.col - row.at(0).col + row.size() - 1) % row.size();
            if (row.at(col).is_wall)
                return false;
            
            if (marker.coord.col - row.at(0).col <= 0)
                marker.coord.col = row.at(row.size() - 1).col;
            else
                marker.coord.col--;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
        case UP:
        {
            vector<coord_t> col = get_column(map, marker.coord.col);
            uint64_t row = (marker.coord.row - col.at(0).row + col.size() - 1) % col.size();
            if (col.at(row).is_wall)
                return false;
    
            if (marker.coord.row - col.at(0).row <= 0)
                marker.coord.row = col.at(col.size() - 1).row;
            else
                marker.coord.row--;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
    }
}

bool move_marker_on_cube(marker_t &marker, vector<vector<vector<coord_t>>> &cube)
{
    // Check if there is a wall directly adjacent to us in the direction we are moving
    // If not, move, else stop
    switch (marker.dir)
    {
        case RIGHT:
        {
            vector<vector<coord_t>> row = cube.at(marker.coord.row);
            uint64_t col = (marker.coord.col - row.at(0).col + 1) % row.size();
            if (row.at(col).is_wall)
                return false;
            
            marker.coord.col++;
            if (marker.coord.col - row.at(0).col >= row.size())
                marker.coord.col = row.at(0).col;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
        case DOWN:
        {
            vector<coord_t> col = get_column(map, marker.coord.col);
            uint64_t row = (marker.coord.row - col.at(0).row + 1) % col.size();
            if (col.at(row).is_wall)
                return false;
    
            marker.coord.row++;
            if (marker.coord.row - col.at(0).row >= col.size())
                marker.coord.row = col.at(0).row;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
        case LEFT:
        {
            vector<coord_t> row = map.at(marker.coord.row);
            uint64_t col = (marker.coord.col - row.at(0).col + row.size() - 1) % row.size();
            if (row.at(col).is_wall)
                return false;
            
            if (marker.coord.col - row.at(0).col <= 0)
                marker.coord.col = row.at(row.size() - 1).col;
            else
                marker.coord.col--;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
        case UP:
        {
            vector<coord_t> col = get_column(map, marker.coord.col);
            uint64_t row = (marker.coord.row - col.at(0).row + col.size() - 1) % col.size();
            if (col.at(row).is_wall)
                return false;
    
            if (marker.coord.row - col.at(0).row <= 0)
                marker.coord.row = col.at(col.size() - 1).row;
            else
                marker.coord.row--;
            #ifdef SHOW_MAP
            add_move(marker);
            #endif
            return true;
        }
    }
}

void execute_instruction(marker_t &marker, vector<vector<coord_t>> &map, instruction_t instruction)
{
    if (instruction.type == INSTRUCTION_TYPE_T::ROTATE)
    {
        switch (instruction.rotation)
        {
            case ROTATION_T::CLOCKWISE:
                marker.dir = (DIRECTION_T)(((int)marker.dir + 1) % 4);
                break;
            case ROTATION_T::COUNTER_CLOCKWISE:
                marker.dir = (DIRECTION_T)(((int)marker.dir + 3) % 4);
                break;
        }
        #ifdef SHOW_MAP
        add_move(marker);
        #endif
        return;
    }
    
    for (int i = 0; i < instruction.steps; ++i)
        if (!move_marker(marker, map))
            break;
}

void execute_instruction_on_cube(marker_t &marker, vector<vector<vector<coord_t>>> &cube, instruction_t instruction)
{
    if (instruction.type == INSTRUCTION_TYPE_T::ROTATE)
    {
        switch (instruction.rotation)
        {
            case ROTATION_T::CLOCKWISE:
                marker.dir = (DIRECTION_T)(((int)marker.dir + 1) % 4);
                break;
            case ROTATION_T::COUNTER_CLOCKWISE:
                marker.dir = (DIRECTION_T)(((int)marker.dir + 3) % 4);
                break;
        }
        #ifdef SHOW_MAP
        add_move(marker);
        #endif
        return;
    }
    
    for (int i = 0; i < instruction.steps; ++i)
        if (!move_marker_on_cube(marker, cube))
            break;
}

typedef vector<vector<coord_t>> tile_t;

vector<vector<vector<coord_t>>> translate_map_to_cube(vector<vector<coord_t>> &map)
{
    vector<tile_t> tiles;
    
    // We know that there will be six sides to a cube, hence we have six (50x50) tiles
    for (int i = 0; i < 6; ++i)
    {
        map.at()
    }
    
}

void print_map(vector<vector<coord_t>> &map, vector<marker_t> moves)
{
    for (auto row : map)
    {
        for (int i = 0; i < row.at(0).col; ++i)
            cout << " ";
        for (auto p : row)
        {
            bool printed = false;
            for (auto m : moves)
                if (p.col == m.coord.col && p.row == m.coord.row)
                {
                    switch (m.dir)
                    {
                        case RIGHT:
                            cout << '>';
                            break;
                        case DOWN:
                            cout << 'v';
                            break;
                        case LEFT:
                            cout << '<';
                            break;
                        case UP:
                            cout << '^';
                            break;
                    }
                    printed = true;
                    break;
                }
            if (printed)
                continue;
            if (p.is_wall)
                cout << '#';
            else
                cout << '.';
        }
        cout << endl;
    }
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
    vector<vector<coord_t>> rows;
    bool reading_map = true;
    vector<instruction_t> instructions;
    while (!file.eof())
    {
        getline(file, temp);
        if (temp == "")
        {
            reading_map = false;
            continue;
        }
        if (reading_map)
            rows.emplace_back(parse_input(temp, rows.size()));
        else
           instructions = parse_instructions(temp);
    }
    
    file.close();
    marker_t marker = {rows.at(0).at(0), DIRECTION_T::RIGHT};
    #ifdef SHOW_MAP
    add_move(marker);
    #endif
    //for (instruction_t ins : instructions)
    //    execute_instruction(marker, rows, ins);

    #ifdef SHOW_MAP
    print_map(rows, moves);
    #endif
    
    //cout << "Password: " << 1000 * (marker.coord.row + 1) + 4 * (marker.coord.col + 1) + marker.dir << endl;
    
    marker = {rows.at(0).at(0), DIRECTION_T::RIGHT};
    vector<vector<vector<coord_t>>> cube = translate_map_to_cube(rows);
    for (instruction_t ins : instructions)
        execute_instruction_on_cube(marker, cube, ins);
    
    cout << "Password: " << 1000 * (marker.coord.row + 1) + 4 * (marker.coord.col + 1) + marker.dir << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
