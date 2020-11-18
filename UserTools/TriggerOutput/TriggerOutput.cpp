#include "TriggerOutput.h"

TriggerOutput::TriggerOutput():Tool(){}


bool TriggerOutput::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);
  m_variables.Get("trigger_od",                   m_trigger_OD);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new util::Stopwatch("TriggerOutput") : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();

  m_data= &data;

  //open the output file
  Log("DEBUG: TriggerOutput::Initialise opening output file...", DEBUG2, m_verbose);
  if(! m_variables.Get("outfilename", m_output_filename)) {
    Log("ERROR: outfilename configuration not found. Cancelling initialisation", ERROR, m_verbose);
    return false;
  }
  m_output_file = new TFile(m_output_filename.c_str(), "RECREATE");


  //setup the out triggers tree
  Log("DEBUG: TriggerOutput::Initialise setting up output trigger tree...", DEBUG2, m_verbose);
  m_triggers_tree = new TTree("triggers","triggers Tree");
  m_triggers_tree->Branch("type", &the_type, "type/I");
  m_triggers_tree->Branch("readout_start_time", &the_readout_start_time, "readout_start_time/F");
  m_triggers_tree->Branch("readout_end_time", &the_readout_end_time, "readout_end_time/F");
  m_triggers_tree->Branch("mask_start_time", &the_mask_start_time, "mask_start_time/F");
  m_triggers_tree->Branch("mask_end_time", &the_mask_end_time, "mask_end_time/F");
  m_triggers_tree->Branch("trigger_time", &the_trigger_time, "trigger_time/F");
  m_triggers_tree->Branch("trigger_info", &the_trigger_info);


  //setup the out header tree
  Log("DEBUG: TriggerOutput::Initialise setting up output header tree...", DEBUG2, m_verbose);
  m_header_tree = new TTree("header","header Tree");
  m_header_tree->Branch("interpose_interval", &m_interpose_interval, "interpose_interval/F");
  m_interpose_interval = m_data->get_interpose_interval();
  m_header_tree->Fill();
  

  //fill the output event-independent trees
  Log("DEBUG: TriggerOutput::Initialise filling event-independent trees...", DEBUG2, m_verbose);

  if(m_stopwatch) Log(m_stopwatch->Result("Initialise"), INFO, m_verbose);

  return true;
}


bool TriggerOutput::Execute(){
  if(m_stopwatch) m_stopwatch->Start();


  TriggerInfo * triggers = m_trigger_OD ? &(m_data->ODTriggers) : &(m_data->IDTriggers);

  /// The number of triggers
  unsigned int num_triggers = triggers->m_num_triggers;
  /// The type of Trigger
  std::vector<TriggerType_t> type = triggers->m_type;
  /// The starting time of the trigger window
  std::vector<TimeDelta> readout_start_time = triggers->m_readout_start_time;
  /// The ending time of the trigger window
  std::vector<TimeDelta> readout_end_time = triggers->m_readout_end_time;
  /// The starting time of the hit mask
  std::vector<TimeDelta> mask_start_time = triggers->m_mask_start_time;
  /// The ending time of the hit mask
  std::vector<TimeDelta> mask_end_time = triggers->m_mask_end_time;
  /// The actual time of the trigger
  std::vector<TimeDelta> trigger_time = triggers->m_trigger_time;
  /// Additional information, specific to the trigger
  std::vector<std::vector<float> > info = triggers->m_info;

  m_ss << " TriggerOutput: Number of triggers " << num_triggers;
  StreamToLog(INFO);
  for(int i=0; i<num_triggers; i++){
    the_type = type[i];
    the_readout_start_time = absolute_time(readout_start_time[i]);
    the_readout_end_time = absolute_time(readout_end_time[i]);
    the_mask_start_time = absolute_time(mask_start_time[i]);
    the_mask_end_time = absolute_time(mask_end_time[i]);
    the_trigger_time = absolute_time(trigger_time[i]);
    the_trigger_info = info[i];
    m_ss << " TriggerOutput: trigger " << i << " type " << WCSimEnumerations::EnumAsString(the_type) << " readout_start_time " << the_readout_start_time << " readout_end_time " << the_readout_end_time << " mask_start_time " << the_mask_start_time << " mask_end_time " << the_mask_end_time << " trigger_time " << the_trigger_time ;
    for(unsigned int ii = 0; ii < info[i].size(); ii++)
      m_ss << " info " << ii << " = " << info[i][ii];

    StreamToLog(INFO);

    m_triggers_tree->Fill();
  }

  if(m_stopwatch) m_stopwatch->Stop();

  return true;
}


bool TriggerOutput::Finalise(){

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Execute", m_stopwatch_file), INFO, m_verbose);
    m_stopwatch->Start();
  }

  //multiple TFiles may be open. Ensure we save to the correct one
  m_output_file->cd(TString::Format("%s:/", m_output_filename.c_str()));
  m_header_tree->Write();
  m_triggers_tree->Write();

  delete m_header_tree;
  delete m_triggers_tree;

  m_output_file->Close();

  delete m_output_file;

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Finalise"), INFO, m_verbose);
    delete m_stopwatch;
  }

  return true;
}


float TriggerOutput::absolute_time(TimeDelta d){

  return d/TimeDelta::ns;

}
