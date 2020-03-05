#include "SubSample.h"
#include <cassert>

SubSample::SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time)
{
  assert(PMTid.size() == time.size());
  m_PMTid  = PMTid;
  m_time   = time;
  m_charge = std::vector<float>(PMTid.size(), 0.);
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

relative_time_t SubSample::TimeDifference(relative_time_t this_time, SubSample& other_sample, relative_time_t other_time){
  /// Factor between unit of timestamp and hit times
  const relative_time_t timestamp_to_relative_time = 1e6; //  = 1 ms/ns
  relative_time_t sub_sample_difference = static_cast<relative_time_t>(m_timestamp - other_sample.m_timestamp)
                                          * timestamp_to_relative_time;
}
