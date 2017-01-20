#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Node.h"

Node::Node()
{	
  long latValue;
  long lonValue;
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
  
  std::string cmb;
  std::string cmb2;
  infile >> cmb >> cmb2;
  
  //infile >> latValue >> lonValue;
  std::time_t result = std::time(nullptr);
  
  latitude = std::stol(cmb);
  longitude = std::stol(cmb2);
  timestamp = result;	
} 
    
    
    //get latitude
long Node::getLatitude()
{
  return latitude;     
}

//get longitude
long Node::getLongitude()
{
  return longitude;
}

//get timestamp 
std::time_t Node::getTimestamp()
{
return timestamp;
}


