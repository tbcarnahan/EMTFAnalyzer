#ifndef MatchersRecoUnpTrkDR_h
#define MatchersRecoUnpTrkDR_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/RecoMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFUnpTrackInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct RecoUnpTrkDR {

  void Match( RecoMuonInfo & recoMuons, EMTFUnpTrackInfo & emtfTrks, 
	      const float min_reco_eta, const float max_reco_eta, const float max_match_dR );
};

#endif
