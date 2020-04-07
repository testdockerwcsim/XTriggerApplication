#ifndef TRIGGERINFO_H
#define TRIGGERINFO_H

#include <iostream>
#include <vector>

#include "WCSimEnumerations.hh"

class TriggerInfo{

 public:

  TriggerInfo();

  void AddTrigger(TriggerType_t type, double readout_start_time, double readout_end_time, double mask_start_time, double mask_end_time, double trigger_time, std::vector<float> info);

  void AddTriggers(TriggerInfo * in);

  void Clear() ;

  unsigned int m_num_triggers;
  std::vector<TriggerType_t> m_type;
  std::vector<double>        m_readout_start_time;
  std::vector<double>        m_readout_end_time;
  std::vector<double>        m_mask_start_time;
  std::vector<double>        m_mask_end_time;
  std::vector<double>        m_trigger_time;
  std::vector<std::vector<float> > m_info;
};

#endif
