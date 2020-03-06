#include "SubSample.h"
#include <cassert>

SubSample::SubSample(std::vector<int> PMTid, std::vector<SubSample::relative_time_t> time)
{
  assert(PMTid.size() == time.size());
  m_PMTid  = PMTid;
  m_time   = time;
  m_charge = std::vector<float>(PMTid.size(), 0.);
  m_timestamp = 0;
}

SubSample::SubSample(std::vector<int> PMTid, std::vector<SubSample::relative_time_t> time, std::vector<float> charge)
{
  assert(PMTid.size() == time.size() && PMTid.size() == charge.size());
  m_PMTid  = PMTid;
  m_time   = time;
  m_charge = charge;
  m_timestamp = 0;
}

SubSample::SubSample(std::vector<int> PMTid, std::vector<SubSample::relative_time_t> time, std::vector<float> charge, SubSample::timestamp_t timestamp)
{
  assert(PMTid.size() == time.size() && PMTid.size() == charge.size());
  m_PMTid  = PMTid;
  m_time   = time;
  m_charge = charge;
  m_timestamp = timestamp;
}

SubSample::relative_time_t SubSample::TimeDifference(SubSample::relative_time_t this_time, SubSample::timestamp_t other_timestamp, SubSample::relative_time_t other_time){
  relative_time_t sub_sample_difference = static_cast<relative_time_t>(m_timestamp - other_timestamp)
                                          * s_TIMESTAMP_TO_RELATIVE_TIME;
  return (this_time - other_time) + sub_sample_difference;
}

SubSample::relative_time_t SubSample::TimeDifference(SubSample::relative_time_t this_time, SubSample& other_sample, SubSample::relative_time_t other_time){
  return TimeDifference(this_time, other_sample.m_timestamp, other_time);
}

void SubSample::SortByTime(){
  //algorithm borrowed from WCSimWCDigi::SortArrayByHitTime()
  int i, j;
  relative_time_t save_time;
  int save_PMTid;
  double save_charge;

  for (i = 1; i < m_PMTid.size(); ++i) {
    save_time       = m_time[i];
    save_PMTid      = m_PMTid[i];
    save_charge     = m_charge[i];
    for (j = i; j > 0 && m_time[j-1] > save_time; j--) {
      m_time[j]     = m_time[j-1];
      m_PMTid[j]    = m_PMTid[j-1];
      m_charge[j]   = m_charge[j-1];
    }//j
    m_time[j]     = save_time;
    m_PMTid[j]    = save_PMTid;
    m_charge[j]   = save_charge;
  }//i
}
