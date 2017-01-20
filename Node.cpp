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
    Node()
    {	
      std::string cwd = getcwd(NULL, 0);
      std::cout << "Current working dir: " << cwd << std::endl;
      boost::filesystem::path p(cwd);
      boost::filesystem::path getNode(p.stem());
      std::cout << "Node is: " << getNode << std::endl;
    
      boost::filesystem::path dir(p.parent_path( ));
      boost::filesystem::path ext(getNode.string() + ".xy");
      boost::filesystem::path target (dir / ext);
    
      std::cout << "Target file is: " << target << std::endl;
      
      boost::filesystem::ifstream infile(target);
      infile >> latValue lonValue;
      std::time_t result = std::time(nullptr);
      
      latitude = latValue;
      longitude = lonValue;
      timestamp = result;	
    } 
    
    
    //get latitude
    long getLatitude()
    {
      return latitude;     
    }
    
    //get longitude
    long getLongitude(target)
    {
      return longitude;
    }
    
    //get timestamp 
    std::time_t getTimestamp()
    {
    return timestamp;
    }

};
