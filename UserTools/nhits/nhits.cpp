#include "nhits.h"

const int nhits::kALongTime = 1E6; // ns = 1ms. event time

nhits::nhits():Tool(){}


bool nhits::Initialise(std::string configfile, DataModel &data){



  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  verbose = 0;
  m_variables.Get("verbose", verbose);

  m_data= &data;

  m_data->triggeroutput=false;



  
  std::string DetectorFile;
  std::string ParameterFile;
  

  m_variables.Get("DetectorFile",DetectorFile);
  m_variables.Get("ParameterFile",ParameterFile);

  // can acess variables directly like this and would be good if you could impliment in your code

  m_variables.Get("trigger_search_window",        fTriggerSearchWindow);
  m_variables.Get("trigger_search_window_step",   fTriggerSearchWindowStep);
  m_variables.Get("trigger_threshold",            fTriggerThreshold);
  m_variables.Get("pretrigger_save_window",       fTriggerSaveWindowPre);
  m_variables.Get("posttrigger_save_window",      fTriggerSaveWindowPost);
  m_variables.Get("trigger_od",                   fTriggerOD);
  
  bool adjust_for_noise;
  m_variables.Get("trigger_threshold_adjust_for_noise", adjust_for_noise);
  if(adjust_for_noise) {
    int npmts = fTriggerOD ? m_data->ODNPMTs : m_data->IDNPMTs;
    double dark_rate_kHZ = fTriggerOD ? m_data->ODPMTDarkRate : m_data->IDPMTDarkRate;
    double trigger_window_seconds = fTriggerSearchWindow * 1E-9;
    double dark_rate_Hz = dark_rate_kHZ * 1000;
    double average_occupancy = dark_rate_Hz * trigger_window_seconds * npmts;

    ss << "INFO: Average number of PMTs in detector active in a " << fTriggerSearchWindow
       << "ns window with a dark noise rate of " << dark_rate_kHZ
       << "kHz is " << average_occupancy
       << " (" << npmts << " total PMTs)";
    StreamToLog(INFO);
    ss << "INFO: Updating the NDigits threshold, from " << fTriggerThreshold
       << " to " << fTriggerThreshold + round(average_occupancy) 
       << " in window " << fTriggerSaveWindowPre << " to " << fTriggerSaveWindowPost
       << " with fTriggerOD " << fTriggerOD
       << std::endl;
    StreamToLog(INFO);
    fTriggerThreshold += round(average_occupancy);    
  }

#ifdef GPU
  //  GPU_daq::nhits_initialize();

  GPU_daq::nhits_initialize_ToolDAQ(ParameterFile,m_data->IDGeom.size(),fTriggerSearchWindow, fTriggerSearchWindowStep, fTriggerThreshold, fTriggerSaveWindowPre, fTriggerSaveWindowPost);
#endif



  return true;
}


bool nhits::Execute(){

  std::vector<SubSample> & samples = fTriggerOD ? (m_data->ODSamples) : (m_data->IDSamples);

  for( std::vector<SubSample>::const_iterator is=samples.begin(); is!=samples.end(); ++is){
#ifdef GPU   

    std::vector<int> trigger_ns;
    std::vector<int> trigger_ts;
    GPU_daq::nhits_execute(is->m_PMTid, is->m_time_int, &trigger_ns, &trigger_ts);
    for(int i=0; i<trigger_ns.size(); i++){
      m_data->IDTriggers.AddTrigger(kTriggerNDigits,
				    trigger_ts[i] - fTriggerSaveWindowPre, 
				    trigger_ts[i] + fTriggerSaveWindowPost,
				    trigger_ts[i],
				    std::vector<float>(1, trigger_ns[i]));

      printf(" trigger! time  %d nhits %d \n", trigger_ts[i], trigger_ns[i]);
    }
#else
  AlgNDigits(&(*is));
#endif
  }


  return true;
}

void nhits::AlgNDigits(const SubSample * sample)
{
  //we will try to find triggers
  //loop over PMTs, and Digits in each PMT.  If ndigits > Threshhold in a time window, then we have a trigger

  const unsigned int ndigits = sample->m_charge.size();
  ss << "DEBUG: nhits::AlgNDigits(). Number of entries in input digit collection: " << ndigits;
  StreamToLog(DEBUG1);
  
  //Loop over each digit
  float firsthit = +nhits::kALongTime;
  float lasthit  = -nhits::kALongTime;
  for(unsigned int idigit = 0; idigit < ndigits; idigit++) {
    //    ss << " input " << idigit << " PMT " <<  sample->m_PMTid.at(idigit)<< " time " << sample->m_time.at(idigit) ;
    float digit_time = sample->m_time.at(idigit);
    //get the time of the last hit (to make the loop shorter)
    if(digit_time > lasthit)
      lasthit = digit_time;
    if(digit_time < firsthit)
      firsthit = digit_time;
  }//loop over Digits
  int window_start_time = firsthit;
  window_start_time -= window_start_time % 5;
  int window_end_time   = lasthit - fTriggerSearchWindow + fTriggerSearchWindowStep;
  window_end_time -= window_end_time % 5;
  ss << "DEBUG: Found first/last hits. Looping from " << window_start_time
     << " to " << window_end_time 
     << " in steps of " << fTriggerSearchWindowStep;
  StreamToLog(DEBUG1);

  std::vector<float> digit_times;

  // the upper time limit is set to the final possible full trigger window
  while(window_start_time <= window_end_time) {
    int n_digits = 0;
    float triggertime; //save each digit time, because the trigger time is the time of the first hit above threshold
    bool triggerfound = false;
    digit_times.clear();
    
    //Loop over each digit
    for(unsigned int idigit = 0; idigit < ndigits; idigit++) {
      //int tube   = sample->m_PMTid.at(idigit);
      //float charge = sample->m_charge.at(idigit);
#if 1
      float digit_time = sample->m_time.at(idigit);
#else
      //F. Nova degrade info from float to int to match GPU run
      int digit_time = (int)sample->m_time.at(idigit);
#endif
      //hit in trigger window?
      if(digit_time >= window_start_time && digit_time <= (window_start_time + fTriggerSearchWindow)) {
	n_digits++;
	digit_times.push_back(digit_time);
      }
    }//loop over Digits
    //F. Nova verbose output
    //printf("interval (%d, %f) has %d hits \n", window_start_time, window_start_time + fTriggerSearchWindow, n_digits);

    //if over threshold, issue trigger
    if(n_digits > fTriggerThreshold) {
      //The trigger time is the time of the first hit above threshold
      std::sort(digit_times.begin(), digit_times.end());
#if 1
      triggertime = digit_times[fTriggerThreshold];
      triggertime -= (int)triggertime % 5;
#else
      //F. Nova degrade time info to be in the middle of the window
      triggertime = window_start_time + fTriggerSearchWindow;
#endif
      triggerfound = true;
      m_data->IDTriggers.AddTrigger(kTriggerNDigits,
				    triggertime - fTriggerSaveWindowPre, 
				    triggertime + fTriggerSaveWindowPost,
				    triggertime,
				    std::vector<float>(1, n_digits));
    }

    if(n_digits)
      ss << "DEBUG: " << n_digits << " digits found in 200nsec trigger window ["
	 << window_start_time << ", " << window_start_time + fTriggerSearchWindow
	 << "]. Threshold is: " << fTriggerThreshold;
    StreamToLog(DEBUG2);

    //move onto the next go through the timing loop
    if(triggerfound) {
      window_start_time = triggertime + fTriggerSaveWindowPost;
      ss << "INFO: nhits trigger found at time " << triggertime
	 << " with " << n_digits << " digits in the decision window";
      StreamToLog(INFO);
    }//triggerfound
    else {
      window_start_time += fTriggerSearchWindowStep;
    }

  }//sliding trigger window while loop
  
  ss << "INFO: Found " << m_data->IDTriggers.m_N << " NDigit triggers";
  StreamToLog(INFO);
}

bool nhits::Finalise(){


#ifdef GPU
  GPU_daq::nhits_finalize();
#endif


  return true;
}
