#include "MyTool.h"

MyTool::MyTool():Tool(){}


bool MyTool::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  m_data= &data;

  return true;
}


bool MyTool::Execute(){

  return true;
}


bool MyTool::Finalise(){

  return true;
}
