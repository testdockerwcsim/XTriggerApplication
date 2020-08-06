#ifndef TRIGGERSCOMPARISON_H
#define TRIGGERSCOMPARISON_H

#include <string>
#include <iostream>
#include <fstream>

#include "Tool.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH1I.h"


class TriggersComparison: public Tool {

 public:

  TriggersComparison();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

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

  /// names of input ROOT trees
  std::string m_input_filename1;
  std::string m_input_filename2;
  /// input ROOT files
  TFile * m_input_file1;
  TFile * m_input_file2;
  /// input trees containing triggers
  TTree * m_triggers_tree1;
  TTree * m_triggers_tree2;
  /// input headers trees
  TTree * m_header_tree1;
  TTree * m_header_tree2;


  float m_interpose_interval1;
  float m_interpose_interval2;

  /// The current event number from file1
  long int m_current_event_num1;
  /// The first event number from file1 to read
  long int m_first_event_num1;
  /// The total number of events from file1
  long int m_n_events1;

  /// The current event number from file2
  long int m_current_event_num2;
  /// The first event number from file2 to read
  long int m_first_event_num2;
  /// The total number of events from file2
  long int m_n_events2;


  /// Output ROOT filename that this tool RECREATE's
  std::string m_output_filename;
  /// Output ROOT file
  TFile * m_output_file;

  /// output histograms
  TH1F * h_triggertime_1;
  TH1F * h_triggertime_2;

  TH1I * h_readouttime_1;
  TH1I * h_readouttime_2;

  TH1F * h_selections_intersection;

  /// trigger time
  float the_trigger_time1;
  float the_trigger_time2;
  float the_readout_start_time1;
  float the_readout_start_time2;
  float the_readout_end_time1;
  float the_readout_end_time2;

  float min_readout_time_1;
  float max_readout_time_1;
  float min_readout_time_2;
  float max_readout_time_2;

  float timebinsize;


};


#endif
