
#ifndef FlatNtupleBranchesEMTFUnpTrackInfo_h
#define FlatNtupleBranchesEMTFUnpTrackInfo_h

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
struct EMTFUnpTrackInfo {
  std::vector<TString> ints = {{"nUnpTracks", "nUnpTracksBX0"}};
  std::vector<TString> vFlt = {{"unp_trk_pt", "unp_trk_eta", "unp_trk_theta", "unp_trk_phi", "unp_trk_phi_loc",
				"unp_trk_dR_match_dEta", "unp_trk_dR_match_dPhi", "unp_trk_dR_match_dR",
				"unp_trk_emu_match_dEta", "unp_trk_emu_match_dPhi", "unp_trk_emu_match_dR"}};
  std::vector<TString> vInt = {{"unp_trk_pt_int", "unp_trk_eta_int", "unp_trk_theta_int", "unp_trk_phi_int", "unp_trk_BX","unp_trk_endcap", 
				"unp_trk_sector", "unp_trk_sector_index", "unp_trk_mode", "unp_trk_mode_CSC", "unp_trk_mode_RPC", "unp_trk_mode_neighbor",
				"unp_trk_qual", "unp_trk_charge", "unp_trk_nHits", "unp_trk_nRPC", "unp_trk_nNeighbor", "unp_trk_found_hits",
				"unp_trk_dBX", "unp_trk_dPhi_int", "unp_trk_dTheta_int",
				"unp_trk_dR_match_iReco", "unp_trk_dR_match_iReco2", "unp_trk_dR_match_nReco", "unp_trk_dR_match_nRecoSoft", "unp_trk_dR_match_unique",
				"unp_trk_emu_match_iTrk", "unp_trk_emu_match_iTrk2", "unp_trk_emu_match_dBX", "unp_trk_emu_match_unique", "unp_trk_emu_match_exact"}};

  std::vector<TString> vvInt = {{"unp_trk_iHit"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;
  std::map<TString, std::vector<std::vector<int> > > mVVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); CHECKSIZE(mVVInt); }
  void Fill(const l1t::EMTFTrack & emtfTrk, const EMTFHitInfo & hits);
};


#endif  // #ifndef FlatNtupleBranchesEMTFUnpTrackInfo_h
