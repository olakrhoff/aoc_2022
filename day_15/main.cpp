#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0

typedef struct coord
{
    int64_t x, y;
    
    coord() = default;
    
    coord(int64_t x, int64_t y) : x(x), y(y)
    {}
    
    bool operator==(const coord &rhs) const
    {
        return x == rhs.x &&
               y == rhs.y;
    }
    
    bool operator!=(const coord &rhs) const
    {
        return !(rhs == *this);
    }
    
    bool operator<(const coord &rhs) const
    {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return y < rhs.y;
    }
    
    bool operator>(const coord &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const coord &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const coord &rhs) const
    {
        return !(*this < rhs);
    }
    
} coord_t;

typedef struct sensor : public coord_t
{
    coord_t beacon {};
    
    sensor(int64_t x, int64_t y, const coord_t &beacon) : coord(x, y), beacon(beacon)
    {}
    
    inline int64_t hamming_distance() const
    {
        return abs(x - beacon.x) + abs(y - beacon.y);
    }
    
    bool contains(const coord_t &point)
    {
        return (int64_t)(abs(x - point.x) + abs(y - point.y)) <= this->hamming_distance();
    }
    
} sensor_t;

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
    
    vector<sensor_t> sensors {};
    set<coord_t> beacons {};
    
    while (!file.eof())
    {
        getline(file, temp);
    
        string sensor_line = temp.substr(0, temp.find(':'));
        int64_t sensor_x, sensor_y, beacon_x, beacon_y;
        
        sensor_x = stoi(sensor_line.substr(sensor_line.find("x=") + 2, sensor_line.find(',')));
        sensor_y = stoi(sensor_line.substr(sensor_line.find("y=") + 2));
        
        string beacon_line = temp.substr(temp.find(':'));
        
        beacon_x = stoi(beacon_line.substr(beacon_line.find("x=") + 2, beacon_line.find(',')));
        beacon_y = stoi(beacon_line.substr(beacon_line.find("y=") + 2));
        
        sensors.emplace_back(sensor_t(sensor_x, sensor_y, coord_t(beacon_x, beacon_y)));
        
        beacons.insert(coord_t(beacon_x, beacon_y));
    }
    
    file.close();
    
    
    int64_t min_x, max_x;
    sensor_t min_sensor = *std::min_element(sensors.begin(), sensors.end(), [](sensor_t a, sensor_t b)
    {
        return (a.x - a.hamming_distance()) < (b.x - b.hamming_distance());
    });
    sensor_t max_sensor = *std::max_element(sensors.begin(), sensors.end(), [](sensor_t a, sensor_t b)
    {
        return (a.x + a.hamming_distance()) < (b.x + b.hamming_distance());
    });
    
    min_x = min_sensor.x - min_sensor.hamming_distance() - 1;
    max_x = max_sensor.x + max_sensor.hamming_distance() + 1;
    
    #define Y 2000000
    uint64_t count {};
    //Task 1
    /*
    for (int64_t i = min_x; i <= max_x; ++i)
    {
        bool done = false;
        for (coord_t b : beacons)
        {
            if (b.x == i && b.y == Y)
            {
                done = true;
                break;
            }
        }
        if (done)
            continue;
        for (sensor_t s : sensors)
        {
            if (s.contains(coord_t(i, Y)))
            {
                count++;
                break;
            }
        }
    }
    
    cout << count << endl;
    */
    
    #define X_LIMIT 4000000
    #define Y_LIMIT 4000000
    //>108000000
    for (int64_t y = 0; y <= Y_LIMIT; ++y)
    {
        for (int64_t x = 0; x <= X_LIMIT; ++x)
        {
            bool done = false;
            for (coord_t b : beacons)
            {
                if (b.x == x && b.y == y)
                {
                    done = true;
                    break;
                }
            }
            if (done)
                continue;
            bool found = true;
            for (sensor_t s : sensors)
            {
                if (s.contains(coord_t(x, y)))
                {
                    x += ((s.hamming_distance() - abs(s.y - y)) - abs(s.x - x));
                    
                    found = false;
                    break;
                }
            }
            if (found)
            {
                cout << x << ", " << y << endl;
                cout << x * 4000000 + y << endl;
                return EXIT_SUCCESS;
            }
        }
        //cout << (double)y / Y_LIMIT * 100 << "%" << endl;
    }
    
    return EXIT_SUCCESS;
}
