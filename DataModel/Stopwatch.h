#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <string>
#include <vector>

#include "TStopwatch.h"

struct StopwatchTimes
{
  double cpu_time;
  double real_time;
};

class Stopwatch
{
 public:

  Stopwatch() {};

  /// Start the stopwatch
  void   Start();
  /// Stop the stopwatch, returning the CPU time
  StopwatchTimes Stop();
  /// Get the formatted results, including min, max, average, total
  ///  If output_file has a length, will save a histogram to a pdf
  std::string Result(std::string output_file = "");
  /// Stop the stopwatch, and clear the results vector
  void Reset();
 private:
  /// The external stopwatch timer
  TStopwatch  m_sw;
  /// Stores time at every Stop
  std::vector<StopwatchTimes> m_results;
};

#endif //STOPWATCH_H
