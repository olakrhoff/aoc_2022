#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

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
    
    vector<vector<uint8_t>> trees {};
    
    while (!file.eof())
    {
        getline(file, temp);
        vector<uint8_t> row {};
        
        for (char c : temp)
            row.emplace_back(c - '0');
        trees.emplace_back(row);
    }
    
    file.close();
    
    const int ROWS = trees.size();
    const int COLS = trees.at(0).size();
    
    uint32_t visible {};
    
    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            uint8_t current_tree = trees.at(row).at(col);
            uint8_t hidden {0};
            for (int left = 0; left < col; ++left)
                if (trees.at(row).at(left) >= current_tree)
                {
                    ++hidden;
                    break;
                }
            if (hidden != 1)
            {
                ++visible;
                continue;
            }
            
            for (int right = col + 1; right < COLS; ++right)
                if (trees.at(row).at(right) >= current_tree)
                {
                    ++hidden;
                    break;
                }
            if (hidden != 2)
            {
                ++visible;
                continue;
            }
            
            for (int top = 0; top < row; ++top)
                if (trees.at(top).at(col) >= current_tree)
                {
                    ++hidden;
                    break;
                }
            if (hidden != 3)
            {
                ++visible;
                continue;
            }
            
            
            for (int bottom = row + 1; bottom < ROWS; ++bottom)
                if (trees.at(bottom).at(col) >= current_tree)
                {
                    ++hidden;
                    break;
                }
            if (hidden != 4)
            {
                ++visible;
                continue;
            }
        }
    }
    
    
    cout << visible << endl;
    
    
    uint64_t best_viewing_distance {0};
    
    for (int row = 1; row < ROWS - 1; ++row)
    {
        for (int col = 1; col < COLS - 1; ++col)
        {
            uint8_t current_tree = trees.at(row).at(col);
            uint64_t viewing_distance {1};
            for (int left = 1; left <= col; ++left)
                if (trees.at(row).at(col - left) >= current_tree || left == col)
                {
                    viewing_distance *= left;
                    break;
                }
            
            if (viewing_distance == 0)
                continue;
            
            for (int right = 1; right <= COLS - col - 1; ++right)
                if (trees.at(row).at(col + right) >= current_tree || right == COLS - col - 1)
                {
                    viewing_distance *= right;
                    break;
                }
            
            if (viewing_distance == 0)
                continue;
            
            for (int top = 1; top <= row; ++top)
                if (trees.at(row - top).at(col) >= current_tree || top == row)
                {
                    viewing_distance *= top;
                    break;
                }
            
            if (viewing_distance == 0)
                continue;
            
            for (int bottom = 1; bottom <= ROWS - row - 1; ++bottom)
                if (trees.at(row + bottom).at(col) >= current_tree || bottom == ROWS - row - 1)
                {
                    viewing_distance *= bottom;
                    break;
                }
            
            if (viewing_distance > best_viewing_distance)
                best_viewing_distance = viewing_distance;
        }
    }
    
    cout << best_viewing_distance << endl;
    
    return EXIT_SUCCESS;
}
