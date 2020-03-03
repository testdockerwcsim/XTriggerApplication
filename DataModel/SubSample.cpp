#include "SubSample.h"
#include <cassert>

SubSample::SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time)
{
  assert(PMTid.size() == time.size());
  m_PMTid  = PMTid;
  m_time   = time;
  std::vector<float> charge;
  for (int i; i<PMTid.size(); ++i){
    charge.push_back(0);
  }
  m_charge = charge;
  m_timestamp = 0;
}

SubSample::SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time, std::vector<float> charge)
{
  assert(PMTid.size() == time.size() && PMTid.size() == charge.size());
  m_PMTid  = PMTid;
  m_time   = time;
  m_charge = charge;
  m_timestamp = 0;
}

SubSample::SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time, std::vector<float> charge, timestamp_t timestamp)
{
  assert(PMTid.size() == time.size() && PMTid.size() == charge.size());
  m_PMTid  = PMTid;
  m_time   = time;
  m_charge = charge;
  m_timestamp = timestamp;
}
