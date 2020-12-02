
#ifndef BranchesEMTFHitInfo_h
#define BranchesEMTFHitInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// EMTF classes
#include "DataFormats/L1TMuon/interface/EMTFHit.h"


//////////////////////////////
///  EMTF hit information  ///
//////////////////////////////
struct EMTFHitInfo {
  std::vector<TString> ints = {{"nHits", "nHitsCSC", "nHitsRPC", "nHitsBX0", "nHitsCSCBX0", "nHitsRPCBX0", "nHitsGEM", "nHitsGEMBX0"}};
  std::vector<TString> vFlt = {{"hit_eta", "hit_theta", "hit_phi", "hit_phi_loc",
                                "hit_eta_sim", "hit_theta_sim", "hit_phi_sim"}};
  std::vector<TString> vInt = {{"hit_eta_int", "hit_theta_int", "hit_phi_int", "hit_endcap", "hit_sector", "hit_sector_index", "hit_station",
                                "hit_ring", "hit_CSC_ID", "hit_chamber", "hit_FR", "hit_pattern", "hit_quality", "hit_roll", "hit_subsector",
                                "hit_isCSC", "hit_isRPC", "hit_valid", "hit_BX", "hit_strip", "hit_strip_hi", "hit_strip_low", "hit_wire", "hit_neighbor",
                                "hit_match_iSimHit", "hit_sim_match_exact", "hit_match_iSeg", "hit_match_iSeg2", "hit_match_nSegs", "hit_seg_match_unique",
                                "hit_isGEM",
                                "hit_strip_quart", "hit_strip_eight", "hit_strip_quart_bit", "hit_strip_eight_bit",
                                "hit_pattern_run3", "hit_slope", "hit_bend"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); }
  void Fill(const l1t::EMTFHit & emtfHit);

  bool ignoreGE11;
  bool ignoreGE21;
  bool ignoreRE31;
  bool ignoreRE41;
  bool ignoreDT;
  bool ignoreME0;
};


/////////////////////////////////
///  Print out for debugging  ///
/////////////////////////////////

inline void PrintEMTFHit( const l1t::EMTFHit & hit ) {
  if (hit.Is_CSC()) {
    std::cout << "CSC LCT in BX " << hit.BX() << ", endcap " << hit.Endcap() << ", sector " << hit.Sector()
              << " (" << hit.Sector_idx() << "), subsector " << hit.Subsector() << ", station " << hit.Station()
              << ", ring " << hit.Ring() << ", CSC ID " << hit.CSC_ID() << ", chamber " << hit.Chamber()
              << ", neighbor " << hit.Neighbor() << ", pattern " << hit.Pattern() << ", quality " << hit.Quality()
              << ", strip " << hit.Strip() << ", wire " << hit.Wire() << std::endl;
  }
  else if (hit.Is_RPC()) {
    std::cout << "RPC hit in BX " << hit.BX() << ", endcap " << hit.Endcap() << ", sector " << hit.Sector()
	      << " (" << hit.Sector_idx() << "), subsector " << hit.Subsector() << ", station " << hit.Station()
	      << ", ring " << hit.Ring() << ", chamber " << hit.Chamber() << ", neighbor " << hit.Neighbor()
	      << ", theta " << hit.Theta_fp() << ", phi " << hit.Phi_fp() << std::endl;
  }
  else if (hit.Is_GEM()) {
    std::cout << "GEM hit in BX " << hit.BX() << ", endcap " << hit.Endcap() << ", station " << hit.Station()
              << ", ring " << hit.Ring() << ", chamber " << hit.Chamber() << ", roll " << hit.Roll()
              << ", neighbor " << hit.Neighbor() << ", pad " << hit.Strip() << std::endl;
  }
  else {
    std::cout << "EMTF hit is neither CSC nor RPC nor GEM?!? " << hit.Is_CSC() << hit.Is_RPC() << hit.Is_GEM() <<std::endl;
  }
}

inline void PrintHit( const std::map<TString, std::vector<int> > * iHit , const int i) {

  if (ACCESS(*iHit, "hit_isCSC").at(i)) {
    std::cout << "* CSC LCT in BX " << ACCESS(*iHit, "hit_BX").at(i) << ", endcap " << ACCESS(*iHit, "hit_endcap").at(i)
              << ", sector " << ACCESS(*iHit, "hit_sector").at(i) << " (" << ACCESS(*iHit, "hit_sector_index").at(i)
              << "), subsector " << ACCESS(*iHit, "hit_subsector").at(i) << ", station " << ACCESS(*iHit, "hit_station").at(i)
              << ", ring " << ACCESS(*iHit, "hit_ring").at(i) << ", CSC ID " << ACCESS(*iHit, "hit_CSC_ID").at(i)
              << ", chamber " << ACCESS(*iHit, "hit_chamber").at(i) << ", neighbor " << ACCESS(*iHit, "hit_neighbor").at(i)
              << ", pattern " << ACCESS(*iHit, "hit_pattern").at(i) << ", quality " << ACCESS(*iHit, "hit_quality").at(i)
              << ", strip " << ACCESS(*iHit, "hit_strip").at(i)
              << ", wire " << ACCESS(*iHit, "hit_wire").at(i) << std::endl;
  }
  else if (ACCESS(*iHit, "hit_isRPC").at(i)) {
    std::cout << "* RPC hit in BX " << ACCESS(*iHit, "hit_BX").at(i) << ", endcap " << ACCESS(*iHit, "hit_endcap").at(i)
	      << ", sector " << ACCESS(*iHit, "hit_sector").at(i) << " (" << ACCESS(*iHit, "hit_sector_index").at(i)
	      << "), subsector " << ACCESS(*iHit, "hit_subsector").at(i) << ", station " << ACCESS(*iHit, "hit_station").at(i)
	      << ", ring " << ACCESS(*iHit, "hit_ring").at(i) << ", chamber " << ACCESS(*iHit, "hit_chamber").at(i)
	      << ", neighbor " << ACCESS(*iHit, "hit_neighbor").at(i)
	      << ", theta " << ACCESS(*iHit, "hit_theta_int").at(i) << ", phi " << ACCESS(*iHit, "hit_phi_int").at(i) << std::endl;
  }
  else if (ACCESS(*iHit, "hit_isGEM").at(i)) {
    std::cout << "* GEM hit in BX " << ACCESS(*iHit, "hit_BX").at(i)
              << ", endcap " << ACCESS(*iHit, "hit_endcap").at(i)
              << ", station " << ACCESS(*iHit, "hit_station").at(i)
              << ", ring " << ACCESS(*iHit, "hit_ring").at(i)
              << ", chamber " << ACCESS(*iHit, "hit_chamber").at(i)
              << ", roll " << ACCESS(*iHit, "hit_roll").at(i)
              << ", neighbor " << ACCESS(*iHit, "hit_neighbor").at(i)
              << ", strip " << ACCESS(*iHit, "hit_strip").at(i) << std::endl;
  }
  else std::cout << "iHit with index " << i << " is neither CSC nor RPC nor GEM?!? " << ACCESS(*iHit, "hit_isCSC").at(i)<<ACCESS(*iHit, "hit_isRPC").at(i)<<ACCESS(*iHit, "hit_isGEM").at(i)<<std::endl;
}

#endif  // #ifndef BranchesEMTFHitInfo_h
