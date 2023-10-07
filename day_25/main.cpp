#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#define DEBUG 0

typedef string SNAFU_t;


bool validate(int decimal);

int64_t SNAFU_to_decimal(SNAFU_t snafu)
{
    int64_t decimal = 0;
    for (int i = 0; i < snafu.length(); ++i)
    {
        int64_t power = (int64_t)pow(5, snafu.length() - 1 - i);
        int64_t number = 0;
        if (snafu.at(i) == '-')
            number = -1;
        else if (snafu.at(i) == '=')
            number = -2;
        else
            number = snafu.at(i) - '0';
        if (number > 3)
            exit(13);
        decimal += power * number;
    }
    
    return decimal;
}
//4890_10 -> 124030_5
SNAFU_t decimal_to_SNAFU(int64_t decimal)
{
    SNAFU_t SNAFU = "";
    // First convert to base 5 for peace of mind
    while (decimal > 0)
    {
        int64_t power = 5;
        int64_t rest = decimal % power;
        int64_t quotient = decimal / power;
        switch (rest)
        {
            case 0:
                SNAFU += '0';
                break;
            case 1:
                SNAFU += '1';
                break;
            case 2:
                SNAFU += '2';
                break;
            case 3:
                SNAFU += '3';
                break;
            case 4:
                SNAFU += '4';
                break;
        }
        decimal = quotient;
    }
    std::reverse(SNAFU.begin(), SNAFU.end());
    
    for (int i = SNAFU.length() - 1; i >= 0; --i)
    {
        if (SNAFU.at(i) == '3')
        {
            SNAFU.at(i) = '=';
            if (i == 0)
            {
                SNAFU = "0" + SNAFU;
                i++;
            }
            SNAFU.at(i - 1) = SNAFU.at(i - 1) + 1;
        }
        else if (SNAFU.at(i) == '4')
        {
            SNAFU.at(i) = '-';
            if (i == 0)
            {
                SNAFU = "0" + SNAFU;
                i++;
            }
            SNAFU.at(i - 1) = SNAFU.at(i - 1) + 1;
        }
        else if (SNAFU.at(i) == '5')
        {
            SNAFU.at(i) = '0';
            if (i == 0)
            {
                SNAFU = "0" + SNAFU;
                i++;
            }
            SNAFU.at(i - 1) = SNAFU.at(i - 1) + 1;
            i++;
        }
        else if (SNAFU.at(i) == '6')
        {
            // Just to be extra sure
            exit(12);
        }
        // If it is anything else, we are good.
    }
    
    return SNAFU;
}

//123_10 -> 100=
//123_5 -> 38_10 -> 2==
/*
 * 1    -   1
 * 2    -   2
 * 1=   -   3
 * 1-   -   4
 * 10   -   5
 * 11   -   10
 * 12   -   11
 * 2=   -   12
 * 2-   -   13
 * 20   -   14
 * 21   -   15
 * 22   -   20
 *
 * 110  -   30
 * 120  -   35
 * 122  -   37
 * 2==  -   38
 */
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
    vector<SNAFU_t> SNAFU_numbers;
    while (!file.eof())
    {
        getline(file, temp);
        SNAFU_numbers.emplace_back(temp);
    }
    
    file.close();
    
    vector<int64_t> decimal_numbers;
    for (auto s : SNAFU_numbers)
        decimal_numbers.emplace_back(SNAFU_to_decimal(s));
        
    int64_t sum = 0;
    int64_t prev = 0;
    for (auto i : decimal_numbers)
    {
        prev = sum;
        sum += i;
        if (!SNAFU_to_decimal(decimal_to_SNAFU(i)) == i)
            exit(123);
        if (!SNAFU_to_decimal(decimal_to_SNAFU(sum)) == sum)
            exit(124);
    }
    
    cout << "SNAFU number: " << decimal_to_SNAFU(sum) << endl;
    cout << "DECIMAL: sum: " << sum << ", converted: " << SNAFU_to_decimal(decimal_to_SNAFU(sum)) << endl;
    
    //This is a quick test loop for validating the first 1000 integers
    for (int64_t i = 0; i < 1000; ++i)
    {
        if (SNAFU_to_decimal(decimal_to_SNAFU(i)) != i)
        {
            cout << i << ": " << decimal_to_SNAFU(i) << endl;
            break;
        }
    }
    
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
