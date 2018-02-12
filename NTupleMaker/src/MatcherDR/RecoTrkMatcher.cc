#include "EMTFAnalyzer/NTupleMaker/interface/MatcherDR/RecoTrkMatcher.h"

void RecoTrkMatcher::Initialize() {
  for (auto & str : vFlt)  mVFlt .insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vvInt) mVVInt.insert( std::pair<TString, std::vector<std::vector<int> > >(str, DVVINT) );
}

void RecoTrkMatcher::Reset() {
  for (auto & it : mVFlt)  it.second.clear();
  for (auto & it : mVVInt) it.second.clear();
}

void RecoTrkMatcher::Fill(const RecoMuonInfo & recoMuons, const EMTFTrackInfo & emtfTrks) {
 
  INSERT(mVFlt, "reco_match_trk_dR", DFLT);
  INSERT(mVFlt, "reco_match_trk_dPhi", DFLT);
  INSERT(mVFlt, "reco_match_trk_dEta", DFLT);
  INSERT(mVVInt, "reco_match_iTrk", DINT ); 
  
  INSERT(mVFlt, "trk_match_reco_dR", DFLT);
  INSERT(mVFlt, "trk_match_reco_dPhi", DFLT);
  INSERT(mVFlt, "trk_match_reco_dEta", DFLT);
  INSERT(mVVInt, "trk_match_iReco", DINT ); 
}
