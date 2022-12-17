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
    
    uint64_t total_score {};
    
    while (!file.eof())
    {
        //file >> temp;
        getline(file, temp);
        char op = temp[0], me = temp[2];
        
        uint8_t score {};
        //Task 1
        /*
        switch (op)
        {
            //Rock
            case *"A":
                if (me == *"X") //Rock - draw
                    score += (1 + 3);
                else if (me == *"Y") //Paper - win
                    score += (2 + 6);
                else //Scissors - loss
                    score += (3 + 0);
                break;
            //Paper
            case *"B":
                if (me == *"X") //Rock - loss
                    score += (1 + 0);
                else if (me == *"Y") //Paper - draw
                    score += (2 + 3);
                else //Scissors - win
                    score += (3 + 6);
                break;
            //Scissors
            case *"C":
                if (me == *"X") //Rock - win
                    score += (1 + 6);
                else if (me == *"Y") //Paper - loss
                    score += (2 + 0);
                else //Scissors - draw
                    score += (3 + 3);
                break;
            default:
                exit(EXIT_FAILURE);
        }
         */
    
        uint8_t temp {};
        switch (op)
        {
            case *"A": //Rock
                temp = 0;
                break;
            case *"B": //Paper
                temp = 1;
                break;
            case *"C": //Scissors
                temp = 2;
                break;
            default:
                exit(EXIT_FAILURE);
        }
        if (me == *"X") //lose
        {
            temp = (temp + 2) % 3;
            score += (temp + 1 + 0);
        }
        else if (me == *"Y") //draw
            score += (temp + 1  + 3);
        else //win
        {
            temp = (temp + 1) % 3;
            score += (temp + 1 + 6);
        }
        
        total_score += score;
    }
    file.close();
    
    
    cout << total_score << endl;
    
    return EXIT_SUCCESS;
}
