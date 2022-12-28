#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <stack>

using namespace std;

#define DEBUG 0


typedef struct file_data
{
    string name {};
    uint64_t size {};
    
    file_data(string name, uint64_t size) : name(std::move(name)), size(size)
    {}
} file_t;

struct directory
{
    string name {};
    directory* parent;
    vector<directory*> sub_dirs {};
    vector<file_t> files {};
    uint64_t size {};
    bool size_valid {false};
    
    directory(string a_name, directory* a_parent) : name(std::move(a_name)), parent(a_parent)
    {}
    
    uint64_t get_size()
    {
        if (!size_valid)
        {
            uint64_t sum {};
            for (file_t f : files)
                sum += f.size;
            for (directory* d : sub_dirs)
                sum += d->get_size();
            size = sum;
        }
        
        return size;
    }
};

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
    
    directory *top_dir = new directory("/", nullptr);
    top_dir->parent = top_dir;
    
    directory *current_dir = top_dir;
    
    while (!file.eof())
    {
        getline(file, temp);
        
        if (temp[0] == '$') //We have a command
        {
            string command = temp.substr(2, 2);
    
            if (command == "cd")
            {
                string dir_name = temp.substr(5);
                if (dir_name == "..")
                {
                    current_dir = current_dir->parent;
                    continue;
                }
                else if (dir_name == "/")
                {
                    current_dir = top_dir;
                    continue;
                }
                
                bool found = false;
                for (directory *dir : current_dir->sub_dirs)
                {
                    if (dir->name == dir_name)
                    {
                        current_dir = dir;
                        found = true;
                    }
                }
                
                if (!found)
                    exit(EXIT_FAILURE);
            }
            else if (command == "ls")
            {
                continue;
            }
            else
            {
                exit(EXIT_FAILURE);
            }
        }
        else if (temp.substr(0, 3) == "dir") //We have a directory
        {
            string dir_name = temp.substr(4);
            current_dir->sub_dirs.emplace_back(new directory(dir_name, current_dir));
        }
        else //We have a file
        {
            string file_name = temp.substr(temp.find(' ') + 1);
            uint64_t file_size = stoi(temp.substr(0, temp.find(' ')));
            current_dir->files.emplace_back(file_t(file_name, file_size));
        }
    }
    
    file.close();
    
    
    stack<directory> dirs {};
    dirs.push(*top_dir);
    
    uint64_t sum {};
    while (!dirs.empty())
    {
        directory current = dirs.top();
        dirs.pop();
        
        for (directory* d : current.sub_dirs)
            dirs.push(*d);
        
        if (current.get_size() <= 100000)
            sum += current.get_size();
    }
    
    cout << sum << endl;
    
    
    //Task 2
    #define TOTAL_SIZE 70000000
    #define NEEDED_SIZE 30000000
    
    uint64_t needs_to_be_deleted = NEEDED_SIZE - (TOTAL_SIZE - top_dir->get_size());
    
    vector<directory> options;
    
    dirs.push(*top_dir);
    
    while (!dirs.empty())
    {
        directory current = dirs.top();
        dirs.pop();
        
        for (directory* d : current.sub_dirs)
            dirs.push(*d);
        
        if (current.get_size() >= needs_to_be_deleted)
            options.emplace_back(current);
    }
    
    std::sort(options.begin(), options.end(), [](directory a, directory b)
    {
       return a.get_size() >= b.get_size();
    });
    
    cout << options.back().get_size() << endl;
    
    return EXIT_SUCCESS;
}
