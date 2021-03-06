
#ifndef BranchesGenMuonInfo_h
#define BranchesGenMuonInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// GEN particles
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


//////////////////////////////
///  GEN muon information  ///
//////////////////////////////
struct GenMuonInfo {
  std::vector<TString> ints = {{"nGenMuons"}};
  std::vector<TString> vFlt = {{"mu_pt", "mu_eta", "mu_theta", "mu_phi"}};
  std::vector<TString> vInt = {{"mu_charge"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); }
  void Fill(const reco::GenParticle & genMuon);
};


#endif  // #ifndef BranchesGenMuonInfo_h
