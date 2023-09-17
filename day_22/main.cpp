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

typedef struct tile {
    vector<vector<coord_t>> coords;
    int8_t sides[4] {-1, -1, -1, -1};
} tile_t;

bool move_marker_on_cube(marker_t &marker, vector<tile_t> &cube)
{
    // Check if there is a wall directly adjacent to us in the direction we are moving
    // If not, move, else stop
    switch (marker.dir)
    {
        case RIGHT:
        {
            
        }
        case DOWN:
        {
        }
        case LEFT:
        {
        }
        case UP:
        {
        }
    }
}

void execute_instruction_on_cube(marker_t &marker, vector<tile_t> &cube, instruction_t instruction)
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

vector<tile_t> translate_map_to_cube(vector<vector<coord_t>> map)
{
    vector<tile_t> tiles(6);
    int LINE_LENGTH = 50;
    if (DEBUG)
        LINE_LENGTH = 4;
    
    
    // We know that there will be six sides to a cube, hence we have six (50x50) tiles
    for (int i = 0; i < 6; ++i)
    {
        tile_t new_tile;
        
        // Fill tile with coords from it's 50x50 coords
        vector<vector<coord_t>> coords;
        vector<vector<coord_t>> write_back;
        for (int l = 0; l < LINE_LENGTH; ++l)
        {
            auto line = map.at(l);
            if (line.size() > LINE_LENGTH)
            {
                vector<coord_t> write_back_line = {line.begin() + LINE_LENGTH, line.end()};
                write_back.emplace_back(write_back_line);
            }
            vector<coord_t> temp = {line.begin(), line.begin() + LINE_LENGTH};
            coords.emplace_back(temp);
        }
        if (write_back.empty())
            map = {map.begin() + LINE_LENGTH, map.end()};
        else
            for (int j = 0; j < write_back.size(); ++j)
                map.at(j) = write_back.at(j);
            
        
        new_tile.coords = coords;
        tiles.at(i) = new_tile;
    }
    
    
    
    for (int i = 0; i < 6; ++i)
    {
        coord_t lower_right_corner = tiles.at(i).coords.at(LINE_LENGTH - 1).at(LINE_LENGTH - 1);
        for (int j = i + 1; j < 6; ++j)
        {
            coord_t upper_left_corner = tiles.at(j).coords.at(0).at(0);
            // If the ULC is at row (height) adjacent to the LRC and
            // the column coord is seperated by LINE_WIDTH (one tile width).
            if (upper_left_corner.row - 1 == lower_right_corner.row &&
                upper_left_corner.col + LINE_LENGTH - 1 == lower_right_corner.col)
            {
                tiles.at(i).sides[DOWN] = j;
                tiles.at(j).sides[UP] = i;
            }
            else if (upper_left_corner.col - 1 == lower_right_corner.col &&
                     upper_left_corner.row + LINE_LENGTH - 1 == lower_right_corner.row)
            {
                tiles.at(i).sides[RIGHT] = j;
                tiles.at(j).sides[LEFT] = i;
            }
        }
    }
    
    // Now that we have stitches together the directly connected tiles
    // we need to fold it to get the complete mapping between the sides.
    bool changed = false;
    do
    {
        changed = false;
        // Loop over all the sides in the cube
        for (int i = 0; i < 6; ++i)
        {
            // For each side, check each adjacent side
            for (int r = 0; r < 4; ++r)
            {
                tile_t &current_tile = tiles.at(i);
                // If we don't know the side in the given direction we can't use it yet
                if (current_tile.sides[r] == -1)
                    continue;
                
                DIRECTION_T other_side_direction;
                for (int j = 0; j < 4; ++j)
                    if (tiles.at(current_tile.sides[r]).sides[j] == i)
                        other_side_direction = (DIRECTION_T)j;
                
                int8_t side = current_tile.sides[r];
                int left_index = (other_side_direction + 4 - 1) % 4;
                int right_index = (other_side_direction + 1) % 4;
                if (tiles.at(side).sides[left_index] != -1)
                {
                    int8_t left_side = tiles.at(side).sides[left_index];
                    if (current_tile.sides[(r + 1) % 4] != -1)
                        continue;
                        
                    current_tile.sides[(r + 1) % 4] = left_side;
                    DIRECTION_T direction_to_current;
                    for (int j = 0; j < 4; ++j)
                        if (tiles.at(left_side).sides[j] == side)
                            direction_to_current = (DIRECTION_T)j;
                    direction_to_current = (DIRECTION_T)((direction_to_current + 4 - 1) % 4);
                    
                    tiles.at(left_side).sides[direction_to_current] = (int8_t)i;
                    changed = true;
                }
                if (tiles.at(side).sides[right_index] != -1)
                {
                
                }
            }
        }
    } while (changed);

    return tiles;
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
    vector<tile_t> cube = translate_map_to_cube(rows);
    for (instruction_t ins : instructions)
        execute_instruction_on_cube(marker, cube, ins);
    
    cout << "Password: " << 1000 * (marker.coord.row + 1) + 4 * (marker.coord.col + 1) + marker.dir << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
