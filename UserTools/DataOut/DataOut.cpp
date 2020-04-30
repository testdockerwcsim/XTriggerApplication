#include "DataOut.h"
#include "TimeDelta.h"

DataOut::DataOut():Tool(){}

/////////////////////////////////////////////////////////////////
bool DataOut::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new util::Stopwatch("DataOut") : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();

  m_data= &data;

  //open the output file
  Log("DEBUG: DataOut::Initialise opening output file...", DEBUG2, m_verbose);
  if(! m_variables.Get("outfilename", m_output_filename)) {
    Log("ERROR: outfilename configuration not found. Cancelling initialisation", ERROR, m_verbose);
    return false;
  }
  m_output_file = new TFile(m_output_filename.c_str(), "RECREATE");

  //other options
  m_save_multiple_hits_per_trigger = true;
  m_variables.Get("save_multiple_hits_per_trigger", m_save_multiple_hits_per_trigger);
  Log("WARN: TODO save_multiple_hits_per_trigger is not currently implemented", WARN, m_verbose);
  double trigger_offset_temp = 0;
  m_variables.Get("trigger_offset", trigger_offset_temp);
  m_trigger_offset = TimeDelta(trigger_offset_temp);
  m_save_only_failed_hits = false;
  m_variables.Get("save_only_failed_hits", m_save_only_failed_hits);
  Log("WARN: TODO save_only_failed_hits is not currently implemented", WARN, m_verbose);

  //setup the out event tree
  Log("DEBUG: DataOut::Initialise setting up output event tree...", DEBUG2, m_verbose);
  // Nevents unique event objects
  Int_t bufsize = 64000;
  m_event_tree = new TTree("wcsimT","WCSim Tree");
  m_data->IDWCSimEvent_Triggered = new WCSimRootEvent();
  m_data->IDWCSimEvent_Triggered->Initialize();
  m_event_tree->Branch("wcsimrootevent", "WCSimRootEvent", &m_data->IDWCSimEvent_Triggered, bufsize,2);
  if(m_data->HasOD) {
    m_data->ODWCSimEvent_Triggered = new WCSimRootEvent();
    m_data->ODWCSimEvent_Triggered->Initialize();
    m_event_tree->Branch("wcsimrootevent_OD", "WCSimRootEvent", &m_data->ODWCSimEvent_Triggered, bufsize,2);
  }
  else {
    m_data->ODWCSimEvent_Triggered = 0;
  }
  m_event_tree->Branch("wcsimfilename", &(m_data->CurrentWCSimFile));
  m_event_tree->Branch("wcsimeventnum", &(m_data->CurrentWCSimEventNum));

  //fill the output event-independent trees
  Log("DEBUG: DataOut::Initialise filling event-independent trees...", DEBUG2, m_verbose);

  //There are 1 unique geom objects, so this is a simple clone of 1 entry
  if(m_data->IsMC) {
    Log("DEBUG:   Geometry...", DEBUG2, m_verbose);
    TTree * geom_tree = m_data->WCSimGeomTree->CloneTree(1);
    geom_tree->Write();
    delete geom_tree;
  }
  else {
    Log("WARN: TODO Geometry tree filling is not yet implemented for data");
  }

  //There are Nfiles unique options objects, so this is a clone of all entries
  // plus a new branch with the wcsim filename 
  if(m_data->IsMC) {
    Log("DEBUG:   Options & file names...", DEBUG2, m_verbose);
    TTree * options_tree = m_data->WCSimOptionsTree->CloneTree();
    m_ss << "DEBUG:     entries: " << options_tree->GetEntries();
    StreamToLog(DEBUG2);
    TObjString * wcsimfilename = new TObjString();
    TBranch * branch = options_tree->Branch("wcsimfilename", &wcsimfilename);
    for(int i = 0; i < options_tree->GetEntries(); i++) {
      m_data->WCSimOptionsTree->GetEntry(i);
      options_tree->GetEntry(i);
      wcsimfilename->SetString(m_data->WCSimOptionsTree->GetFile()->GetName());
      branch->Fill();
    }//i
    options_tree->Write();
    delete wcsimfilename;
    delete options_tree;
  }

  Log("DEBUG: DataOut::Initialise creating trigger info...", DEBUG2, m_verbose);
  m_all_triggers = new TriggerInfo();
  m_event_num = 0;

  if(m_stopwatch) Log(m_stopwatch->Result("Initialise"), INFO, m_verbose);

  return true;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool DataOut::Execute(){
  if(m_stopwatch) m_stopwatch->Start();

  //Gather together all the trigger windows
  m_all_triggers->Clear();
  m_all_triggers->AddTriggers(&(m_data->IDTriggers));
  if(m_data->HasOD)
    m_all_triggers->AddTriggers(&(m_data->ODTriggers));
  m_all_triggers->SortByStartTime();
  m_ss << "INFO: Have " << m_all_triggers->m_N << " triggers to save times:";
  StreamToLog(INFO);
  for(int i = 0; i < m_all_triggers->m_N; i++) {
    m_ss << "INFO: \t[" << m_all_triggers->m_starttime.at(i)
       << ", " << m_all_triggers->m_endtime.at(i) << "] "
       << m_all_triggers->m_triggertime.at(i) << " with type "
       << WCSimEnumerations::EnumAsString(m_all_triggers->m_type.at(i)) << " extra info";
    for(unsigned int ii = 0; ii < m_all_triggers->m_info.at(i).size(); ii++)
      m_ss << " " << m_all_triggers->m_info.at(i).at(ii);
    StreamToLog(INFO);
  }//i

  //Note: the trigger ranges vector can contain overlapping ranges
  //we want to make sure the triggers output aren't overlapping
  // This is actually handled in DataOut::FillHits()
  // It puts hits into the output event in the earliest trigger they belong to

  //Fill the WCSimRootEvent with hit/trigger information, and truth information (if available)
  if(m_all_triggers->m_N) {
    ExecuteSubDet(m_data->IDWCSimEvent_Triggered, m_data->IDSamples, m_data->IDWCSimEvent_Raw);
    if(m_data->HasOD) {
      ExecuteSubDet(m_data->ODWCSimEvent_Triggered, m_data->ODSamples, m_data->ODWCSimEvent_Raw);
    }
  }//>=1 trigger found

  //Fill the tree with what we've just created
  m_event_tree->Fill();

  //make sure the triggers are reset for the next event
  m_data->IDTriggers.Clear();
  m_data->ODTriggers.Clear();

  //increment event number
  m_event_num++;

  if(m_stopwatch) m_stopwatch->Stop();

  return true;
}
/////////////////////////////////////////////////////////////////
void DataOut::ExecuteSubDet(WCSimRootEvent * wcsim_event, std::vector<SubSample> & samples, WCSimRootEvent * original_wcsim_event) {
  //ReInitialise the WCSimRootEvent
  // This clears the TObjArray of WCSimRootTrigger's,
  // and creates a WCSimRootTrigger in the first slot
  wcsim_event->ReInitialize();

  //If there are multiple triggers in the event,
  // create subevents (i.e. new WCSimRootTrigger's) in the WCSimRootEvent
  //Also sets the time correctly
  CreateSubEvents(wcsim_event);

  //Get the difference between the WCSim "date" and the time of the first TriggerApp trigger,
  // in order for the hits to have the correct absolute time.
  // Also add the trigger offset from the config file
  TimeDelta time_shift = GetOffset(original_wcsim_event);

  //For every hit, if it's in a trigger window,
  //add it to the appropriate WCSimRootTrigger in the WCSimRootEvent
  FillHits(wcsim_event, time_shift, samples);

  //If this is an MC file, we also need to add
  // - true tracks
  // - true hits
  if(m_data->IsMC)
    AddTruthInfo(wcsim_event, original_wcsim_event, time_shift);

  //Set some trigger header infromation that requires all the hits to be 
  // present to calculate e.g. sumq
  FinaliseSubEvents(wcsim_event);  
}
/////////////////////////////////////////////////////////////////
void DataOut::CreateSubEvents(WCSimRootEvent * wcsim_event) {
  const int n = m_all_triggers->m_N;
  if (n==0) return;

  // Change trigger times and create new SubEvents where necessary
  for(int i = 0; i < n; i++) {
    if(i)
      wcsim_event->AddSubEvent();
    WCSimRootTrigger * trig = wcsim_event->GetTrigger(i);
    trig->SetHeader(m_event_num, 0, (m_all_triggers->m_triggertime.at(i) / TimeDelta::ns), i+1);
    trig->SetTriggerInfo(m_all_triggers->m_type.at(i), m_all_triggers->m_info.at(i));
    trig->SetMode(0);
  }//i
}
/////////////////////////////////////////////////////////////////
TimeDelta DataOut::GetOffset(WCSimRootEvent * original_wcsim_event) {

  TimeDelta time_shift(0);

  if(m_data->IsMC) {
    // Move all hit times in trigger 0 to be relative to that trigger,
    // i.e. move them by the difference of the old header date and the
    // new taken from the trigger
    WCSimRootTrigger * trig0 = original_wcsim_event->GetTrigger(0);
    // The new time
    TimeDelta new_trigger_time = m_all_triggers->m_triggertime.at(0);
    // The old time (stored in ns)
    TimeDelta old_trigger_time = trig0->GetHeader()->GetDate() * TimeDelta::ns;
    // The difference
    TimeDelta time_shift = new_trigger_time - old_trigger_time;
    m_ss << "DEBUG: Trigger date shift from " << old_trigger_time << " to " << new_trigger_time << ": " << time_shift;
    StreamToLog(DEBUG2);
  }

  m_ss << "DEBUG: Adding additional user-defined time shift of "
       << m_trigger_offset;
  StreamToLog(DEBUG2);

  time_shift += m_trigger_offset;
  m_ss << "DEBUG: Total time shift is " << time_shift;
  StreamToLog(DEBUG1);

  return time_shift;
}
/////////////////////////////////////////////////////////////////
void DataOut::FillHits(WCSimRootEvent * wcsim_event, const TimeDelta & time_shift, std::vector<SubSample> & samples) {
  unsigned int trigger_window_to_check;
  TimeDelta time;
  std::vector<int> photon_id_temp;
  //Loop over all SubSamples
  for(std::vector<SubSample>::iterator is=samples.begin(); is!=samples.end(); ++is){
    // Make sure hit times are ordered in time
    Log("WARN: TODO Sorting by time, and doing time window checks, will not be required after #49");
    is->SortByTime();
    //loop over every hit
    const unsigned int nhits = is->m_time.size();
    int counter = 0;
    for(int ihit = 0; ihit < nhits; ihit++) {
      time = is->m_time[ihit];
      m_ss << "Hit " << ihit << " is at time " << time << std::endl
	   << "Checking hit is in range [" << m_all_triggers->m_starttime.at(trigger_window_to_check)
	   << ", " << m_all_triggers->m_endtime.at(trigger_window_to_check) << "]";
      StreamToLog(DEBUG3);
      
      //hit time is before trigger window
      if(time < m_all_triggers->m_starttime.at(trigger_window_to_check)) {
	Log("Too early", DEBUG3, m_verbose);
	continue;
      }
      //hit time is after trigger window; check the next trigger window
      else if(time > m_all_triggers->m_endtime.at(trigger_window_to_check)) {
	ihit--;
	trigger_window_to_check++;
	Log("Too late", DEBUG3, m_verbose);
	if(trigger_window_to_check >= m_all_triggers->m_N)
	  break;
	continue;
      }
      //hit is in this window. Let's save it

      // Move all hits by the *negative* shift. If 5 seconds are added to the
      // trigger time, the hit times (relative to the trigger) must be moved 5
      // seconds back, so they remain at the same absolute time.
      time -= time_shift / TimeDelta::ns; //Is the time shift in the right place?
      wcsim_event->GetTrigger(trigger_window_to_check)->AddCherenkovDigiHit(is->m_charge[ihit],
									    time / TimeDelta::ns, //do i need to do any division here?
									    is->m_PMTid[ihit],
									    photon_id_temp);
      m_ss << "Saved hit " << counter++;
      StreamToLog(DEBUG3);
    }//ihit
  }//loop over SubSamples
}
/////////////////////////////////////////////////////////////////
void DataOut::AddTruthInfo(WCSimRootEvent * wcsim_event, WCSimRootEvent * original_wcsim_event, const TimeDelta & time_shift) {
}
/////////////////////////////////////////////////////////////////
void DataOut::FinaliseSubEvents(WCSimRootEvent * wcsim_event) {
  const int n = m_all_triggers->m_N;
  for(int i = 0; i < n; i++) {
    WCSimRootTrigger * trig = wcsim_event->GetTrigger(i);
    TClonesArray * hits = trig->GetCherenkovDigiHits();
    double sumq = 0;
    int nhits = 0;
    for(int j = 0; j < trig->GetNcherenkovdigihits_slots(); j++) {
      WCSimRootCherenkovDigiHit * digi = (WCSimRootCherenkovDigiHit *)hits->At(j);
      if(digi) {
	sumq += digi->GetQ();
	nhits++;
      }
    }//j
    trig->SetSumQ(sumq);
    //this is actually number of hits, not number of unique PMTs with hits
    trig->SetNumDigitizedTubes(nhits);
    m_ss << "DEBUG: Trigger " << i << " has " << nhits << " hits";
    StreamToLog(DEBUG1);
  }//i
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool DataOut::Finalise(){
  if(m_stopwatch) {
    Log(m_stopwatch->Result("Execute", m_stopwatch_file), INFO, m_verbose);
    m_stopwatch->Start();
  }

  //multiple TFiles may be open. Ensure we save to the correct one
  m_output_file->cd(TString::Format("%s:/", m_output_filename.c_str()));
  m_event_tree->Write();

  delete m_event_tree;
  delete m_data->IDWCSimEvent_Triggered;
  if(m_data->ODWCSimEvent_Triggered)
    delete m_data->ODWCSimEvent_Triggered;

  delete m_all_triggers;

  m_output_file->Close();
  delete m_output_file;

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Finalise"), INFO, m_verbose);
    delete m_stopwatch;
  }

  return true;
}
