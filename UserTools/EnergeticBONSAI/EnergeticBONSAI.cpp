#include "EnergeticBONSAI.h"

EnergeticBONSAI::EnergeticBONSAI():Tool(){}


bool EnergeticBONSAI::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  verbose = 0;
  m_variables.Get("verbose", verbose);

  m_data= &data;

  return true;
}


bool EnergeticBONSAI::Execute(){

  return true;
}


bool EnergeticBONSAI::Finalise(){

  return true;
}
