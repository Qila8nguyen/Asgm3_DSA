#include "hash.h"


int HashTable::linearHash (Element& ele)
{
    int hk = ele.decode % this->size;
    int index = hk;
    int i = 0;
    while (this->haveElement[index])
    {
        if (opAdress[index] == ele) return -1;
        index = (hk + c1*i) % size;
        i++;
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
        index = (hk + c1*i + c2*i*i) % this->size;
        i++;
    }
    ele.collision = i;
    return index;
}

int HashTable::doubleHash (Element& ele)
{
    int hk1 = ele.decode % this->size;
    int hk2 = 1 + ele.decode & (size - 2);
    int index = hk1;
    int i = 0;
    while (haveElement[index])
    {
        if (opAdress[index] == ele) return -1;
        index = (hk1 + c1*i*hk2) % size;
        i++;
    }

    ele.collision = i;  
    return index; 
}


void HashTable::print ()
{
    for (int i = 0; i<size;i++)
    {
        if (opAdress[i].index != -1)
        cout << opAdress[i].identifier <<"//" << opAdress[i].collision<<" ";
        else
        cout <<"-- ";
    }
}

void HashTable::deleteScope (int scope)
{
    for (int i =0; i < this->size;i++)
    {
        if (haveElement[i] && opAdress[i].scope == scope)
        {
            haveElement[i] = false;
            opAdress[i].index = -1;
        }
    }
}

int HashTable::findElement (string id)
{
    int res = -1;
    for (int i =0; i < this->size;i++)
    {
        if (haveElement[i] && opAdress[i].identifier == id)
        {
            if (opAdress[i].scope > res)
                res = opAdress[i].scope;
        }
    }
    return res;
}

void HashTable::insert(Element &ele)
{
    if (this->current_count >= this->size)
    {
        // throw ;
        cout <<"OVERFLOW"<<endl;
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
        //throw
        cout << "REDECLARED"<<endl;
        return;
;    }

    haveElement[index] = true; 
    ele.index = index;
    opAdress[index] = ele;   
}