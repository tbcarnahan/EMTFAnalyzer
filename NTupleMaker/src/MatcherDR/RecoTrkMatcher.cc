#include "EMTFAnalyzer/NTupleMaker/interface/MatcherDR/RecoTrkMatcher.h"
#include "TMath.h"
#include <vector>

void RecoTrkMatcher::Initialize() {
  for (auto & str : vFlt)  mVFlt .insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt)  mVInt .insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void RecoTrkMatcher::Reset() {
  for (auto & it : mVFlt)  it.second.clear();
  for (auto & it : mVInt)  it.second.clear();
}

void RecoTrkMatcher::Fill(const RecoMuonInfo & recoMuons, const EMTFTrackInfo & emtfTrks, const float min_eta, const float max_eta) {
  
  const double NOMATCH = -999.;
  const float MIN_RECO_ETA = 1.0;
  const float MAX_RECO_ETA = 2.5;
	
  INSERT(mVFlt, "reco_match_trk_dR", DFLT);
  INSERT(mVFlt, "reco_match_trk_dPhi", DFLT);
  INSERT(mVFlt, "reco_match_trk_dEta", DFLT);
  INSERT(mVInt, "reco_match_iTrk", DINT ); 
  
  const int n1 = ACCESS(recoMuons.mInts, "nRecoMuons");
  const int n2 = ACCESS(emtfTrks.mInts, "nTracks");
  
  vector<int> result(n1, -1);
  vector<double> reco_eta_St2(n1, NOMATCH);
  vector<double> reco_phi_St2(n1, NOMATCH);
  vector<double> reco_eta_St1(n1, NOMATCH);
  vector<double> reco_phi_St1(n1, NOMATCH);
  vector<double> recoEta(n1, NOMATCH);
  vector<double> recoPhi(n1, NOMATCH);
  vector<vector<double> > deltaRMatrix(n1, vector<double>(n2, NOMATCH));
  vector<vector<double> > deltaEtaMatrix(n1, vector<double>(n2, NOMATCH));
  vector<vector<double> > deltaPhiMatrix(n1, vector<double>(n2, NOMATCH));
  
  for (size_t i = 0; i < n1; i++){
    for (size_t j = 0; j < n2; j++) {
	    
      //Use reco mu extrapolated coordinates
      const std::map<TString, std::vector<int> > * imu = &(recoMuons.mVInt);
      reco_eta_St2[i] = ACCESS(*imu, "reco_eta_St2").at(i);
      reco_phi_St2[i] = ACCESS(*imu, "reco_phi_St2").at(i);
      reco_eta_St1[i] = ACCESS(*imu, "reco_eta_St1").at(i);
      reco_phi_St1[i] = ACCESS(*imu, "reco_phi_St1").at(i);
	    
      //2nd station higher priority    
      if(  fabs(reco_eta_St2[i] ) < max_eta && fabs(reco_eta_St2[i]) > min_eta
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
    }//end for i
  }//end for j
  
  // Run through the matrix n1 times to make sure we've found all matches.
  for (size_t k = 0; k < n1; k++) {
    size_t i_min = -1;
    size_t j_min = -1;
    double minDeltaR = -1.0*NOMATCH;
    // find the smallest deltaR b/t reco muons and trks
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
    if (minDeltaR < -1.0*NOMATCH) {
      result[i_min] = j_min;
      deltaRMatrix[i_min] = vector<double>(n2, NOMATCH);
      for (size_t i = 0; i < n1; i++) deltaRMatrix[i][j_min] = NOMATCH;
    }
  }//end for k
  
  for (size_t k = 0; k < n1; k++) {
	  INSERT(mVFlt, "reco_match_trk_dR", deltaRMatrix[k][result[k]]);
          INSERT(mVFlt, "reco_match_trk_dPhi", deltaPhiMatrix[k][result[k]]);
          INSERT(mVFlt, "reco_match_trk_dEta", deltaEtaMatrix[k][result[k]]);
          INSERT(mVInt, "reco_match_iTrk", result[k]); 
  }
  
}//end Fill
