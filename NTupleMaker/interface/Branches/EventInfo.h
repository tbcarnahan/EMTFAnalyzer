
#ifndef BranchesEventInfo_h
#define BranchesEventInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Event info
#include "FWCore/Framework/interface/Event.h"


///////////////////////////
///  Event information  ///
///////////////////////////
struct EventInfo {
  std::vector<TString> ints = {{"evt_run", "evt_LS", "evt_BX"}};
  std::vector<TString> longs = {{"evt_event", "evt_orbit"}};
  std::map<TString, int> mInts;
  std::map<TString, long long> mLongs;

  void Initialize();
  void Reset();
  void Fill(const edm::Event & iEvent);
};

#endif  // #ifndef BranchesEventInfo_h
