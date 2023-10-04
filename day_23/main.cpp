#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

#define ROUNDS 10

typedef struct coord
{
    int row, col;
    
    coord(int row, int col) : row(row), col(col)
    {}
    
} coord_t;

bool has_pos(const vector<coord_t> &positions, const coord_t &pos)
{
    //TODO: This might be a bottle neck if the search through the vector is slow
    for (auto p : positions)
        if (p.row == pos.row && p.col == pos.col)
            return true;
    return false;
}

void print_positions(const vector<coord_t> &positions)
{
    int min_col = positions.at(0).col,
            max_col = positions.at(0).col,
            min_row = positions.at(0).row,
            max_row = positions.at(0).row;
    
    for (auto pos : positions)
    {
        if (pos.col < min_col)
            min_col = pos.col;
        else if (pos.col > max_col)
            max_col = pos.col;
        
        if (pos.row < min_row)
            min_row = pos.row;
        else if (pos.row > max_row)
            max_row = pos.row;
    }
    
    for (int i = min_row - 2; i < max_row + 3; ++i)
    {
        for (int j = min_col - 2; j < max_col + 3; ++j)
        {
            if (has_pos(positions, coord_t(i, j)))
                cout << "#";
            else
                cout << ".";
        }
        cout << endl;
    }
}

void simulate(vector<coord_t> &positions, int rounds)
{
    vector<coord_t> next_pos = positions;
    for (int itr = 0; itr < rounds; ++itr)
    {
        next_pos = positions;
        if (DEBUG)
        {
            print_positions(positions);
            cout << "\n\n";
        }
        for (int i = 0; i < positions.size(); ++i)
        {
            coord_t pos = positions.at(i);
            bool has_adjacent_elf = false;
            for (int row = -1; row < 2; ++row)
            {
                for (int col = -1; col < 2; ++col)
                {
                    if (row == 0 && col == 0)
                        continue; // We do not want to check our self
                    if (has_pos(positions, coord_t(pos.row + row, pos.col + col)))
                    {
                        has_adjacent_elf = true;
                        break;
                    }
                }
                if (has_adjacent_elf)
                    break;
            }
            
            if (!has_adjacent_elf)
                continue;
            bool moved = false;
            for (int c = 0; c < 4; ++c)
            {
                if (moved)
                    break;
                int cond = (c + itr) % 4;
                switch (cond)
                {
                    case 0: //NORTH
                        if (!has_pos(positions, coord_t(pos.row - 1, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row - 1, pos.col)) &&
                            !has_pos(positions, coord_t(pos.row - 1, pos.col + 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row - 1, pos.col);
                            moved = true;
                        }
                        break;
                    case 1: //SOUTH
                        if (!has_pos(positions, coord_t(pos.row + 1, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col + 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row + 1, pos.col);
                            moved = true;
                        }
                        break;
                    case 2: //WEST
                        if (!has_pos(positions, coord_t(pos.row - 1, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col - 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row, pos.col - 1);
                            moved = true;
                        }
                        break;
                    case 3: //EAST
                        if (!has_pos(positions, coord_t(pos.row - 1, pos.col + 1)) &&
                            !has_pos(positions, coord_t(pos.row, pos.col + 1)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col + 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row, pos.col + 1);
                            moved = true;
                        }
                        break;
                    default:
                        cout << "Bad, switch should identify all cases, got cond: " << cond << ", i: " << i << endl;
                        exit(123);
                }
            }
        }
        
        //Check for duplicates in movement
        for (int i = 0; i < next_pos.size(); ++i)
        {
            for (int j = i + 1; j < next_pos.size(); ++j)
            {
                coord_t left = next_pos.at(i);
                coord_t right = next_pos.at(j);
                if (left.col == right.col && left.row == right.row)
                {
                    next_pos.at(i) = positions.at(i);
                    next_pos.at(j) = positions.at(j);
                }
            }
        }
        positions = next_pos;
    }
}

int simulate_find_rounds(vector<coord_t> &positions)
{
    vector<coord_t> next_pos = positions;
    bool elf_moved = true;
    int itr = 0;
    while (elf_moved)
    {
        elf_moved = false;
        next_pos = positions;
        if (DEBUG)
        {
            print_positions(positions);
            cout << "\n\n";
        }
        for (int i = 0; i < positions.size(); ++i)
        {
            coord_t pos = positions.at(i);
            bool has_adjacent_elf = false;
            for (int row = -1; row < 2; ++row)
            {
                for (int col = -1; col < 2; ++col)
                {
                    if (row == 0 && col == 0)
                        continue; // We do not want to check our self
                    if (has_pos(positions, coord_t(pos.row + row, pos.col + col)))
                    {
                        has_adjacent_elf = true;
                        break;
                    }
                }
                if (has_adjacent_elf)
                    break;
            }
            
            if (!has_adjacent_elf)
                continue;
            bool moved = false;
            for (int c = 0; c < 4; ++c)
            {
                if (moved)
                    break;
                int cond = (c + itr) % 4;
                switch (cond)
                {
                    case 0: //NORTH
                        if (!has_pos(positions, coord_t(pos.row - 1, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row - 1, pos.col)) &&
                            !has_pos(positions, coord_t(pos.row - 1, pos.col + 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row - 1, pos.col);
                            moved = true;
                        }
                        break;
                    case 1: //SOUTH
                        if (!has_pos(positions, coord_t(pos.row + 1, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col + 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row + 1, pos.col);
                            moved = true;
                        }
                        break;
                    case 2: //WEST
                        if (!has_pos(positions, coord_t(pos.row - 1, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row, pos.col - 1)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col - 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row, pos.col - 1);
                            moved = true;
                        }
                        break;
                    case 3: //EAST
                        if (!has_pos(positions, coord_t(pos.row - 1, pos.col + 1)) &&
                            !has_pos(positions, coord_t(pos.row, pos.col + 1)) &&
                            !has_pos(positions, coord_t(pos.row + 1, pos.col + 1)))
                        {
                            next_pos.at(i) = coord_t(pos.row, pos.col + 1);
                            moved = true;
                        }
                        break;
                    default:
                        cout << "Bad, switch should identify all cases, got cond: " << cond << ", i: " << i << endl;
                        exit(123);
                }
            }
            if (moved)
                elf_moved = true;
        }
        
        //Check for duplicates in movement
        for (int i = 0; i < next_pos.size(); ++i)
        {
            for (int j = i + 1; j < next_pos.size(); ++j)
            {
                coord_t left = next_pos.at(i);
                coord_t right = next_pos.at(j);
                if (left.col == right.col && left.row == right.row)
                {
                    next_pos.at(i) = positions.at(i);
                    next_pos.at(j) = positions.at(j);
                }
            }
        }
        positions = next_pos;
        
        itr++;
    }
    
    return itr;
}

int get_empty_tiles(vector<coord_t> &positions)
{
    int min_col = positions.at(0).col,
        max_col = positions.at(0).col,
        min_row = positions.at(0).row,
        max_row = positions.at(0).row;
    
    for (auto pos : positions)
    {
        if (pos.col < min_col)
            min_col = pos.col;
        else if (pos.col > max_col)
            max_col = pos.col;
        
        if (pos.row < min_row)
            min_row = pos.row;
        else if (pos.row > max_row)
            max_row = pos.row;
    }
    
    int num_elfs = positions.size();
    
    int width = max_col - min_col + 1;
    int height = max_row - min_row + 1;
    
    return width * height - num_elfs;
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
    vector<coord_t> positions;
    int row = 0;
    while (!file.eof())
    {
        getline(file, temp);
        
        for (int col = 0; col < temp.length(); ++col)
            if (temp.at(col) == '#')
                positions.emplace_back(coord_t(row, col));
        ++row;
    }
    
    file.close();
    
    //Part 1
    //simulate(positions, ROUNDS);
    //cout << get_empty_tiles(positions) << endl;
    //Part 2
    cout << "Rounds: " << simulate_find_rounds(positions) << endl;
    print_positions(positions);
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
