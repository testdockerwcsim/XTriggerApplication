#include "LEAFRecon.h"

LEAFRecon::LEAFRecon():Tool(){}


bool LEAFRecon::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new util::Stopwatch("LEAFRecon") : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();

  m_data= &data;

  m_variables.Get("nhitsmin", m_nhits_min);
  m_variables.Get("nhitsmax", m_nhits_max);

  //setup LEAF with the geometry & dark rate
  WCSimRootGeom * geo = 0;
  m_data->WCSimGeomTree->SetBranchAddress("wcsimrootgeom", &geo);
  m_data->WCSimGeomTree->GetEntry(0);
  //Initialise the LEAF geometry
  // 2nd argument is 20" PMT dark rate in Hz
  // 3rd argument is mPMT dark rate in Hz
  BQFitter::GetME()->SetGeometry(geo, m_data->IDPMTDarkRate * 1E3, 0);
  m_data->WCSimGeomTree->ResetBranchAddresses();


  if(m_stopwatch) Log(m_stopwatch->Result("Initialise"), INFO, m_verbose);

  return true;
}


bool LEAFRecon::Execute(){

  if(m_stopwatch) m_stopwatch->Start();

  const int i_hybrid = 0;
  Log("TODO: Assuming all hits are from 50cm PMTs. When we include mPMTs too, they need to be added with i_hybrid = 1", WARN, m_verbose);

  std::vector<double> true_vertex(3);
  double true_time;

  //loop over ID triggers
  for (int itrigger = 0; itrigger < m_data->IDTriggers.m_num_triggers; itrigger++) {

    // Reset Hit vector
    BQFitter::GetME()->ResetHitInfo();
    m_in_nhits = 0;

    //Loop over ID SubSamples
    for(std::vector<SubSample>::iterator is = m_data->IDSamples.begin(); is != m_data->IDSamples.end(); ++is){

      // Give hits to the fitter one-by-one
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

	//it belongs. Give it to LEAF
	m_ss << "DEBUG: Hit " << ihit << " at time " << is->m_time[ihit];
	StreamToLog(DEBUG2);
	BQFitter::GetME()->AddHit(is->m_time[ihit], is->m_charge[ihit], i_hybrid, is->m_PMTid[ihit]);
	m_in_nhits++;
      }//ihit

    }//ID SubSamples

    //don't run LEAF on large or small events
    if(m_in_nhits < m_nhits_min || m_in_nhits > m_nhits_max) {
      m_ss << "INFO: " << m_in_nhits << " hits in current trigger. Not running LEAF";
      StreamToLog(INFO);
      continue;
    }

    if(m_data->IsMC) {
      // Send True Position to LEAF for check
      for(int idim = 0; idim < 3; idim++)
	true_vertex[idim] = m_data->IDWCSimEvent_Raw->GetTrigger(0)->GetVtxs(0, idim);
      true_time = m_data->IDWCSimEvent_Raw->GetTrigger(0)->GetHeader()->GetDate();
      Log("TODO: True vertex currently assumed to be the first vertex from the first trigger in the WCSimRootEvent. Some complicated truth matching should occur for multi-vertex events...", WARN, m_verbose);
      BQFitter::GetME()->SetTrueVertexInfo(true_vertex, true_time);
    }//IsMC

    m_ss << "DEBUG: LEAF running over " << m_in_nhits << " hits";
    StreamToLog(DEBUG1);

    //run the fit
    Log("TODO: Just using 50cm PMTs in the fit. When we include mPMTs too, need to call MakeFit with true", WARN, m_verbose);
    m_leaf_result = BQFitter::GetME()->MakeFit(false);

    m_ss << "DEBUG: Vertex reconstructed at x, y, z, t:";
    for(int idim = 0; idim < 3; idim++)
      m_ss << " " << m_leaf_result.Vtx[idim] << ",";
    m_ss << " " << m_leaf_result.Time;
    StreamToLog(DEBUG1);

    if(m_data->IsMC) {
      m_ss << "DEBUG: True vertex at x, y, z, t:";
      for(int idim = 0; idim < 3; idim++)
	m_ss << " " << true_vertex[idim] << ",";
      m_ss << " " << true_time;
      StreamToLog(DEBUG1);
      m_ss << "DEBUG: Difference between true & reconstructed:"
	   << " NLL: " << m_leaf_result.True_NLLDiff
	   << " time: " << m_leaf_result.True_TimeDiff
	   << " position: " << m_leaf_result.True_TistDiff;
      StreamToLog(DEBUG1);
    }

    //fill the data model with the result
    // - Time is in unit???
    // - Vertex is x,y,z in unit???
    // - Direction variables are returned 3 times: 0=20", 1=mPMT, 2=combined
    // - Direction is returned as a unit vector???
    // - Goodness of time/vertex & direction fits. Higher/lower??? number means a better fit
    TimeDelta vertex_time = m_data->IDTriggers.m_trigger_time.at(itrigger) + TimeDelta(m_leaf_result.Time * TimeDelta::ns);
    double time_goodness = 0; //No goodness of fit for time-only fit by LEAF
    double cone[2] = {0,0}; //No cone reconstructed by LEAF
    Log("TODO: Filling ReconInfo with results for 50cm PMTs only. When we include mPMTs, need to use the 2nd dimension of dimension quantities (i.e. 0=50cm, 1=mPMT, 2=combined)", WARN, m_verbose);
    m_data->RecoInfo.AddRecon(kReconLEAF, itrigger, m_in_nhits,
			      vertex_time, &(m_leaf_result.Vtx[0]), m_leaf_result.Good, time_goodness,
			      &(m_leaf_result.dir[0][0]), &(cone[0]), m_leaf_result.dir_goodness[0]);
    m_ss << "TODO: LEAF reconstruction result is not filling TriggerApp's ReconInfo class correctly. Need to find out:" << std::endl
	 << "\tIs direction returned as a 3-vector?" << std::endl
	 << "\tAre the times in ns and the positions in cm?" << std::endl
	 << "\tBetter goodness of fits corresponds to a higher/lower value?" << std::endl
	 << "\tDo we want to save other LEAF quantities?" << std::endl
	 << "\t- n50: maximum number of hit of an event within a 50 ns window" << std::endl
	 << "\t- dirKS: a KS test to check if the PMT hit of an event are clustered or not (in order to discriminate wall BG)" << std::endl
	 << "For now, just enjoy the time/vertex positions...";
    StreamToLog(DEBUG1);

  }//itrigger

  if(m_stopwatch) m_stopwatch->Stop();

  return true;
}


bool LEAFRecon::Finalise(){

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Execute", m_stopwatch_file), INFO, m_verbose);
    m_stopwatch->Start();
  }

  //// YOUR CODE HERE

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Finalise"), INFO, m_verbose);
    delete m_stopwatch;
  }

  return true;
}
