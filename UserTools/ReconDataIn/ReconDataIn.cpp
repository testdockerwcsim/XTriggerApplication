#include "ReconDataIn.h"

ReconDataIn::ReconDataIn():Tool(){}


bool ReconDataIn::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  verbose = 0;
  m_variables.Get("verbose", verbose);

  m_data= &data;

  //get the filename(s)
  std::string infilestr;
  if(! m_variables.Get("infilenames", infilestr)) {
    Log("ERROR: infilenames configuration not found. Cancelling initialisation", ERROR, verbose);
    return false;
  }
  //TODO add ability to get filelist, like WCSimReader() - need to take functions out of there & put into a general utilties file? Library? namespace?`

  //open the chain
  fTreeRecon = new TChain("reconTree");
  fTreeRecon->Add(infilestr.c_str());

  //setup the branches
  //fTreeRecon->SetBranchAddress("EventNum", &fEvtNum); //Not usde
  fTreeRecon->SetBranchAddress("TriggerNum", &fRTTriggerNum);
  fTreeRecon->SetBranchAddress("NDigits", &fRTNHits);
  fTreeRecon->SetBranchAddress("Energy", &fRTEnergy);
  fTreeRecon->SetBranchAddress("Reconstructer", &fRTReconstructerInt);
  fTreeRecon->SetBranchAddress("Time", &fRTTime);
  fTreeRecon->SetBranchAddress("Vertex", fRTVertex);
  fTreeRecon->SetBranchAddress("GoodnessOfFit", &fRTGoodnessOfFit);
  fTreeRecon->SetBranchAddress("GoodnessOfTimeFit", &fRTGoodnessOfTimeFit);
  fTreeRecon->SetBranchAddress("HasDirection", &fRTHasDirection);
  fTreeRecon->SetBranchAddress("DirectionEuler", fRTDirectionEuler);
  fTreeRecon->SetBranchAddress("CherenkovCone", fRTCherenkovCone);
  fTreeRecon->SetBranchAddress("DirectionLikelihood", &fRTDirectionLikelihood);

  return true;
}


bool ReconDataIn::Execute(){

  const int nrecons = fTreeRecon->GetEntries();
  ss << "DEBUG: Reading the result of " << nrecons << " reconstructions";
  StreamToLog(DEBUG1);
  for(int irecon = 0; irecon < nrecons; irecon++) {
    fTreeRecon->GetEntry(irecon);

    if(fRTHasDirection) {
      m_data->RecoInfo.AddRecon((Reconstructer_t)fRTReconstructerInt, fRTTriggerNum, fRTNHits, fRTTime, &(fRTVertex[0]), fRTGoodnessOfFit, fRTGoodnessOfTimeFit, 
				&(fRTDirectionEuler[0]), &(fRTCherenkovCone[0]), fRTDirectionLikelihood, fRTEnergy);
    }
    else {
      m_data->RecoInfo.AddRecon((Reconstructer_t)fRTReconstructerInt, fRTTriggerNum, fRTNHits, fRTTime, &(fRTVertex[0]), fRTGoodnessOfFit, fRTGoodnessOfTimeFit, true, fRTEnergy);
    }
  }//irecon

  //All data in the chain is put into RecoInfo
  // Therefore only need to call Execute() once
  // Therefore set this data member appropriately
  m_data->vars.Set("StopLoop",1);

  return true;
}


bool ReconDataIn::Finalise(){
  delete fTreeRecon;

  return true;
}
