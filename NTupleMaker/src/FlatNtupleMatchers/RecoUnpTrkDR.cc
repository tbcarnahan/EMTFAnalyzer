
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleMatchers/RecoUnpTrkDR.h"

void RecoUnpTrkDR::Match( RecoMuonInfo & recoMuons, EMTFUnpTrackInfo & emtfTrks, 
			  const float min_reco_eta, const float max_reco_eta, const float max_match_dR ) {

  // std::cout << "\nEntering RecoUnpTrkDR.cc" << std::endl;

  const float NOMATCH = 999.;

  const int nReco = ACCESS(recoMuons.mInts, "nRecoMuons");
  const int nTrk  = ACCESS(emtfTrks.mInts,  "nUnpTracks");

  std::vector<bool>                valid_St2  (nReco, 0);
  std::vector<bool>                standalone (nReco, 0);
  std::vector<float>               reco_pt_vec(nReco, 0);
  std::vector<std::vector<float> > dR_matrix  (nReco, std::vector<float>(nTrk, NOMATCH));
  std::vector<std::vector<float> > dEta_matrix(nReco, std::vector<float>(nTrk, NOMATCH));
  std::vector<std::vector<float> > dPhi_matrix(nReco, std::vector<float>(nTrk, NOMATCH));

  std::map<TString, std::vector<int>   > * iReco = &(recoMuons.mVInt); // Can't be "const" because we modify recoMuons below
  std::map<TString, std::vector<int>   > * iTrk  = &(emtfTrks.mVInt);  // Can't be "const" because we modify emtfTrks below
  std::map<TString, std::vector<float> > * fReco = &(recoMuons.mVFlt); // Can't be "const" because we modify recoMuons below
  std::map<TString, std::vector<float> > * fTrk  = &(emtfTrks.mVFlt);  // Can't be "const" because we modify emtfTrks below

  // Find dR, dEta, and dPhi between all RECO-EMTF pairs
  for (int i = 0; i < nReco; i++) {

    float reco_pt      = ACCESS(*fReco, "reco_pt")     .at(i);
    float reco_eta_St2 = ACCESS(*fReco, "reco_eta_St2").at(i);
    float reco_phi_St2 = ACCESS(*fReco, "reco_phi_St2").at(i) * M_PI / 180.;
    float reco_eta_St1 = ACCESS(*fReco, "reco_eta_St1").at(i);
    float reco_phi_St1 = ACCESS(*fReco, "reco_phi_St1").at(i) * M_PI / 180.;
    float reco_eta_nom = ACCESS(*fReco, "reco_eta")    .at(i);
    float reco_phi_nom = ACCESS(*fReco, "reco_phi")    .at(i) * M_PI / 180.;

    float recoEta = NOMATCH;
    float recoPhi = NOMATCH;

    // Use RECO muon coordinates extrapolated to 2nd muon station (if available), otherwise 1st
    if ( fabs(reco_eta_St2) < 3.0 ) {
      recoEta = reco_eta_St2;
      recoPhi = reco_phi_St2;
      valid_St2.at(i) = 1;
    } else if ( fabs(reco_eta_St1) < 3.0 ) {
      recoEta = reco_eta_St1;
      recoPhi = reco_phi_St1;
    } else {
      recoEta = reco_eta_nom;
      recoPhi = reco_phi_nom;
    }
    reco_pt_vec.at(i) = reco_pt;

    // For standalone muons, don't use the extrapolation
    int global  = ACCESS(*iReco, "reco_ID_global").at(i);
    int tracker = ACCESS(*iReco, "reco_ID_tracker").at(i);
    if ( global == 0 && tracker == 0 ) {
      recoEta = reco_eta_nom;
      recoPhi = reco_phi_nom;
      valid_St2 .at(i) = 1;
      standalone.at(i) = 1;
    }

    if ( fabs(recoEta) > max_reco_eta || fabs(recoEta) < min_reco_eta )
      continue;

    // Loop over EMTF tracks 
    for (int j = 0; j < nTrk; j++) {

      // Discard EMTF tracks which have no hits in BX = 0
      if ( abs(ACCESS(*iTrk, "unp_trk_BX").at(j)) >  1 ) continue;
      if ( abs(ACCESS(*iTrk, "unp_trk_BX").at(j)) == 1 && ACCESS(*iTrk, "unp_trk_dBX").at(j) == 0 ) continue;

      // Discard EMTF tracks composed entirely of neighbor hits (should be built in other sector)
      if ( ACCESS(*iTrk, "unp_trk_mode_neighbor").at(j) == ACCESS(*iTrk, "unp_trk_mode").at(j) ) continue;

      float trkEta = ACCESS(*fTrk, "unp_trk_eta").at(j);
      float trkPhi = ACCESS(*fTrk, "unp_trk_phi").at(j) * M_PI / 180.;

      dEta_matrix[i][j] = trkEta - recoEta;
      dPhi_matrix[i][j] = calc_dPhi(recoPhi, trkPhi) * 180. / M_PI;
      dR_matrix[i][j]   = calc_dR(recoEta, recoPhi, trkEta, trkPhi);
    } // End loop over nTrk (j)

  } // End loop over nReco (i)


  // Find closest EMTF track to each RECO muon
  for (int i = 0; i < nReco; i++) {
    int jMin   = -1;
    int jMin2  = -1;
    int nMatch =  0;
    float max_match_dR_pt = max_match_dR;
    if (standalone.at(i) == 0) // Require tighter dR cuts for higher pT muons from collisions, ~99.9% efficient
      max_match_dR_pt = std::max(0.15, std::min(double(max_match_dR), (0.5 - 0.08*log2(reco_pt_vec.at(i))) ) );
    float min_dR  = max_match_dR_pt;
    float min_dR2 = max_match_dR_pt;
    for (int j = 0; j < nTrk; j++) {
      if ( fabs( dR_matrix[i][j] ) < max_match_dR_pt )
	nMatch += 1;

      if ( fabs( dR_matrix[i][j] ) < min_dR ) {
	jMin2   = jMin;
	jMin    = j;
	min_dR2 = min_dR;
	min_dR  = dR_matrix[i][j];
      }
      else if ( fabs( dR_matrix[i][j] ) < min_dR2 ) {
	jMin2   = j;
	min_dR2 = dR_matrix[i][j];
      }
    }

    if (jMin >= 0) {
      INSERT(recoMuons.mVInt, "reco_dR_match_unp_iTrk", i, jMin);
      INSERT(recoMuons.mVFlt, "reco_dR_match_unp_dEta", i, dEta_matrix[i][jMin]);
      INSERT(recoMuons.mVFlt, "reco_dR_match_unp_dPhi", i, dPhi_matrix[i][jMin]);
      INSERT(recoMuons.mVFlt, "reco_dR_match_unp_dR",   i, dR_matrix[i][jMin]);
    }
    if (jMin2 >= 0) {
      INSERT(recoMuons.mVInt, "reco_dR_match_unp_iTrk2", i, jMin2);
    }
    INSERT(recoMuons.mVInt, "reco_dR_match_unp_nTrk",   i, nMatch);
    INSERT(recoMuons.mVInt, "reco_dR_match_unp_unique", i, 0);

    assert( (ACCESS(recoMuons.mVInt, "reco_dR_match_unp_nTrk").at(i) >= 1) == (ACCESS(recoMuons.mVInt, "reco_dR_match_unp_iTrk") .at(i) >= 0) );
    assert( (ACCESS(recoMuons.mVInt, "reco_dR_match_unp_nTrk").at(i) >= 2) == (ACCESS(recoMuons.mVInt, "reco_dR_match_unp_iTrk2").at(i) >= 0) );

  } // End loop: for (int i = 0; i < nReco; i++)

  // Find closest RECO muon to each EMTF track
  for (int j = 0; j < nTrk; j++) {
    int iMin       = -1;
    int iMin2      = -1;
    int nMatch     = 0;
    int nMatchSoft = 0;
    float min_dR  = max_match_dR;
    float min_dR2 = max_match_dR;

    // At first, only consider RECO muons propagated to station 2
    for (int i = 0; i < nReco; i++) {

      float max_match_dR_pt = max_match_dR;
      if (standalone.at(i) == 0) // Require tighter dR cuts for higher pT muons from collisions, ~99.9% efficient
	max_match_dR_pt = std::max(0.15, std::min(double(max_match_dR), (0.5 - 0.08*log2(reco_pt_vec.at(i))) ) );
      min_dR  = std::min(min_dR,  max_match_dR_pt);
      min_dR2 = std::min(min_dR2, max_match_dR_pt);

      if ( fabs( dR_matrix[i][j] ) < max_match_dR_pt ) {
	if ( valid_St2.at(i) == 0 ) {
	  nMatchSoft += 1;
	  continue;
	} else {
	  nMatch += 1;
	}
      }

      if ( (fabs( dR_matrix[i][j] ) < min_dR) ||
           (fabs( dR_matrix[i][j] ) < max_match_dR_pt && iMin < 0) ) {
        iMin2   = iMin;
        iMin    = i;
	min_dR2 = min_dR;
        min_dR  = dR_matrix[i][j];
      }
      else if ( (fabs( dR_matrix[i][j] ) < min_dR2) ||
                (fabs( dR_matrix[i][j] ) < max_match_dR_pt && iMin2 < 0) ) {
        iMin2   = i;
        min_dR2 = dR_matrix[i][j];
      }
    }

    // If no matches found, consider all RECO muons
    if (iMin < 0) {
      for (int i = 0; i < nReco; i++) {

	float max_match_dR_pt = max_match_dR;
	if (standalone.at(i) == 0) // Require tighter dR cuts for higher pT muons from collisions, ~99.9% efficient
	  max_match_dR_pt = std::max(0.15, std::min(double(max_match_dR), (0.5 - 0.08*log2(reco_pt_vec.at(i))) ) );
	min_dR  = std::min(min_dR,  max_match_dR_pt);
	min_dR2 = std::min(min_dR2, max_match_dR_pt);

	if ( (fabs( dR_matrix[i][j] ) < min_dR) ||
	     (fabs( dR_matrix[i][j] ) < max_match_dR_pt && iMin < 0) ) {
	  iMin2   = iMin;
	  iMin    = i;
	  min_dR2 = min_dR;
	  min_dR  = dR_matrix[i][j];
	}
	else if ( (fabs( dR_matrix[i][j] ) < min_dR2) ||
		  (fabs( dR_matrix[i][j] ) < max_match_dR_pt && iMin2 < 0) ) {
	  iMin2   = i;
	  min_dR2 = dR_matrix[i][j];
	}
      }
    }

    if (iMin >= 0) {
      INSERT(emtfTrks.mVInt, "unp_trk_dR_match_iReco", j, iMin);
      INSERT(emtfTrks.mVFlt, "unp_trk_dR_match_dEta" , j, dEta_matrix[iMin][j]);
      INSERT(emtfTrks.mVFlt, "unp_trk_dR_match_dPhi",  j, dPhi_matrix[iMin][j]);
      INSERT(emtfTrks.mVFlt, "unp_trk_dR_match_dR",    j, dR_matrix[iMin][j]);
    }
    if (iMin2 >= 0) {
      INSERT(emtfTrks.mVInt, "unp_trk_dR_match_iReco2", j, iMin2);
    }
    INSERT(emtfTrks.mVInt, "unp_trk_dR_match_nReco",     j, nMatch);
    INSERT(emtfTrks.mVInt, "unp_trk_dR_match_nRecoSoft", j, nMatchSoft);
    INSERT(emtfTrks.mVInt, "unp_trk_dR_match_unique",    j, 0);

    assert( (ACCESS(emtfTrks.mVInt, "unp_trk_dR_match_nReco").at(j) >= 1) <= (ACCESS(emtfTrks.mVInt, "unp_trk_dR_match_iReco") .at(j) >= 0) );
    assert( (ACCESS(emtfTrks.mVInt, "unp_trk_dR_match_nReco").at(j) >= 2) <= (ACCESS(emtfTrks.mVInt, "unp_trk_dR_match_iReco2").at(j) >= 0) );

  } // End loop: for (int j = 0; j < nTrk; j++)


  // Loop over both, check for reciprocal matches
  for (int i = 0; i < nReco; i++) {
    for (int j = 0; j < nTrk; j++) {

      if ( ACCESS(recoMuons.mVInt, "reco_dR_match_unp_iTrk").at(i) == j &&
	   ACCESS(emtfTrks.mVInt,  "unp_trk_dR_match_iReco").at(j) == i ) {
	INSERT(recoMuons.mVInt, "reco_dR_match_unp_unique", i, 1);
	INSERT(emtfTrks.mVInt,  "unp_trk_dR_match_unique",  j, 1);
      }

    } // End loop: for (int j = 0; j < nTrk; j++)
  } // End loop: for (int i = 0; i < nReco; i++)

  // std::cout << "Finished with RecoUnpTrkDR.cc\n" << std::endl;

} // End function: void RecoUnpTrkDR::Match()
