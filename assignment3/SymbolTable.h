#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Element {
    public:
    string identifier;
    int scope, index;
    long long decode ;
    string type;
    int num_of_parameters;
    string *typeArray;
    int collision;

    public:
    Element () {index = -1; identifier = ""; };
    Element (string id, int _sc, int num = -1): 
            identifier(id), scope (_sc), index (-1),
            type (""), num_of_parameters (num) , collision (0)              
    {
            string key = to_string (scope);
            for (int i = 0; i < (int)id.size() ; i++){
                int temp = identifier[i] - 48;
                key += to_string (temp);
            }

        decode = stoll (key);
        if (num != 0 && num != -1) {
            typeArray = new string [num];
            for (int i = 0; i< num;i++) {
                typeArray[i] = "";
            }
        }
        
    };

    bool operator== (Element b)
    {
        return (this->identifier == b.identifier && this->scope == b.scope);
    }

};

enum WhichHash{LINEAR,QUADRATIC,DOUBLE};

class HashTable {
    public:
    int size;
    int current_count;
    Element *opAdress;
    bool *haveElement;
    int c1,c2;
    WhichHash hash;

    public:
    HashTable (string command,int s, int a, int b){
        size = s; current_count = 0;
        c1 = a; c2 = b;
        opAdress = new Element [size];
        haveElement = new bool [size];
        for (int i=0;i<size;i++)
        {
            haveElement[i] = false;
        }
        if (command == "LINEAR"){
            hash = LINEAR;
        }
        else if (command == "QUADRATIC")
        {
            hash = QUADRATIC;
        }
        else if (command == "DOUBLE")
        {
            hash = DOUBLE;
        }
        
    }

    void insert (Element&, string);
    void print ();
    void printDemo ();
    void deleteScope (int);
    int findElement (string);

    // return index of element
    int linearHash (Element& ele);
    int quadraticHash (Element& ele);
    int doubleHash (Element& ele);

};

class SymbolTable
{
public:
    SymbolTable() {}
    void run(string filename);
};
#endif