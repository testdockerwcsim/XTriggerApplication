#ifndef ReconDataIn_H
#define ReconDataIn_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "Stopwatch.h"

class ReconDataIn: public Tool {


 public:

  ReconDataIn();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  TChain * fTreeRecon;

  int    fRTTriggerNum;
  int    fRTNHits;
  double fRTEnergy;
  int    fRTReconstructerInt; //TODO when the enum is actually save in the tree, replace casting from int with Reconstructer_t
  double fRTTime;
  double fRTVertex[4];  //x,y,z
  bool   fRTHasDirection;
  double fRTDirectionEuler[3]; // theta (zenith), phi (azimuth), alpha
  double fRTCherenkovCone[2];  // cos(Cherenkov angle), ellipticity
  double fRTDirectionLikelihood;
  double fRTGoodnessOfFit;
  double fRTGoodnessOfTimeFit;

  /// The stopwatch, if we're using one
  util::Stopwatch * m_stopwatch;
  /// Image filename to save the histogram to, if required
  std::string m_stopwatch_file;

  int m_verbose;

  std::stringstream ss;

  void StreamToLog(int level) {
    Log(ss.str(), level, m_verbose);
    ss.str("");
  }

  enum LogLevel {FATAL=-1, ERROR=0, WARN=1, INFO=2, DEBUG1=3, DEBUG2=4, DEBUG3=5};


};


#endif
