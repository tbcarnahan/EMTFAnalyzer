#ifndef FlatNtupleMatchers_LCTSeg_h
#define FlatNtupleMatchers_LCTSeg_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// Specific branches to grab
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/CSCSegInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct LCTSeg {

  void Match(CSCSegInfo & cscSegs, EMTFHitInfo & emtfHits);

};

#endif
