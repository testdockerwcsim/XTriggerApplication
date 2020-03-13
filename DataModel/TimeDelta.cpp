#include <cmath>
#include "TimeDelta.h"

TimeDelta::TimeDelta(double naive_ns){
  m_long_time = 0;
  m_short_time = naive_ns;
}

void TimeDelta::Normalize(){
  long_time_t long_diff = std::floor(m_short_time / s_long_time_unit);
  m_long_time += long_diff;
  m_short_time -= (long_diff * s_long_time_unit);
}

TimeDelta operator*(const TimeDelta& old_delta, double factor){
  TimeDelta new_delta(old_delta);
  new_delta.m_short_time *= factor;
  new_delta.m_long_time *= factor;
  // Make sure no information of m_long_time is lost
  double remainder = old_delta.m_long_time - (new_delta.m_long_time * factor);
  new_delta.m_short_time += (remainder * TimeDelta::s_long_time_unit);
  new_delta.Normalize();
  return new_delta;
}

TimeDelta operator*(double factor, const TimeDelta& old_delta){
  return old_delta * factor;
}

double operator/(const TimeDelta& left_delta, const TimeDelta& right_delta){
  double ratio = (left_delta.m_long_time * TimeDelta::s_long_time_unit)
                  + left_delta.m_short_time;
  ratio /= (right_delta.m_long_time * TimeDelta::s_long_time_unit)
                  + right_delta.m_short_time;
  return ratio;
}

TimeDelta operator+(const TimeDelta& left_delta, const TimeDelta& right_delta){
  TimeDelta new_delta(left_delta);
  new_delta.m_long_time += right_delta.m_long_time;
  new_delta.m_short_time += right_delta.m_short_time;
  new_delta.Normalize();
  return new_delta;
}

TimeDelta operator-(const TimeDelta& left_delta, const TimeDelta& right_delta){
  return left_delta + (-1.0 * right_delta);
}

// Unit constants
const TimeDelta TimeDelta::ps = TimeDelta(0.001);
const TimeDelta TimeDelta::ns = TimeDelta(1.0);
const TimeDelta TimeDelta::us = 1000 * TimeDelta::ns;
const TimeDelta TimeDelta::ms = 1000 * TimeDelta::us;
const TimeDelta TimeDelta::s = 1000 * TimeDelta::ms;
