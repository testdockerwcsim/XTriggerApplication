#include "WCSimRootEventReset.h"

WCSimRootEventReset::WCSimRootEventReset():Tool(){}


bool WCSimRootEventReset::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new util::Stopwatch("WCSimRootEventReset") : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();

  m_data= &data;


  /// YOUR CODE HERE


  if(m_stopwatch) Log(m_stopwatch->Result("Initialise"), INFO, verbose);

  return true;
}


bool WCSimRootEventReset::Execute(){

  if(m_stopwatch) m_stopwatch->Start();

  //Clear the WCSimRootEvents to prevent memory leaks
  // that happen if we've added more than 1 trigger to the
  m_data->IDWCSimEvent_Triggered->Clear();
  if(m_data->HasOD) {
    m_data->ODWCSimEvent_Triggered->Clear();

  if(m_stopwatch) m_stopwatch->Stop();

  return true;
}


bool WCSimRootEventReset::Finalise(){

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Execute", m_stopwatch_file), INFO, verbose);
    m_stopwatch->Start();
  }

  //// YOUR CODE HERE

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Finalise"), INFO, verbose);
    delete m_stopwatch;
  }

  return true;
}
