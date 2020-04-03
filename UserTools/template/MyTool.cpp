#include "MyTool.h"

MyTool::MyTool():Tool(){}


bool MyTool::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  Log("INFO: MyTool::Initialise() starting", INFO, m_verbose);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new Stopwatch() : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();


  m_data= &data;


  /// YOUR CODE HERE


  if(m_stopwatch) {
    m_stopwatch->Stop();
    ss << "INFO: MyTool::Initialise() run stats" << m_stopwatch->Result();
    StreamToLog(INFO);
  }

  Log("INFO: MyTool::Initialise() complete", INFO, m_verbose);

  return true;
}


bool MyTool::Execute(){
  Log("INFO: MyTool::Execute() starting", INFO, m_verbose);

  if(m_stopwatch) m_stopwatch->Start();

  //// YOUR CODE HERE

  if(m_stopwatch) m_stopwatch->Start();

  Log("INFO: MyTool::Execute() complete", INFO, m_verbose);

  return true;
}


bool MyTool::Finalise(){

  Log("INFO: MyTool::Finalise() starting", INFO, m_verbose);

  if(m_stopwatch) {
    ss << "INFO: MyTool::Execute() run stats" << m_stopwatch->Result(m_stopwatch_file);
    StreamToLog(INFO);
    m_stopwatch->Reset();
    m_stopwatch->Start();
  }

  //// YOUR CODE HERE

  if(m_stopwatch) {
    m_stopwatch->Stop();
    ss << "INFO: MyTool::Initialise() run stats" << m_stopwatch->Result();
    StreamToLog(INFO);
    delete m_stopwatch;
  }

  Log("INFO: MyTool::Finalise() complete", INFO, m_verbose);

  return true;
}
