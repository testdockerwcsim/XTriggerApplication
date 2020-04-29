#ifndef DataOut_H
#define DataOut_H

#include <string>
#include <iostream>
#include <map>

#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootOptions.hh"

#include "TFile.h"
#include "TTree.h"

#include "Tool.h"

class DataOut: public Tool {


 public:

  DataOut();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:
  void CreateSubEvents(WCSimRootEvent * WCSimEvent);
  void FinaliseSubEvents(WCSimRootEvent * WCSimEvent);
  void RemoveDigits(WCSimRootEvent * WCSimEvent,
		    std::map<int, std::map<int, bool> > & NDigitPerPMTPerTriggerMap);
  void MoveTracks(WCSimRootEvent * WCSimEvent);
  int  TimeInTriggerWindow(TimeDelta time);
  unsigned int TimeInTriggerWindowNoDelete(TimeDelta time);

  /// Output ROOT filename that this tool RECREATE's
  std::string m_output_filename;
  /// Output ROOT file
  TFile m_output_file;
  /// Tree contain WCSimRootEvent(s), and the original WCSim filename / event number
  TTree * m_event_tree;
  TTree * m_tree_geom;
  TTree * m_tree_options;

  /// Combined list of triggers from all sources (ID+OD)
  TriggerInfo * m_all_triggers;
  /// A time used to offset all digit times. Set by config file
  double m_trigger_offset;

  /// Current event number
  int m_event_num;

  /// If true, saves digits that failed the trigger, rather those that passed
  bool m_save_only_failed_digits;
  /// If false, only 1 digit is allowed to be saved per trigger, rather than all digits from that trigger
  bool m_save_multiple_hits_per_trigger;
  /// For each PMT, for each trigger, has an ID digit been saved already?
  std::map<int, std::map<int, bool> > m_id_nhits_per_pmt_per_trigger;
  /// For each PMT, for each trigger, has an OD digit been saved already?
  std::map<int, std::map<int, bool> > m_od_nhits_per_pmt_per_trigger;

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
