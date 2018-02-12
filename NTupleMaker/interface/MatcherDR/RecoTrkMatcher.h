#ifndef MatcherDRRecoTrkMatcher_h
#define MatcherDRRecoTrkMatcher_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Input
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFTrackInfo.h"
//#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFHitInfo.h"
#include "DataFormats/L1TMuon/interface/EMTFHit.h"
//#include "DataFormats/L1TMuon/interface/EMTFTrack.h"

////////////////////////////////
///  Matching variables      ///
////////////////////////////////

struct RecoTrkMatcher {
  std::vector<TString> vFlt = {{"reco_match_trk_dR", "reco_match_trk_dPhi", "reco_match_trk_dEta",
                                "trk_match_reco_dR", "trk_match_reco_dPhi", "trk_match_reco_dEta"}};
  std::vector<TString> vInt = {{"reco_match_iTrk", "trk_match_iReco"}};
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;
  
  void Initialize();
  void Reset();
  void Fill(const RecoMuonInfo & recoMuons, const EMTFTrackInfo & emtfTrks);
};

#endif
