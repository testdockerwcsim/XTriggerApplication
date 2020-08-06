#ifndef TriggerOutput_H
#define TriggerOutput_H

#include <string>
#include <iostream>
#include <fstream>

#include "Tool.h"

#include "TFile.h"
#include "TTree.h"

#include "Stopwatch.h"

class TriggerOutput: public Tool {


 public:

  TriggerOutput();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:


  /// Trigger on OD digits, rather than ID digits?
  bool m_trigger_OD;

  /// Output ROOT filename that this tool RECREATE's
  std::string m_output_filename;
  /// Output ROOT file
  TFile * m_output_file;
  /// Tree contain WCSimRootEvent(s), and the original WCSim filename / event number
  TTree * m_event_tree;

  /// The type of Trigger
  TriggerType_t the_type;
  /// The starting time of the trigger window
  float the_readout_start_time;
  /// The ending time of the trigger window
  float the_readout_end_time;
  /// The starting time of the hit mask
  float the_mask_start_time;
  /// The ending time of the hit mask
  float the_mask_end_time;
  /// The actual time of the trigger
  float the_trigger_time;

  /// The stopwatch, if we're using one
  util::Stopwatch * m_stopwatch;
  /// Image filename to save the histogram to, if required
  std::string m_stopwatch_file;

  int m_verbose;

  std::stringstream m_ss;

  void StreamToLog(int level) {
    Log(m_ss.str(), level, m_verbose);
    m_ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};

  float absolute_time(TimeDelta d);

};


#endif
