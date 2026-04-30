// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797926254D

// EECS 281 Project 3 SillyQL Field class
// Copyright 2025, Regents of the University of Michigan

#include "Field.h"
#include <getopt.h>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <optional>
using namespace std;

/*
TODO: print_where_bst does not handle the = case properly, fix that
Change the print_where logic such that:
if a bstIndex exists and it is in the correct column print_where_bst will be called 
if a hashIndex exists, the hashIndex is built on the correct column, and the operator is = print_where_bst will be called
otherwise call print_where_no_index
*/

enum class IndexType : uint8_t {hash,bst,none};

class Table{
    private:
    //variables
    friend class DataBase;
    bool quiet = false;
    vector<string> colNames;
    vector<ColumnType> colTypes; //Note that the column name and type share the same index so we can use one to find the other
    IndexType indexType = IndexType::none;
    vector<vector<Field>> data;
    size_t generateIndex;   //the column that the current index was generated on
    std::map<Field,vector<size_t>> bstIndex;
    unordered_map <Field, vector<size_t>> hashIndex;
    string tableName;
    optional<size_t> col_name_to_index(string name);
    optional<size_t> col_name_to_index(string name, Table &table);
    

    void insert();
    void print(); // calls either print all or where depending on user input
    void print_all(const vector<size_t> &indices);
    void print_where(const vector<size_t> &indices);
    void print_where_helper(char op,size_t colIndex,const vector<size_t> &indices,Field target);
    void print_where_bst(const vector<size_t> &indices,Field target,char op);
    void print_where_hash(const vector<size_t> &indices,Field target);
    void print_where_no_index(const vector<size_t> &indices,size_t target_col_index,Field target,char op);
    template<typename Comp>
    void print_where_no_index_helper(const vector<size_t> &print_indices, Comp comp);
    void delete_where();
    void delete_helper(Field val, size_t colIndex, char comp);
    void generate(size_t colIndex, string indexType);
    void join(Table &addedTable);
};

class DataBase{
    private:
    friend class Table;
    unordered_map <string, Table> Tables;
    bool quiet = false;
    public:
    void getOptions(int argc, char **argv);
    void create(string name);
    void remove(string name);
    void read_command();
};

/*
Functors
*/

class Greater{
    private:
    size_t colIndex;
    Field f;
    public:
    Greater(size_t i,Field val): colIndex(i), f(val){}
    bool operator()(const vector<Field> &row){
        return row[colIndex] > f;
    }
};

class Lesser{
    private:
    size_t colIndex;
    Field f;
    public:
    Lesser(size_t i,Field val): colIndex(i), f(val){}
    bool operator()(const vector<Field> &row){
        return row[colIndex] < f;
    }
};

class Equal{
    private:
    size_t colIndex;
    Field f;
    public:
    Equal(size_t i,Field val): colIndex(i), f(val){}
    bool operator()(const vector<Field> &row){
        return row[colIndex] == f;
    }
};



void DataBase:: getOptions(int argc, char **argv) {
    // These are used with getopt_long()
    opterr = static_cast<int>(false);  // Let us handle all error output for command line options
    int choice = 0;
    int index = 0;

    // NOLINTBEGIN: getopt predates C++ style, this usage is from `man getopt`
    option longOptions[] = {
        {"help", no_argument, nullptr, 'h' },
        {"quiet", no_argument, nullptr, 'q' },
    };  // longOptions[]
    // NOLINTEND

    while ((choice = getopt_long(argc, argv, "hq", static_cast<option *>(longOptions), &index)) != -1) {
        switch(choice) {
        case 'h':
            exit(0);
        case 'q':
            quiet = true;
            break;
        }  // switch ..choice
    }  // while
 // if ..mode
}

void DataBase :: read_command(){
    string command;
    string tableName;
    do{
        if (cin.fail()) {
            cerr << "Error: Reading from cin has failed" << endl;
            exit(1);
        }
        cout << "% ";
        cin >> command;
        switch(command[0]){
            case 'Q':
                cout << "Thanks for being silly! \n"; 
                break;
            case '#':
                getline(cin,command);   //read and ignore the comment line
                break;
            case 'C':
            {
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it == Tables.end()){
                    Tables[tableName];  //creates a table in the database
                    create(tableName);   //sets up the column names and types of the created table
                    cout << "New table " << tableName << " with column(s) ";
                    for(string name: Tables[tableName].colNames){
                        cout << name << " ";
                    }
                    cout << "created" <<endl;

                }//if the table does not exist in tables
                else{
                    cout << "Error during CREATE: Cannot create already existing table " << tableName << endl;
                    getline(cin,command);
                }//else the table does exists in tables
                break;
            }
            case 'R':
            {
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it != Tables.end()){
                    remove(tableName);
                }//if the table does not exist in tables
                else{
                    cout << "Error during REMOVE: " << tableName << " does not name a table in the database " << endl;
                    getline(cin,command);
                }//else the table does exists in tables
                break;
            }
            case 'I':
            {
                cin >> command; //gets rid of the INTO
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it != Tables.end()){
                    Tables[tableName].insert();
                }
                else{
                    cout << "Error during INSERT: "<< tableName << " does not name a table in the database" << endl;
                    getline(cin,command);
                }
                break;
            }
            case 'P':
            {
                cin >> command; //gets rid of the FROM
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it != Tables.end()){
                    Tables[tableName].print();
                }
                else{
                    cout << "Error during PRINT: "<< tableName << " does not name a table in the database" << endl;
                    getline(cin,command);
                }
                break;
            }
            case 'D':
            {
                cin >> command; //gets rid of the FROM
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it != Tables.end()){
                    Tables[tableName].delete_where();
                }
                else{
                    cout << "Error during DELETE: "<< tableName << " does not name a table in the database" << endl;
                    getline(cin,command);
                }
                break;
            }
            case 'J':
            {
                string tableName2;
                string colName;
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it == Tables.end()){
                    cout << "Error during JOIN: "<< tableName << " does not name a table in the database" << endl;
                    getline(cin,command);
                    break;
                }
                cin >> command; //get rid of AND 
                cin >> tableName2;
                it = Tables.find(tableName2);
                if(it == Tables.end()){
                    cout << "Error during JOIN: "<< tableName2 << " does not name a table in the database" << endl;
                    getline(cin,command);
                    break;
                }
                cin >> command;// get rid of WHERE
                Tables[tableName].join(Tables[tableName2]);
                break;
            }
            case 'G':
            {
                cin >> command; //gets rid of the FOR
                cin >> tableName;
                auto it = Tables.find(tableName);
                if(it != Tables.end()){
                    string garbage;
                    string indexType;
                    string colName;
                    optional<size_t> colIndex;
                    cin >> indexType;
                    cin >> garbage >> garbage;  //gets rid of the INDEX ON
                    cin >> colName;
                    colIndex = Tables[tableName].col_name_to_index(colName);
                    if(!colIndex){
                        cout << "Error during GENERATE: " << colName << " does not name a column in " << tableName << '\n';
                        getline(cin,garbage);//consume the invalid delete call as the column to compare on is invalid
                        break;
                    }
                    Tables[tableName].generate(*colIndex,indexType);
                }
                else{
                    cout << "Error during GENERATE: "<< tableName << " does not name a table in the database" << endl;
                    getline(cin,command);
                }
                break;
            }
            default:
                cout << "Error: unrecognized command \n"; 
                getline(cin,command);
                break;
        }              
    } while(command != "QUIT");
}

optional<size_t> Table::col_name_to_index(string name){
    for(size_t i = 0; i < colNames.size(); ++i){
        if(colNames[i] == name){
            return static_cast<int>(i);
        }
    }
    return nullopt;
}

optional<size_t> Table:: col_name_to_index(string name, Table& table){
    for(size_t i = 0; i < table.colNames.size(); ++i){
        if(table.colNames[i] == name){
            return static_cast<int>(i);
        }
    }
    return nullopt;
}

void DataBase :: create(string name){
    if(quiet){
        Tables[name].quiet = true;
    }
    uint32_t numCol;
    string currLine;
    char firstChar;
    Tables[name].tableName = name;
    cin >> numCol;
    Tables[name].colTypes.reserve(numCol);   //reserve space for column type and name vectors
    Tables[name].colNames.reserve(numCol);
    for(size_t i = 0; i < numCol; ++i){
        cin >> currLine;
        firstChar = currLine[0];
        switch(firstChar){
            case 'd':
                Tables[name].colTypes.push_back(ColumnType::Double);
                break;
            case 'i':
                Tables[name].colTypes.push_back(ColumnType::Int);
                break;
            case 'b':
                Tables[name].colTypes.push_back(ColumnType::Bool);
                break;
            case 's':
                Tables[name].colTypes.push_back(ColumnType::String);
                break;
        }   // add the enumerated type to the colType vector
    }//reads column types
    for(size_t j = 0; j < numCol; ++j){
        cin >> currLine;
        Tables[name].colNames.push_back(currLine);
    }//reads column names
}

void DataBase :: remove(string name){
    Tables.erase(name);
    cout << "Table " << name <<" removed" << endl;
}

void Table :: insert(){
    uint32_t numRows;
    size_t currentNumRows = data.size();
    string garbage; 
    cin >> numRows;
    cin >> garbage; //get rid of ROWS
    data.resize(currentNumRows + numRows);
        for(size_t row = currentNumRows; row < data.size(); ++row){
            data[row].reserve(colNames.size()); //reserve space for the new row
            for(ColumnType &type: colTypes){
                bool b;
                int i;
                double d;
                string s;
                switch(type){
                    case ColumnType::Bool:
                        cin >> b;
                        data[row].emplace_back(b);
                        break;
                    case ColumnType::Int:
                        cin >> i;
                        data[row].emplace_back(i);
                        break;
                    case ColumnType::Double:
                        cin >> d;
                        data[row].emplace_back(d);
                        break;
                    case ColumnType::String:
                        cin >> s;
                        data[row].emplace_back(s);
                        break;
                }//split based on data type of the column
            } //for every column in the current row
        }// for every row
        if(indexType == IndexType::bst){
            for (size_t i = currentNumRows; i < data.size(); ++i){
                bstIndex[data[i][generateIndex]].emplace_back(i);
            }//for every row in the table add it to the unordered map
        }
        if(indexType == IndexType::hash){
            for (size_t i = currentNumRows; i < data.size(); ++i){
                hashIndex[data[i][generateIndex]].emplace_back(i);
            }//for every row in the table add it to the unordered map
        }
cout << "Added " << numRows <<" rows to " << tableName 
<< " from position " << currentNumRows << " to " << currentNumRows + numRows - 1 <<  endl;
}

void Table:: delete_helper(Field val, size_t colIndex, char comp){
    long int num_deleted = 0;
    switch(comp){
        case '>':
        {
            auto it = stable_partition(data.begin(),data.end(), Greater(colIndex,val));
            data.erase(data.begin(),it);
            num_deleted = it - data.begin();
            cout << "Deleted " << num_deleted << " rows from " << tableName << endl;
        break;
        }
        case '<':
        {
            auto it = stable_partition(data.begin(),data.end(), Lesser(colIndex,val));
            data.erase(data.begin(),it);
            num_deleted = it - data.begin();
            cout << "Deleted " << num_deleted << " rows from " << tableName << endl;
        break;
        }
        case '=':
        {
            auto it = stable_partition(data.begin(),data.end(), Equal(colIndex,val));
            data.erase(data.begin(),it);
            num_deleted = it - data.begin();
            cout << "Deleted " << num_deleted << " rows from " << tableName << endl;
        break;
        }
    }
    if(num_deleted > 0){
        if(indexType == IndexType::bst){
            bstIndex.clear();
            for (size_t i = 0; i < data.size(); ++i){
                bstIndex[data[i][generateIndex]].emplace_back(i);
            }//for every row in the table add it to the unordered map
        }
        if(indexType == IndexType::hash){
            hashIndex.clear();
            for (size_t i = 0; i < data.size(); ++i) {
                hashIndex[data[i][generateIndex]].emplace_back(i);
            }//for every row in the table add it to the unordered map
        }
    }//if any rows were deleted regenerate the Index if one exists
}

void Table:: delete_where(){
    string garbage;
    string colName;
    char comparison;
    size_t typeIndex;
    cin >> garbage; //get rid of WHERE
    cin >> colName;
    cin >> comparison;
    auto it = find(colNames.begin(),colNames.end(),colName);
    if(it == colNames.end()){
        cout << "Error during DELETE: " << colName <<" does not name a column in " << tableName << '\n';
        getline(cin,garbage);//consume the invalid delete call as the column to compare on is invalid
        return;
    }//if the col name does not exist
    typeIndex = static_cast<size_t>(it - colNames.begin());
    switch(colTypes[typeIndex]){
        case ColumnType::Bool:
        {
            bool b;
            cin >> b;
            Field temp(b);
            delete_helper(temp,typeIndex,comparison);
            break;
        }
        case ColumnType::Int:
        {
            int i;
            cin >> i;
            Field temp(i);
            delete_helper(temp,typeIndex,comparison);
            break;
        }
        case ColumnType::Double:
        {
            double d;
            cin >> d;
            Field temp(d);
            delete_helper(temp,typeIndex,comparison);
            break;
        }
        case ColumnType::String:
        {
            string s;
            cin >> s;
            Field temp(s);
            delete_helper(temp,typeIndex,comparison);
            break;
        }
    }
}

void Table::print_all(const vector<size_t> &print_indices){
    if(!quiet){
        for(size_t i : print_indices){
                cout << colNames[i] << " ";
            }
            for(size_t row = 0; row < data.size(); ++row){
                cout << "\n";
                for(size_t col : print_indices){
                    cout << data[row][col] << " ";
                }
        }//for every row in the table
        cout << "\n";
    }
    cout << "Printed " << data.size() << " matching rows from "<< tableName << endl;
}

void Table::print_where_bst(const vector<size_t> &print_indices,Field target,char op){
    uint32_t count = 0;
    if(!quiet){
        for(size_t i: print_indices){
            cout << colNames[i] << " ";
        }
        cout << '\n';
    }
    if(op == '>'){
        for(auto it = bstIndex.upper_bound(target); it != bstIndex.end(); ++it){
        const vector<size_t> &bst_indices = it->second; //the indices
            for(const size_t &row : bst_indices){
                for(const size_t &i : print_indices){
                    if(!quiet){cout << data[row][i] << " ";}
                }
                if(!quiet){std::cout << "\n";}
                ++count;
            }//for every row index in the bst
        }//loop from lower bound to end
    }
    else if (op == '<'){
        for(auto it = bstIndex.begin(); it != bstIndex.lower_bound(target); ++it){
            const vector<size_t> &indices = it->second; //the indices
            for(const size_t &row : indices){
                for(const size_t &i : print_indices){
                    if(!quiet){cout << data[row][i] << " ";}
                }
                if(!quiet){std::cout << "\n";}
                ++count;
            }//for every row index in the bst
        }//loop until lower bound
    }
    else{
        const auto it = bstIndex.find(target);
        if(it != bstIndex.end()){
            for(const size_t &row : it->second){
                for(const size_t &i : print_indices){
                    if(!quiet){cout << data[row][i] << " ";}
                }
                if(!quiet){std::cout << "\n";}
                ++count;
            }//for every row index in the bst
        }
    }
    cout << "Printed " << count << " matching rows from "<< tableName << endl;
}

void Table::print_where_hash(const vector<size_t> &print_indices,Field target){
    uint32_t count = 0;
    if(!quiet){
        for(size_t i: print_indices){
            cout << colNames[i] << " ";
        }
        cout << '\n';
    }
    for(const auto &row : hashIndex[target]){
        for(const size_t &i : print_indices){
                if(!quiet){cout << data[row][i] << " ";}
        }//for every column to print
        if(!quiet){cout << '\n';}
        ++count;
    }// for every row index stored in the hash
    cout << "Printed " << count << " matching rows from "<< tableName << endl;
}

void Table::print_where_no_index(const vector<size_t> &print_indices,size_t target_col_index,Field target,char op){
    if(!quiet){
        for(size_t i: print_indices){
            cout << colNames[i] << " ";
        }
        cout << '\n';
    }
    switch(op){
        case '>':
            print_where_no_index_helper(print_indices,Greater(target_col_index,target));
            break;
        case '<':
            print_where_no_index_helper(print_indices,Lesser(target_col_index,target));
            break;
        case '=':
            print_where_no_index_helper(print_indices,Equal(target_col_index,target));
            break;
    }
}

template<typename Comp>
void Table:: print_where_no_index_helper(const vector<size_t> &print_indices, Comp comp){
    uint32_t count = 0;
    for(const vector<Field> &row: data){
        if(comp(row)){
            for(const size_t &i : print_indices){
                if(!quiet){cout << row[i] << " ";}
            }//for every column to print
            if(!quiet){cout << '\n';}
            ++count;
        }
    }//for every row in data
    cout << "Printed " << count << " matching rows from "<< tableName << endl;
}


void Table:: print_where_helper(char op, size_t colIndex,const vector<size_t> &indices,Field target){
    if(indexType == IndexType::bst && colIndex == generateIndex){
        print_where_bst(indices,target,op);
    }
    else if(indexType == IndexType::hash && colIndex == generateIndex && op == '='){
        print_where_hash(indices,target);
    }
    else{
        print_where_no_index(indices,colIndex,target,op);
    }
}

void Table:: print_where(const vector<size_t> &indices){
    string colName;
    char op;
    cin >> colName;
    optional<size_t>colIndex = col_name_to_index(colName);
    if(!colIndex){
        cout << "Error during PRINT: " << colName << " does not name a column in " << tableName << "\n";
        getline(cin,colName);
        return;
    }
    cin >> op;
    switch(colTypes[*colIndex]){
        case ColumnType::Bool:
        {
            bool b;
            cin >> b;
            Field temp(b);
            print_where_helper(op,*colIndex,indices,temp);
            break;
        }
        case ColumnType::String:
        {
            string s;
            cin >> s;
            Field temp(s);
            print_where_helper(op,*colIndex,indices,temp);
            break;
        }
        case ColumnType::Int:
        {
            int i;
            cin >> i;
            Field temp(i);
            print_where_helper(op,*colIndex,indices,temp);
            break;
        }
        case ColumnType::Double:
        {
            double d;
            cin >> d;
            Field temp(d);
            print_where_helper(op,*colIndex,indices,temp);
            break;
        }
    }
}

void Table :: print(){
    uint32_t numCols;
    string columnName;
    vector<size_t> indices;
    string condition;
    cin >> numCols;
    for(size_t i = 0; i < numCols; ++i){
        cin >> columnName;
        optional<size_t> colIndex = col_name_to_index(columnName);
        if(colIndex){
            indices.emplace_back(*colIndex);
        }
        else{
            cout << "Error during PRINT: " << columnName << " does not name a column in " << tableName << '\n';
        }
    }//will store the columns to read as thier indices in the 2d vector to be passed to the member functions
    cin >> condition;
    if(condition == "WHERE"){
        print_where(indices);
    }
    else{
        print_all(indices);
    }
}

void Table::generate(size_t colIndex, string type){
    generateIndex = colIndex;   //store the index the bst or hash is built off of in the table
    hashIndex.clear();  //empty the hash index before making a new one
    bstIndex.clear();   //empty the bst index before making a new one
    switch(type[0]){
        case 'h':
            for (size_t i = 0; i < data.size(); ++i) {
                hashIndex[data[i][colIndex]].emplace_back(i);
            }//for every row in the table add it to the unordered map
            indexType = IndexType::hash;
            cout << "Generated hash index for table " << tableName << " on column " << colNames[colIndex] << ", with " << hashIndex.size() << " distinct keys \n";
        break;
        case 'b':
            for (size_t i = 0; i < data.size(); ++i) {
                bstIndex[data[i][colIndex]].emplace_back(i);
            }//for every row in the table add it to the map
            indexType = IndexType::bst;
            cout << "Generated bst index for table " << tableName << " on column " << colNames[colIndex] << ", with " << bstIndex.size() << " distinct keys \n";
        break;
    }
}

void Table::join(Table &table2){
    string s;
    size_t numCol;
    uint32_t count = 0;
    cin >> s;//read name of column to compare from table 1
    auto targetCol1 = col_name_to_index(s);
    if(!targetCol1){
        cout << "Error during JOIN: " << s << " does not name a column in " << tableName << '\n';
        return;
    }
    cin >> s;   //get rid of =
    cin >> s;//read name of column to compare from table 2
    auto targetCol2 = table2.col_name_to_index(s);
    if(!targetCol2){
        cout << "Error during JOIN: " << s << " does not name a column in " << table2.tableName << '\n';
        return;
    }
    cin >> s >> s;//get rid of the AND PRINT
    cin >> numCol;//read number of columns to read
    vector<pair<size_t,size_t>> colToPrint;//first = table number, second = column number
    for(size_t i = 0; i < numCol; ++i){
        string colName;
        size_t tableNumber;
        cin >> colName;
        cin >> tableNumber;
        optional<size_t> colIndex;//index of a column to print
        if(tableNumber == 1){
            colIndex = col_name_to_index(colName);
        }
        else{
            colIndex = col_name_to_index(colName,table2);
        }
        if(colIndex){
            colToPrint.push_back({tableNumber,*colIndex});
        }
        else{
            if(tableNumber == 1){
                cout << "Error during JOIN: " << colName << " does not name a column in " << tableName << "\n";
            }
            else{
                cout << "Error during JOIN: " << colName << " does not name a column in " << table2.tableName << '\n';
            }
            getline(cin, colName);  //clear the line
            return;
        }
    }   //fills colToPrint
    if(!quiet){
        for(pair<size_t,size_t> &pair : colToPrint){
            if(pair.first == 1){
                cout << colNames[pair.second] << " ";
            }
            else{
                cout << table2.colNames[pair.second] << " ";
            }

        }
        cout << '\n';
    }
    unordered_map<Field,vector<size_t>> tempIndex;  //temporary index if we don't have a useful hashIndex
    if(table2.hashIndex.size() == 0 || table2.generateIndex != targetCol2){
        for(size_t i = 0; i < table2.data.size(); ++i){
            tempIndex[table2.data[i][*targetCol2]].emplace_back(i);
        }//for every row in table2
    }//if no useful hash Index exists on table 2 create a locally unordered map to use as index
    if(tempIndex.size() == 0){
        for(const vector<Field>&row1 : data){
            auto it = table2.hashIndex.find(row1[*targetCol1]); //a vector of every row in table 2 who's column matchs the target value
            if(it != table2.hashIndex.end()){
                for(size_t row2 : it->second){
                    if(!quiet){
                        for(pair<size_t,size_t> &pair : colToPrint){
                            if(pair.first == 1){
                                cout << row1[pair.second] << " ";  //print col to print from row we are at in table 1 loop 
                            }//if in first table
                            else{
                                cout << table2.data[row2][pair.second] << " "; //print col to print from 
                            }//if in second table
                        }//for every table print column in columns to print vector
                        cout << '\n';
                    }
                    ++count;
                }//for every row in table 2 that matchs current row in table 1
            }//if we found at least 1 row in table 1 that matchs a row in table 2 
        }//for every row in table 1
    }//if we have a prexisting index
    else{
    for(const vector<Field>&row1 : data){
        auto it = tempIndex.find(row1[*targetCol1]); //a vector of every row in table 2 who's column matchs the target value
        if(it != tempIndex.end()){
            for(size_t row2 : it->second){
                if(!quiet){
                    for(pair<size_t,size_t> &pair : colToPrint){
                            if(pair.first == 1){
                            cout << row1[pair.second] << " ";  //print col to print from row we are at in table 1 loop 
                            }//if in first table
                            else{
                                cout << table2.data[row2][pair.second] << " "; //print col to print from 
                            }//if in second table
                    }//for every table print column in columns to print vector
                    cout << '\n';
                }
                ++count;
            }//for every row in table 2 that matchs current row in table 1
        }//if we found at least 1 row in table 1 that matchs a row in table 2 
    }//for every row in table 1
    }
    cout << "Printed " << count << " rows from joining " << tableName << " to " << table2.tableName << '\n';
}

int main(int argc, char** argv){
    ios_base::sync_with_stdio(false); // you should already have this
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha; // add these two lines
    DataBase db;
    db.getOptions(argc,argv);
    db.read_command();
    return 0;
}