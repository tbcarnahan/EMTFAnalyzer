#include "EMTFAnalyzer/NTupleMaker/interface/MatcherDR/RecoTrkMatcher.h"

void RecoTrkMatcher::Initialize() {
  for (auto & str : vFlt)  mVFlt .insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt)  mVInt .insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void RecoTrkMatcher::Reset() {
  for (auto & it : mVFlt)  it.second.clear();
  for (auto & it : mVInt)  it.second.clear();
}

void RecoTrkMatcher::Fill(const RecoMuonInfo & recoMuons, const EMTFTrackInfo & emtfTrks) {
 
  INSERT(mVFlt, "reco_match_trk_dR", DFLT);
  INSERT(mVFlt, "reco_match_trk_dPhi", DFLT);
  INSERT(mVFlt, "reco_match_trk_dEta", DFLT);
  INSERT(mVInt, "reco_match_iTrk", DINT ); 
  
  INSERT(mVFlt, "trk_match_reco_dR", DFLT);
  INSERT(mVFlt, "trk_match_reco_dPhi", DFLT);
  INSERT(mVFlt, "trk_match_reco_dEta", DFLT);
  INSERT(mVInt, "trk_match_iReco", DINT ); 
}
