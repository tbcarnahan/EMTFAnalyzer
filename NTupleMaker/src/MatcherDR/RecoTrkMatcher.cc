#include "EMTFAnalyzer/NTupleMaker/interface/MatcherDR/RecoTrkMatcher.h"
#include "TMath.h"

void RecoTrkMatcher::Initialize() {
  for (auto & str : vFlt)  mVFlt .insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt)  mVInt .insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void RecoTrkMatcher::Reset() {
  for (auto & it : mVFlt)  it.second.clear();
  for (auto & it : mVInt)  it.second.clear();
}

void RecoTrkMatcher::Fill(const RecoMuonInfo & recoMuons, const EMTFTrackInfo & emtfTrks) {
  
  const double NOMATCH = -999.;
  INSERT(mVFlt, "reco_match_trk_dR", DFLT);
  INSERT(mVFlt, "reco_match_trk_dPhi", DFLT);
  INSERT(mVFlt, "reco_match_trk_dEta", DFLT);
  INSERT(mVInt, "reco_match_iTrk", DINT ); 
  
  INSERT(mVFlt, "trk_match_reco_dR", DFLT);
  INSERT(mVFlt, "trk_match_reco_dPhi", DFLT);
  INSERT(mVFlt, "trk_match_reco_dEta", DFLT);
  INSERT(mVInt, "trk_match_iReco", DINT ); 
  
  const size_t n1 = recoMuons.size();
  const size_t n2 = emtfTrks.size();
  
  vector<size_t> result(n1, -1);
  vector<vector<double> > deltaRMatrix(n1, vector<double>(n2, NOMATCH));
  vector<vector<double> > deltaEtaMatrix(n1, vector<double>(n2, NOMATCH));
  vector<vector<double> > deltaPhiMatrix(n1, vector<double>(n2, NOMATCH));
  
  for (size_t i = 0; i < n1; i++){
    for (size_t j = 0; j < n2; j++) {
      if(  fabs(recoMuons[i].eta()) < 2.5 && fabs(recoMuons[i].eta()) > 1.0
        && fabs(recoMuons[i].phi()) < TMath::Pi() && fabs(recoMuons[i].phi()) > -1.0*TMath::Pi()){
        
      }
      else{
        
      }
      deltaEtaMatrix[i][j] = recoMuons[i].eta()-emtfTrks[j].Eta();
      deltaPhiMatrix[i][j] = recoMuons[i].phi()-emtfTrks[j].Phi_glob()*TMath::Pi()/180.;
      deltaRMatrix[i][j] = sqrt( pow(deltaEtaMatrix[i][j],2) + pow(deltaPhiMatrix[i][j],2) );
    }
  }
  
  // Run through the matrix n1 times to make sure we've found all matches.
  for (size_t k = 0; k < n1; k++) {
    size_t i_min = -1;
    size_t j_min = -1;
    double minDeltaR = maxDeltaR;
    // find the smallest deltaR
    for (size_t i = 0; i < n1; i++)
      for (size_t j = 0; j < n2; j++)
	if (deltaRMatrix[i][j] < minDeltaR) {
	  i_min = i;
	  j_min = j;
	  minDeltaR = deltaRMatrix[i][j];
	}
    
    if (minDeltaR < maxDeltaR) {
      result[i_min] = j_min;
      deltaRMatrix[i_min] = vector<double>(n2, NOMATCH);
      for (size_t i = 0; i < n1; i++)
	deltaRMatrix[i][j_min] = NOMATCH;
    }
  }
  return result;

}
