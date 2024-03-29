#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>
#include <cmath>
#include <chrono>
#include <map>
#include <unordered_map>

using namespace std;

//To run this file in VS Code
//Open Developer console
//Type code -> enter

//Struct variables are public by default instead of private
struct Sensor 
{
    int x, y, cost, coverage; //coverage: the number of other sensors within the current circle
    double weight;

    Sensor(int x, int y, int cost): x(x), y(y), cost(cost), coverage(0) {} // Short for this.x = x this.y = y this.cost=cost   
};

long long nanos = chrono::steady_clock::now().time_since_epoch().count();
minstd_rand randomGenerator(nanos);

int randint(int a, int b) 
{
    uniform_int_distribution<int> distribution(a, b);
    return distribution(randomGenerator);
}

double randfloat(double a, double b) 
{
    uniform_real_distribution<double> distribution(a, b);
    return distribution(randomGenerator);
}

double randnormal(double mean, double deviation) {
    normal_distribution<double> distribution(mean, deviation);
    return distribution(randomGenerator);
}

vector<Sensor> sensors;
const int NUM_POINTS = 40;
int R = 20;
int budget = 10000;

vector<int> chooseSensorsRandomly() 
{    
    int totalCost = 0;
    set<int> chosen;
    int brokeCount = 0;

    while(chosen.size() < sensors.size() && brokeCount < 40)
    {
        int index = randint(0, sensors.size() - 1);
        while (chosen.count(index) != 0) 
        {
            index = randint(0, NUM_POINTS - 1);
        }

        if((totalCost + sensors[index].cost) > budget)
        {
            brokeCount++;
            continue;
        }        
        
        chosen.insert(index);
        totalCost += sensors[index].cost;
    }
    return vector<int>(chosen.begin(), chosen.end());
}

vector<int> greedyAlgorithm()
{
    vector<Sensor> greedySort(sensors);
    vector<int32_t> chosen;
    int totalCost = 0;
    //Check greedySort.size first to prevent out of bounds error
    for (int i = 0; i < greedySort.size() && (totalCost + greedySort[i].cost) <= budget; ++i)
    {
        totalCost += sensors[i].cost;
        chosen.push_back(i);
    }
    return chosen;
}

double calculateDistance(Sensor s1, Sensor s2)
{
    return sqrt( pow(s2.x - s1.x, 2) + pow(s2.y - s1.y, 2) );
}

double calculateDistance(pair<int, int> p1, pair<int, int> p2)
{
    return sqrt( pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2) );
}

vector<int> returnCoveredSensors(int index)
{
    vector<int> covered;    
    for(int j = 0; j < sensors.size(); j++)
    {
        //Don't count coverage for the sensor itself            
        if(index != j && calculateDistance(sensors[index], sensors[j]) < R) 
        {                                                            
            covered.push_back(j);                                                 
        }
    }        
    return covered;
}

int countBits(int x) {
    /*int count = 0;
    while (x > 0) {
        count += x & 1;
        x >>= 1;
    }
    return count;*/
    return __builtin_popcount(x);
}

namespace std
{
    template<typename... TTypes>
    class hash<std::tuple<TTypes...>>
    {
    private:
        typedef std::tuple<TTypes...> Tuple;

        template<int N>
        size_t operator()(Tuple value) const { return 0; }

        template<int N, typename THead, typename... TTail>
        size_t operator()(Tuple value) const
        {
            constexpr int Index = N - sizeof...(TTail) - 1;
            return hash<THead>()(std::get<Index>(value)) ^ operator()<N, TTail...>(value);
        }

    public:
        size_t operator()(Tuple value) const
        {
            return operator()<sizeof...(TTypes), TTypes...>(value);
        }
    };
}

//{{budget, j}: {items}}
unordered_map<tuple<int, int, int>, int> cache;

int recursiveSolve(int budget, int j, int mask, vector<int> &sets, vector<int> &costs, vector<int> &cumulativeSet) {
    if (j == 0 || budget <= 0) {
        return 0;
    }
    
    mask &= cumulativeSet[j];
    if (cache.count({budget, j, mask}) > 0) {
        return cache[{budget, j, mask}];
    }
    
    int ans = recursiveSolve(budget, j - 1, mask, sets, costs, cumulativeSet);
    if (mask | sets[j-1] != mask) {
        int ans2 = recursiveSolve(budget - 1, j, mask, sets, costs, cumulativeSet);
        if (countBits(ans2 | mask) > countBits(ans | mask)) {
            ans = ans2;
        }

        if (budget >= costs[j-1]) {
            int ans3 = recursiveSolve(budget - costs[j-1], j - 1, mask | sets[j-1], sets, costs, cumulativeSet) | sets[j-1];
            if (countBits(ans3 | mask) > countBits(ans | mask)) {
                ans = ans3;
            }
        }
    }
    cache[{budget, j, mask}] = ans;
    return ans;
}

vector<int> dynamicAlgorithm() {
    vector<int> sets(sensors.size());
    for (int i = 0; i < sensors.size(); i++) {
        int s = 0;
        vector<int> touching = returnCoveredSensors(i);
        touching.push_back(i);
        for (int j : touching) {
            s |= (1 << j);
        }

        sets[i] = s;
    }

    vector<int> cumulativeSets(sensors.size() + 1);
    cumulativeSets[0] = 0;
    for (int i = 0; i < sensors.size(); i++) {
        cumulativeSets[i+1] |= cumulativeSets[i] | sets[i];
    }

    vector<int> costs;
    for (Sensor &s : sensors) {
        costs.push_back(s.cost);
    }

    int ans = recursiveSolve(budget, sensors.size(), 0, sets, costs, cumulativeSets);
    vector<int> chosen;
    for (int i = 0; i < sensors.size(); i++) {
        if ((ans & (1 << i)) > 0) {
            chosen.push_back(i);
        }
    }

    return chosen;
}

void calculateCoverage(set<int> covered)
{
    for (Sensor &s : sensors) {
        s.coverage = 0;
    }
    for(int i = 0; i < sensors.size(); i++)
    {
        if (covered.count(i) == 0) {
            sensors[i].coverage++;
        }
        for(int j = 0; j < sensors.size(); j++)
        {
            if (covered.count(j) == 1) {
                continue;
            }
            //Don't count coverage for the sensor itself            
            if(i != j && calculateDistance(sensors[i], sensors[j]) < R) 
            {
                sensors[i].coverage++;
            }
        }
    }    
}

void calculateCoverage() {
    set<int> covered;
    calculateCoverage(covered);
}

/*
Formula: 
give weight to each sensor (coverage/cost placeholder)
sort sensors based on weight
function picks highest weight first, then goes down
skip if under budget

what if sensors already covered? create bool covered[]
*/

void assignWeight()
{
    for(int i = 0; i < sensors.size(); i++)
    {
        sensors[i].weight = (double)sensors[i].coverage/sensors[i].cost;
    }
}

vector<Sensor> sortSensorsByWeight(vector<Sensor> s)
{
    //copy sensors array to new array for sorting
    vector<Sensor> sortedSensors(s); 

    //selection sort
    int i, j, max_idx; 
    for(int i = 0; i < sortedSensors.size()-1; i++)
    {
        max_idx = i; 
        for (j = i+1; j < sortedSensors.size(); j++)
        {
          if (sortedSensors[j].weight > sortedSensors[max_idx].weight) 
              max_idx = j;
        }
        // Swap the found maximum element 
        // with the first element 
        if (max_idx!=i)
        {        
            swap(sortedSensors[max_idx], sortedSensors[i]);                    
        }
    }
    return sortedSensors;
}

vector<Sensor> sortSensors(vector<Sensor> s)
{
    //copy sensors array to new array for sorting
    vector<Sensor> sortedSensors(s); 

    //selection sort
    int i, j, min_idx; 
    for(int i = 0; i < sortedSensors.size()-1; i++)
    {
        min_idx = i; 
        for (j = i+1; j < sortedSensors.size(); j++)
        {
          if (sortedSensors[j].cost < sortedSensors[min_idx].cost) 
              min_idx = j;
        }
        // Swap the found minimum element 
        // with the first element 
        if (min_idx!=i)
        {        
            swap(sortedSensors[min_idx], sortedSensors[i]);                    
        }
    }
    return sortedSensors;
}



int randomCost(bool project2Mode) 
{
    return project2Mode ? randint(10, 100) : randint(250, 500);
}

void generateSensorsRandomly(bool project2Mode) 
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        sensors.push_back(Sensor(randint(0, 100), randint(0, 100), randomCost(project2Mode)));
    }
}

void generateSensorsUniformly() 
{
    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 10; j++) 
        {
            sensors.push_back(Sensor(i * 10, j * 10, randomCost(false)));
        }
    }
}

const int NEIGHBORHOODS = 4;
void generateSensorsClustered(bool project2Mode) 
{
    vector<pair<int, int>> points;
    double threshold = 40;
    int neighnborhoods = project2Mode ? 3 : NEIGHBORHOODS;
    while (points.size() < NEIGHBORHOODS)
    {
        pair<int, int> point = make_pair(randint(0, 100), randint(0, 100));
        bool tooClose = false;
        for (auto otherPoint : points) 
        {
            if (calculateDistance(point, otherPoint) < 40) 
            {
                threshold -= 0.01;
                tooClose = true;
                break;
            }
        }

        if (tooClose) 
        {
            continue;
        }
        points.push_back(point);
    }

    int counter = 0;
    for (auto point : points) 
    {
        counter++;
        
        int mean;
        int deviation;
        if (project2Mode) 
        {
            switch (counter) {
                case 1:
                    mean = 5;
                    deviation = 4;
                    break;
                case 2:
                    mean = 30;
                    deviation = 10;
                    break;
                case 3:
                    mean = 65;
                    deviation = 15;
                    break;
            }
        }
        else 
        {
            mean = randint(200, 400);
            deviation = 8;
        }
        for (int i = 0; i < (project2Mode ? 6 : 25); i++) 
        {
            int x = randint(max(point.first - 20, 0), min(point.first + 20, 100));
            int y = randint(max(point.second - 20, 0), min(point.second + 20, 100));
            sensors.push_back(Sensor(x, y, project2Mode ? randint(mean - deviation, mean + deviation) : randnormal(mean, deviation)));
        }
    }
}

bool contains(vector<int> v, int find)
{
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i] == find)
        return true;
    }
    return false;
}

/*void removeMutualCoverage(int index)
{   
    vector<int> sourceCoveredSensors = returnCoveredSensors(index);   
    vector<int> childCoveredSensors;        
    int mutualSensors = 0;                

    for(int sr : sourceCoveredSensors)
    {    
        childCoveredSensors = returnCoveredSensors(sr);            
        mutualSensors = 0;            

        for(int index : sourceCoveredSensors)
        {
            if(contains(childCoveredSensors, index))        
                mutualSensors++;        
        }              
        sensors[sr].coverage -= (1 + mutualSensors);
    }
    sensors[index].coverage = 0;
}*/

void removeMutualSensors(int origin, set<int> covered) 
{
    /*
    vector<int> touched;
    sensors[origin].coverage--;
    for (int inner : returnCoveredSensors(origin)) {
        sensors[inner].coverage--;
        if (covered.count(inner) == 0) {
            touched.push_back(inner);
        }
    }
    //touched.push_back(origin);

    for (int inner : touched) 
    {
        for (int outer : returnCoveredSensors(inner)) 
        {
            if (covered.count(outer) == 1) {
                continue;
            }
            sensors[outer].coverage--;
            if (sensors[outer].coverage < 0) {
                cout << "NERP";
            }
        }
    }*/
    calculateCoverage(covered);
}

vector<int> budgetAlgorithm()
{    
    vector<Sensor> sensorsCopy(sensors);
    vector<int> chosen;
    int totalCost = 0;
    vector< vector<int> > universe;

    for (int i = 0; i < sensorsCopy.size() && (totalCost + sensorsCopy[i].cost) <= budget; ++i)
    {
        totalCost += sensors[i].cost;
        chosen.push_back(i);
    }
    return chosen;
}

vector<int> weightedAlgorithm()
{
    set<int> chosen;
    set<int> considered;
    set<int> covered;
    int totalCost = 0;
    
    //initialize with the collection of sets S
    //allSets = ..

    while (considered.size() < sensors.size())
    {
        //get element with max weight
        int index_maxweight = -1;
        double maxweight = 0;
        for (int i = 0; i < sensors.size(); ++i)
        {
            if (considered.count(i) != 0) {
                continue;
            }

            if ((((double)sensors[i].coverage) / ((double)sensors[i].cost)) > maxweight)
            {
                index_maxweight = i;
                maxweight = (((double)sensors[i].coverage) / ((double)sensors[i].cost));
            }

            skip:
            ;
        }

        if (index_maxweight == -1) {
            break;
        }

        considered.insert(index_maxweight);
        if (totalCost + sensors[index_maxweight].cost < budget)
        {
            //cout << sensors[index_maxweight].cost << ' ' << maxweight << ' ' << sensors[index_maxweight].coverage << '\n';
            chosen.insert(index_maxweight);
            for (int i : returnCoveredSensors(index_maxweight)) {
                covered.insert(i);
            }
            removeMutualSensors(index_maxweight, covered);
            totalCost += sensors[index_maxweight].cost;
        }
    }
    return vector<int>(chosen.begin(), chosen.end());
    //select a set St from G that maximizes Wt over S;

    /*if (G weight >= weight)
        return G;
    else
        return ;*/

}

int calculateTotalCoverage(vector<int> sensors) {
    set<int> covered;
    for (int i : sensors) {
        covered.insert(i);
    }
    for (int sensor : sensors) {
        for (int touching : returnCoveredSensors(sensor)) {
            covered.insert(touching);
        }
    }

    return covered.size();
}

int calculateAreaCoverage(vector<int> sensors_) {
    int coverage = 0;
    for (int x = 0; x <= 100; x++) {
        for (int y = 0; y <= 100; y++) {
            pair<int, int> p1 = make_pair(x, y);
            for (int index : sensors_) {
                Sensor sensor = sensors[index];
                pair<int, int> p2 = make_pair(sensor.x, sensor.y);
                if (calculateDistance(p1, p2) < R) {
                    coverage++;
                    break;
                }
            }
        }
    }

    return coverage;
}

struct TrialResult {
    int coverage;
    int areaCoverage;
    int totalCost;

    double coveragePercent() {
        return ((double) coverage / sensors.size());
    }

    double areaCoveragePercent() {
        return ((double) areaCoverage / (101 * 101));
    }

    TrialResult (int coverage, int areaCoverage, int totalCost) : coverage(coverage), areaCoverage(areaCoverage), totalCost(totalCost) {}
};

TrialResult runTrial(int algorithmChoice, int distributionChoice, int R_, int budget_) {
    sensors.clear();
    R = R_;
    budget = budget_;
    ofstream output;
    output.open ("output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file
    output << "Sensors:\n";

    int totalCost = 0;
    int totalCoverage = 0;

    if(distributionChoice == 1)
    {
        generateSensorsUniformly();
    }
    else if(distributionChoice == 2)
    {
        generateSensorsClustered(false);
    }
    else if(distributionChoice == 3)
    {
        generateSensorsRandomly(false);
    }
    else if (distributionChoice == 4) {
        generateSensorsClustered(true);
    } else if (distributionChoice == 5) {
        generateSensorsRandomly(true);
    }

    sensors = sortSensors(sensors);    
    calculateCoverage();

    for (Sensor &s : sensors)
    {
        output << '(' << s.x << ", " << s.y << ", " << s.cost << ")\n";
    }

    vector<int> chosen;
    if(algorithmChoice == 1)
    {
        chosen = greedyAlgorithm();
    }
    else if(algorithmChoice == 2)
    {
        chosen = chooseSensorsRandomly();    
    }    
    else if(algorithmChoice == 3)
    {
        chosen = weightedAlgorithm();
        
    } else if (algorithmChoice == 4) {
        //time it
        auto start = chrono::steady_clock::now();

        chosen = dynamicAlgorithm();

        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;        
    }


    for (int index: chosen)
    {
        output << index << '\n';
        totalCost += sensors[index].cost;
        totalCoverage += sensors[index].coverage;   
    }     

    output << endl;    
    output << "Chosen:\n";

    output << endl;

    output << "R:\n";
    output << R << '\n';

    int area = calculateAreaCoverage(chosen);

    return TrialResult(calculateTotalCoverage(chosen), calculateAreaCoverage(chosen), totalCost);
}

void experiment() {
    // ofstream output;
    // output.open ("experiment_output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file

    // output << "budget coverage\n";
    // output << "random greedy smart\n";

    // for (int budget = 500; budget <= 8000; budget += 500) {
    //     auto randomTrial = runTrial(2, 1, 15, budget);
    //     auto greedyTrial = runTrial(1, 1, 15, budget);
    //     auto smartTrial = runTrial(3, 1, 15, budget);
    //     output << budget << ' ' << randomTrial.coveragePercent() << ' ' << greedyTrial.coveragePercent() << ' ' << smartTrial.coveragePercent() << endl;
    // }

    ofstream output;
    output.open ("experiment_output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file

    output << "budget coverage\n";
    output << "pure_greedy random greedy dynamic\n";
    int budgets[] = {100, 120, 200, 250, 400};

    for (int i = 0; i < 5; i++) {
        int budget  = budgets[i];
        auto trial  = runTrial(1, 5, 5, budget);
        auto trial1 = runTrial(2, 5, 5, budget);
        auto trial2 = runTrial(3, 5, 5, budget);
        auto trial3 = runTrial(4, 5, 5, budget);
        //output << R << ' ' << smartTrial.coveragePercent() << ' ' << smartTrial1.coveragePercent() << ' ' << smartTrial2.coveragePercent() << endl;
        output << budget << ' ' << trial.coveragePercent() << ' ' << trial1.coveragePercent() << ' ' << trial2.coveragePercent() << ' ' << trial3.coveragePercent() << endl;
    }
}

int main() 
{
    experiment();
    //runTrial(3, 3, 15, 2500);
    int algorithmChoice;
    std::cout << "Which algorithm would you like to use?\n1. Greedy Algorithm\n2. Random Algorithm\n3. Budgeted Algorithm\n4. Dynamic Algorithm";        
    std::cin >> algorithmChoice;

    cout << endl;

    int distributionChoice;
    cout << "Choose a distribution\n1. Uniform\n2. Clustered\n3. Random\n4. Project 2 distribution";
    cin >> distributionChoice;

    cout << endl;

    // for (int i = 0; i < NUM_POINTS; i++)
    // {
    //     sensors.push_back(Sensor(randint(0, 100), randint(0, 100), randint(250, 500)));        
    // }

    TrialResult result = runTrial(algorithmChoice, distributionChoice, 5, 400);

    std::cout << "Total Cost: " << result.totalCost;
    std::cout << "\nTotal Coverage: " << result.coverage << ", " << result.coveragePercent() << '%' << endl;
    std::cout << "\nArea Coverage: " << result.areaCoverage << ", " << result.areaCoveragePercent() << '%' << endl;
}

//for (Sensor s : sensors) 
    // {
    //     std::cout << s.cost << ' ';
    // }

// cout << "Covered Sensors for sensor at x=" << sensors[20].x << ", y=" << sensors[20].y;
    // cout << "\n";
    // for(int i : returnCoveredSensors(20))
    // {
    //     cout << "Sensor at x=" << sensors[i].x << ", y=" << sensors[i].y << "\n";
    // }        

    //sensors.push_back({1,10,0});
    // sensors.push_back({15,10,0});
    // sensors.push_back({30,10,0});
    // sensors.push_back({43,10,0});

// std::cout << endl;
//     int i = 0;
//     for(Sensor s : sensors)
//     {
//         if(sensors[i].y == 10)
//         std::cout << "10 y found at index: " << i << '\n';
//         i++;
//     }
//     std::cout << endl << endl;

//     for(int i = 0; i < 4; i++)    
//     {
//         std::cout << "\nCoverage of sensor (" << i << ") at x=" << sensors[i].x << ", y=" << sensors[i].y 
//         << " before method: " << sensors[i].coverage;
//     }
//     std::cout << endl;

//     processChosenSensor(1);
//     for(int i = 0; i < 4; i++)    
//     {
//         std::cout << "\nCoverage of sensor (" << i << ") at x=" << sensors[i].x << ", y=" << sensors[i].y 
//         << " after method: " << sensors[i].coverage;
//     }
//     std::cout << endl << endl;