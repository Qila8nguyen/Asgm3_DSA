#ifndef HASH_TABLE
#define HASH_TABLE

#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;
class HashTable;

class Element {
    public:
    string identifier;
    int scope;
    int decode, index;
    string type;
    int num_of_parameters;
    int collision;

    public:
    Element () {index = -1; identifier = ""; };
    Element (string id, int _sc, int num = 0): 
            identifier(id), scope (_sc), index (-1),
            type (""), num_of_parameters (num) , collision (0)              
    {
            string key = to_string (scope);
            for (int i = 0; i < (int)id.size() ; i++){
                int temp = identifier[i] - 48;
                key += to_string (temp);
            }

        decode = stoi (key);
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

    void insert (Element&);
    void print ();
    void deleteScope (int);
    int findElement (string);

    // return index of element
    int linearHash (Element& ele);
    int quadraticHash (Element& ele);
    int doubleHash (Element& ele);

};

#endif