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

  float f_distance_between_vertices;
  float f_wall_like_distance;
  float f_water_like_threshold_number_of_pmts;
  float f_wall_like_threshold_number_of_pmts;
  float f_coalesce_time;
  float f_trigger_gate_up;
  float f_trigger_gate_down;
  int f_output_txt;
  int f_correct_mode;
  int f_n_direction_bins_theta;
  bool f_cylindrical_grid;
  float f_costheta_cone_cut;
  bool f_select_based_on_cone;
  bool  f_trigger_threshold_adjust_for_noise;
  int f_max_n_hits_per_job;
  int f_num_blocks_y;
  int f_num_threads_per_block_y;
  int f_num_threads_per_block_x;
  int f_write_output_mode;



};


#endif
