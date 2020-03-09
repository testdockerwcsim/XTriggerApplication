#include "PrepareSubSamples.h"

PrepareSubSamples::PrepareSubSamples():Tool(){}


bool PrepareSubSamples::Initialise(std::string configfile, DataModel &data){

  m_data= &data;

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  verbose = 0;
  m_variables.Get("verbose", verbose);

  ss << "DEBUG: In PrepareSubSamples::Initialise";
  StreamToLog(DEBUG1);

  m_variables.Get("sample_width", m_sample_width);
  m_variables.Get("sample_overlap", m_sample_overlap);

  // Configuration is in ns, timestamps are in different unit
  // Convert here
  m_sample_width /= SubSample::s_TIMESTAMP_TO_RELATIVE_TIME;
  m_sample_overlap /= SubSample::s_TIMESTAMP_TO_RELATIVE_TIME;

  ss << "DEBUG: Leaving PrepareSubSamples::Initialise";
  StreamToLog(DEBUG1);
  return true;
}


bool PrepareSubSamples::Execute(){

  ss << "DEBUG: Entering PrepareSubSamples::Execute";
  StreamToLog(DEBUG1);

  // Split ID samples
  std::vector<SubSample> new_samples;
  for (std::vector<SubSample>::iterator it = m_data->IDSamples.begin(); it != m_data->IDSamples.end(); ++it){
    ss << "DEBUG: Splitting ID sample";
    StreamToLog(DEBUG2);
    std::vector<SubSample> temp_samples = it->Split(m_sample_width, m_sample_overlap);
    new_samples.insert(new_samples.end(), temp_samples.begin(), temp_samples.end());
    ss << "DEBUG:   Created " << temp_samples.size() << " samples at times (timestamp unit != hit time unit):";
    StreamToLog(DEBUG3);
    for (std::vector<SubSample>::iterator it2 = temp_samples.begin(); it2 != temp_samples.end(); ++it2){
      ss << "DEBUG:   " << it2->m_timestamp << " First hit: " << (it2->m_time.size()==0 ? -999 : it2->m_time.at(0));
    }
    StreamToLog(DEBUG3);
  }
  m_data->IDSamples = new_samples;

  // Split OD samples
  new_samples.clear();
  for (std::vector<SubSample>::iterator it = m_data->ODSamples.begin(); it != m_data->ODSamples.end(); ++it){
    ss << "DEBUG: Splitting OD sample";
    StreamToLog(DEBUG2);
    std::vector<SubSample> temp_samples = it->Split(m_sample_width, m_sample_overlap);
    new_samples.insert(new_samples.end(), temp_samples.begin(), temp_samples.end());
    ss << "DEBUG:   Created " << temp_samples.size() << " samples at times (timestamp unit != hit time unit):";
    StreamToLog(DEBUG3);
    for (std::vector<SubSample>::iterator it2 = temp_samples.begin(); it2 != temp_samples.end(); ++it2){
      ss << "DEBUG:   " << it2->m_timestamp << " First hit: " << (it2->m_time.size()==0 ? -999 : it2->m_time.at(0));
    }
    StreamToLog(DEBUG3);
  }
  m_data->ODSamples = new_samples;

  ss << "DEBUG: Exiting PrepareSubSamples::Execute";
  StreamToLog(DEBUG1);

  return true;
}


bool PrepareSubSamples::Finalise(){

  return true;
}
