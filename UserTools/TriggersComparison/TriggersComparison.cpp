#include "TriggersComparison.h"

TriggersComparison::TriggersComparison():Tool(){}

bool TriggersComparison::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  //Setup and start the stopwatch
  bool use_stopwatch = false;
  m_variables.Get("use_stopwatch", use_stopwatch);
  m_stopwatch = use_stopwatch ? new util::Stopwatch("TriggersComparison") : 0;

  m_stopwatch_file = "";
  m_variables.Get("stopwatch_file", m_stopwatch_file);

  if(m_stopwatch) m_stopwatch->Start();

  //open the output file
  Log("DEBUG: TriggersComparison::Initialise opening output file...", DEBUG2, m_verbose);
  if(! m_variables.Get("outfilename", m_output_filename)) {
    Log("ERROR: outfilename configuration not found. Cancelling initialisation", ERROR, m_verbose);
    return false;
  }
  m_output_file = new TFile(m_output_filename.c_str(), "RECREATE");

  //open the input files
  Log("DEBUG: TriggersComparison::Initialise opening input file 1...", DEBUG2, m_verbose);
  if(! m_variables.Get("inputfilename1", m_input_filename1)) {
    Log("ERROR: inputfilename1 configuration not found. Cancelling initialisation", ERROR, m_verbose);
    return false;
  }
  m_input_file1 = new TFile(m_input_filename1.c_str(), "READ");
  m_header_tree1 = (TTree*)m_input_file1->Get("header");
  m_header_tree1->SetBranchAddress("interpose_interval",&m_interpose_interval1);
  m_header_tree1->GetEntry(0);
  m_triggers_tree1 = (TTree*)m_input_file1->Get("triggers");
  m_triggers_tree1->SetBranchAddress("trigger_time",&the_trigger_time1);
  m_triggers_tree1->SetBranchAddress("readout_start_time",&the_readout_start_time1);
  m_triggers_tree1->SetBranchAddress("readout_end_time",&the_readout_end_time1);

  Log("DEBUG: TriggersComparison::Initialise opening input file 2...", DEBUG2, m_verbose);
  if(! m_variables.Get("inputfilename2", m_input_filename2)) {
    Log("ERROR: inputfilename2 configuration not found. Cancelling initialisation", ERROR, m_verbose);
    return false;
  }
  m_input_file2 = new TFile(m_input_filename2.c_str(), "READ");
  m_header_tree2 = (TTree*)m_input_file2->Get("header");
  m_header_tree2->SetBranchAddress("interpose_interval",&m_interpose_interval2);
  m_header_tree2->GetEntry(0);
  m_triggers_tree2 = (TTree*)m_input_file2->Get("triggers");
  m_triggers_tree2->SetBranchAddress("trigger_time",&the_trigger_time2);
  m_triggers_tree2->SetBranchAddress("readout_start_time",&the_readout_start_time2);
  m_triggers_tree2->SetBranchAddress("readout_end_time",&the_readout_end_time2);

  std::clog << " m_interpose_interval1 " << m_interpose_interval1 << " m_interpose_interval2 " << m_interpose_interval2 << std::endl;

  //set number of events
  if(! m_variables.Get("nevents1",  m_n_events1) ) {
    m_n_events1 = -1;
  }
  if(! m_variables.Get("first_event1",  m_first_event_num1) ) {
    m_first_event_num1 = 0;
  }
  if(m_n_events1 <= 0)
    m_n_events1 = m_triggers_tree1->GetEntries();
  else if (m_n_events1 > m_triggers_tree1->GetEntries())
    m_n_events1 = m_triggers_tree1->GetEntries();
  if(m_first_event_num1 >= m_n_events1) {
    m_first_event_num1 = m_n_events1 - 1;
  }
  else if(m_first_event_num1 < 0) {
    m_first_event_num1 = 0;
  }
  m_current_event_num1 = m_first_event_num1;
  if(! m_variables.Get("nevents2",  m_n_events2) ) {
    m_n_events2 = -1;
  }
  if(! m_variables.Get("first_event2",  m_first_event_num2) ) {
    m_first_event_num2 = 0;
  }
  if(m_n_events2 <= 0)
    m_n_events2 = m_triggers_tree2->GetEntries();
  else if (m_n_events2 > m_triggers_tree2->GetEntries())
    m_n_events2 = m_triggers_tree2->GetEntries();
  if(m_first_event_num2 >= m_n_events2) {
    m_first_event_num2 = m_n_events2 - 1;
  }
  else if(m_first_event_num2 < 0) {
    m_first_event_num2 = 0;
  }
  m_current_event_num2 = m_first_event_num2;

  m_variables.Get("timebinsize", timebinsize);


  float min_trigger_time_1 = m_triggers_tree1->GetMinimum("trigger_time");
  float max_trigger_time_1 = m_triggers_tree1->GetMaximum("trigger_time");

  float min_trigger_time_2 = m_triggers_tree2->GetMinimum("trigger_time");
  float max_trigger_time_2 = m_triggers_tree2->GetMaximum("trigger_time");

  min_readout_time_1 = m_triggers_tree1->GetMinimum("readout_start_time");
  max_readout_time_1 = m_triggers_tree1->GetMaximum("readout_end_time");

  min_readout_time_2 = m_triggers_tree2->GetMinimum("readout_start_time");
  max_readout_time_2 = m_triggers_tree2->GetMaximum("readout_end_time");

  h_triggertime_1 = new TH1F("h_triggertime_1","h_triggertime_1; time [ns];",(int)((max_trigger_time_1 - min_trigger_time_1 + 1)/timebinsize), min_trigger_time_1-timebinsize/2.,max_trigger_time_1+timebinsize/2.);
  h_triggertime_1->SetLineColor(kBlack);
  h_triggertime_1->SetLineWidth(2);
  h_triggertime_2 = new TH1F("h_triggertime_2","h_triggertime_2; time [ns];",(int)((max_trigger_time_2 - min_trigger_time_2 + 1)/timebinsize), min_trigger_time_2-timebinsize/2.,max_trigger_time_2+timebinsize/2.);
  h_triggertime_2->SetLineColor(kRed);
  h_triggertime_2->SetLineWidth(2);

  h_acceptedtime_1 = new TH1I("h_acceptedtime_1","h_acceptedtime_1; time [ns];",(int)((max_readout_time_1 - min_readout_time_1 + 1)/timebinsize), min_readout_time_1-timebinsize/2.,max_readout_time_1+timebinsize/2.);
  h_acceptedtime_1->SetLineColor(kBlack);
  h_acceptedtime_1->SetLineWidth(2);
  h_acceptedtime_2 = new TH1I("h_acceptedtime_2","h_acceptedtime_2; time [ns];",(int)((max_readout_time_2 - min_readout_time_2 + 1)/timebinsize), min_readout_time_2-timebinsize/2.,max_readout_time_2+timebinsize/2.);
  h_acceptedtime_2->SetLineColor(kRed);
  h_acceptedtime_2->SetLineWidth(2);

  h_selections_intersection = new TH1F("h_selections_intersection","h_selections_intersection",3,-2.5,1.5);
  h_selections_intersection->SetLineColor(kBlack);
  h_selections_intersection->SetLineWidth(2);

  if(m_stopwatch) Log(m_stopwatch->Result("Initialise"), INFO, m_verbose);

  return true;
}


bool TriggersComparison::Execute(){
  if(m_stopwatch) m_stopwatch->Start();

  if(m_n_events1 <= 0) {
    return true;
  }

  //get the digits
  if(m_triggers_tree1->GetEntry(m_current_event_num1)) {
    h_triggertime_1->Fill(the_trigger_time1);
    for(int ibin = h_acceptedtime_1->GetXaxis()->FindBin(the_readout_start_time1); ibin <= h_acceptedtime_1->GetXaxis()->FindBin(the_readout_end_time1); ibin ++){
      h_acceptedtime_1->SetBinContent(ibin,1);
    }
  }

  if(m_triggers_tree2->GetEntry(m_current_event_num2)) {
    h_triggertime_2->Fill(the_trigger_time2);
    for(int ibin = h_acceptedtime_2->GetXaxis()->FindBin(the_readout_start_time2); ibin <= h_acceptedtime_2->GetXaxis()->FindBin(the_readout_end_time2); ibin ++){
      h_acceptedtime_2->SetBinContent(ibin,1);
    }
  }


  std::clog << " m_current_event_num1 " << m_current_event_num1 << std::endl;
  std::clog << " m_current_event_num2 " << m_current_event_num2 << std::endl;

  //and finally, increment event counter
  m_current_event_num1++;
  m_current_event_num2++;

  //and flag to exit the Execute() loop, if appropriate
  if(m_current_event_num1 >= m_n_events1
     && m_current_event_num2 >= m_n_events2){
    if(m_stopwatch) m_stopwatch->Stop();
    return false;
  }

  if(m_stopwatch) m_stopwatch->Stop();

  return true;
}


bool TriggersComparison::Finalise(){

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Execute", m_stopwatch_file), INFO, m_verbose);
    m_stopwatch->Start();
  }

  float time_min = std::min(min_readout_time_1, min_readout_time_2);
  float time_max = std::max(max_readout_time_1, max_readout_time_2);
  float local_time;

  for(int itime = 0; itime <= (int)(time_max - time_min + 1)/timebinsize; itime ++){
    local_time = time_min + itime*timebinsize;
    bool trigger1 = (bool)h_acceptedtime_1->GetBinContent(h_acceptedtime_1->FindBin(local_time));
    bool trigger2 = (bool)h_acceptedtime_2->GetBinContent(h_acceptedtime_2->FindBin(local_time));

    if( trigger1 && trigger2 )
      h_selections_intersection->Fill(0.,timebinsize);
    else if( trigger1 )
      h_selections_intersection->Fill(-1.,timebinsize);
    else if( trigger2 )
      h_selections_intersection->Fill(1.,timebinsize);
    else
      h_selections_intersection->Fill(-2.,timebinsize);
  }

  m_output_file->cd();

  h_triggertime_1->Write();
  h_triggertime_2->Write();
  h_acceptedtime_1->Write();
  h_acceptedtime_2->Write();
  h_selections_intersection->Write();

  m_output_file->Close();

  m_input_file1->Close();
  m_input_file2->Close();

  delete m_input_file1;
  delete m_input_file2;
  delete m_output_file;

  if(m_stopwatch) {
    Log(m_stopwatch->Result("Finalise"), INFO, m_verbose);
    delete m_stopwatch;
  }
  
  return true;
}
