#ifndef SUBSAMPLE_H
#define SUBSAMPLE_H

#include <iostream>
#include <vector>
#include <stdint.h>

/// Type for relative hit times within a SubSample. Unit = ns
typedef float relative_time_t;

/// Type for absolute timestamps of a SubSample. Unit = ms
typedef int64_t timestamp_t;

class SubSample{

 public:
  SubSample() {};
  SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time);
  SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time, std::vector<float> charge);
  SubSample(std::vector<int> PMTid, std::vector<relative_time_t> time, std::vector<float> charge, timestamp_t timestamp);

  /// Timestamp of the whole SubSample. Unit: ms
  timestamp_t m_timestamp;

  /// Vector of PMT IDs for all hits in SubSample
  std::vector<int> m_PMTid;
  /// Vector of hit times relative to timestamp for all hits in SubSample. Unit: ns
  std::vector<relative_time_t> m_time;
  /// Vector of charges for all hits in SubSample. Unit: ?
  std::vector<float> m_charge;

  /// Calculate the time difference to between two relative times in this SubSample
  relative_time_t TimeDifference(relative_time_t this_time, relative_time_t other_time) {return this_time - other_time;};
  /// Calculate the time difference to between a relative times in this SubSample and a realtive time in another SubSample
  relative_time_t TimeDifference(relative_time_t this_time, SubSample& other_sample, relative_time_t other_time);

  /// Sort all digits in the SubSample by their time
  void SortByTime();
};

#endif
