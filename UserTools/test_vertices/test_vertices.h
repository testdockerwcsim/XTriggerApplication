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
  int f_write_output_mode;
  bool  f_trigger_threshold_adjust_for_noise;
  



};


#endif
