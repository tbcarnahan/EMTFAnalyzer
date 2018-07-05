#ifndef MatchersSimUnpHit_h
#define MatchersSimUnpHit_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFSimHitInfo.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/HelperFunctions.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct SimUnpHit {

  void Match( EMTFHitInfo & emtfHits, EMTFSimHitInfo & emtfSimHits ); 

};

#endif
