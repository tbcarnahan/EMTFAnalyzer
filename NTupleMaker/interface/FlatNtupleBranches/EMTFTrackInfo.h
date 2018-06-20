
#ifndef FlatNtupleBranchesEMTFTrackInfo_h
#define FlatNtupleBranchesEMTFTrackInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// EMTF hit branch
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFHitInfo.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// EMTF classes
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
#include "DataFormats/L1TMuon/interface/EMTFTrack.h"


////////////////////////////////
///  EMTF track information  ///
////////////////////////////////
struct EMTFTrackInfo {
  std::vector<TString> ints = {{"nTracks", "nTracksBX0"}};
  std::vector<TString> vFlt = {{"trk_pt", "trk_eta", "trk_theta", "trk_phi", "trk_phi_loc",
                                "trk_dR_match_dEta", "trk_dR_match_dPhi", "trk_dR_match_dR",
				"trk_unp_match_dEta", "trk_unp_match_dPhi", "trk_unp_match_dR"}};
  std::vector<TString> vInt = {{"trk_pt_int", "trk_eta_int", "trk_theta_int", "trk_phi_int", "trk_BX", "trk_endcap", 
				"trk_sector", "trk_sector_index", "trk_mode", "trk_mode_CSC", "trk_mode_RPC", "trk_mode_neighbor",
				"trk_qual", "trk_charge", "trk_nHits", "trk_nRPC", "trk_nNeighbor", "trk_dBX", "trk_dPhi_int", "trk_dTheta_int",
                                "trk_dR_match_iReco", "trk_dR_match_nReco", "trk_dR_match_nRecoSoft", "trk_dR_match_unique",
                                "trk_unp_match_iTrk", "trk_unp_match_dBX", "trk_unp_match_unique", "trk_unp_match_exact"}};
  std::vector<TString> vvInt = {{"trk_iHit"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;
  std::map<TString, std::vector<std::vector<int> > > mVVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); CHECKSIZE(mVVInt); }
  void Fill(const l1t::EMTFTrack & emtfTrk, const EMTFHitInfo & hits);
};


#endif  // #ifndef FlatNtupleBranchesEMTFTrackInfo_h
