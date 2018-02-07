#ifndef FlatNtupleBranchesRecoMuonInfo_h
#define FlatNtupleBranchesRecoMuonInfo_h

//-------------------------------------------------------------------------------
// Create by Wei Shi
//-------------------------------------------------------------------------------
// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

////////////////////////////////
///  RECO muon  information  ///
////////////////////////////////

struct RecoMuonInfo {
  std::vector<TString> ints = {{"nRecoMuons"}};
  std::vector<TString> vFlt = {{"reco_pt", "reco_eta", "reco_phi", "reco_St1_eta", "reco_St1_phi", "reco_St2_eta", "reco_St2_phi"}};
  std::vector<TString> vInt = {{"reco_charge", "reco_loose", "reco_medium", "reco_tight"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  void Fill(float reco_pt, float reco_eta, float reco_phi, int reco_charge, 
			      int reco_loose, int reco_medium, int reco_tight, 
			      float reco_St1_eta, float reco_St1_phi,
		        float reco_St2_eta, float reco_St2_phi);
};

#endif
