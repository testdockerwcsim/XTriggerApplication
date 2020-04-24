#ifndef test_vertices_H
#define test_vertices_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "GPUFunctions.h"

class test_vertices: public Tool {


 public:

  test_vertices();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  float m_distance_between_vertices;
  float m_wall_like_distance;
  float m_water_like_threshold_number_of_pmts;
  float m_wall_like_threshold_number_of_pmts;
  float m_coalesce_time;
  float m_trigger_gate_up;
  float m_trigger_gate_down;
  int m_output_txt;
  int m_correct_mode;
  int m_n_direction_bins_theta;
  bool m_cylindrical_grid;
  float m_costheta_cone_cut;
  bool m_select_based_on_cone;
  bool  m_trigger_threshold_adjust_for_noise;
  int m_max_n_hits_per_job;
  int m_num_blocks_y;
  int m_num_threads_per_block_y;
  int m_num_threads_per_block_x;
  int m_write_output_mode;

#ifdef GPU   
  /// integer times to run over GPU card
  std::vector<int> m_time_int;
#endif

  int m_verbose;

  std::stringstream m_ss;

  void StreamToLog(int level) {
    Log(m_ss.str(), level, m_verbose);
    m_ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};

};


#endif
