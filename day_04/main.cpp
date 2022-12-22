#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

vector<string> split(string val, char criteria)
{
    vector<string> values;
    
    while (val.find(criteria) != -1)
    {
        string new_val = val.substr(0, val.find(criteria));
        if (new_val.length())
            values.emplace_back(new_val);
        val = val.substr(val.find(criteria) + 1, val.length());
    }
    values.emplace_back(val);
    
    return values;
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
    
    uint16_t count {};
    
    while (!file.eof())
    {
        getline(file, temp);
        auto sections = split(temp, ',');
        string first_section = sections[0];//temp.substr(0, temp.find(','));
        string second_section = sections[1];//temp.substr(temp.find(',') + 1, temp.length());
    
        auto section_values = split(first_section, '-');
        uint8_t fs_1 = stoi(section_values[0]), fs_2 = stoi(section_values[1]);
        
        section_values = split(second_section, '-');
        uint8_t ss_1 = stoi(section_values[0]), ss_2 = stoi(section_values[1]);
        
        //cout << (int16_t)fs_1 << "-" << (int16_t)fs_2 << "," << (int16_t)ss_1 << "-" << (int16_t)ss_2 << endl;
        
        if ((fs_1 <= ss_1 && fs_2 >= ss_2) || (fs_1 >= ss_1 && fs_2 <= ss_2))
            ++count;
        else if (!(fs_2 < ss_1 || ss_2 < fs_1)) //Task 2
            ++count;
    }
    
    file.close();
    
    cout << count << endl;
    
    return EXIT_SUCCESS;
}
