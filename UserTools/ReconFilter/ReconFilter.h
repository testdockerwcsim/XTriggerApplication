#ifndef ReconFilter_H
#define ReconFilter_H

#include <string>
#include <iostream>

#include "Tool.h"

class ReconFilter: public Tool {


 public:

  ReconFilter();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:



  int verbose;

  std::stringstream ss;

  void StreamToLog(int level) {
    Log(ss.str(), level, verbose);
    ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};


};


#endif