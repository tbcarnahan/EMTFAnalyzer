#ifndef FlatNtupleMatchersUnpEmuTrkDR_h
#define FlatNtupleMatchersUnpEmuTrkDR_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFTrackInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFUnpTrackInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct UnpEmuTrkDR {

  void Match( EMTFUnpTrackInfo & unpTrks, EMTFTrackInfo & emuTrks, const float max_match_dR );

};

#endif
