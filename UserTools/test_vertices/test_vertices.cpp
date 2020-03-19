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
  
  m_variables.Get("distance_between_vertices",        f_distance_between_vertices);
  m_variables.Get("wall_like_distance",   f_wall_like_distance);
  m_variables.Get("water_like_threshold_number_of_pmts",   f_water_like_threshold_number_of_pmts);
  m_variables.Get("wall_like_threshold_number_of_pmts",   f_wall_like_threshold_number_of_pmts);
  m_variables.Get("coalesce_time",   f_coalesce_time);
  m_variables.Get("trigger_gate_up",   f_trigger_gate_up);
  m_variables.Get("trigger_gate_down",   f_trigger_gate_down);
  m_variables.Get("output_txt",   f_output_txt);
  m_variables.Get("correct_mode",   f_correct_mode);
  m_variables.Get("write_output_mode",   f_write_output_mode);
  m_variables.Get("trigger_threshold_adjust_for_noise",                   f_trigger_threshold_adjust_for_noise);

  printf(" DetectorFile %s \n ", DetectorFile.c_str());
  printf(" ParameterFile %s \n ",ParameterFile.c_str() );
  printf(" f_distance_between_vertices %f \n ", f_distance_between_vertices);
  printf(" f_wall_like_distance %f \n ", f_wall_like_distance);
  printf(" f_water_like_threshold_number_of_pmts %f \n ", f_water_like_threshold_number_of_pmts);
  printf(" f_wall_like_threshold_number_of_pmts %f \n ", f_wall_like_threshold_number_of_pmts);
  printf(" f_coalesce_time %f \n ", f_coalesce_time);
  printf(" f_trigger_gate_up %f \n ", f_trigger_gate_up);
  printf(" f_trigger_gate_down %f \n ", f_trigger_gate_down);
  printf(" f_output_txt %d \n ", f_output_txt);
  printf(" f_correct_mode %d \n ", f_correct_mode);
  printf(" f_write_output_mode %d \n ", f_write_output_mode);
  printf(" f_trigger_threshold_adjust_for_noise %d \n ", f_trigger_threshold_adjust_for_noise);

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

  

  GPU_daq::test_vertices_initialize_ToolDAQ(DetectorFile, ParameterFile, tube_no, tube_x, tube_y, tube_z);
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


  //do stuff with m_data->Samples
  /*
  for( std::vector<SubSample>::const_iterator is=m_data->Samples.begin(); is!=m_data->Samples.end(); ++is){
    PMTids.push_back(is->m_PMTid);
    times.push_back(is->m_time);
  }
  */
  int the_output;
  //  the_output = CUDAFunction(m_data->Samples.at(0).m_PMTid, m_data->Samples.at(0).m_time);
#ifdef GPU   
  the_output =   GPU_daq::test_vertices_execute();
#endif
  m_data->triggeroutput=(bool)the_output;

  return true;
}


bool test_vertices::Finalise(){

#ifdef GPU
  GPU_daq::test_vertices_finalize();
#endif

  return true;
}
