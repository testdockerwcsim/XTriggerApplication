#ifndef TRIGGERSCOMPARISON_H
#define TRIGGERSCOMPARISON_H

#include <string>
#include <iostream>

#include "Tool.h"

#include <vector>

#include "WCSimRootEvent.hh"

class TriggersComparison: public Tool {

 public:

  TriggersComparison();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  /// Number of hits in a trigger
  int m_in_nhits;
  /// PMT IDs of hits in a trigger
  std::vector<int>   * m_in_PMTIDs;
  /// Times of hits in a trigger
  std::vector<float> * m_in_Ts;
  /// Charges of hits in a trigger
  std::vector<float> * m_in_Qs;

  /// Below this number of hits in a trigger, don't run TriggersComparison. Equality is run. Set in config file
  unsigned int m_nhits_min;
  /// Above this number of hits in a trigger, don't run TriggersComparison. Equality is run. Set in config file
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
