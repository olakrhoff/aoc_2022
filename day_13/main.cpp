#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0


void strip(string &val)
{
    uint8_t count {};
    for (int i = 0; i < val.length(); ++i)
    {
        if (val.at(i) == '[')
            count++;
        else if (val.at(i) == ']')
            count--;
        else
            continue;
        if (count == 0)
        {
            string temp = val.substr(1, i - 1);
            string end = val.substr(i + 1);
            
            val = temp + end;
            break;
        }
    }
}

vector<string> tokenize(string input)
{
    strip(input);
    
    vector<string> tokens {};
    
    string temp {};
    uint8_t count {};
    for (char c : input)
    {
        if (c == ',' && count == 0)
        {
            tokens.emplace_back(temp);
            temp.clear();
            continue;
        }
        else if (c == '[')
            count++;
        else if (c == ']')
            count--;
        
        temp += c;
    }
    if (temp != "")
        tokens.emplace_back(temp);
    
    return tokens;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

/*
 *  1 - correct order
 *  0 - equal
 * -1 - incorrect order
 */
int compare_strings(string &left, string &right)
{
    vector<string> left_tokens = tokenize(left);
    vector<string> right_tokens = tokenize(right);
    
    for (int i = 0; i < left_tokens.size() && i < right_tokens.size(); ++i)
    {
        if (is_number(left_tokens.at(i)) && is_number(right_tokens.at(i)))
        {
            if (stoi(left_tokens.at(i)) < stoi(right_tokens.at(i)))
                return 1;
            else if (stoi(left_tokens.at(i)) > stoi(right_tokens.at(i)))
                return -1;
            continue;
        }
        else if (is_number(left_tokens.at(i)))
        {
            left_tokens.at(i) = "[" + left_tokens.at(i) + "]";
            int res = compare_strings(left_tokens.at(i), right_tokens.at(i));
            if (res != 0)
                return res;
            continue;
        }
        else if (is_number(right_tokens.at(i)))
        {
            right_tokens.at(i) = "[" + right_tokens.at(i) + "]";
            int res = compare_strings(left_tokens.at(i), right_tokens.at(i));
            if (res != 0)
                return res;
            continue;
        }
        else
        {
            int res = compare_strings(left_tokens.at(i), right_tokens.at(i));
            if (res != 0)
                return res;
            continue;
        }
    }
    
    if (left_tokens.size() > right_tokens.size())
        return -1;
    else if (left_tokens.size() < right_tokens.size())
        return 1;
    return 0;
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
    
    vector<pair<string, string>> pairs {};
    
    while (!file.eof())
    {
        getline(file, temp);
        string left = temp;
        getline(file, temp);
        string right = temp;
        
        pairs.emplace_back(make_pair(left, right));
        
        getline(file, temp);
    }
    
    file.close();
    
    vector<string> packets {}; //Task 2
    
    uint64_t sum {};
    for (int i = 0; i < pairs.size(); ++i)
    {
        string left = pairs.at(i).first;
        string right = pairs.at(i).second;
        packets.emplace_back(left); //Task 2
        packets.emplace_back(right); //Task 2
        if (compare_strings(left, right) == 1)
            sum += i + 1;
    }
    
    
    cout << sum << endl;
    
    
    //Task 2
    
    packets.emplace_back("[[2]]");
    packets.emplace_back("[[6]]");
    
    std::sort(packets.begin(), packets.end(), [](string a, string b)
    {
        return compare_strings(a, b) == 1;
    });
    
    
    uint64_t answer {1};
    
    for (int i = 0; i < packets.size(); ++i)
        if (packets.at(i) == "[[2]]" || packets.at(i) == "[[6]]")
            answer *= i + 1;
        
    cout << answer << endl;
    
    return EXIT_SUCCESS;
}
