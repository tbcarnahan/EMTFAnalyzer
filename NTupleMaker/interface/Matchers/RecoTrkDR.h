#ifndef MatchersRecoTrkDR_h
#define MatchersRecoTrkDR_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/RecoMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFTrackInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct RecoTrkDR {

  void Match( RecoMuonInfo & recoMuons, EMTFTrackInfo & emtfTrks, 
	      const float min_reco_eta, const float max_reco_eta, const float max_match_dR );
};

#endif
