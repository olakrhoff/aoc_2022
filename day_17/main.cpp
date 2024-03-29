#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

#define DEBUG 0


#define NUM_ROCKS_1 2022 // Part 1
#define NUM_ROCKS_2 1000000000000 // Part 2
#define NUM_UNIQUE_ROCKS 5

typedef enum
{
    LEFT = 0,
    RIGHT = 1
} DIRECTION_T;

static uint64_t STREAM_LENGTH;

/**
 * Sequence of bits representing either left (0) or right (1)
 * stream.
 */
typedef struct stream
{
    vector<uint64_t> buffer {};
    uint8_t index {65};
    uint64_t byte_index {};
    uint8_t bit_index {};
    
    void push(DIRECTION_T direction)
    {
        if (index >= 64)
        {
            index = 0;
            buffer.emplace_back(0);
        }
        if (direction == RIGHT)
        {
            uint64_t bit_mask = (uint64_t)1 << (63 - index);
            buffer.back() |= bit_mask;
        }
        index++;
    }
    
    DIRECTION_T next_stream()
    {
        uint64_t bit_mask = (uint64_t)1 << (63 - bit_index);
        DIRECTION_T direction = (buffer.at(byte_index) & bit_mask) ? RIGHT : LEFT;
        
        if (byte_index < buffer.size() - 1) // If we are not in the last byte, we can safely increment (no chance of overflow)
        {
            bit_index++;
            if (bit_index >= 64)
            {
                byte_index++;
                bit_index = 0;
            }
        }
        else
        {
            bit_index++;
            if (bit_index >= index)
            {
                byte_index = 0;
                bit_index = 0;
            }
        }
        
        return direction;
    }

    uint64_t get_index() const
    {
        return byte_index * 8 + bit_index;
    }
} stream_t;

typedef struct rock
{
    vector<uint8_t> stack;
    
    explicit rock(const vector<uint8_t> &stack) : stack(stack)
    {}
    
    bool can_move_rock(DIRECTION_T direction) const
    {
        if (direction == LEFT)
        {
            for (auto l : stack)
                if (l & 0b01000000)
                    return false;
        }
        else // RIGHT
        {
            for (auto l : stack)
                if (l & 0b00000001)
                    return false;
        }
        
        return true;
    }
    
    void move(DIRECTION_T direction)
    {
        if (!can_move_rock(direction))
            exit(EXIT_FAILURE); // Safeguard
            
        for (auto &l : stack)
            if (direction == LEFT)
                l = l << 1;
            else
                l = l >> 1;
    }
} rock_t;

rock_t ROCKS[NUM_UNIQUE_ROCKS] =
        {
                rock_t({0b00011110}),
                rock_t({0b00001000,
                        0b00011100,
                        0b00001000}),
                rock_t({0b00000100,
                        0b00000100,
                        0b00011100}),
                rock_t({0b00010000,
                        0b00010000,
                        0b00010000,
                        0b00010000}),
                rock_t({0b00011000,
                        0b00011000})
        };

typedef struct chamber
{
    vector<uint8_t> stack {}; // Init the base, ignore the left-most bit
    rock_t current_rock {{}};
    uint64_t rock_height {}; // The index in stack where the bottom of the rock starts
    uint64_t rocks_added {};
    bool is_current_rock_floating {true};
    //std::map<std::tuple<uint64_t, uint64_t, vector<uint8_t>>, std::tuple<uint64_t, uint64_t, uint64_t>> seen;

    void add_rock(rock_t rock)
    {
        rocks_added++;
        is_current_rock_floating = true;
        for (int i = 0; i < 3; ++i)
            stack.emplace_back(0x00);
        rock_height = stack.size();
        current_rock.stack = {};
        for (int i = rock.stack.size() - 1; i >= 0; --i)
        {
            stack.emplace_back(rock.stack.at(i));
            current_rock.stack.emplace_back(rock.stack.at(i));
        }
    }
    
    bool has_floating_rock() const
    {
        return is_current_rock_floating;
    }
    
    void clean_chamber()
    {
        int index = -1;
        for (int i = 0; i < stack.size(); ++i)
        {
            if (stack.at(i) == 0xEF)
            {
                index = i;
                break;
            }
        }
        // Did not find a line to remove everything below
        if (index == -1)
            return;
        stack = {stack.begin() + index + 1, stack.end()};
        rock_height -= index + 1;
        //clean_chamber();
    }
    
    bool can_rock_move_down() {
        if (rock_height == 0)
        {
            is_current_rock_floating = false;
            return false; // It is already at the bottom
        }
        for (int i = rock_height; i < rock_height + current_rock.stack.size(); ++i)
        {
            uint8_t filter_out_rock {};
            if (i != rock_height)
                filter_out_rock = current_rock.stack.at(i - rock_height - 1);
            if ((stack.at(i - 1) ^ filter_out_rock) & current_rock.stack.at(i - rock_height))
            {
                is_current_rock_floating = false;
                //clean_chamber(); // Was not as effective as I predicted
                return false;
            }
        }
        
        return true;
    }
    
    void apply_gas(DIRECTION_T direction)
    {
        // If we can't move the rock there is no point. Here we check if it hits the walls
        if (!current_rock.can_move_rock(direction))
            return;
        
        // A slice of the chamber that contains the rock
        vector<uint8_t> cut_out = {stack.begin() + rock_height, stack.begin() + rock_height + current_rock.stack.size()};
        // Begin by removing the rock
        for (int i = 0; i < current_rock.stack.size(); ++i)
            cut_out.at(i) = cut_out.at(i) ^ current_rock.stack.at(i);
        
        current_rock.move(direction);
        for (int i = 0; i < current_rock.stack.size(); ++i)
            if (cut_out.at(i) & current_rock.stack.at(i))
            {
                // We have an overlap, redo the move and exit, move not valid
                if (direction == LEFT)
                    current_rock.move(RIGHT);
                else
                    current_rock.move(LEFT);
                return;
            }
        
        // Add the rock back in
        for (int i = rock_height; i < rock_height + current_rock.stack.size(); ++i)
            stack.at(i) = cut_out.at(i - rock_height) | current_rock.stack.at(i - rock_height);
    }
    
    void apply_falling()
    {
        if (!can_rock_move_down())
            return;
        for (int i = rock_height; i < rock_height + current_rock.stack.size(); ++i)
        {
            // Remove rock layer
            stack.at(i) ^= current_rock.stack.at(i - rock_height);
            // Add rock layer one step down
            stack.at(i - 1) = stack.at(i - 1) | current_rock.stack.at(i - rock_height);
        }
        rock_height--;
        // Remove all the empty layers at the top
        while (stack.back() == 0)
            stack.pop_back();
    }
    
    uint64_t highest_rock()
    {
        return stack.size();
    }
    
    vector<uint8_t> get_current_rock_slice() const
    {
        return {stack.begin() + rock_height, stack.begin() + rock_height + current_rock.stack.size()};
    }
    
    void print()
    {
        cout << endl;
        for (int line = stack.size() - 1; line > 0; --line)
        {
            uint8_t l = stack.at(line);
            cout << "|";
            for (int i = 0; i < 7; ++i)
            {
                if (l & (1 << (6 - i)))
                {
                    if (line >= rock_height && line < rock_height + current_rock.stack.size()
                        && ((l & (1 << (6 - i))) & current_rock.stack.at(line - rock_height)))
                        cout << "@";
                    else
                        cout << "#";
                }
                else
                {
                    cout << ".";
                }
            }
            cout << "|";
            cout << endl;
        }
        cout << "+-------+" << endl;
    }
    
} chamber_t;

uint64_t simulate(stream_t &stream)
{
    chamber_t chamber;
    // Loop over the number of rocks to fall
    for (int i = 0; i < NUM_ROCKS_1; ++i)
    {
        rock_t current_rock = ROCKS[i % NUM_UNIQUE_ROCKS];
        chamber.add_rock(current_rock);
        //chamber.print();
        while (chamber.has_floating_rock())
        {
            chamber.apply_gas(stream.next_stream());
            //chamber.print();
            chamber.apply_falling();
            //chamber.print();
        }
    }
    
    return chamber.highest_rock() + 1;
}

uint64_t simulate2(stream_t &stream)
{
    chamber_t chamber;
    vector<vector<uint8_t>> starts;
    vector<uint64_t> res;
    vector<uint64_t> rocks_used;
    
    vector<pair<vector<uint8_t>, uint64_t>> prevs;
    vector<chamber_t> prev_chambers;
    uint64_t height_by_reps = 0;
    uint8_t rep = 0;
    bool found = false;
    // Loop over the number of rocks to fall
    for (uint64_t i = 0; i < NUM_ROCKS_2; ++i)
    {
        if (i % (NUM_UNIQUE_ROCKS * STREAM_LENGTH) == 0 && i != 0 && !found)
        {
            auto temp = chamber.get_current_rock_slice();
            if (!prevs.empty())
            {
                if (temp == prevs.at(0).first)
                {
                    rep++;
                    if (rep >= 2)
                    {
                        int start;
                        bool pattern;
                        // Find all instances of potential reps
                        for (start = 1; start < prevs.size(); ++start)
                        {
                            if (prevs.at(start).first == prevs.at(0).first && start * 2 == prevs.size())
                            {
                                pattern = true;
                                // Loop through and check if there is a perfect pattern
                                for (int idx = 0; idx < start; ++idx)
                                {
                                    if (prevs.at(idx).first != prevs.at(idx + start).first)
                                    {
                                        pattern = false;
                                        break;
                                    }
                                }
                                
                                if (pattern)
                                    break;
                            }
                        }
                        if (pattern)
                        {
                            uint64_t current_height = chamber.highest_rock();
                            uint64_t height_per_rep = current_height - prevs.at(0).second;
                            uint64_t rocks_left = NUM_ROCKS_2 - (i - 1);
                            uint64_t rocks_per_rep = NUM_UNIQUE_ROCKS * STREAM_LENGTH * prevs.size();
                            uint64_t reps = (rocks_left / rocks_per_rep) + 1;
                            height_by_reps = reps * height_per_rep;
    
                            uint64_t rocks_to_iterate = NUM_ROCKS_2 - (rocks_left % rocks_per_rep);
                            i = rocks_to_iterate;
    
                            prevs.emplace_back(temp, chamber.highest_rock());
                            prev_chambers.emplace_back(chamber);
                            found = true;
                            continue;
                        }
                    }
                }
            }
            prevs.emplace_back(temp, chamber.highest_rock());
            prev_chambers.emplace_back(chamber);
        }
        
        rock_t current_rock = ROCKS[i % NUM_UNIQUE_ROCKS];
        chamber.add_rock(current_rock);
        //chamber.print();
        while (chamber.has_floating_rock())
        {
            chamber.apply_gas(stream.next_stream());
            //chamber.print();
            chamber.apply_falling();
            //chamber.print();
        }
    }
    
    uint64_t start = prevs.at(0).second;
    uint64_t end = chamber.highest_rock() - prevs.back().second;
    uint64_t height = start + height_by_reps + end;
    return height;
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
    
    stream_t stream;
    while (!file.eof())
    {
        getline(file, temp);
        STREAM_LENGTH = temp.size();
        for (const char c : temp)
        {
            if (c == '<')
                stream.push(LEFT);
            else
                stream.push(RIGHT);
        }
    }
    
    file.close();
    

    //cout << "Height: " << simulate(stream) << endl;
    uint64_t ans = simulate2(stream);
    cout << "Height: " << ans << endl;
    cout << "Diff: ";
    if (DEBUG)
        cout << min(1514285714288 - ans, ans - 1514285714288) << endl;
    else
        cout << min(1564705882327 - ans, ans - 1564705882327) << (ans > 1564705882327 ? " Big" : " Small") << endl;
    
    
    
    return EXIT_SUCCESS;
}
