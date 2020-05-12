#ifndef LEAFRecon_H
#define LEAFRecon_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "Stopwatch.h"

#include "BQFitter.hh"

class LEAFRecon: public Tool {


 public:

  LEAFRecon();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  /// The result of a LEAF fit
  BQFitter::FitterOutput m_leaf_result;
  /// Number of hits in a trigger
  int m_in_nhits;

  /// Below this number of hits in a trigger, don't run LEAF. Equality is run. Set in config file
  unsigned int m_nhits_min;
  /// Above this number of hits in a trigger, don't run LEAF. Equality is run. Set in config file
  unsigned int m_nhits_max;

  /// The stopwatch, if we're using one
  util::Stopwatch * m_stopwatch;
  /// Image filename to save the histogram to, if required
  std::string m_stopwatch_file;

  /// Verbosity level, as defined in tool parameter file
  int m_verbose;

  /// For easy formatting of Log messages
  std::stringstream m_ss;

  /// Print the current value of the streamer at the set log level,
  ///  then clear the streamer
  void StreamToLog(int level) {
    Log(m_ss.str(), level, m_verbose);
    m_ss.str("");
  }

  /// Log level enumerations
  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};


};


#endif