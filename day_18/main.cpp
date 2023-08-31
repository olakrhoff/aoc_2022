#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

vector<string> split(const string& input, char delimiter)
{
    vector<string> result;
    
    string remainder = input;
    int idx;
    while ((idx = remainder.find(delimiter)) != string::npos)
    {
        string temp = remainder.substr(0, idx);
        result.emplace_back(temp);
        remainder = remainder.substr(idx + 1);
    }
    result.emplace_back(remainder);
    
    return result;
}

typedef struct pixel
{
    int x, y, z;
    
    bool operator==(const pixel &rhs) const
    {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z;
    }
    
    bool operator!=(const pixel &rhs) const
    {
        return !(rhs == *this);
    }
    
    bool operator<(const pixel &rhs) const
    {
        if (x >= rhs.x)
            return false;
        if (y >= rhs.y)
            return false;
        return z < rhs.z;
    }
    
    bool operator>(const pixel &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const pixel &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const pixel &rhs) const
    {
        return !(*this < rhs);
    }
} pixel_t;

bool is_aligned(pixel_t lhs, pixel_t rhs)
{
    int x, y, z;
    if ((x = abs(lhs.x - rhs.x)) > 1)
        return false;
    if ((y = abs(lhs.y - rhs.y)) > 1)
        return false;
    if ((z = abs(lhs.z - rhs.z)) > 1)
        return false;
    
   // Comparing a point to itself
    if (x + y + z == 1)
        return true;
    else
        return false;
    
}

uint64_t find_surface_area(vector<pixel_t> &pixels)
{
    uint64_t current_area = 0;
    vector<pixel_t> added_pixels;
    
    for (pixel_t p : pixels)
    {
        current_area += 6;
        for (pixel_t added : added_pixels)
            if (is_aligned(p, added))
                current_area -= 2;
        added_pixels.emplace_back(p);
    }
    
    return current_area;
}

vector<pixel_t> generate_neighbours(pixel_t current, pixel_t start, pixel_t end)
{
    start.x--;
    start.y--;
    start.z--;
    end.x++;
    end.y++;
    end.z++;
    
    vector<pixel_t> result;
    pixel_t temp = current;
    
    temp.x--;
    if (temp > start)
        result.emplace_back(temp);
    temp.x += 2;
    
    if (end > temp)
        result.emplace_back(temp);
    temp.x--;
    
    
    temp.y--;
    if (temp > start)
        result.emplace_back(temp);
    temp.y += 2;
    
    if (end > temp)
        result.emplace_back(temp);
    temp.y--;
    
    
    temp.z--;
    if (temp > start)
        result.emplace_back(temp);
    temp.z += 2;
    
    if (end > temp)
        result.emplace_back(temp);
    temp.z--;
    
    return result;
}

void fill_container(vector<pixel_t> &container, pixel_t start, pixel_t end)
{
    container.emplace_back(start);
    stack<pixel_t> added;
    added.push(start);
    
    while (!added.empty())
    {
        pixel_t current = added.top();
        added.pop();
        
        vector<pixel_t> possible_moves = generate_neighbours(current, start, end);
        for (pixel_t p : possible_moves)
        {
            bool already_added = false;
            for (pixel_t c : container)
            {
                if (p == c)
                {
                    already_added = true;
                    break;
                }
            }
            if (!already_added)
            {
                added.push(p);
                container.emplace_back(p);
            }
        }
    }
}

uint64_t find_surface_area_outside(vector<pixel_t> &pixels)
{
    uint64_t current_area = 0;
    vector<pixel_t> added_pixels;
    
    for (pixel_t p : pixels)
    {
        current_area += 6;
        for (pixel_t added : added_pixels)
            if (is_aligned(p, added))
                current_area -= 2;
        added_pixels.emplace_back(p);
    }
    
    // Simulate a container with the droplet in being filled with water, recursively
    // When the water cannot populate any more room, note the empty coordinates
    // if they align with the droplet, remove a surface area.
    
    int min_x = pixels.at(0).x,
        max_x = pixels.at(0).x,
        min_y = pixels.at(0).y,
        max_y = pixels.at(0).y,
        min_z = pixels.at(0).z,
        max_z = pixels.at(0).z;
    
    for (pixel_t p : pixels)
    {
        if (p.x < min_x)
            min_x = p.x;
        if (p.y < min_y)
            min_y = p.y;
        if (p.z < min_z)
            min_z = p.z;
        
        if (p.x > max_x)
            max_x = p.x;
        if (p.y > max_y)
            max_y = p.y;
        if (p.z > max_z)
            max_z = p.z;
    }
    
    min_x--;
    max_x++;
    min_y--;
    max_y++;
    min_z--;
    max_z++;
    
    
    vector<pixel_t> container = pixels;
    
    fill_container(container, {min_x, min_y, min_z}, {max_x, max_y, max_z});
    
    // Find all potential air pockets
    for (int row = min_x; row < max_x; ++row)
    {
        for (int col = min_y; col < max_y; ++col)
        {
            for (int height = min_z; height < max_z; ++height)
            {
                bool found = false;
                pixel_t temp = {row, col, height};
                for (pixel_t p : container)
                    if (temp == p)
                    {
                        found = true;
                        break;
                    }
                
                if (!found)
                {
                    for (pixel_t added : added_pixels)
                        if (is_aligned(temp, added))
                            --current_area;
                }
            }
        }
    }
    
    
    return current_area;
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
    
    vector<pixel_t> pixels;
    
    string temp {};
    
    while (!file.eof())
    {
        getline(file, temp);
        vector<string> numbers = split(temp, ',');
        pixel_t new_pixel = {stoi(numbers.at(0)), stoi(numbers.at(1)), stoi(numbers.at(2))};
        pixels.emplace_back(new_pixel);
    }
    
    file.close();
    
    
    cout << "Surface area is: " << find_surface_area(pixels) << endl;
    cout << "Surface area outside is: " << find_surface_area_outside(pixels) << endl;
    
    
    return EXIT_SUCCESS;
}
