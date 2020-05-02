 #include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long int time= 0;
    long int hour = 0;
    long int min = 0 ; 
    long int sec = 0; 

    time = seconds ; 
    hour = time / 3600;
    time = time % 3600; 
    min = time / 60; 
    time = time% 60; 
    sec = time; 
    string mytime = std::to_string(hour) +":"+ std::to_string(min)+ ":" + std::to_string(sec); 

    return mytime ; 
     }