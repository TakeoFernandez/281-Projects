// Project Identifier: B99292359FFD910ED13A7E6C7F9705B874262D79

// EECS 281: Project 1
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include <deque>
using namespace std;
/*
TODO: 
*/

struct location{
    uint32_t room_num = 0;
    uint32_t row = 0;
    uint32_t col = 0;
    char c;
    char direction;
    location():room_num{0}, row{0}, col{0}, c{'u'}, direction{'/'}{};
    location(uint32_t room_num,uint32_t row, uint32_t col, char c, char direction):room_num(room_num),row(row),col(col),c(c),direction(direction){}
};

struct tile{
    char c;
    char direction;
    bool discovered;
    tile():c{'.'},direction{'/'},discovered{false}{};
};

class Castle {
public:
    Castle(): route_mode('\0'), output_mode('M'), num_rooms(0), dimensions(0),
    start_pos(), end_pos(), castle_layout(), search_container(){};
    // Read in the txt file through stdin.
    void readData();
    // Read and process command line arguments.
    void getOptions(int argc, char **argv);
    // find the path and print the data.
    void Search();
    void Backtrack();
    void outputPath();


private:
    tile& tileAt(location &loc); 
    char route_mode;
    char output_mode;
    uint32_t num_rooms;
    uint32_t dimensions;
    location start_pos;
    location end_pos;
    vector< vector< vector <tile> > > castle_layout;
    std::deque<location> search_container;
    vector<location> path;
};


// Print help for the user when requested.
// argv[0] is the name of the currently executing program
void printHelp(char *command) {
cout << "hello world" << endl;
cout << command << endl;

}  // printHelp()

void Castle::getOptions(int argc, char **argv) {
    // These are used with getopt_long()
    opterr = static_cast<int>(false);  // Let us handle all error output for command line options
    int choice = 0;
    int index = 0;

    // NOLINTBEGIN: getopt predates C++ style, this usage is from `man getopt`
    option longOptions[] = {
        {"output", required_argument, nullptr, 'o' },
        {"stack", no_argument, nullptr, 's' },
        {"queue", no_argument, nullptr, 'q' },
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'},
    };  // longOptions[]
    // NOLINTEND

    while ((choice = getopt_long(argc, argv, "o:hsq", static_cast<option *>(longOptions), &index)) != -1) {
        switch (choice) {
        case 'h':
            printHelp(*argv);
            exit(0);
        case 's':
            if(route_mode != '\0') {
                cerr << "Stack or queue can only be specified once\n" << flush;
                exit(1);
            }
            route_mode = 'S';
            break;
        case 'q':
            if(route_mode != '\0') {
                cerr << "Stack or queue can only be specified once\n" << flush;
                exit(1);
            }
            route_mode = 'Q';
            break;
        case 'o': { 
            string arg { optarg };
            if (arg == "L") {
                output_mode = 'L';
            } else if (arg == "M") {
                output_mode = 'M';
            }
            break;
        }  // case 'm'

        default:
            if(optarg != nullptr){
                cerr << "Unknown command line option\n" << flush;
                exit(1);
            }
        }  // switch ..choice
    }  // while
    if (route_mode == '\0') {
        cerr << "Stack or queue must be specified\n" << flush;
        exit(1);
    } 
 // if ..mode
}  // getOptions()

tile& Castle::tileAt(location &loc){
    return castle_layout[loc.room_num][loc.row][loc.col];
}

void Castle::readData(){
    string read_type;
    string curr_line;
    char curr_char;
    uint32_t room = 0;
    uint32_t row = 0;
    uint32_t col = 0;
    cin >> read_type;
    cin >> num_rooms;
    cin >> dimensions;
    castle_layout.resize(num_rooms,vector<vector <tile> >(dimensions,vector<tile>(dimensions)));
    if(read_type == "M"){
        cin.ignore();
        while(getline(cin,curr_line) && curr_line != ""){
            if(curr_line.substr(0,2) != "//"){
                for(uint32_t col = 0; col < dimensions; ++col){
                    if(curr_line[col] == 'S'){
                        start_pos = {room,row,col,'S','/'};
                    }
                    else if(curr_line[col] == 'C'){
                        end_pos = {room,row,col,'C','/'};
                    }
                    else if(curr_line[col] != '.' && curr_line[col] != '#' && curr_line[col] != '!' 
                        && (curr_line[col] < '0' || curr_line[col] >'9')){
                    cerr << "Unknown map character" << endl;
                    exit(1);
                    }
                    castle_layout[room][row][col].c = curr_line[col];
                }
                ++row;
                row%=dimensions;
                if(row == 0){
                    ++room;
                }
            }
        }
    }
    else{
        while(cin >> curr_char){
            if(curr_char == '/'){
                getline(cin,curr_line);
            }
            else{
                cin >> room;
                if(room >= num_rooms){
                    cerr << "Invalid room number" << endl; 
                    exit(1);
                }
                cin.ignore(1);
                cin >> row;
                if(row >= dimensions){
                    cerr << "Invalid row number" << endl; 
                    exit(1);
                }
                cin.ignore(1);
                cin >> col;
                if(col >= dimensions){
                    cerr << "Invalid column number" << endl; 
                    exit(1);
                }
                cin.ignore(1);
                cin >> curr_char;
                cin.ignore(1);
                castle_layout[room][row][col].c = curr_char;
                if(curr_char == 'S'){
                    start_pos = {room,row,col,'S','/'};
                }
                else if(curr_char == 'C'){
                    end_pos = {room,row,col,'C','/'};
                }
                else if(curr_char != '.' && curr_char != '#' && curr_char != '!' && (curr_char < '0' || curr_char > '9')){
                    cerr << "Unknown map character" << endl;
                    exit(1);
                }
            }
        }
    }
}

void Castle::outputPath(){
    Backtrack();
    if(output_mode == 'M'){
        cout << "Start in room " << start_pos.room_num << ", row " << start_pos.row << ", column " << start_pos.col << '\n';
        for(uint32_t room = 0; room < num_rooms; ++room){
            cout << "//castle room " << room << '\n';
            for(uint32_t row = 0; row < dimensions; ++row){
                for(uint32_t col = 0; col < dimensions; ++col){
                    cout << castle_layout[room][row][col].c;
                }
                cout << '\n';
            }
        }
        cout << endl;
    }//if map output
    else{
        cout << "Path taken:" << '\n';
        for(size_t i = path.size() - 1; 0 < i; --i){
            cout << "(" << path[i].room_num << "," << path[i].row << "," 
            << path[i].col << "," << path[i].direction << ")" << '\n';
        }
    }//else list output
}

void Castle::Backtrack(){
    location curr_pos = end_pos;
    if(output_mode == 'M'){
        while(tileAt(curr_pos).c != 'S'){
            if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 'n'){
                ++curr_pos.row;
                tileAt(curr_pos).c = 'n';
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 's'){
                --curr_pos.row;
                tileAt(curr_pos).c = 's';
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 'e'){
                --curr_pos.col;
                tileAt(curr_pos).c = 'e';
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 'w'){
                ++curr_pos.col;
                tileAt(curr_pos).c = 'w';
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction >= '0' 
                && castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction <= '9'){
                curr_pos.room_num = static_cast<uint32_t>(tileAt(curr_pos).direction - '0');
                tileAt(curr_pos).c = 'p';
            }//if ... pipe
            else{
                return;
            }
        }    
    }
    else{
        path.push_back(curr_pos);   //pushs back the end position
        while(tileAt(curr_pos).c != 'S'){
            if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 'n'){
                ++curr_pos.row;
                curr_pos.direction = 'n';
                path.push_back(curr_pos);
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 's'){
                --curr_pos.row;
                curr_pos.direction = 's';
                path.push_back(curr_pos);
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 'e'){
                --curr_pos.col;
                curr_pos.direction = 'e';
                path.push_back(curr_pos);
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction == 'w'){
                ++curr_pos.col;
                curr_pos.direction = 'w';
                path.push_back(curr_pos);
            }
            else if(castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction >= '0' 
                && castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].direction <= '9'){
                curr_pos.room_num = static_cast<uint32_t>(tileAt(curr_pos).direction - '0');
                curr_pos.direction = 'p';
                path.push_back(curr_pos);
            }
            else{
                return;
            }
        }    
    }
}

void Castle::Search(){
    location curr_pos = start_pos;
    int row_offset[4] = {-1,0,1,0};
    int col_offset[4] = {0,1,0,-1};
    char direction[4] = {'n','e','s','w'};
    int num_discovered = 0;
    search_container.push_back(start_pos);
    castle_layout[curr_pos.room_num][curr_pos.row][curr_pos.col].discovered = true;
    ++num_discovered;
    while(!search_container.empty()){
        if(route_mode == 'S'){
            curr_pos = search_container.back();
            search_container.pop_back();
        }
        else{
            curr_pos = search_container.front();
            search_container.pop_front();
        }
        if(tileAt(curr_pos).c >= '0' && tileAt(curr_pos).c <= '9'){
            //read the number of the pipe
            uint32_t pipe_num = static_cast<uint32_t>(curr_pos.c -'0');
            if(pipe_num < num_rooms && castle_layout[pipe_num][curr_pos.row][curr_pos.col].c != '#'&&
            castle_layout[pipe_num][curr_pos.row][curr_pos.col].c != '!'&&
            !(castle_layout[pipe_num][curr_pos.row][curr_pos.col].discovered)){
                char prev_room = static_cast<char>(curr_pos.room_num + '0');
                curr_pos.room_num = static_cast<uint32_t>(tileAt(curr_pos).c - '0');
                tileAt(curr_pos).discovered = true;
                ++num_discovered;
                tileAt(curr_pos).direction = prev_room;
                curr_pos.c = tileAt(curr_pos).c;
                search_container.push_back(curr_pos);
                if(search_container.back().c == 'C'){
                    end_pos.direction = curr_pos.direction;
                    outputPath();
                    return;
                }//if warp location is countess
            }//if the warp location is valid
        } //if the curr_pos is a pipe
        else{
                for(uint32_t i = 0; i < 4; ++i){
                    int checkRow = static_cast<int>(curr_pos.row) + row_offset[i];
                    int checkCol = static_cast<int>(curr_pos.col) + col_offset[i];
                    if(checkRow >= 0 && checkRow < static_cast<int>(dimensions) 
                    && checkCol < static_cast<int>(dimensions) && checkCol >= 0){
                        uint32_t nRow = static_cast<uint32_t>(checkRow);
                        uint32_t nCol = static_cast<uint32_t>(checkCol);
                        if(castle_layout[curr_pos.room_num][nRow][nCol].c != '#' && castle_layout[curr_pos.room_num][nRow][nCol].c != '!' 
                        && !castle_layout[curr_pos.room_num][nRow][nCol].discovered){
                                search_container.push_back({curr_pos.room_num,nRow,nCol,castle_layout[curr_pos.room_num][nRow][nCol].c,direction[i]});
                                castle_layout[curr_pos.room_num][nRow][nCol].discovered = true;
                                ++num_discovered;
                                castle_layout[curr_pos.room_num][nRow][nCol].direction = direction[i];
                                if(search_container.back().c == 'C'){
                                    end_pos.direction = search_container.back().c;
                                    outputPath();
                                    return;
                                }//if this new location was the countess
                        }//if undiscovered and not # or !
                    } //if within bounds of vector
                }//search adjacent tiles
            }
    }//if search container not empty
    cout <<"No solution, " << num_discovered << " tiles discovered."  << '\n';
}
int main(int argc, char *argv[]) {
ios_base::sync_with_stdio(false);
    try {
        Castle castle;
        // Read and process the command line options.
        castle.getOptions(argc, argv);
        // Read in the provided file through stdin.
        castle.readData();
        castle.Search();
    }
    // Catch runtime_errors, print the message, and exit the
    // program with a non-zero status.
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    // All done!
    return 0;
}  // main()
