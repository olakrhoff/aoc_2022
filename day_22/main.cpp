#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;

#define DEBUG 0

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
    for (instruction_t ins : instructions)
        execute_instruction(marker, rows, ins);

    #ifdef SHOW_MAP
    print_map(rows, moves);
    #endif
    
    cout << "Password: " << 1000 * (marker.coord.row + 1) + 4 * (marker.coord.col + 1) + marker.dir << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    //3L45L5L39R13L21L5L27R
    //3L45L5L39R13L21L5L27R8R24R10L41L22L2L1R28L22L46L50R12L47R43R12L12R41L8L49R24L46R33L1R45L25R1R34R11L47L25R43L26R5R13L5R19L20L50R27L20L25L49R30R24R8L49L10R46R4R21L26R16R33L8L26L47R48L24R8R13R38L35L39L47L33R22R31R43R31R3L27R4L23L44R31R33R4L4L49L37L45L16R9R10R49L25R17L2R35L30L2L24R3L17R37R29R36L38L7R44L5L36R29L20L46L48R10L38L8R6L14R45R18R36R42L22L18L9L43R50R30R24R10R30L32L23L38L5R14L23R11L30L20R45R48R24L31L45L13L48L26R34L7L4R46L31R13R31L41R48R29R3L33R14L4L33R36L30L46R16L33L33R23R4L7L34L5L2R5R4R30R8L49L36L3L44R42R28R38R46R33L38R43R20R2L22L4L37R2L33R35L41R48R37R16R42R47R15L24L8L44R3L33L24R21L32R14L27R41L9L29R47R11R36L38R46R6R27L16L28R26R7R15L35R46R1L26L38L11R21L28L4R32L16R10L16R18R30L23L44L40L3R2R30L5L30R11R13L4L50L25R34R37R24R27R39L36L43R24L8L31L30R5R19R44L16R24L21R38L4R26L21R5R39R17R23R28R21R25L50L20R20L10L34L2R44L32L37R20R33R4L9L22R50R38R14R30R39R21R18L7L33R30L49R39R14R14R6L9R15L39R11L45L36R8R37L34R4R30R42R10R27L33R14L21R22R36R21R24R10R16R6R43L5L30L12R13L23R27L48R31L28R4R14R43L8R7L23R16L34L28R36L21R44R13L15L39L31R44R31R1R31R30R7L37R30R33L36R7L50L46L7L48R6R22R37L25R44R35L42L32R21R29L19L14L21R7R15L24R30L3R1L13R33L41R18L44L23L48L2L18L21L4R43L47L6R18R37R28L13R28R31R20R2R30R29L12R18R26L8R36L28L27L28R22R38L4R33L36R38L48R14L27L24L15L38R31R40L17R19L16R11R41R39L29L1L30L22L24R44R29R50L36R39L42R26R6R39L27L30R19L24R32L37R28R36L35L33L19R27L46R28R34L48L44R19R43R46L10R8L29R41R37R5R35R44R37R22L23L35R12R11L15R42R5L41R25R15R31R30R45L15R38R1R33L38R10R26L15L35R12L9R30L35L4R4R11R18R33L12R36R33L8L15R46L39R22R15L32L6L7L2L36L14R2L24L3R9L31L38R16R25R42R22R9L44R11R4R32R12L50R30R10L14L28R38L46L39L41L35L46R6R3L15L35R32R11L12R16L26L32L26R45R24L27R1L10L50L10L26L44L8R7R33R5L5L22L12R22R49L42L4R26L43R1R16L5L40R38L47L40R15R16R46R2L50L44R43R16R40R39L43R25R31R15R40L47R23R27L6R36L20R7L43R33R20R41L32L29L32R25L27R4R19L1L45R20L9R2R48R7L46L47L39R44L16R46R42R5R21R11L33R46L29R8R6L41R19L2R30L32R11R43L28L37R45R36L18L18R21R41R30L16R14L33R22L42L8R9R20R29R6R45L18L7R16R18R50R49L15R50R43L15L33R34R31L11R41L18L41L8R20R44L46L6L21R42L8L13R48R35L28R1L12R25R35R42R38L23L19L18R9R41R6L45L8L48R11L50L26R32R10L16R15L8R41R6R13R33R11L14L47L35R35R7L42L21R50L18R33L6R45R7L16R41L1R18R24L8R35R3L36R42L39R23L9L34R47R10R4R50L50L21L1L10L35R4L14L22R30L39L1R34L3L36L26R6R25L23L23L24R25R26L28R6L28L7R30R24L1R39L33R9L15L7R36L28L11L20R31R8R14R47L42R19L37R46R11R27L8R27R50R33L36L27R4R43R39R5R37R39R25R16L42L3L49L31L1R15R23R48L4R21R48L43L7L30R43R38R14L37R3R20R3R37R42L28L43R20L36L36R19R26L13R1L9R9R40R40L37L4L48L16R40L16L27R12L17R24R17R7R35L26R33R19L4L15L37L33R45L50R35L39L22R2R40R43L44L15L25L29L46L1L36R15L44R3R6R17L40L43L14L7L36L29L4R43L32R46R47R14L36L49L24L14R31L6R23L39L3R10R8L21R28R11R33R33L34L20R19R1R28L10R20R9L11L13L27L40R14R33L7L5L43L7L30L39L18L4R3R9R12R31L40L50L38R26L9L14L16L10R15R7R24R5L42R42L11L37L4L32L29L8L46R12R24R29L20R39L50L19R33L34L47R26R15L18R37L30R7R21R21L10R1R50R25L36R45R28R39L20L31L25R31R34R44L35R11L23R46L21R11L9L36L4R39R29R19R2L27L9L41L9L10R3L1L17L9L42L23R39R3R23R5R28L27L28L36L48R20L15L8R24L35L26L17R31L27R1L3L35L33R31R26L22L37L38L28R15R30L36R13R19L34R40R48L32L22L42R44L49R23R21L38R29R4L17L43L21R40L24R9L43L2L31L2L41L20L6L15R39R12L32R35L10R15R8R48R8L22L9R12R18R23L28L34R27R6R4L34R13L25R18L17R44R17L19L30L48L38L27L13L1L25R19L35R48R20L25L2L13L6R4R10R37R8R43R14L13R9L17R17L29L48R15R47R12L22L36L49R6R25L12L48R24R4R4L48L16R22R29L31L9L4R8L40R22L2R4L30L24L39L43L42R32R6R46R26L4R9R39R49L1L38L18R41L40R43R19L22R15L9L43R2R44R43L47L18L25R38R50R43L5R34R37R9L3L48R41R21R49R15R33L44L36L31R34L44L13L49L37L47R42R13L32L18L1L1L33R11L35R40R4L19R33R47R47R11R28R11L43L30L45L38R27L24R6R47L20L33R15R4L1R6R33L1L23R10R49R26R24R34R5L18R24R22R48L22L47L38L47R18R31L36L27L9L46R10L19R14R7R17R15L45R17R36R20L31L33L7L18L16R44L15R34L23L7R35R50L26R39R41L14L40R8L29R39R30L10L20R36R14R16R10L37L5R9R47R49R17R13L1R28L15R18L22R12R21R18L37L16L44L9L10R23L19R3L29R25R24L19R4R29L3R37L12R26L13L34R9L11R9L10R41L34L16L12L48L28R23R17L37L15R41L8R20R45L49L31R35L28L48R10L37L37R9L14R8L24R12L17R49L30L38L27L27R34R31L45R13L45L32R11R32R48L31L30L28R27L32R20L28R6R14L15L25R3L22R5L22R15L34R36R11L27L13L12R25L38L23L10L27R30R46R12L18L44R14L16R44R1L31R47R38L4L32L43L25L5L18L43R41R25R45R33L27L31L46R47L36R48L44L4R17L2L34L6R19L20R20L12L6R15L48L39R24R45L2R17R29R16L29R18R47L43L41R28R4R12R26L6R38L8R50R25R9L38L14L45L24L6R17R44R26L10L9L35R17R30L35R24L2R13L33R12R15L47L19L50L19R25L13L26L30R7R9R28R37L49L49L34R43R25L37R4R38L30L9R12L26L1L19R31R46L9L5L10L16L45R2R28L29R14L42R34R47R46R42R50R48R8R6R50L17R47L7R48L16R24L26L21L6R20L14R13L13L39L34R8L43L32L32R47R47R31R39R42L32L28L9R37L34L1R26L4L48R45R43L7R5L28R43R40L34R1R47L39L41L14R43L46R41L3L19R14R22L15R2R6R23R45L17L43L15R32R1R37L38L20L15R9R9R47L32R31L8L16R23L24L46L38R34R46L7R33L8L40L38R37R42R42L1L40L40R37L42R47L28R5L39L32L15L18L45R7R6L22L46R8L3R46L29L32L36L18R42R22L8L29L26L4R36R9R2R22R49L25R8L15L10R48L20R5R41L32R11L13R24R38R8L13R42R19L46R26R18L23L27L43R30R34R32L28L45R2L43L4R4L27L27L27L27R8L45R31R43R41L13R29L21L8R25R4R33R35R39L9R18L2L2L11L42L28L7L8L46R7L4L42R14R31L19R1R20L10R7L44R35R3R6R10L36R19L40L35R26R15R31L48R29L19R17L16R32L27R49L17L20R23L30R45R48R44R9L7L36L16L36R9L13L30R48L1L16L50L15R13R47R20L30R12R2L9L36R7L39L36L17R16R5R41R41L44R24L15L45L49L33R30R11L13R41R35L16R5R24R3L47R14R39L33L1L12R8R49R8R1R40L6R1L14L19R36L7L24L47L18L19R10R3L20L36L31L48L45L35L50R14L5R1L30R10L44L50L6L17R33R4L48L4L45R7L4R35L29L35R19R19R47R6R12L3L4L32R46R35L23L17L28L12R22L48L14L46R2L10R11L35R14R6R12L8R5R8L6R22R50R13L19R3R24R16R37L48R45L37R36R49L4L19L47L20R33R25L46L33R30R6L31R4R6L23R7R13R16L46L17L4L22L30R37L6L19L5R9R24L12R20R18L15L45L5R39R23L17R49L9R47L5L20L50L16L2R6R8L8R1R12L36L20R6L41R47R23R33L1R45L3L3R11L14R33L34L23R38R13R3L1R25L6R39L25R29
    return EXIT_SUCCESS;
}
