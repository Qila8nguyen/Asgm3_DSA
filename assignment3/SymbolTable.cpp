#include "SymbolTable.h"

void insertElementToTable (string, string, HashTable&, int);
void assignExistedElement (string, string, HashTable&) ;
void lookupElementInScope (string, string, HashTable&);
int callExpression (string, string, HashTable&, int&);
int nonvoidFuntion (string line, string function, HashTable& table, int& total);
int fill_paraType (string line, HashTable &table,int size, string *&arr, Element &ele, string parameter);
void removeAllNodeInScope (string, HashTable&, int scope);

string getFirstSplitString (string line)
{
    size_t pos = line.find_first_of (' ');

    if (pos == string::npos) return line;

    return line.substr(0,pos);
}

string noCommandLine (string line)
{
    size_t pos = line.find(' ');
    if (pos == string::npos) return "";
    return line.erase(0,pos+1);
}


void SymbolTable::run(string filename)
{
    ifstream inputFile (filename);
    string line;
    int scope = 0;

    // // get first line for which hasing
    getline (inputFile, line);
    string hash = getFirstSplitString (line);
    if (hash != "LINEAR" && hash != "QUADRATIC" && hash != "DOUBLE") {
        throw InvalidInstruction (line);
    }
    line.erase (0,hash.size()+1);
    string tempsize = getFirstSplitString (line);
    line.erase (0,tempsize.size()+1);
    string tempc1 = getFirstSplitString(line);
    int c2 = 0;
    if (tempc1 != line) {
        if (hash == "LINEAR" || hash == "DOUBLE") 
            throw InvalidInstruction (line);
        line.erase (0,tempc1.size()+1);
        c2 = stoi (line);
    } 

    // string hash ="";
    // int size =0, c1 =0,c2 =0;
    // inputFile>>hash>>size>>c1>>c2;
    // HashTable hashTable (hash,size,c1,c2);
    HashTable hashTable (hash,stoi(tempsize),stoi(tempc1), c2);


    while (getline(inputFile,line)) {
        string command = getFirstSplitString (line);
        string noCommand = noCommandLine (line);
        if (command == "INSERT") 
            insertElementToTable (line, noCommand, hashTable, scope);
        else if (command == "ASSIGN") 
            assignExistedElement (line, noCommand, hashTable); // assign in same scope nếu kh thì kiếm scope giảm
        else if (command == "CALL")
            {
                int sum = 0;
                sum = callExpression (line, noCommand, hashTable,sum);
            }
        else if (command == "LOOKUP") 
            lookupElementInScope (line,noCommand, hashTable);
        else if (command == "BEGIN") 
            {
                scope++;
                if (noCommand != "")
                {
                    std::cout <<noCommand<<endl;
                    throw InvalidInstruction(line);
                }
            }
        else if (command == "END") 
            {
                if (noCommand != "")
                {
                    throw InvalidInstruction(line);
                }

                --scope;
                if (scope < 0)
                {
                    throw UnknownBlock ();
                }
                removeAllNodeInScope (line, hashTable, scope+1);
            }
        else if (command == "PRINT") 
            {
                // hashTable.printDemo();
                hashTable.print();
            }
        else
        {
            throw InvalidInstruction (line);
        }
    }
    

        if (scope > 0)
        {
            throw UnclosedBlock (scope);
        }
}

bool isNumber (string value)
{
    int size = value.size();
    for (int i = 0; i<size; i++)
    {
        char c = value[i];
        if (c > '9' || c < '0')
        {
            return false;
        }        
    }
    return true;
}

bool isString (string value)
{
    int size = value.size();
    if (value[0] !='\''|| value[size-1] != '\'' ) return false;

    for (int i = 1; i<size-1; i++)
    {
        char c = value[i];

        if (c <= 'Z' && c >='A')    continue;
        else if (c <= 'z' && c >= 'a') continue;
        else if ( c == ' ') continue;
        else if ( c <= '9' && c >= '0') continue;
        else
        {
            return false;
        }
    }
    return true;
}

bool checkID (string id)
{
    if (id == "number" || id == "string" || id == "false" || id == "true")  return false;
    if (id[0] >= 'a' && id[0] <= 'z') 
    {
        int size = id.size();
        for (int i=1;i<size;i++)
        {
            if (id[i] >= 'a' && id[i] <= 'z') continue;
            else if (id[i] >= '0' && id[i] <= '9') continue;
            else if (id[i] >= 'A' && id[i] <= 'Z') continue;
            else if (id[i] == '_') continue;
            else
            return false;
        }
        return true;
    }

    return false;
}

void insertElementToTable (string line, string noCommand, HashTable& hashTable, int scope) {
    string id = getFirstSplitString(noCommand);
    int para_num = -1;
    if (id != noCommand) {
        string para = noCommand.erase(0,id.size()+1);
        if (para.find(" ") != string::npos) {
            throw InvalidInstruction (line);
        }

        para_num = stoi (para);
    }

    // if (para_num >= 0 && scope != 0) {
    //     throw InvalidDeclaration (id);
    // }
    Element ele (id,scope,para_num);
    hashTable.insert(ele, line);
    std::cout << ele.collision<<endl;
}

int nonvoidFuncOrVariable (string line, string function, HashTable& table, int& total) {
    size_t open = function.find ("(");
    size_t close = function.find (")");
    string func_name = function.substr(0,open);
    int index = table.findElement (func_name);
    if (index == -1) 
        throw Undeclared (func_name);
    if (func_name == function) {  //this is variable not function
        return index;
    }
    string parameters = function.substr(open+1, close - open - 1);
    if (func_name +"(" + parameters +")" != function) {
        throw InvalidInstruction (line);
    }
   
    int para_num = table.opAdress[index].num_of_parameters;
    total += table.opAdress[index].collision;   
    int para_collision = fill_paraType (line,table,para_num, table.opAdress[index].typeArray,table.opAdress[index], parameters);
    total += para_collision;

    // if (table.opAdress[index].type == "void") {
    //     throw TypeMismatch(line);
    // }
    return index;
}

void assignExistedElement (string line, string noCommand, HashTable& table) {
    string id = getFirstSplitString(noCommand);
    if (!checkID(id)){
        throw InvalidDeclaration(id);
    }
    if (id == noCommand) {
        throw InvalidInstruction (line);
    }
    
    string value = noCommand.erase (0,id.size()+1);
    int idSlot = table.findElement(id), valueSlot = 0;
    int res =0; string idType = "no";
    if (idSlot != -1) {
        // throw Undeclared(id);
        res = table.opAdress[idSlot].collision;
        idType = table.opAdress[idSlot].type;
    }

    // if ( table.opAdress[idSlot].num_of_parameters != -1) {
    //     throw TypeMismatch (line);
    // }
    if (isString(value)) {
        if (idType == "no") {
            throw Undeclared (id);
        }
        if (idType == "number") {
            throw TypeMismatch (line);
        }
        if (idType == "") {
            idType = "string";
        }
    }
    else if (isNumber(value)) {
        if (idType == "no") {
            throw Undeclared (id);
        }
        if (idType == "string") {
            throw TypeMismatch (line);
        }
        if (idType == "") {
            idType = "number";
        }
    }
    else {
        valueSlot = nonvoidFuncOrVariable (line,value,table,res);
        string valueType = table.opAdress[valueSlot].type;
        if (idType == "no") {
            throw Undeclared (id);
        }
        if ( table.opAdress[idSlot].num_of_parameters != -1 || valueType == "void") {
            throw TypeMismatch (line);
        }
        if (valueType == "" && idType == "") {
            throw TypeCannotBeInferred (line);
        }
        if ((valueType == "number" && idType =="string") || (valueType == "string" && idType =="number") ) {
            throw TypeMismatch (line);
        }
        if (valueType == "") {
            valueType = idType;
        }
        else if (idType == "") {
            idType = valueType;
        }
        table.opAdress[valueSlot].type = valueType;
    } 

    table.opAdress[idSlot].type = idType;
    std::cout <<res<<endl;
}
void lookupElementInScope (string line, string noCommand, HashTable& table) {
    if (noCommand.find(" ") != string::npos) {
        throw InvalidInstruction (line);
    }
    int slot = table.findElement (noCommand);
    if (slot == -1) {
        throw Undeclared (noCommand);
    }
    std::cout << slot <<endl;
}
void removeAllNodeInScope (string line, HashTable& table, int scope)
{   
    table.deleteScope(scope);
}

int fill_paraType (string line, HashTable &table,int size, string *&arr, Element &ele, string parameter) {
    int total = 0;
    for (int i=0;i<size;i++) {
        size_t comma = parameter.find(",");
        string eachpara = "";
        bool last = false;
        if (comma != string::npos) eachpara = parameter.substr (0,comma);
        else
        {
            if (i != size-1){
                throw TypeMismatch (line); 
            }
            last  = true;
            eachpara = parameter;
        }
            if (isString(eachpara)) {
                if (arr[i] == "" )    arr[i] = "string";
                else if (arr[i] != "string") {
                    throw TypeMismatch (line);
                }

            }
                
            else if (isNumber(eachpara)) {
                if (arr[i] == "")   arr[i] = "number";
                else if (arr[i] != "number") {
                    throw TypeMismatch (line);
                }
            }
                
            else if (checkID(eachpara))
            {
                int index = table.findElement(eachpara);
                if (index == -1)
                {
                    throw Undeclared (eachpara);
                }
                Element ele = table.opAdress[index];
                string eleType = ele.type;
                if (ele.num_of_parameters >= 0) {
                    throw TypeMismatch (line);
                }
                if (eleType == "" && arr[i] == "") {
                    throw TypeCannotBeInferred (line);
                }
                if (eleType == "") {
                    table.opAdress[index].type = arr[i];
                }
                else if (arr[i] == "") {
                    arr[i] = eleType;
                }
                else if (arr[i] != eleType)
                {
                    throw TypeMismatch(line);
                }
                total += table.opAdress[index].collision;
            }

            else 
            {
                throw InvalidInstruction (line);
            }
        if (!last) parameter.erase (0,comma+1);
        else    parameter.erase(0,eachpara.size());
    }
    if (!parameter.empty()) {
        throw TypeMismatch(line);
    }
    return total;
}

int callExpression (string line, string noCommand, HashTable& table, int& total) {
    size_t open = noCommand.find ("(");
    size_t close = noCommand.find (")");
    string func_name = noCommand.substr(0,open);
    string parameters = noCommand.substr(open+1, close - open - 1);
    if (func_name + "(" + parameters +")" != noCommand) {
        throw InvalidInstruction (line);
    }
    int index = table.findElement (func_name);
    if (open == close) {
        throw InvalidInstruction (line);
    }
    if (index == -1) 
        throw Undeclared (func_name);
    int para_num = table.opAdress[index].num_of_parameters;
    if (para_num == -1) {
        throw TypeMismatch (line);
    }
    total += table.opAdress[index].collision;
    if (table.opAdress[index].type != "void" && table.opAdress[index].type != "") {
        throw TypeMismatch(line);
    }
    int para_collision = fill_paraType (line,table,para_num, table.opAdress[index].typeArray,table.opAdress[index], parameters);


    total += para_collision;
    table.opAdress[index].type = "void";  
    cout <<total<<endl;
    return index;
}

int HashTable::linearHash (Element& ele)
{
    int hk = ele.decode % this->size;
    int index = hk;
    int i = 0;
    while (this->haveElement[index])
    {
        if (opAdress[index] == ele) return -1;
        i++;
        index = (hk + c1*i) % size;
        if (i > size) return -2;        
    }
    ele.collision = i;
    return index; 
}

int HashTable::quadraticHash (Element& ele)
{
    int hk = ele.decode % size;
    int index = hk;
    int i = 0;
    while (haveElement[index])
    {
        if (opAdress[index] == ele) return -1;
        i++;
        index = (hk + c1*i + c2*i*i) % this->size;   
        if (i > size) return -2;
    }
    ele.collision = i;
    return index;
}

int HashTable::doubleHash (Element& ele)
{
    int hk1 = ele.decode % this->size;
    int hk2 = 1 + ele.decode % (size - 2);
    int index = hk1;
    int i = 0;
    while (haveElement[index])
    {
        if (opAdress[index] == ele) return -1;
        i++;
        index = (hk1 + c1*i*hk2) % size;
        if (i > size) return -2;
    }

    ele.collision = i;  
    return index; 
}


void HashTable::print ()
{
    int track = 0;
    for (int i = 0; i<size && track < current_count;i++)
    {
        if (haveElement[i]) {
            std::cout << opAdress[i].index <<" "<< opAdress[i].identifier <<"//" <<opAdress[i].scope;
            if (track != current_count - 1)
                cout <<";"; 
            track++;
        }      
    }
    if (current_count != 0) cout <<endl;
}

void HashTable::printDemo ()
{
    cout <<"cur count =" <<current_count<<endl;
    for (int i = 0; i<size;i++)
    {
        if (opAdress[i].index != -1)
        cout << opAdress[i].identifier <<"//" <<opAdress[i].type<<" " <<opAdress[i].scope<<";";
        else
        cout <<"-- ";
    }
    std::cout <<endl;
}

void HashTable::deleteScope (int scope)
{
    for (int i =0; i < this->size;i++)
    {
        if (haveElement[i] && opAdress[i].scope == scope)
        {
            haveElement[i] = false;
            opAdress[i].index = -1;
            current_count--;
        }
    }
}

int HashTable::findElement (string id)
{
    int res = -1;
    int index = -1;
    for (int i =0; i < this->size;i++)
    {
        if (haveElement[i] && opAdress[i].identifier == id)
        {
            if (opAdress[i].scope > res) {
                res = opAdress[i].scope;
                index = i;
            }                
        }
    }
    return index;
}

void HashTable::insert(Element &ele, string line)
{
    if (this->current_count >= this->size)
    {
        throw Overflow (line);
    }

    int index = -1;
    if (hash == LINEAR)
    {
        index = this->linearHash (ele);
    }
    else if (hash == QUADRATIC)
    {
        index = this->quadraticHash (ele);
    }
    else if (hash == DOUBLE)
    {
        index = this->doubleHash (ele);
    }

    if (index == -1)
    {
        throw Redeclared (ele.identifier);
    }
    if (index == -2) 
    {
        throw Overflow (line);
    }
    if (ele.num_of_parameters >= 0 && ele.scope != 0) {
        throw InvalidDeclaration (ele.identifier);
    }
    current_count++;
    haveElement[index] = true; 
    ele.index = index;
    opAdress[index] = ele;   
}