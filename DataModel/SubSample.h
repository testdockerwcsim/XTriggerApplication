#ifndef SUBSAMPLE_H
#define SUBSAMPLE_H

#include <iostream>
#include <vector>
#include <stdint.h>

/// Type for relative hit times within a SubSample. Unit = ns
typedef float relative_time_t;

/// Type for absolute timestamps of a SubSample. Unit = ms
typedef int64_t timestamp_t;

/// Factor between unit of timestamp and hit times
const relative_time_t timestamp_to_relative_time = 1e6; //  = 1 ms/ns

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

};

#endif
