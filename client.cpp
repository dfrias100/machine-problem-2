/* 
    File: client.cpp

    Author: Daniel Frias
            Department of Computer Science
            Texas A&M University
    Date  : 2020/09/24

    Client main program for MP2 in CSCE 313, derived from simpleclient.cpp
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <errno.h>
#include <unistd.h>

#include "reqchannel.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string int2string(int number) {
  std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

/*-----------------------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- GENERATE THE DATA (Borrowed from dataserver.cpp to measure overhead )*/
/*-----------------------------------------------------------------------------------------*/

std::string generate_data() {
  // Generate the data to be returned to the client.
  return int2string(rand() % 100);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  struct timeval t1;
  struct timeval t2;
  long diff_sec;
  long diff_usec;
  long total = 0;
  // File to hold the data for overhead
  std::ofstream fout;

  std::cout << "CLIENT STARTED:" << std::endl;

  /* -- Start sending a sequence of requests */

  if (fork() == 0) {
    execve("dataserver", NULL, NULL);
  } else {
    std::cout << "Establishing control channel... " << std::flush;
    RequestChannel chan("control", RequestChannel::Side::CLIENT);
    std::cout << "done." << std::endl;

    std::string reply1 = chan.send_request("hello");
    std::cout << "Reply to request 'hello' is '" << reply1 << "'" << std::endl;

    std::string reply2 = chan.send_request("data Joe Smith");
    std::cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << std::endl;

    std::string reply3 = chan.send_request("data Jane Smith");
    std::cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << std::endl;

    fout.open("data1.csv");
    
    for (int i = 0; i < 30; i++) {
      for(int i = 0; i < 100; i++) {
        std::string request_string("data TestPerson" + int2string(i));
        
        gettimeofday(&t1, 0);
        std::string reply_string = chan.send_request(request_string);
        gettimeofday(&t2, 0);
        diff_sec = t2.tv_sec - t1.tv_sec;
        diff_usec = t2.tv_usec - t1.tv_usec;
        if(diff_usec < 0) {
          diff_usec += 1000000;
          diff_sec--;
        }
        // This will output to data1.csv
        total += diff_sec*1000000 + diff_usec;
        //fout << diff_sec*1e6 + diff_usec << std::endl;
        std::cout << "reply to request " << i << ":" << reply_string << std::endl;;
      }
      double mean = total / 100.0;
      fout << mean << std::endl;
      total = 0;
    }

    fout.close();

    std::string reply4 = chan.send_request("quit");
    std::cout << "Reply to request 'quit' is '" << reply4 << std::endl;
  }

  fout.open("data2.csv");

  std::cout << "Local overhead for generate_data()" << std::endl;
  for (int i = 0; i < 30; i++) {
    for(int i = 0; i < 100; i++) {
        gettimeofday(&t1, 0);
        std::string d = generate_data();
        gettimeofday(&t2, 0);
        diff_sec = t2.tv_sec - t1.tv_sec;
        diff_usec = t2.tv_usec - t1.tv_usec;
        if(diff_usec < 0) {
          diff_usec += 1000000;
          diff_sec--;
        }
        // This will output to data2.csv
        total += diff_sec*1000000 + diff_usec;
    }
    double mean = total / 100.0;
    fout << mean << std::endl;
    total = 0;
  }

  fout.close();

  usleep(1000000);
}
