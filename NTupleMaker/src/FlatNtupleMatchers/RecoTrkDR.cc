
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleMatchers/RecoTrkDR.h"

void RecoTrkDR::Match( RecoMuonInfo & recoMuons, EMTFTrackInfo & emtfTrks, 
		       const float min_reco_eta, const float max_reco_eta, const float max_match_dR ) {

  const float NOMATCH = -999.;
	
  const int nReco = ACCESS(recoMuons.mInts, "nRecoMuons");
  const int nTrk = ACCESS(emtfTrks.mInts, "nTracks");
  
  std::vector<int> result(nReco, -1);
  std::vector<float> reco_eta_St2(nReco, NOMATCH);
  std::vector<float> reco_phi_St2(nReco, NOMATCH);
  std::vector<float> reco_eta_St1(nReco, NOMATCH);
  std::vector<float> reco_phi_St1(nReco, NOMATCH);
  std::vector<float> reco_eta_nom(nReco, NOMATCH);
  std::vector<float> reco_phi_nom(nReco, NOMATCH);
  std::vector<float> recoEta(nReco, NOMATCH);
  std::vector<float> recoPhi(nReco, NOMATCH);
  std::vector<float> trkEta(nTrk, NOMATCH);
  std::vector<float> trkPhi(nTrk, NOMATCH);
  std::vector<std::vector<float> > dR_matrix(nReco, std::vector<float>(nTrk, NOMATCH));
  std::vector<std::vector<float> > dEta_matrix(nReco, std::vector<float>(nTrk, NOMATCH));
  std::vector<std::vector<float> > dPhi_matrix(nReco, std::vector<float>(nTrk, NOMATCH));

  const std::map<TString, std::vector<float> > * iReco = &(recoMuons.mVFlt);
  const std::map<TString, std::vector<float> > * iTrk  = &(emtfTrks.mVFlt);

  // Find dR, dEta, and dPhi between all RECO-EMTF pairs
  for (int i = 0; i < nReco; i++) {

    reco_eta_St2[i] = ACCESS(*iReco, "reco_eta_St2").at(i);
    reco_phi_St2[i] = ACCESS(*iReco, "reco_phi_St2").at(i) * M_PI / 180.;
    reco_eta_St1[i] = ACCESS(*iReco, "reco_eta_St1").at(i);
    reco_phi_St1[i] = ACCESS(*iReco, "reco_phi_St1").at(i) * M_PI / 180.;
    reco_eta_nom[i] = ACCESS(*iReco, "reco_eta")    .at(i);
    reco_phi_nom[i] = ACCESS(*iReco, "reco_phi")    .at(i) * M_PI / 180.;

    // Use RECO muon coordinates extrapolated to 2nd muon station (if available), otherwise 1st
    if ( reco_eta_St2[i] > -9 ) {
      recoEta[i] = reco_eta_St2[i];
      recoPhi[i] = reco_phi_St2[i];
    } else if ( reco_eta_St1[i] > -9 ) {
      recoEta[i] = reco_eta_St1[i];
      recoPhi[i] = reco_phi_St1[i];
    } else {
      recoEta[i] = reco_eta_nom[i];
      recoPhi[i] = reco_phi_nom[i];
    }

    if (fabs(recoEta[i]) > max_reco_eta || fabs(recoEta[i]) < min_reco_eta )
      continue;

    // Loop over EMTF tracks 
    for (int j = 0; j < nTrk; j++) {

      trkEta[j] = ACCESS(*iTrk, "trk_eta").at(j);
      trkPhi[j] = ACCESS(*iTrk, "trk_phi").at(j) * M_PI / 180.;

      dEta_matrix[i][j] = trkEta[j] - recoEta[i];
      dPhi_matrix[i][j] = calc_dPhi(recoPhi[i], trkPhi[j]);
      dR_matrix[i][j]   = calc_dR(recoEta[i], recoPhi[i], trkEta[j], trkPhi[j]);
    } // End loop over nTrk (j)

  } // End loop over nReco (i)


  // Find closest EMTF track to each RECO muon
  for (int i = 0; i < nReco; i++) {
    int jMin = -1;
    float min_dR = max_match_dR;
    for (int j = 0; j < nTrk; j++) {
      if (dR_matrix[i][j] < min_dR) {
	jMin = j;
	min_dR = dR_matrix[i][j];
      }
    }

    if (jMin >= 0) {
      INSERT(recoMuons.mVInt, "reco_dR_match_iTrk", jMin);
      INSERT(recoMuons.mVFlt, "reco_dR_match_dEta", dEta_matrix[i][jMin]);
      INSERT(recoMuons.mVFlt, "reco_dR_match_dPhi", dPhi_matrix[i][jMin]);
      INSERT(recoMuons.mVFlt, "reco_dR_match_dR",   dR_matrix[i][jMin]);
    } else {
      INSERT(recoMuons.mVInt, "reco_dR_match_iTrk", DINT);
      INSERT(recoMuons.mVFlt, "reco_dR_match_dEta", DFLT);
      INSERT(recoMuons.mVFlt, "reco_dR_match_dPhi", DFLT);
      INSERT(recoMuons.mVFlt, "reco_dR_match_dR",   DFLT);
    }
      
  } // End loop: for (int i = 0; i < nReco; i++)

  // Find closest RECO muon to each EMTF track
  for (int j = 0; j < nTrk; j++) {
    int iMin = -1;
    float min_dR = max_match_dR;
    for (int i = 0; i < nReco; i++) {
      if (dR_matrix[i][j] < min_dR) {
	iMin = i;
	min_dR = dR_matrix[i][j];
      }
    }

    if (iMin >= 0) {
      INSERT(emtfTrks.mVInt, "trk_dR_match_iReco", iMin);
      INSERT(emtfTrks.mVFlt, "trk_dR_match_dEta" , dEta_matrix[iMin][j]);
      INSERT(emtfTrks.mVFlt, "trk_dR_match_dPhi",  dPhi_matrix[iMin][j]);
      INSERT(emtfTrks.mVFlt, "trk_dR_match_dR",    dR_matrix[iMin][j]);
    } else {
      INSERT(emtfTrks.mVInt, "trk_dR_match_iReco", DINT);
      INSERT(emtfTrks.mVFlt, "trk_dR_match_dEta" , DFLT);
      INSERT(emtfTrks.mVFlt, "trk_dR_match_dPhi",  DFLT);
      INSERT(emtfTrks.mVFlt, "trk_dR_match_dR",    DFLT);
    }

  } // End loop: for (int j = 0; j < nTrk; j++)

  // Loop over both, find unique minDR pairs

  // Fill number of matches

  // // Run through the matrix nReco times to make sure we've found all matches.
  // for (int k = 0; k < nReco; k++) {
  //   int i_min = -1;
  //   int jMin = -1;
  //   float minDeltaR = fabs(NOMATCH);
  //   // Find the smallest deltaR between RECO muons and EMTF tracks
  //   for (int i = 0; i < nReco; i++){
  //     for (int j = 0; j < nTrk; j++){
  // 	if (deltaRMatrix[i][j] < minDeltaR) {
  // 	  i_min = i;
  // 	  jMin = j;
  // 	  minDeltaR = deltaRMatrix[i][j];
  // 	}
  //     }
  //   }

  //   //removed matched pairs
  //   if (minDeltaR < fabs(NOMATCH)) {
  //     result[i_min] = j_min;
  //     deltaRMatrix[i_min] = std::vector<float>(nTrk, NOMATCH);
  //     for (int i = 0; i < nReco; i++) deltaRMatrix[i][j_min] = NOMATCH;
  //   }
  // }//end for k
  
  // for (int k = 0; k < nReco; k++) {

  // 	  if(result[k]!=-1){
  // 		  INSERT(mVFlt, "reco_match_trk_dPhi", deltaPhiMatrix[k][result[k]]);
  //                 INSERT(mVFlt, "reco_match_trk_dEta", deltaEtaMatrix[k][result[k]]);
  // 	          INSERT(mVFlt, "reco_match_trk_dR", sqrt( pow(deltaEtaMatrix[k][result[k]],2) + pow(deltaPhiMatrix[k][result[k]],2) ) );       
  // 	  }
  // 	  else{//didn't find a match
  // 		  INSERT(mVFlt, "reco_match_trk_dPhi", NOMATCH);
  //                 INSERT(mVFlt, "reco_match_trk_dEta", NOMATCH);
  // 	          INSERT(mVFlt, "reco_match_trk_dR", NOMATCH);
  // 	  }
  // 	  INSERT(mVInt, "reco_match_iTrk", result[k]); 
  // }
  
} // End function: void RecoTrkDR::Match()
