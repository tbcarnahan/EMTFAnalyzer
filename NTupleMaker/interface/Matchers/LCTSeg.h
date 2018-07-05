#ifndef Matchers_LCTSeg_h
#define Matchers_LCTSeg_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// Specific branches to grab
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/CSCSegInfo.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct LCTSeg {

  void Match(CSCSegInfo & cscSegs, EMTFHitInfo & emtfHits);

};

#endif
