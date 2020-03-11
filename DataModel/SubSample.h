#ifndef SUBSAMPLE_H
#define SUBSAMPLE_H

#include <iostream>
#include <vector>
#include <stdint.h>

class SubSample{

 public:

  /// Type for relative hit times within a SubSample. Unit = ns
  typedef float relative_time_t;

  /// Type for absolute timestamps of a SubSample. Unit = us
  typedef int64_t timestamp_t;

  /// Factor between unit of timestamp and hit times
  static const relative_time_t s_TIMESTAMP_TO_RELATIVE_TIME = 1e3; //  = 1 us/ns

  SubSample() {};
  SubSample(
    std::vector<int> PMTid,
    std::vector<relative_time_t> time,
    std::vector<float> charge = std::vector<float>(),
    timestamp_t timestamp = 0
  );

  /// Timestamp of the whole SubSample. Unit: us
  timestamp_t m_timestamp;

  /// Vector of PMT IDs for all hits in SubSample
  std::vector<int> m_PMTid;
  /// Vector of hit times relative to timestamp for all hits in SubSample. Unit: ns
  std::vector<relative_time_t> m_time;
  /// Vector of charges for all hits in SubSample. Unit: ?
  std::vector<float> m_charge;

  /// Calculate the time difference to between two relative times in this SubSample
  relative_time_t TimeDifference(relative_time_t this_time, relative_time_t other_time) {return this_time - other_time;};
  /// Calculate the time difference to between a relative times in this SubSample and a time relative to another timestamp
  relative_time_t TimeDifference(relative_time_t this_time, timestamp_t other_timestamp, relative_time_t other_time);
  /// Calculate the time difference to between a relative times in this SubSample and a realtive time in another SubSample
  relative_time_t TimeDifference(relative_time_t this_time, SubSample& other_sample, relative_time_t other_time);

  /// Sort all digits in the SubSample by their time
  void SortByTime();

  /// Split SubSample into multiple overlapping ones
  std::vector<SubSample> Split(timestamp_t target_width, timestamp_t target_overlap);
};

#endif
