#ifndef TRIGGERINFO_H
#define TRIGGERINFO_H

#include <iostream>
#include <vector>

#include "WCSimEnumerations.hh"
#include "TimeDelta.h"

class TriggerInfo{

 public:

  TriggerInfo();

  void AddTrigger(TriggerType_t type, double starttime, double endtime, double triggertime, std::vector<float> info);

  void AddTrigger(TriggerType_t type, TimeDelta starttime, TimeDelta endtime, TimeDelta triggertime, std::vector<float> info);

  void AddTriggers(TriggerInfo * in);

  void Clear() ;

  void SortByStartTime();

  unsigned int m_N;
  std::vector<TriggerType_t> m_type;
  std::vector<TimeDelta>     m_starttime;
  std::vector<TimeDelta>     m_endtime;
  std::vector<TimeDelta>     m_triggertime;
  std::vector<std::vector<float> > m_info;
};





#endif
