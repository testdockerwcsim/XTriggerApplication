#include "EnergeticBONSAI.h"

EnergeticBONSAI::EnergeticBONSAI():Tool(){}


bool EnergeticBONSAI::Initialise(std::string configfile, DataModel &data){
  Log("DEBUG: EnergeticBONSAI::Initialise() Starting", DEBUG1, m_verbose);

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  m_data= &data;

  //setup energetic BONSAI with the geometry info
  int ebonsai_verbose = 0;
  m_variables.Get("ebonsai_verbose", ebonsai_verbose);
  WCSimRootGeom * geo = 0;
  m_data->WCSimGeomTree->SetBranchAddress("wcsimrootgeom", &geo);
  m_data->WCSimGeomTree->GetEntry(0);
  m_ebonsai = new WCSimEBonsai(detectorname, geo, ebonsai_verbose);

  //override any energetic BONSAI assumptions
  m_ebonsai->SetDarkRate(m_data->IDPMTDarkRate);
  m_ebonsai->SetNPMTs(m_data->IDNPMTs);
  
  //tell energetic BONSAI how many PMTs are turned off
  m_n_working_pmts = m_data->IDNPMTs;
  m_variables.Get("n_working_pmts", m_n_working_pmts);
  if(m_n_working_pmts > m_data->IDNPMTs) {
    m_ss << "WARN: Config value of number of working PMTs " << m_n_working_pmts
	 << " is more than the total number of PMTs " << m_data->IDNPMTs
	 << ". Setting the number of working PMTs to the total number of PMTs";
    StreamToLog(WARN);
    m_n_working_pmts = m_data->IDNPMTs;
  }
  m_ebonsai->SetNWorkingPMTs(m_n_working_pmts);

  //Get the reconstructed events filter you want to save
  if(!m_variables.Get("input_filter_name", fInputFilterName)) {
    Log("INFO: input_filter_name not given. Using ALL", WARN, m_verbose);
    fInputFilterName = "ALL";
  }
  fInFilter  = m_data->GetFilter(fInputFilterName, false);
  if(!fInFilter) {
    m_ss << "FATAL: no filter named " << fInputFilterName << " found. Returning false";
    StreamToLog(FATAL);
    return false;
  }

  //allocate memory for the digit vectors
  m_variables.Get("nhitsmin", m_nhits_min);
  m_variables.Get("nhitsmax", m_nhits_max);
  m_in_PMTIDs = new std::vector<int>  (m_nhits_max);
  m_in_Ts     = new std::vector<float>(m_nhits_max);

  Log("DEBUG: EnergeticBONSAI::Initialise() Finished", DEBUG1, m_verbose);

  return true;
}


bool EnergeticBONSAI::Execute(){

  Log("DEBUG: EnergeticBONSAI::Execute() Starting", DEBUG1, m_verbose);

  for (int itrigger = 0 ; itrigger < m_data->IDWCSimEvent_Triggered->GetNumberOfEvents(); itrigger++) {
    m_trigger = m_data->IDWCSimEvent_Triggered->GetTrigger(itrigger);

    //clear the previous triggers' digit information
    m_in_PMTIDs->clear();
    m_in_Ts->clear();

    //get the number of digits
    m_in_nhits = m_trigger->GetNcherenkovdigihits();
    int nhits_slots = m_trigger->GetNcherenkovdigihits_slots();

    //don't run energetic bonsai on large or small events
    if(m_in_nhits < m_nhits_min || m_in_nhits > m_nhits_max) {
      m_ss << "INFO: " << m_in_nhits << " digits in current trigger. Not running BONSAI";
      StreamToLog(INFO);
      return true;
    }

    //fill the inputs to energetic BONSAI with the current triggers' digit information
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
      m_ss << "WARN: BONSAI expected " << m_in_nhits << " digits. Found " << digits_found;
      StreamToLog(WARN);
      m_in_nhits = digits_found;
    }
    
    m_ss << "DEBUG: BONSAI running over " << m_in_nhits << " digits";
    StreamToLog(DEBUG1);

    Pos3D vertex = fInFilter->GetVertex(irecon);
    m_vertex[0] = vertex.x;
    m_vertex[1] = vertex.y;
    m_vertex[2] = vertex.z;
    m_ebonsai->GetEnergy(*m_in_Ts, *m_in_PMTIDs, &(m_vertex[0]));

    //TODO: loop over reco object, not triggers; sort filter member variables (including renaming to m_); clear variables / cleanup
  }//itrigger

  Log("DEBUG: EnergeticBONSAI::Execute() Finished", DEBUG1, m_verbose);


  return true;
}


bool EnergeticBONSAI::Finalise(){

  Log("DEBUG: EnergeticBONSAI::Finalise() Starting", DEBUG1, m_verbose);

  Log("DEBUG: EnergeticBONSAI::Finalise() Finished", DEBUG1, m_verbose);

  return true;
}
