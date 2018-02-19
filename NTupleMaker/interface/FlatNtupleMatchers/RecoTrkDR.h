#ifndef FlatNtupleMatchersRecoTrkDR_h
#define FlatNtupleMatchersRecoTrkDR_h

/* #include "TMath.h" */
/* #include <iostream> */
/* #include <vector> */

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonInfo.h"
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
#include "DataFormats/L1TMuon/interface/EMTFTrack.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFTrackInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct RecoTrkDR {

  void Match( RecoMuonInfo & recoMuons, EMTFTrackInfo & emtfTrks, 
	      const float min_reco_eta, const float max_reco_eta, const float max_match_dR );
};

#endif
