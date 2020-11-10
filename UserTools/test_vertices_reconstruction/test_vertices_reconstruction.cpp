#include "test_vertices_reconstruction.h"

test_vertices_reconstruction::test_vertices_reconstruction():Tool(){}


bool test_vertices_reconstruction::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new util::Stopwatch("test_vertices_reconstruction") : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();

  m_data= &data;


  m_variables.Get("nhitsmax", m_nhits_max);

  //allocate memory for the hit vectors
  m_in_PMTIDs = new std::vector<int>  (m_nhits_max);
  m_in_Ts     = new std::vector<float>(m_nhits_max);
  m_in_Qs     = new std::vector<float>(m_nhits_max);

  if(m_stopwatch) Log(m_stopwatch->Result("Initialise"), INFO, m_verbose);

  return true;
}


bool test_vertices_reconstruction::Execute(){

  if(m_stopwatch) m_stopwatch->Start();

  float out_vertex[3]={0,0,0};
  float out_direction[6]={0,0,0,0,0,0};
  float out_cosine[3]={0,0,0};
  float out_maxlike[3]={0,0,0};
  double dout_vertex[3]={0,0,0}; 
  double dout_direction[3]={0,0,0};
  double dout_cone[2]={0,0};

  unsigned int num_triggers = m_data->IDTriggers.m_num_triggers;
  std::vector<std::vector<float> > info = m_data->IDTriggers.m_info;

  //loop over ID triggers
  for (int itrigger = 0; itrigger < num_triggers; itrigger++) {
    //clear the previous triggers' hit information
    m_in_PMTIDs->clear();
    m_in_Ts->clear();
    m_in_Qs->clear();
    std::vector<float> the_info = info[itrigger];

    //Loop over ID SubSamples
    for(std::vector<SubSample>::iterator is = m_data->IDSamples.begin(); is != m_data->IDSamples.end(); ++is){

      //loop over all hits
      const size_t nhits_in_subsample = is->m_time.size();
      //starting at m_first_unique, rather than 0, to avoid double-counting hits
      // that are in multiple SubSamples
      for(size_t ihit = is->m_first_unique; ihit < nhits_in_subsample; ihit++) {
	//see if the hit belongs to this trigger
	if(std::find(is->m_trigger_readout_windows[ihit].begin(),
		     is->m_trigger_readout_windows[ihit].end(),
		     itrigger) == is->m_trigger_readout_windows[ihit].end())
	  continue;

	//it belongs. Add it to the input arrays
	m_ss << "DEBUG: Hit " << ihit << " at time " << is->m_time[ihit];
	StreamToLog(DEBUG2);
	m_in_PMTIDs->push_back(is->m_PMTid[ihit]);
	m_in_Ts    ->push_back(is->m_time[ihit]);
	m_in_Qs    ->push_back(is->m_charge[ihit]);
      }//ihit
    }//ID SubSamples
  
    //get the number of hits
    m_in_nhits = m_in_PMTIDs->size();

    //don't run on large events
    if(m_in_nhits == 0 || m_in_nhits > m_nhits_max) {
      m_ss << "INFO: " << m_in_nhits << " hits in current trigger. Not running test vertices reconstruction";
      StreamToLog(INFO);
      continue;
    }
    
    m_ss << "DEBUG: test vertices reconstruction running over " << m_in_nhits << " hits";
    StreamToLog(DEBUG1);
    m_ss << "DEBUG: First hit time relative to sample: " << m_in_Ts->at(0);
    StreamToLog(DEBUG1);

    bool success = false;
    int ninfos = the_info.size();
    if( ninfos == 7 ){
      success = true;
      // n hits, vertex, direction
      out_vertex[0] = the_info[1];
      out_vertex[1] = the_info[2];
      out_vertex[2] = the_info[3];
      out_cosine[0] = the_info[4];
      out_cosine[1] = the_info[5];
      out_cosine[2] = the_info[6];

      //    double dir_z = cos(dir.theta);
      //    double dir_y = sin(dir.theta) * sin(dir.phi);
      //    double dir_x = sin(dir.theta) * cos(dir.phi);
      //  double theta, phi, alpha;

      out_direction[0] = atan2(sqrt(pow(out_cosine[0],2) + pow(out_cosine[1],2)),out_cosine[2]);
      out_direction[1] = atan2(out_cosine[1],out_cosine[0]);
      out_direction[2] = 0;
    }else{
      m_ss << "INFO: warning: unexpected trigger info structure: " << ninfos << " info entries ";
      StreamToLog(INFO);
    }

    if (success) {
      m_ss << "DEBUG: vertex reconstructed at x, y, z:";
      for(int i = 0; i < 3; i++) {
        m_ss << " " << out_vertex[i] << ",";
      }
      m_ss << "DEBUG: reconstructed euler angles:";
      for(int i = 0; i < 3; i++) {
        m_ss << " " << out_direction[i] << ",";
      }
      StreamToLog(DEBUG1);

      //fill the data model with the result
      // need to convert to double...
      for(int i = 0; i < 3; i++) {
        dout_vertex[i]    = out_vertex[i];
        dout_direction[i] = out_direction[i];
      }

      TimeDelta vertex_time = m_data->IDTriggers.m_trigger_time.at(itrigger);
      m_data->RecoInfo.AddRecon(kReconTestVertices, itrigger, m_in_nhits,
                                vertex_time, &(dout_vertex[0]), out_maxlike[2], out_maxlike[1],
                                &(dout_direction[0]), &(dout_cone[0]), out_direction[5]);
    }
  }//itrigger

  if(m_stopwatch) m_stopwatch->Stop();

  if(m_stopwatch) m_stopwatch->Stop();

  return true;
}


bool test_vertices_reconstruction::Finalise(){

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Execute", m_stopwatch_file), INFO, m_verbose);
    m_stopwatch->Start();
  }

  delete m_in_PMTIDs;
  delete m_in_Ts;
  delete m_in_Qs;


  if(m_stopwatch) {
    Log(m_stopwatch->Result("Finalise"), INFO, m_verbose);
    delete m_stopwatch;
  }

  return true;
}
