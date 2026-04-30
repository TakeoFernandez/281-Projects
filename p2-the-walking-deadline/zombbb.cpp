// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA262043
#include <algorithm>
#include <getopt.h>
#include <iostream>
#include <queue>
#include "P2random.h"
using namespace std;

struct Zombie{
    std::string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t rounds_active;
    Zombie():name{},distance{},speed{},health{},rounds_active{1}{};
    Zombie(string name, uint32_t distance, uint32_t speed, uint32_t health):name{name},
    distance{distance},speed{speed},health{health},rounds_active{1}{};
};

struct zombieCompare{
    bool operator()(const Zombie *a, const Zombie *b)const{
        if((a->distance/a->speed) !=b->distance/b->speed){
            return (a->distance/a->speed) > (b->distance/b->speed);
        }
        else if(a->health != b->health){
            return a->health > b->health;
        }
        return a->name > b->name;
    }
};

struct zombieGreater{
    bool operator()(const Zombie *a, const Zombie *b)const{
        return a->rounds_active > b->rounds_active;
    }
};
struct zombieLess{
    bool operator()(const Zombie *a, const Zombie *b)const{
        return a->rounds_active < b->rounds_active;
    }
    
};
struct zombieGreater2{
    bool operator()(const Zombie *a, const Zombie *b)const{
        if(a->rounds_active == b->rounds_active){
            return a->name < b->name;
        }
        return a->rounds_active > b->rounds_active;
    }
};
struct zombieLess2{
    bool operator()(const Zombie *a, const Zombie *b)const{
        if(a->rounds_active == b->rounds_active){
            return a->name < b->name;
        }
        return a->rounds_active < b->rounds_active;
    }
    
};

class Game{
    private:
    uint32_t quiver_capacity;
    uint32_t arrows;
    uint32_t seed;
    uint32_t max_distance;
    uint32_t max_speed;
    uint32_t max_health;
    uint32_t round;
    uint32_t num_rand_zombies;
    uint32_t num_named_zombies;
    bool verbose;
    bool median;
    bool statistics;
    int entries;
    bool dead;
    uint32_t next_spawn_round;  //the next round that will spawn new zombies
    string killer_zombie_name;  //name of the zombie who killed you
    bool more_rounds;
    string last_zombie_killed;
    priority_queue<Zombie*,vector<Zombie*>,zombieCompare> PQ;
    //For Running Median implementation
    priority_queue<Zombie*,vector<Zombie*>,zombieLess> leftPQ;  //max PQ of dead zombies base on rounds active
    priority_queue<Zombie*,vector<Zombie*>,zombieGreater> rightPQ;  //min PQ of dead zombies based on rounds active
    deque<Zombie*> deadZombies;
    deque<Zombie*> currentZombies;  //Stores zombies in order of creation and we must use this rather 
    priority_queue<Zombie*,vector<Zombie*>,zombieGreater2> mostActive;  // store n most Active zombies canidates
    priority_queue<Zombie*,vector<Zombie*>,zombieLess2> leastActive;  //store n least Active zombies  
    public:
    Game():quiver_capacity{},arrows{quiver_capacity}, seed{},max_distance{},max_speed{},max_health{},round{}
    ,num_rand_zombies{},num_named_zombies{},verbose{false},median{false},statistics{false},entries{},dead{false},next_spawn_round{0}, more_rounds{true}{};
    ~Game();
    void getOptions(int argc, char** argv);
    void readGameData();    //reads the game data
    void readRoundData();   //reads number of rand and named zombies
    void spawnZombies();    //creates rand and named zombie objects
    void shootZombies();    //
    void moveZombies(); //
    void playRound();   // 
    void zombiePush(Zombie* z); //
    uint32_t getMedian();   //
    void run(); //
    void nActive(size_t n); //

};

Game::~Game(){
    while(!currentZombies.empty()){
        delete currentZombies.front();
        currentZombies.pop_front();
    }
}

void Game::getOptions(int argc, char **argv) {
    // These are used with getopt_long()
    opterr = static_cast<int>(false);  // Let us handle all error output for command line options
    int choice = 0;
    int index = 0;

    // NOLINTBEGIN: getopt predates C++ style, this usage is from `man getopt`
    option longOptions[] = {
        {"verbose", no_argument, nullptr, 'v' },
        {"statistics", required_argument, nullptr, 's' },
        {"median", no_argument, nullptr, 'm' },
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'},
    };  // longOptions[]
    // NOLINTEND

    while ((choice = getopt_long(argc, argv, "vs:mh", static_cast<option *>(longOptions), &index)) != -1) {
        switch(choice) {
        case 'v':
            verbose = true;
            break;
        case 's':{
            string arg { optarg };
            statistics = true;
            entries = atoi(optarg);
            break;
        }
        case 'm':
            median = true;
            break;

        default: 
            if(optarg != nullptr){
            cerr << "Unknown command line option\n" << flush;
            exit(1);
                }
        }  // switch ..choice
    }  // while
 // if ..mode
}

void Game:: readGameData(){
    string curr;
    getline(cin,curr);    //ignore comment (ie the first line)
    cin >> curr;
    cin >> quiver_capacity;
    cin >> curr;
    cin >> seed;
    cin >> curr;
    cin >> max_distance;
    cin >> curr;
    cin>> max_speed;
    cin >> curr;
    cin >> max_health;
    P2random::initialize(seed,max_distance,max_speed,max_health);   //init the random number generator
    // getline(cin,curr);  //consumes the \n
}

void Game::readRoundData(){
    string curr;
    // getline(cin,curr);  //consumes the beginning ---
    cin >> curr;
    if(curr != "---"){
        more_rounds= false;//somehow signify that there are no more rounds to read
        return;
    }//if curr is not --- then we are out of round to read
    cin >> curr >> next_spawn_round >> curr >> num_rand_zombies >> curr >> num_named_zombies;
    }
    
    void Game::spawnZombies(){
        string curr;
        std::string name;
        uint32_t distance;
        uint32_t speed;
        uint32_t health;        
        for(size_t i = 0; i < num_rand_zombies; ++i){
            name  = P2random::getNextZombieName();
            distance = P2random::getNextZombieDistance();
            speed    = P2random::getNextZombieSpeed();
            health   = P2random::getNextZombieHealth();
            Zombie * z = new Zombie(name,distance,speed,health);
            PQ.push(z);        //add that zombie to our PQ
            currentZombies.push_back(z);    //add zombie to our Deque to keep track of order of creation
            if(verbose){
                cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")\n";
            }
        }   // create random zombies objects equal to num_rand_zombies as read from readRoundData func
        for(size_t i = 0; i < num_named_zombies; ++i){
            cin >> name;
            cin >> curr;
            cin >> distance;
            cin >> curr;
            cin >> speed;
            cin >> curr;
            cin >> health;
            Zombie * z = new Zombie(name,distance,speed,health);
            PQ.push(z);        //add that zombie to our PQ
            currentZombies.push_back(z);    //add zombie to our master list
            if(verbose){
                cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")\n";
            }//if verbose mode active
        }   // while there are still named zombie entires
        readRoundData();    //call read round data to now update
    }

void Game:: moveZombies(){
    for(size_t i = 0; i < currentZombies.size(); ++i){
        if(currentZombies[i]->health !=0){
            currentZombies[i]->distance-=min(currentZombies[i]->distance,currentZombies[i]->speed); //move zombie closer to you
            ++currentZombies[i]->rounds_active;//increase active rounds of current zombie by 1
            if(currentZombies[i]->distance == 0 && !dead){
                killer_zombie_name = currentZombies[i]->name;
                dead = true;
            }   //if a zombie reachs you change dead to true and note the name of the first zombie to kill you, however continue to update the rest of the zombies
            if(verbose){
                cout << "Moved: "<< currentZombies[i]->name <<  " (distance: " << currentZombies[i]->distance 
                << ", speed: "<< currentZombies[i]->speed << ", health: " << currentZombies[i]->health << ")\n";
            }   // if in verbose mode state how the zombie has moved
        }   //if zombie is not dead
    }   //for every zombie
}

void Game::shootZombies(){
    while(arrows != 0 && !PQ.empty()){
        uint32_t health = PQ.top()->health;
        PQ.top()->health-= min(PQ.top()->health,arrows);//shoot zombie with enough arrows to kill it or all your arrows
        arrows-= min(health,arrows);
        if(PQ.top()->health == 0){
            if(verbose){
                cout << "Destroyed: " << PQ.top()->name << " (distance: " << PQ.top()->distance << ", speed: " 
                << PQ.top()->speed << ", health: " << PQ.top()->health << ")\n";
            }
            if(statistics){
                deadZombies.push_back(PQ.top());   //do this so that we know the order zombies were killed in
            }
            if(median){
                zombiePush(PQ.top());
            }
            if(PQ.size() == 1 && !more_rounds){
                last_zombie_killed = PQ.top()->name;
            }//if there are no more rounds to read and this is the last zombie record it's name
            PQ.pop();
        }   //if the current zombie we are shooting dies
    }//while there are still active zombies and we still have arrows
}

void Game:: playRound(){
    ++round;                                                        //increment round number
    arrows = quiver_capacity;   //refill arrows
    if(verbose){
        cout << "Round: " << round << '\n';                         //print out current round number
    }
    moveZombies();  //defeat is triggered here as this is when zombies could reach you
    if(dead){
        cout << "DEFEAT IN ROUND " << round << "! " << killer_zombie_name << " ate your brains!\n";
        return; //ends the round
    }   // if we are dead then print defeat message along with who killed us
    if(round == next_spawn_round && more_rounds){
        spawnZombies();
    }//if current round is equal to the spawn round and there are more rounds to spawn
    shootZombies();
    if(median && (!leftPQ.empty() || !rightPQ.empty())){
        cout << "At the end of round " << round << ", the median zombie lifetime is " << getMedian() << endl;
    }//if median is active and zombies have died
}

void Game::zombiePush(Zombie* z){
    if(leftPQ.empty()){
        leftPQ.push(z);
    }
    //leftPQ.top()->rounds_active <= z->rounds_active && (leftPQ.size() - rightPQ.size() < 2)
    else if(leftPQ.top()->rounds_active <= z->rounds_active && (leftPQ.size() >= rightPQ.size())){
        rightPQ.push(z);
    }   //if added zombie is greater than top of left and right has space, add it to right
    else{
        if(rightPQ.top()->rounds_active < z->rounds_active){
            leftPQ.push(rightPQ.top());
            rightPQ.pop();
            rightPQ.push(z);
        }   //if added zombie is larger enough to be added to right then move top of right to left, remove top of right and then add z to right
        else{
            leftPQ.push(z);
        }
    }
}

uint32_t Game::getMedian(){
    //if either PQ is empty return the top of the other
    if(leftPQ.empty()){
        return rightPQ.top()->rounds_active;
    }
    if(rightPQ.empty()){
        return leftPQ.top()->rounds_active;
    }
    //if both PQ's are non-empty
    if(rightPQ.size() == leftPQ.size()){
        return (leftPQ.top()->rounds_active + rightPQ.top()->rounds_active)/2;
    }
    if(leftPQ.size() > rightPQ.size()){
        return leftPQ.top()->rounds_active;
    }
    return rightPQ.top()->rounds_active;
}

void Game::nActive(size_t n){
    for(size_t i = 0; i < currentZombies.size(); ++i){
        leastActive.push(currentZombies[i]);
        mostActive.push(currentZombies[i]);
        if(n < leastActive.size()){
            leastActive.pop();
        }   //if leastActive is not full
        if(n < mostActive.size()){
            mostActive.pop();
        }   //if leastActive is not full
    }
}


void Game::run(){
    readRoundData();
    if(next_spawn_round != 1){
        while(round != next_spawn_round - 1){
            ++round;
            if(verbose){
                cout << "Round: " << round << '\n';
            }
        }
    }//handles the case when we don't spawn zombies in the first round
    playRound();
    while(!dead && (!PQ.empty() || more_rounds)){
        playRound();
    } //if we are not dead, the PQ is not empty, and there are more rounds to read, then play another round
    if(!dead){
        cout << "VICTORY IN ROUND " << round << "! " << last_zombie_killed << " was the last zombie.\n";
    } //if we are not dead after all rounds then we win
    if(statistics){
        vector<Zombie*> least_active; //stores contents Least/MostActive PQ in reverese order such that it can be printed   
        vector<Zombie*> most_active;
        size_t num_output = min(static_cast<size_t>(entries),deadZombies.size());   //this is how many zombies to print for first and last statistics
        size_t num_active = min(static_cast<size_t>(entries),currentZombies.size());   //this is how many zombies to print for active zombies statistics
        least_active.reserve(num_output);
        most_active.reserve(num_output);
        cout << "Zombies still active: " << PQ.size() <<"\n";   //size of Priority queue is the number of currently alive zombies
        cout << "First zombies killed: \n"; 
        for(size_t i = 0; i < num_output; ++i){
            cout << deadZombies[i]->name << " " << i + 1 << "\n";
        }
        cout << "Last zombies killed: \n"; 
        for(size_t i = 0; i < num_output; ++i){
            cout << deadZombies[deadZombies.size()-1-i]->name<< " " << num_output-i << "\n";
        }
        nActive(num_active);
        while(!leastActive.empty()){
            least_active.push_back(leastActive.top());
            leastActive.pop();
        }
        while(!mostActive.empty()){
            most_active.push_back(mostActive.top());
            mostActive.pop();
        }
        reverse(least_active.begin(),least_active.end());
        reverse(most_active.begin(),most_active.end());
        cout << "Most active zombies:" << '\n';
        for(const auto &s: most_active){
            cout << s->name << " "<< s->rounds_active << '\n';
        }
        cout << "Least active zombies:" << '\n';
        for(const auto &s: least_active){
            cout << s->name << " " << s->rounds_active << '\n';
        }
    }
}

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    /*
    !!!!
    Remember to comment out this top line when running Valgrind!!!!
    !!!!
    */
    try {
        Game g;
        g.getOptions(argc,argv);
        g.readGameData();
        g.run();
    }
    // Catch runtime_errors, print the message, and exit the
    // program with a non-zero status.
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    // All done!
    return 0;
}
//test-1-v.txt 