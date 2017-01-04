/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

// correct way to include ndn-cxx headers
// #include <ndn-cxx/face.hpp>
// #include <ndn-cxx/security/key-chain.hpp>

#include "face.hpp"
#include "security/key-chain.hpp"

// For string parsing
#include <ctime>
#include <iostream>
#include <string>  
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace examples {

class Producer : noncopyable
{
public:
  void
  run()
  {
    m_face.setInterestFilter("/example/tracks",
                             bind(&Producer::onInterest, this, _1, _2),
                             RegisterPrefixSuccessCallback(),
                             bind(&Producer::onRegisterFailed, this, _1, _2));
    m_face.processEvents();
  }

private:
  void
  onInterest(const InterestFilter& filter, const Interest& interest)
  {
    std::cout << "<< I: " << interest << std::endl;

    // Create new name, based on Interest's name
    Name dataName(interest.getName());
    dataName
      .append("testApp") // add "testApp" component to Interest name
      .appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)
      //.appendTimestamp();
    
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

    std::cout << "Content payload: " << std::endl;
    std::cout << "    Node #: " << getNode << std::endl;
    std::cout << "    latitude: " << latValue << std::endl;
    std::cout << "    longitude: " << lonValue << std::endl;     
    std::cout << "    altitude: 00.00" << std::endl;
    std::cout << "    Timestamp: " << std::asctime(std::localtime(&result)) << std::endl;
    
    
    // print content    
    static const std::string content = "Node #: " + getNode.string() +
				", latitude: " + std::to_string(latValue) +  
				", longitude: " + std::to_string(lonValue) +
				", altitude: 00.00" +
				", Timestamp: " + std::asctime(std::localtime(&result));
    //std::cout << content << std::endl;

    // Create Data packet
    shared_ptr<Data> data = make_shared<Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(time::seconds(1));
    data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

    // Sign Data packet with default identity
    m_keyChain.sign(*data);
    // m_keyChain.sign(data, <identityName>);
    // m_keyChain.sign(data, <certificate>);

    // Return Data packet to the requester
    std::cout << ">> D: " << *data << std::endl;
    m_face.put(*data);
  }


  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix \""
              << prefix << "\" in local hub's daemon (" << reason << ")"
              << std::endl;
    m_face.shutdown();
  }

private:
  Face m_face;
  KeyChain m_keyChain;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{ 
  ndn::examples::Producer producer;
  try {
    producer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
