#ifndef FlatNtupleBranchesRecoPairInfo_h
#define FlatNtupleBranchesRecoPairInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// RECO muon branch
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonInfo.h"

// TLorentzVector
#include "TLorentzVector.h"


///////////////////////////////
///  RECO pair information  ///
///////////////////////////////

struct RecoPairInfo {

  std::vector<TString> ints = {{"nRecoPairs", "nRecoPairsFwd"}};
  std::vector<TString> vFlt = {{"recoPair_p", "recoPair_pt", "recoPair_pz", "recoPair_mass",
				"recoPair_eta", "recoPair_theta", "recoPair_phi",
				"recoPair_dR", "recoPair_dEta", "recoPair_dTheta", "recoPair_dPhi",
				"recoPair_dR_St1", "recoPair_dEta_St1", "recoPair_dTheta_St1", "recoPair_dPhi_St1",
				"recoPair_dR_St2", "recoPair_dEta_St2", "recoPair_dTheta_St2", "recoPair_dPhi_St2"}};
  std::vector<TString> vInt = {{"recoPair_iReco1", "recoPair_iReco2"}};

  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); }
  void Fill(const RecoMuonInfo & recoMuons);

};

#endif
