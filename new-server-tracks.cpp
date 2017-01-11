/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2013,2015 Alexander Afanasyev
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <iostream>
#include <string>

// For string parsing
#include <ctime>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

class Server
{
public:
  Server(ndn::Face& face)
    : m_face(face)
    , m_baseName("/example/tracks")
    , m_counter(0)
  {
    m_face.setInterestFilter(m_baseName,
                             std::bind(&Server::onInterest, this, _2),
                             std::bind([] {
                                 std::cerr << "Prefix registered" << std::endl;
                               }),
                             [] (const ndn::Name& prefix, const std::string& reason) {
                               std::cerr << "Failed to register prefix: " << reason << std::endl;
                             });                
  }

private:
  void
  onInterest(const ndn::Interest& interest)
  {
    std::cerr << "<< interest for " << interest << std::endl;

    // create data packet with the same name as interest
    std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(interest.getName());
    
    
    // Get working directory and mobility file path 
    std::string cwd = getcwd(NULL, 0);
    std::cout << "Current working dir: " << cwd << std::endl;
    boost::filesystem::path p(cwd);
    boost::filesystem::path getNode(p.stem());
    std::cout << "Node is: " << getNode << std::endl;
    
    boost::filesystem::path dir(p.parent_path( ));
    boost::filesystem::path ext(getNode.string() + ".xy");
    boost::filesystem::path target (dir / ext);
    
    std::cout << "Target file is: " << target << std::endl;
    
    // Print node position (x,y)
    boost::filesystem::ifstream infile(target);
    float latValue, lonValue;
	infile >> latValue >> lonValue;
	std::time_t result = std::time(nullptr); // for epoch time

    /*
    std::cout << "Content payload: " << std::endl;
    std::cout << "    Node #: " << getNode << std::endl;
    std::cout << "    latitude: " << latValue << std::endl;
    std::cout << "    longitude: " << lonValue << std::endl;     
    std::cout << "    altitude: 00.00" << std::endl;
    std::cout << "    Timestamp: " << std::asctime(std::localtime(&result)) << std::endl;
    */
    
    /*
    static const std::string content = "Node #: " + getNode.string() +
				", latitude: " + std::to_string(latValue) +  
				", longitude: " + std::to_string(lonValue) +
				", altitude: 00.00" +
				", Timestamp: " + std::asctime(std::localtime(&result)) +
				os.str();
	*/
	// prepare and assign content of the data packet
	std::ostringstream os;
    os << "Content payload: " << std::endl
       << "    SequenceNo: " << (m_counter++) << std::endl
       << "    Node #: " << getNode << std::endl
       << "    latitude: " << latValue << std::endl
       << "    longitude: " << lonValue << std::endl
       << "    altitude: 00.00" << std::endl
	   << "    Timestamp: " << std::asctime(std::localtime(&result)) <<std::endl;
    std::string content = os.str();
	
	// print content 
    std::cout << content << std::endl;    
    
    data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

    // set metainfo parameters
    data->setFreshnessPeriod(ndn::time::seconds(1));

    // sign data packet
    m_keyChain.sign(*data);

    // make data packet available for fetching
    m_face.put(*data);
  }

private:
  ndn::Face& m_face;
  ndn::KeyChain m_keyChain;
  ndn::Name m_baseName;
  uint64_t m_counter;
};

int
main(int argc, char** argv)
{
  try {
    // create Face instance
    ndn::Face face;

    // create server instance
    Server server(face);

    // start processing loop (it will block forever)
    face.processEvents();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
