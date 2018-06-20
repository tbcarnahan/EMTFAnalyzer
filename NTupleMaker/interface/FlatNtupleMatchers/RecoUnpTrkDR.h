#ifndef FlatNtupleMatchersRecoUnpTrkDR_h
#define FlatNtupleMatchersRecoUnpTrkDR_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFUnpTrackInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct RecoUnpTrkDR {

  void Match( RecoMuonInfo & recoMuons, EMTFUnpTrackInfo & emtfTrks, 
	      const float min_reco_eta, const float max_reco_eta, const float max_match_dR );
};

#endif
