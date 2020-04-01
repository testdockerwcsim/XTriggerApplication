#include "PrepareSubSamples.h"

PrepareSubSamples::PrepareSubSamples():Tool(){}


bool PrepareSubSamples::Initialise(std::string configfile, DataModel &data){

  m_data= &data;

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_verbose = 0;
  m_variables.Get("verbose", m_verbose);

  double temp_width, temp_overlap;
  m_variables.Get("sample_width", temp_width);
  m_variables.Get("sample_overlap", temp_overlap);

  // Configuration is in ns
  m_sample_width = temp_width * TimeDelta::ns;
  m_sample_overlap = temp_overlap * TimeDelta::ns;

  return true;
}

void PrepareSubSamples::SortSubSampleVector(std::vector<SubSample> &samples){
  for (std::vector<SubSample>::iterator it = samples.begin(); it != samples.end(); ++it){
    m_ss << "DEBUG: Sorting sample";
    StreamToLog(DEBUG2);
    it->SortByTime();
  }
}

bool PrepareSubSamples::CheckSubSampleVectorNeedsSplitting(const std::vector<SubSample> &samples){
  for (std::vector<SubSample>::const_iterator it = samples.begin(); it != samples.end(); ++it){
    if (CheckSubSampleNeedsSplitting(*it)){
      return true;
    }
  }
  return false;
}

bool PrepareSubSamples::CheckSubSampleNeedsSplitting(const SubSample &sample){
  int N = sample.m_time.size();
  if (N == 0)
    return false;
  return (TimeDelta(sample.m_time[N-1]) - TimeDelta(sample.m_time[0])) > m_sample_width;
}

std::vector<SubSample> PrepareSubSamples::SplitSubSampleVector(std::vector<SubSample> &samples){
  std::vector<SubSample> new_samples;
  for (std::vector<SubSample>::iterator it = samples.begin(); it != samples.end(); ++it){
    m_ss << "DEBUG: Splitting sample";
    StreamToLog(DEBUG2);
    std::vector<SubSample> temp_samples = it->Split(m_sample_width, m_sample_overlap);
    new_samples.insert(new_samples.end(), temp_samples.begin(), temp_samples.end());
    m_ss << "DEBUG:   Created " << temp_samples.size() << " samples at times (timestamp unit != hit time unit):";
    StreamToLog(DEBUG3);
    for (std::vector<SubSample>::iterator it2 = temp_samples.begin(); it2 != temp_samples.end(); ++it2){
      m_ss << "DEBUG:   " << it2->m_timestamp / TimeDelta::s << " First hit: " << (it2->m_time.size()==0 ? -999 : it2->m_time.at(0));
    }
    StreamToLog(DEBUG3);
  }
  return new_samples;
}

bool PrepareSubSamples::Execute(){


  // Split ID samples
  m_ss << "DEBUG: Preparing " << m_data->IDSamples.size() << " ID samples";
  StreamToLog(DEBUG1);
  SortSubSampleVector(m_data->IDSamples);
  if (CheckSubSampleVectorNeedsSplitting(m_data->IDSamples)){
    m_data->IDSamples = SplitSubSampleVector(m_data->IDSamples);
  }

  // Split OD samples
  m_ss << "DEBUG: Preparing " << m_data->ODSamples.size() << " OD samples";
  StreamToLog(DEBUG1);
  SortSubSampleVector(m_data->ODSamples);
  if (CheckSubSampleVectorNeedsSplitting(m_data->ODSamples)){
    m_data->ODSamples = SplitSubSampleVector(m_data->ODSamples);
  }


  return true;
}


bool PrepareSubSamples::Finalise(){

  return true;
}