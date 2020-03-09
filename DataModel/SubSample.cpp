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

std::vector<SubSample> SubSample::Split(SubSample::timestamp_t target_width, SubSample::timestamp_t target_overlap){
  // SubSample width in relative units
  relative_time_t relative_width = static_cast<relative_time_t>(target_width)
                                            * s_TIMESTAMP_TO_RELATIVE_TIME;
  relative_time_t relative_overlap = static_cast<relative_time_t>(target_overlap)
                                            * s_TIMESTAMP_TO_RELATIVE_TIME;

  // Distance between SubSamples
  timestamp_t timestamp_stride = target_width - target_overlap;
  relative_time_t relative_stride = static_cast<relative_time_t>(timestamp_stride)
                                            * s_TIMESTAMP_TO_RELATIVE_TIME;

  // Ensure everything is sorted
  SortByTime();

  // The vector of samples to be returned
  std::vector<SubSample> split_samples;

  // Temporary information for storing digits that will be added to the samples
  std::vector<float> temp_charge;
  std::vector<int> temp_PMTid;
  std::vector<relative_time_t> temp_time;
  timestamp_t temp_timestamp = m_timestamp;

  // Set first SubSample timestamp according to first digit time
  // Make sure hit times are not negative:
  while ( TimeDifference(m_time.at(0), temp_timestamp, 0.) < 0.){
    temp_timestamp -= timestamp_stride;
  }
  // Make sure first SubSample is not empty
  while ( TimeDifference(m_time.at(0), temp_timestamp, 0.) > relative_stride){
    temp_timestamp += timestamp_stride;
  }

  // Add digits to new SubSamples
  for (int i = 0; i < m_time.size(); ++i){
    relative_time_t time_in_window = TimeDifference(m_time.at(i), temp_timestamp, 0.);
    if (time_in_window < relative_width){
      // Add digit to thin time window to current SubSample
      temp_time.push_back(time_in_window);
      temp_charge.push_back(m_charge[i]);
      temp_PMTid.push_back(m_PMTid[i]);
    } else {
      // Digit outside target window
      // Save current SubSample and rewind to prepare a new one at the overlap position
      split_samples.push_back(SubSample(temp_PMTid, temp_time, temp_charge, temp_timestamp));
      // Reset temporary vectors
      temp_PMTid.clear();
      temp_time.clear();
      temp_charge.clear();
      // Update timestamp
      while ( not (TimeDifference(m_time.at(i), temp_timestamp, 0.) < relative_width) ){
        temp_timestamp += timestamp_stride;
      }
      // Rewind index to cover overlap
      while ( TimeDifference(m_time.at(i), temp_timestamp, 0.) > 0. ){
        --i;
        // This will stop when `i` is just outside the new time window
        // Then `i` will get increased by one at the end of the loop
      }
    }
  }
  // Add final SubSample
  split_samples.push_back(SubSample(temp_PMTid, temp_time, temp_charge, temp_timestamp));

  return split_samples;
}
