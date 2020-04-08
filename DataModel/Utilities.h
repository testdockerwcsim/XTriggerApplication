#ifndef UTILITIES_H
#define UTILITIES_H

#include "TimeDelta.h"

namespace util {
  ///
  struct Window {
    int m_trigger_num;
    TimeDelta m_start;
    TimeDelta m_end;
    Window() {}
    Window(int trigger_num, TimeDelta start, TimeDelta end) :
      m_trigger_num(trigger_num), m_start(start), m_end(end) {}
  };

  ///
  static bool WindowSorter(const Window & lhs,
			   const Window & rhs) {
    return lhs.m_start < rhs.m_start;
  }

} //namespace util

#endif
