#ifndef EnergeticBONSAI_H
#define EnergeticBONSAI_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "WCSimEBonsai.h"

class EnergeticBONSAI: public Tool {


 public:

  EnergeticBONSAI();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  WCSimEBonsai * m_ebonsai;
  WCSimRootTrigger * m_trigger;
  int m_in_nhits;
  std::vector<int>   * m_in_PMTIDs;
  std::vector<float> * m_in_Ts;
  float m_vertex[3];

  unsigned int m_nhits_min;
  unsigned int m_nhits_max;


  int m_verbose;

  std::stringstream m_ss;

  void StreamToLog(int level) {
    Log(m_ss.str(), level, m_verbose);
    m_ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};


};


#endif
