#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <functional>

using namespace std;

#define DEBUG 0

typedef uint64_t item_t;

typedef struct monkey
{
    queue<item_t> items {};
    function<item_t(item_t)> operation {nullptr};
    uint64_t divisor_test {}, inspected {};
    uint32_t to_true {}, to_false {};
    
    monkey(const queue<item_t> &items, const function<item_t(item_t)> &operation,
           uint64_t divisorTest, uint32_t toTrue, uint32_t toFalse) : items(items), operation(operation),
                                                                      divisor_test(divisorTest), to_true(toTrue),
                                                                      to_false(toFalse)
    {}
    
} monkey_t;

vector<string> split(const string val, const string delimiter)
{
    vector<string> values {};
    string temp = val;
    
    while (temp.find(delimiter) != -1)
    {
        values.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    
    values.emplace_back(temp);
    
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
    
    vector<monkey_t> monkeys {};
    queue<item_t> items {};
    function<item_t(item_t)> operation {};
    uint64_t divisor_test {};
    uint32_t to_true {}, to_false {};
    
    while (!file.eof())
    {
        getline(file, temp);
        
        items = {};
        divisor_test = 0;
        operation = nullptr;
    
    
        getline(file, temp);
        vector<string> temp_items = split(temp.substr(temp.find("Starting items: ") + 16), ", ");
        
        for (string s : temp_items)
            items.push(stoi(s));
    
        getline(file, temp);
    
        string number = temp.substr(25);
        if (number == "old")
        {
            if (temp[23] == '*')
            {
                operation = [](item_t a) -> item_t
                {
                    return a * a;
                };
            }
            else
            {
                operation = [](item_t a) -> item_t
                {
                    return a + a;
                };
            }
        }
        else
        {
            uint64_t operation_value = stoi(number);
            if (temp[23] == '*')
            {
                operation = [operation_value](item_t a) -> item_t
                {
                    return a * operation_value;
                };
            }
            else
            {
                operation = [operation_value](item_t a) -> item_t
                {
                    return a + operation_value;
                };
            }
        }
        
        getline(file, temp);
        divisor_test = stoi(temp.substr(21));
        
        getline(file, temp);
        to_true = stoi(temp.substr(29));
        
        getline(file, temp);
        to_false = stoi(temp.substr(30));
        
        monkeys.emplace_back(monkey_t(items, operation, divisor_test, to_true, to_false));
        getline(file, temp);
    }
    
    file.close();
    
    //Task 1
    //#define ROUNDS 20
    //Task 2
    #define ROUNDS 10000
    
    uint64_t overall_divisor {1};
    for (monkey_t m : monkeys)
        overall_divisor *= m.divisor_test;
    
    for (int i = 0; i < ROUNDS; ++i)
    {
        for (int m = 0; m < monkeys.size(); ++m)
        {
            while (!monkeys.at(m).items.empty())
            {
                monkeys.at(m).inspected++;
                
                item_t item = monkeys.at(m).items.front();
                monkeys.at(m).items.pop();
                
                item = monkeys.at(m).operation(item);
                //Task 1
                //item /= 3;
                //Task 2
                item %= overall_divisor;
                
                if ((item % monkeys.at(m).divisor_test) == 0)
                {
                    monkeys.at(monkeys.at(m).to_true).items.push(item);
                }
                else
                {
                    monkeys.at(monkeys.at(m).to_false).items.push(item);
                }
            }
        }
    }
    
    std::sort(monkeys.begin(), monkeys.end(), [](monkey_t a, monkey_t b)
    {
        return a.inspected >= b.inspected;
    });
    
    cout << monkeys.at(0).inspected * monkeys.at(1).inspected  << endl;
    
    
    return EXIT_SUCCESS;
}
