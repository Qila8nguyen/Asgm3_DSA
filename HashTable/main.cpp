#include "hash.h"
#include "hashtable.cpp"

int main ()
{
    HashTable table ("QUADRATIC",10,2,3);
    Element a1 ("a1",1,0);
    Element a2 ("a2",1);
    Element a3 ("k",1,2);
    Element a4 ("a4",1,0);
    Element a5 ("n12",0);
    Element a6 ("a6",1,3);
    Element a7 ("t3",1,4);
    Element a8 ("b",0,5);
    Element a9 ("c",0,2);
    Element a ("a2",1);

    table.insert (a1);
    table.insert (a2);
    table.insert (a3);
    table.insert (a4);
    table.insert (a5);
    table.insert (a6);
    table.insert (a7);
    table.insert (a8);
    table.insert (a);
    table.print ();

    cout << table.findElement("t")<<endl;
    table.deleteScope (1);

    table.print();

    return 0;
}