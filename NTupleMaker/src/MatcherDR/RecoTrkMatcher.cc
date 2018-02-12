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
  
  const size_t n1 = recoMuons.size();
  const size_t n2 = emtfTrks.size();
  
  vector<size_t> result(n1, -1);
  vector<double> recoEta(n1, NOMATCH);
  vector<double> recoPhi(n1, NOMATCH);
  vector<vector<double> > deltaRMatrix(n1, vector<double>(n2, NOMATCH));
  vector<vector<double> > deltaEtaMatrix(n1, vector<double>(n2, NOMATCH));
  vector<vector<double> > deltaPhiMatrix(n1, vector<double>(n2, NOMATCH));
  
  for (size_t i = 0; i < n1; i++){
    for (size_t j = 0; j < n2; j++) {
      //Use reco mu extrapolated coordinates 
      if(  fabs(reco_eta_St2[i]) < 2.5 && fabs(reco_eta_St2[i]) > 1.0
        && fabs(reco_phi_St2[i]*TMath::Pi()/180.) < TMath::Pi() 
	&& fabs(reco_phi_St2[i]*TMath::Pi()/180.) > -1.0*TMath::Pi() ){
	      recoEta[i] = reco_eta_St2[i];
	      recoPhi[i] = reco_phi_St2[i]*TMath::Pi()/180.;
      }
      else{
	      recoEta[i] = reco_eta_St1[i];
	      recoPhi[i] = reco_phi_St1[i]*TMath::Pi()/180.;
      }
      deltaEtaMatrix[i][j] = recoEta[i]-emtfTrks[j].Eta();
      deltaPhiMatrix[i][j] = recoPhi[i]-emtfTrks[j].Phi_glob()*TMath::Pi()/180.;
      deltaRMatrix[i][j] = sqrt( pow(deltaEtaMatrix[i][j],2) + pow(deltaPhiMatrix[i][j],2) );
    }
  }
  
  // Run through the matrix n1 times to make sure we've found all matches.
  for (size_t k = 0; k < n1; k++) {
    size_t i_min = -1;
    size_t j_min = -1;
    double minDeltaR = -1.0*NOMATCH;
    // find the smallest deltaR
    for (size_t i = 0; i < n1; i++){
      for (size_t j = 0; j < n2; j++){
	if (deltaRMatrix[i][j] < minDeltaR) {
	  i_min = i;
	  j_min = j;
	  minDeltaR = deltaRMatrix[i][j];
	}
      }
    }
	  
    //removed matched pairs
    if (minDeltaR < maxDeltaR) {
      result[i_min] = j_min;
      deltaRMatrix[i_min] = vector<double>(n2, NOMATCH);
      for (size_t i = 0; i < n1; i++) deltaRMatrix[i][j_min] = NOMATCH;
    }
  }//end for k
  return result;
  for (size_t k = 0; k < n1; k++) {
	  INSERT(mVFlt, "reco_match_trk_dR", deltaRMatrix[k][result[k]]);
          INSERT(mVFlt, "reco_match_trk_dPhi", deltaPhiMatrix[k][result[k]]);
          INSERT(mVFlt, "reco_match_trk_dEta", deltaEtaMatrix[k][result[k]]);
          INSERT(mVInt, "reco_match_iTrk", result[k]); 
	
  }
  
}//end Fill
