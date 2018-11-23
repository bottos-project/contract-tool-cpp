#include "contractcomm.hpp"

extern "C" {
    int start();
    int add();
    int del();
}

int start() 
{
    myprints("hello world in start");
    
    return 0;
}

int add() 
{
    myprints("hello world in add");
    
    return 0;
}

int del() 
{
    myprints("hello world in del");
    
    return 0;
}