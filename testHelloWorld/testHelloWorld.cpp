#include "contractcomm.hpp"

extern "C" {
    int start();
}

int start() 
{
    myprints("hello world in start");
    
    return 0;
}
