
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/CSCCorrelatedLCTDigiInfo.h"

void LCTDigiInfo::Initialize() {
  for (auto & str : vInt)  mVInt.insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
 }

void LCTDigiInfo::Reset() {
  for (auto & it : mVInt) it.second.clear();
}

void LCTDigiInfo::Fill(const CSCCorrelatedLCTDigi& lctDigi) {
  // std::cout << "Filling LCTDigiInfo" << std::endl;

  INSERT(mVInt, "lct_patternRun3",      lctDigi.second.getRun3Pattern() + 1 );

  // std::cout << "Filled LCTDigiInfo" << std::endl;
} // End function: LCTDigiInfo::Fill(const CSCCorrelatedLCTDigi& lctDigi)
  
