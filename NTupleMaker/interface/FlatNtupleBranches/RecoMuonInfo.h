#ifndef FlatNtupleBranchesRecoMuonInfo_h
#define FlatNtupleBranchesRecoMuonInfo_h

//-------------------------------------------------------------------------------
// Create by Wei Shi
//-------------------------------------------------------------------------------
// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// EMTF classes
#include "L1Trigger/L1TNtuples/interface/L1AnalysisRecoMuon2.h"

////////////////////////////////
///  EMTF track information  ///
////////////////////////////////
struct RecoMuonInfo {
  std::vector<TString> ints = {{"nRecoMuons"}};
  std::vector<TString> vFlt = {{"reco_pt", "reco_eta", "reco_phi", "reco_St1_eta", "reco_St1_phi", "reco_St2_eta", "reco_St2_phi"}};
  std::vector<TString> vInt = {{"reco_charge", "reco_loose", "reco_medium", "reco_tight"}};
  std::vector<TString> vvInt = {{"trk_iHit"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;
  std::map<TString, std::vector<std::vector<int> > > mVVInt;

  void Initialize();
  void Reset();
  void Fill(const L1Analysis::L1AnalysisRecoMuon2 & recoMuon_);
};

#endif
