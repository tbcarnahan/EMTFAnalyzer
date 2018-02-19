#ifndef FlatNtupleMatchersSimUnpHit_h
#define FlatNtupleMatchersSimUnpHit_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFSimHitInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct SimUnpHit {

  void Match( EMTFHitInfo & emtfHits, EMTFSimHitInfo & emtfSimHits ); 

};

#endif
