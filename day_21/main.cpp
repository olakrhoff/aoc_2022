#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;

#define DEBUG 0

#define TASK_1 0

typedef enum
{
    PLUS,
    MINUS,
    MULTIPLICATION,
    DIVISION,
    EQUALITY
} OPERATOR_T;

typedef struct job
{
    string name;
    bool waiting;
    OPERATOR_T op;
    string name_left, name_right;
    int64_t value;
    
    job(string name, bool waiting, OPERATOR_T op, string nameLeft, string nameRight, int64_t value)
            : name(std::move(name)), waiting(waiting), op(op), name_left(std::move(nameLeft)), name_right(std::move(nameRight)), value(value)
    {}
    
} job_t;

job_t *parse_input(string &input)
{
    string name = input.substr(0, input.find(':'));
    input = input.substr(input.find(':') + 2);
    // We have a number
    if (input.find(' ') == -1)
    {
        int64_t val = stoi(input);
        return new job_t(name, false, PLUS, "", "", val);
    }
    string left_name = input.substr(0, input.find(' '));
    input = input.substr(input.find(' ') + 1);
    OPERATOR_T op;
    switch (*(const char *)input.substr(0, 1).data())
    {
        case '+':
            op = PLUS;
            break;
        case '-':
            op = MINUS;
            break;
        case '*':
            op = MULTIPLICATION;
            break;
        case '/':
            op = DIVISION;
            break;
        default:
            cout << "operator not classified" << endl;
            exit(12);
    }
    input = input.substr(2);
    string right_name = input;
    return new job_t(name, true, op, left_name, right_name, 0);
}

int64_t solve_number_recursive(vector<job_t *> &jobs, job_t *current)
{
    if (!current->waiting)
        return current->value;
    
    job_t *left = nullptr;
    job_t *right = nullptr;
    
    int found = 0;
    for (job_t *j : jobs)
    {
        if (found >= 2)
            break;
        if (j->name == current->name_left)
        {
            left = j;
            found++;
            continue;
        }
        if (j->name == current->name_right)
        {
            right = j;
            found++;
            continue;
        }
    }
    if (left == nullptr || right == nullptr)
    {
        cout << "Did not find the job in the list" << endl;
        exit(EXIT_FAILURE);
    }
    
    int64_t left_job = solve_number_recursive(jobs, left);
    int64_t right_job = solve_number_recursive(jobs, right);
    
    current->waiting = false;
    switch (current->op)
    {
        case PLUS:
            current->value = left_job + right_job;
            break;
        case MINUS:
            current->value = left_job - right_job;
            break;
        case MULTIPLICATION:
            current->value = left_job * right_job;
            break;
        case DIVISION:
            current->value = left_job / right_job;
            break;
    }
    return current->value;
}

int64_t solve_to_find_number(vector<job_t *> &jobs, job_t *current);

int64_t collapse_equations(vector<job_t *> &jobs, job_t *current)
{
    if (current->name == "humn")
        return current->value;
    
    job_t *left = nullptr;
    job_t *right = nullptr;
    
    int found = 0;
    for (job_t *j : jobs)
    {
        if (found >= 2)
            break;
        if (j->name == current->name_left)
        {
            left = j;
            found++;
            continue;
        }
        if (j->name == current->name_right)
        {
            right = j;
            found++;
            continue;
        }
    }
    if (left == nullptr || right == nullptr)
    {
        cout << "Did not find the job in the list" << endl;
        exit(EXIT_FAILURE);
    }
    
    int64_t left_job = solve_to_find_number(jobs, left);
    int64_t right_job = solve_to_find_number(jobs, right);
    
    if (left_job != INT64_MIN && right_job != INT64_MIN)
    {
        cout << "Neither side is a parent of 'humn'" << endl;
        exit(EXIT_FAILURE);
    }
    
    bool is_left = left_job == INT64_MIN;
    job_t *next_child = is_left ? left : right;
    job_t *done_child = is_left ? right : left;
    
    switch (current->op)
    {
        case PLUS:
            next_child->value = current->value - done_child->value;
            break;
        case MINUS:
            if (is_left)
                next_child->value = current->value + done_child->value;
            else
                next_child->value = done_child->value - current->value;
            break;
        case MULTIPLICATION:
            next_child->value = current->value / done_child->value;
            break;
        case DIVISION:
            if (is_left)
                next_child->value = current->value * done_child->value;
            else
                next_child->value = done_child->value / current->value;
            break;
        case EQUALITY:
            next_child->value = done_child->value;
            break;
    }
    
    return collapse_equations(jobs, next_child);
}

int64_t solve_to_find_number(vector<job_t *> &jobs, job_t *current)
{
    if (current->name == "humn")
        return INT64_MIN; // This needs to be unique
    if (!current->waiting)
        return current->value;
    
    job_t *left = nullptr;
    job_t *right = nullptr;
    
    int found = 0;
    for (job_t *j : jobs)
    {
        if (found >= 2)
            break;
        if (j->name == current->name_left)
        {
            left = j;
            found++;
            continue;
        }
        if (j->name == current->name_right)
        {
            right = j;
            found++;
            continue;
        }
    }
    if (left == nullptr || right == nullptr)
    {
        cout << "Did not find the job in the list" << endl;
        exit(EXIT_FAILURE);
    }
    
    int64_t left_job = solve_to_find_number(jobs, left);
    int64_t right_job = solve_to_find_number(jobs, right);
    
    // If we need the 'humn' number to solve, indicate it
    if (left_job == INT64_MIN || right_job == INT64_MIN)
    {
        if (current->name == "root")
            return collapse_equations(jobs, current);
        return INT64_MIN;
    }
    
    current->waiting = false;
    switch (current->op)
    {
        case PLUS:
            current->value = left_job + right_job;
            break;
        case MINUS:
            current->value = left_job - right_job;
            break;
        case MULTIPLICATION:
            current->value = left_job * right_job;
            break;
        case DIVISION:
            current->value = left_job / right_job;
            break;
    }
    return current->value;
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
    vector<job_t *> jobs;
    while (!file.eof())
    {
        getline(file, temp);
        
        jobs.emplace_back(parse_input(temp));
    }
    
    file.close();
    
    job_t *root = nullptr;
    for (job_t *j : jobs)
    {
        if (j->name == "root")
        {
            root = j;
            if (!TASK_1)
                root->op = EQUALITY;
            else
                break;
        }
    }
    if (root == nullptr)
    {
        cout << "Could not find start point" << endl;
        exit(EXIT_FAILURE);
    }
    if (TASK_1)
        cout << "Number: " << solve_number_recursive(jobs, root) << endl;
    else
        cout << "Number: " << solve_to_find_number(jobs, root) << endl;
    for (job_t *j : jobs)
        delete j;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    
    return EXIT_SUCCESS;
}
