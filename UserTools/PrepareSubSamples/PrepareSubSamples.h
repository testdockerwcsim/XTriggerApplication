#ifndef PrepareSubSamples_H
#define PrepareSubSamples_H

#include <string>
#include <iostream>

#include "Tool.h"

class PrepareSubSamples: public Tool {


 public:

  PrepareSubSamples();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  SubSample::timestamp_t m_sample_width;
  SubSample::timestamp_t m_sample_overlap;

  int verbose;

  std::stringstream ss;

  void StreamToLog(int level) {
    Log(ss.str(), level, verbose);
    ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};


};


#endif
