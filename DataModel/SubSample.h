#ifndef SUBSAMPLE_H
#define SUBSAMPLE_H

#include <iostream>
#include <vector>

class SubSample{

 public:

  SubSample() {};

  SubSample(std::vector<int> PMTid,std::vector<float> time);

  SubSample(std::vector<int> PMTid, std::vector<float> time, std::vector<float> charge);

  void Append(SubSample & sub);

  void Append(std::vector<int> PMTid, std::vector<float> time, std::vector<float> charge);

  std::vector<int> m_PMTid;
  std::vector<float> m_time;
  std::vector<float> m_charge;
  std::vector<int> m_time_int;
  std::vector<int> m_charge_int;

  /// Stores the trigger readout windows each hit is associated with
  std::vector<std::vector<int> > m_trigger_readout_windows;
  /// Is each hit masked?
  std::vector<bool> m_masked;
};





#endif
