
#ifndef BranchesEMTFHitInfo_h
#define BranchesEMTFHitInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// LCTDigi classes
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h"


//////////////////////////////
///  LCT Digi information  ///
//////////////////////////////
struct LCTDigiInfo {
  /*
  std::vector<TString> ints = {{"nHits", "nHitsCSC", "nHitsRPC", "nHitsBX0", "nHitsCSCBX0", "nHitsRPCBX0", "nHitsGEM", "nHitsGEMBX0"}};
  std::vector<TString> vFlt = {{"hit_eta", "hit_theta", "hit_phi", "hit_phi_loc",
                                "hit_eta_sim", "hit_theta_sim", "hit_phi_sim"}};
  std::vector<TString> vInt = {{"hit_eta_int", "hit_theta_int", "hit_phi_int", "hit_endcap", "hit_sector", "hit_sector_index", "hit_station",
                                "hit_ring", "hit_CSC_ID", "hit_chamber", "hit_FR", "hit_pattern", "hit_quality", "hit_roll", "hit_subsector",
                                "hit_isCSC", "hit_isRPC", "hit_valid", "hit_BX", "hit_strip", "hit_strip_hi", "hit_strip_low", "hit_wire", "hit_neighbor", "hit_pattern_Run3",
                                "hit_match_iSimHit", "hit_sim_match_exact", "hit_match_iSeg", "hit_match_iSeg2", "hit_match_nSegs", "hit_seg_match_unique",
                                "hit_isGEM"}};
  */
  
  std::vector<TString> ints = {{"lct_pattern"}};

  
  std::map<TString, int> mInts;
  //std::map<TString, std::vector<float> > mVFlt;
  //std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); }
  void Fill(const CSCCorrelatedLCTDigi& lctDigi);

  bool ignoreGE11;
  bool ignoreGE21;
  bool ignoreRE31;
  bool ignoreRE41;
  bool ignoreDT;
  bool ignoreME0;
};