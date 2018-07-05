#ifndef MatchersUnpEmuTrkDR_h
#define MatchersUnpEmuTrkDR_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFTrackInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFUnpTrackInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct UnpEmuTrkDR {

  void Match( EMTFUnpTrackInfo & unpTrks, EMTFTrackInfo & emuTrks, const float max_match_dR );

};

#endif
