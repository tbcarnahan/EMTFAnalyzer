
//#ifndef BranchesLCTDigiInfo_h
//#define BranchesLCTDigiInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// LCTDigi classes
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"


//////////////////////////////
///  LCT Digi information  ///
//////////////////////////////
struct LCTDigiInfo {
  
  std::vector<TString> ints = {{"lct_pattern"}};
  
  std::map<TString, int> mInts;

  void Initialize();
  void Reset();
  void Fill(const CSCCorrelatedLCTDigi& lctDigi);

  bool ignoreGE11;
  bool ignoreGE21;
  bool ignoreRE31;
  bool ignoreRE41;
  bool ignoreDT;
  bool ignoreME0;
};
