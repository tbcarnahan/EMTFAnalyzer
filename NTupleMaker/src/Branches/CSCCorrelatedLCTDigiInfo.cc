
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/CSCCorrelatedLCTDigiInfo.h"

void LCTDigiInfo::Initialize() {
  for (auto & str : ints)  mInts.insert( std::pair<TString, int>(str, DINT) );
 }

void LCTDigiInfo::Reset() {
  for (auto & it : mInts) it.second = DINT;
  INSERT(mInts, "lct_patternRun3", 0);
}

void LCTDigiInfo::Fill(const CSCCorrelatedLCTDigi& lctDigi) {
  // std::cout << "Filling LCTDigiInfo" << std::endl;
  INSERT(mInts, "lct_patternRun3", ACCESS(mInts, "lct_patternRun3") + 1 );

  // std::cout << "Filled LCTDigiInfo" << std::endl;
} // End function: LCTDigiInfo::Fill(const CSCCorrelatedLCTDigi& lctDigi)
  
