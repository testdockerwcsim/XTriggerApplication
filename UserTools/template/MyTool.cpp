#include "MyTool.h"

MyTool::MyTool():Tool(){}


bool MyTool::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  Log("INFO: MyTool::Initialise() starting", INFO, m_verbose);

  m_data= &data;

  Log("INFO: MyTool::Initialise() complete", INFO, m_verbose);

  return true;
}


bool MyTool::Execute(){

  Log("INFO: MyTool::Execute() starting", INFO, m_verbose);


  Log("INFO: MyTool::Execute() complete", INFO, m_verbose);

  return true;
}


bool MyTool::Finalise(){

  Log("INFO: MyTool::Finalise() starting", INFO, m_verbose);


  Log("INFO: MyTool::Finalise() complete", INFO, m_verbose);

  return true;
}
