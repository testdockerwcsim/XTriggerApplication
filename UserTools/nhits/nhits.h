#ifndef nhits_H
#define nhits_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "GPUFunctions.h"

class NHits: public Tool {

 public:

  NHits();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();

 private:
  /// Width of the sliding window, in ns
  float m_trigger_search_window;
  /// Trigger threshold - number of digits must be above this value (equal to does not fire the trigger)
  unsigned int m_trigger_threshold;
  /// Pre-trigger time for saving digits in ns
  float m_trigger_save_window_pre;
  /// Post-trigger time for saving digits in ns
  float m_trigger_save_window_post;
  /// Trigger on OD digits, rather than ID digits?
  bool m_trigger_OD;

  /// CPU version of the NDigits algorithm
  void AlgNDigits(const SubSample * samples);

  /// An arbitrary long time to use in loop, ns
  static const relative_time_t s_LONG_TIME = (2 << 31) - 1;

  int m_verbose;

  std::stringstream m_ss;

  void StreamToLog(int level) {
    Log(m_ss.str(), level, m_verbose);
    m_ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};
};


#endif
