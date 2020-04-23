#include "test_vertices.h"

test_vertices::test_vertices():Tool(){}


bool test_vertices::Initialise(std::string configfile, DataModel &data){


  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_data->triggeroutput=false;



  
  std::string DetectorFile;
  std::string ParameterFile;
  

  m_variables.Get("DetectorFile",DetectorFile);
  m_variables.Get("ParameterFile",ParameterFile);
  
  m_variables.Get("distance_between_vertices",        m_distance_between_vertices);
  m_variables.Get("wall_like_distance",   m_wall_like_distance);
  m_variables.Get("water_like_threshold_number_of_pmts",   m_water_like_threshold_number_of_pmts);
  m_variables.Get("wall_like_threshold_number_of_pmts",   m_wall_like_threshold_number_of_pmts);
  m_variables.Get("coalesce_time",   m_coalesce_time);
  m_variables.Get("trigger_gate_up",   m_trigger_gate_up);
  m_variables.Get("trigger_gate_down",   m_trigger_gate_down);
  m_variables.Get("output_txt",   m_output_txt);
  m_variables.Get("correct_mode",   m_correct_mode);
  m_variables.Get("n_direction_bins_theta",   m_n_direction_bins_theta);
  m_variables.Get("cylindrical_grid",   m_cylindrical_grid);
  m_variables.Get("costheta_cone_cut",   m_costheta_cone_cut);
  m_variables.Get("select_based_on_cone",   m_select_based_on_cone);
  m_variables.Get("write_output_mode",   m_write_output_mode);
  m_variables.Get("trigger_threshold_adjust_for_noise",                   m_trigger_threshold_adjust_for_noise);
  m_variables.Get("max_n_hits_per_job",   m_max_n_hits_per_job);
  m_variables.Get("num_blocks_y",   m_num_blocks_y);
  m_variables.Get("num_threads_per_block_y",   m_num_threads_per_block_y);
  m_variables.Get("num_threads_per_block_x",   m_num_threads_per_block_x);

  printf(" DetectorFile %s \n ", DetectorFile.c_str());
  printf(" ParameterFile %s \n ",ParameterFile.c_str() );
  printf(" m_distance_between_vertices %f \n ", m_distance_between_vertices);
  printf(" m_wall_like_distance %f \n ", m_wall_like_distance);
  printf(" m_water_like_threshold_number_of_pmts %f \n ", m_water_like_threshold_number_of_pmts);
  printf(" m_wall_like_threshold_number_of_pmts %f \n ", m_wall_like_threshold_number_of_pmts);
  printf(" m_coalesce_time %f \n ", m_coalesce_time);
  printf(" m_trigger_gate_up %f \n ", m_trigger_gate_up);
  printf(" m_trigger_gate_down %f \n ", m_trigger_gate_down);
  printf(" m_output_txt %d \n ", m_output_txt);
  printf(" m_correct_mode %d \n ", m_correct_mode);
  printf(" m_n_direction_bins_theta %d \n ",   m_n_direction_bins_theta);
  printf(" m_cylindrical_grid %d \n ",   m_cylindrical_grid);
  printf(" m_costheta_cone_cut %f \n ",   m_costheta_cone_cut);
  printf(" m_select_based_on_cone %d \n ",   m_select_based_on_cone);
  printf(" m_write_output_mode %d \n ", m_write_output_mode);
  printf(" m_max_n_hits_per_job %d \n ", m_max_n_hits_per_job);
  printf(" m_trigger_threshold_adjust_for_noise %d \n ", m_trigger_threshold_adjust_for_noise);
  printf(" m_num_blocks_y %d \n ",   m_num_blocks_y);
  printf(" m_num_threads_per_block_y %d \n ",   m_num_threads_per_block_y);
  printf(" m_num_threads_per_block_x %d \n ",   m_num_threads_per_block_x);


  //  gpu_daq_initialize(PMTFile,DetectorFile,ParameterFile);

#ifdef GPU
  //  GPU_daq::test_vertices_initialize();


  std::vector<int> tube_no;
  std::vector<float> tube_x;
  std::vector<float> tube_y;
  std::vector<float> tube_z;
  for( std::vector<PMTInfo>::const_iterator ip=m_data->IDGeom.begin(); ip!=m_data->IDGeom.end(); ip++){
    tube_no.push_back(ip->m_tubeno);
    tube_x.push_back(ip->m_x);
    tube_y.push_back(ip->m_y);
    tube_z.push_back(ip->m_z);
  }

  

  GPU_daq::test_vertices_initialize_ToolDAQ(m_data->detector_length, m_data->detector_radius, m_data->pmt_radius, ParameterFile, tube_no, tube_x, tube_y, tube_z, m_data->IDPMTDarkRate*1000,
 m_distance_between_vertices,
 m_wall_like_distance,
 m_water_like_threshold_number_of_pmts,
 m_wall_like_threshold_number_of_pmts,
 m_coalesce_time,
 m_trigger_gate_up,
 m_trigger_gate_down,
 m_output_txt,
 m_correct_mode,
 m_n_direction_bins_theta,
 m_cylindrical_grid,
 m_costheta_cone_cut,
 m_select_based_on_cone,
 m_trigger_threshold_adjust_for_noise,
 m_max_n_hits_per_job,
 m_num_blocks_y,
 m_num_threads_per_block_y,
 m_num_threads_per_block_x,
 m_write_output_mode
);
#endif

  // can acess variables directly like this and would be good if you could impliment in your code

  float dark_rate;
  m_variables.Get("dark_rate",dark_rate);

  //to do this in your code instead of passing the three strings you could just do

  //gpu_daq_initialize(m_variables);

  // then in your code you can include 
  //#include "Store.h"
  //  gpu_daq_initialize(Store variables);

  //and pull them out with the get function in the same way 


  return true;
}


bool test_vertices::Execute(){


  std::vector<SubSample> & samples = m_data->IDSamples;

  for( std::vector<SubSample>::const_iterator is=samples.begin(); is!=samples.end(); ++is){
#ifdef GPU   

    std::vector<int> trigger_ns;
    std::vector<int> trigger_ts;
    std::vector<int> m_time_int;
    for(unsigned int i = 0; i < is->m_time.size(); i++) {
      m_time_int.push_back(is->m_time[i]);
    }
    GPU_daq::test_vertices_execute(is->m_PMTid, m_time_int, &trigger_ns, &trigger_ts);
    for(int i=0; i<trigger_ns.size(); i++){
      m_data->IDTriggers.AddTrigger(kTriggerUndefined,
				    trigger_ts[i] + m_trigger_gate_down, 
				    trigger_ts[i] + m_trigger_gate_up,
				    trigger_ts[i],
				    std::vector<float>(1, trigger_ns[i]));

      printf(" trigger! time  %d nhits %d \n", trigger_ts[i], trigger_ns[i]);
    }
#else
    ;
#endif
  }


  return true;

}


bool test_vertices::Finalise(){

#ifdef GPU
  GPU_daq::test_vertices_finalize();
#endif

  return true;
}
