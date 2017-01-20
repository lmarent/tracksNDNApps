#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>


class Node
{
  private:
    long latitude;
    long longitude;
    std::time_t timestamp;
  
  public:
    Node();    
    
    //get latitude
    long getLatitude();
    
    //get longitude
    long getLongitude();
    
    //get timestamp 
    std::time_t getTimestamp();

    std::string getContent();
  
};
