#include "FLOWERRecon.h"

FLOWERRecon::FLOWERRecon():Tool(){}


bool FLOWERRecon::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  m_data= &data;

  //setup FLOWER with the geometry info
  int flower_verbose = 0;
  m_variables.Get("flower_verbose", flower_verbose);
  WCSimRootGeom * geo = 0;
  m_data->WCSimGeomTree->SetBranchAddress("wcsimrootgeom", &geo);
  m_data->WCSimGeomTree->GetEntry(0);
  m_variables.Get("detector_name", m_detector_name);
  Log("TODO: detector_name should come from the geometry, rather than the parameter file", WARN, m_verbose);
  m_overwrite_nearest = false;
  m_variables.Get("overwrite_nearest_neighbours", m_overwrite_nearest);
  m_flower = new WCSimFlower(m_detector_name.c_str(), geo, m_overwrite_nearest, flower_verbose);

  //override any FLOWER assumptions
  m_flower->SetDarkRate(m_data->IDPMTDarkRate);
  m_flower->SetNPMTs(m_data->IDNPMTs);
  
  //tell FLOWER how many PMTs are turned off
  m_n_working_pmts = m_data->IDNPMTs;
  m_variables.Get("n_working_pmts", m_n_working_pmts);
  if(m_n_working_pmts > m_data->IDNPMTs) {
    m_ss << "WARN: Config value of number of working PMTs " << m_n_working_pmts
	 << " is more than the total number of PMTs " << m_data->IDNPMTs
	 << ". Setting the number of working PMTs to the total number of PMTs";
    StreamToLog(WARN);
    m_n_working_pmts = m_data->IDNPMTs;
  }
  m_flower->SetNWorkingPMTs(m_n_working_pmts);

  //Get the reconstructed events filter you want to save
  if(!m_variables.Get("input_filter_name", m_input_filter_name)) {
    Log("INFO: input_filter_name not given. Using ALL", WARN, m_verbose);
    m_input_filter_name = "ALL";
  }
  m_input_filter  = m_data->GetFilter(m_input_filter_name, false);
  if(!m_input_filter) {
    m_ss << "FATAL: no filter named " << m_input_filter_name << " found. Returning false";
    StreamToLog(FATAL);
    return false;
  }

  //allocate memory for the digit vectors
  m_variables.Get("nhitsmin", m_nhits_min);
  m_variables.Get("nhitsmax", m_nhits_max);
  m_in_PMTIDs = new std::vector<int>  (m_nhits_max);
  m_in_Ts     = new std::vector<float>(m_nhits_max);

  return true;
}


bool FLOWERRecon::Execute(){

  for(int ireco = 0; ireco < m_input_filter->GetNRecons(); ireco++) {
    //get the vertex
    Pos3D vertex = m_input_filter->GetVertex(ireco);
    m_vertex[0] = vertex.x;
    m_vertex[1] = vertex.y;
    m_vertex[2] = vertex.z;

    //get the trigger this reconstructed object is associated with
    m_trigger = m_data->IDWCSimEvent_Triggered->GetTrigger(m_input_filter->GetTriggerNum(ireco));

    //clear the previous triggers' digit information
    m_in_PMTIDs->clear();
    m_in_Ts->clear();

    //get the number of digits
    m_in_nhits = m_trigger->GetNcherenkovdigihits();
    int nhits_slots = m_trigger->GetNcherenkovdigihits_slots();

    //don't run FLOWER on large or small events
    if(m_in_nhits < m_nhits_min || m_in_nhits > m_nhits_max) {
      m_ss << "INFO: " << m_in_nhits << " digits in current trigger. Not running FLOWER";
      StreamToLog(INFO);
      return true;
    }

    //fill the inputs to FLOWER with the current triggers' digit information
    long n_not_found = 0;
    for (long idigi=0; idigi < nhits_slots; idigi++) {
      TObject *element = (m_trigger->GetCherenkovDigiHits())->At(idigi);
      WCSimRootCherenkovDigiHit *digi = 
	dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
      if(!digi) {
	n_not_found++;
	//this happens regularly because removing digits doesn't shrink the TClonesArray
	m_ss << "DEBUG: Digit " << idigi << " of " << m_in_nhits << " not found in WCSimRootTrigger";
	StreamToLog(DEBUG2);
	continue;
      }
      m_ss << "DEBUG: Digit " << idigi << " at time " << digi->GetT();
      StreamToLog(DEBUG2);
      m_in_PMTIDs->push_back(digi->GetTubeId());
      m_in_Ts    ->push_back(digi->GetT());
    }//idigi
    int digits_found = nhits_slots - n_not_found;
    if(m_in_nhits != digits_found) {
      m_ss << "WARN: FLOWER expected " << m_in_nhits << " digits. Found " << digits_found;
      StreamToLog(WARN);
      m_in_nhits = digits_found;
    }
    
    m_ss << "DEBUG: FLOWER running over " << m_in_nhits << " digits";
    StreamToLog(DEBUG1);

    //get the energy
    double energy = m_flower->GetEnergy(*m_in_Ts, *m_in_PMTIDs, &(m_vertex[0]));

    m_ss << "INFO: FLOWER reconstructed energy " << energy;
    StreamToLog(INFO);

  }//ireco

  return true;
}


bool FLOWERRecon::Finalise(){

  delete m_flower;
  delete m_in_PMTIDs;
  delete m_in_Ts;

  return true;
}
