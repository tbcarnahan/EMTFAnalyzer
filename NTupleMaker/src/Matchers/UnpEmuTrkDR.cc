
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/UnpEmuTrkDR.h"

void UnpEmuTrkDR::Match( EMTFUnpTrackInfo & unpTrks, EMTFTrackInfo & emuTrks, const float max_match_dR ) {

  const float NOMATCH = 999.;
	
  const int nUnp = ACCESS(unpTrks.mInts, "nUnpTracks");
  const int nEmu = ACCESS(emuTrks.mInts, "nTracks");
  
  std::vector<std::vector<int>   > dBX_matrix (nUnp, std::vector<int>  (nEmu, NOMATCH));
  std::vector<std::vector<float> > dEta_matrix(nUnp, std::vector<float>(nEmu, NOMATCH));
  std::vector<std::vector<float> > dPhi_matrix(nUnp, std::vector<float>(nEmu, NOMATCH));
  std::vector<std::vector<float> > dR_matrix  (nUnp, std::vector<float>(nEmu, NOMATCH));

  std::map<TString, std::vector<int>   > * iUnp = &(unpTrks.mVInt); // Can't be "const" because we modify unpTrks below
  std::map<TString, std::vector<int>   > * iEmu = &(emuTrks.mVInt); // Can't be "const" because we modify emuTrks below
  std::map<TString, std::vector<float> > * fUnp = &(unpTrks.mVFlt); // Can't be "const" because we modify unpTrks below
  std::map<TString, std::vector<float> > * fEmu = &(emuTrks.mVFlt); // Can't be "const" because we modify emuTrks below

  // Find dR, dEta, and dPhi between all unpacker-emulator pairs
  for (int i = 0; i < nUnp; i++) {

    int   unpSect = ACCESS(*iUnp, "unp_trk_sector_index").at(i);
    int   unpBX   = ACCESS(*iUnp, "unp_trk_BX") .at(i);
    float unpEta  = ACCESS(*fUnp, "unp_trk_eta").at(i);
    float unpPhi  = ACCESS(*fUnp, "unp_trk_phi").at(i) * M_PI / 180.;

    for (int j = 0; j < nEmu; j++) {

      int   emuSect = ACCESS(*iEmu, "trk_sector_index").at(j);
      int   emuBX   = ACCESS(*iEmu, "trk_BX") .at(j);
      float emuEta  = ACCESS(*fEmu, "trk_eta").at(j);
      float emuPhi  = ACCESS(*fEmu, "trk_phi").at(j) * M_PI / 180.;

      if (emuSect == unpSect) {
	dBX_matrix [i][j] = emuBX  - unpBX;
	dEta_matrix[i][j] = emuEta - unpEta;
	dPhi_matrix[i][j] = calc_dPhi(unpPhi, emuPhi) * 180. / M_PI;
	dR_matrix  [i][j] = calc_dR(unpEta, unpPhi, emuEta, emuPhi);
      }
    } // End loop over nEmu (j)

  } // End loop over nUnp (i)


  // Find closest emulated track to each unpacked track
  for (int i = 0; i < nUnp; i++) {
    int   jMin    = -1;
    int   jMin2   = -1;
    float min_dR  = max_match_dR;
    float min_dR2 = max_match_dR;
    for (int j = 0; j < nEmu; j++) {
      if ( abs ( dBX_matrix[i][j] ) >= 2 ) continue;
      if ( fabs( dR_matrix [i][j] ) < min_dR ) {
	jMin2   = jMin;
	jMin    = j;
	min_dR2 = min_dR;
	min_dR  = dR_matrix[i][j];
      }
      else if ( fabs( dR_matrix [i][j] ) < min_dR2 ) {
	jMin2   = j;
	min_dR2 = dR_matrix[i][j];
      }
    }

    if (jMin >= 0) {
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_iTrk", i, jMin);
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_dBX",  i, dBX_matrix [i][jMin]);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dEta", i, dEta_matrix[i][jMin]);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dPhi", i, dPhi_matrix[i][jMin]);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dR",   i, dR_matrix  [i][jMin]);
    }
    if (jMin2 >= 0) {
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_iTrk2", i, jMin2);
    }
      
  } // End loop: for (int i = 0; i < nUnp; i++)

  // Find closest unpacked track to each emulated track
  for (int j = 0; j < nEmu; j++) {
    int iMin      = -1;
    int iMin2     = -1;
    float min_dR  = max_match_dR;
    float min_dR2 = max_match_dR;
    for (int i = 0; i < nUnp; i++) {
      if ( abs ( dBX_matrix[i][j] ) >= 2 ) continue;
      if ( fabs( dR_matrix [i][j] ) < min_dR ) {
	iMin2   = iMin;
	iMin    = i;
	min_dR2 = min_dR;
	min_dR  = dR_matrix[i][j];
      }
      else if ( fabs( dR_matrix [i][j] ) < min_dR2 ) {
	iMin2   = i;
	min_dR2 = dR_matrix[i][j];
      }
    }

    if (iMin >= 0) {
      INSERT(emuTrks.mVInt, "trk_unp_match_iTrk", j, iMin);
      INSERT(emuTrks.mVInt, "trk_unp_match_dBX",  j, dBX_matrix [iMin][j]);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dEta", j, dEta_matrix[iMin][j]);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dPhi", j, dPhi_matrix[iMin][j]);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dR",   j, dR_matrix  [iMin][j]);
    }
    if (iMin2 >= 0) {
      INSERT(emuTrks.mVInt, "trk_unp_match_iTrk2", j, iMin2);
    }

  } // End loop: for (int j = 0; j < nEmu; j++)


  // Loop over both, check for reciprocal and exact matches
  for (int i = 0; i < nUnp; i++) {
    for (int j = 0; j < nEmu; j++) {

      if ( ACCESS(unpTrks.mVInt, "unp_trk_emu_match_iTrk").at(i) == j &&
	   ACCESS(emuTrks.mVInt, "trk_unp_match_iTrk")    .at(j) == i ) {
	INSERT(unpTrks.mVInt, "unp_trk_emu_match_unique", i, 1);
	INSERT(emuTrks.mVInt, "trk_unp_match_unique",     j, 1);
      }

      if ( ACCESS(unpTrks.mVInt, "unp_trk_BX")      .at(i) == ACCESS(emuTrks.mVInt, "trk_BX")      .at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_endcap")  .at(i) == ACCESS(emuTrks.mVInt, "trk_endcap")  .at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_sector")  .at(i) == ACCESS(emuTrks.mVInt, "trk_sector")  .at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_mode")    .at(i) == ACCESS(emuTrks.mVInt, "trk_mode")    .at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_mode_CSC").at(i) == ACCESS(emuTrks.mVInt, "trk_mode_CSC").at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_mode_RPC").at(i) == ACCESS(emuTrks.mVInt, "trk_mode_RPC").at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_eta_int") .at(i) == ACCESS(emuTrks.mVInt, "trk_eta_int") .at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_phi_int") .at(i) == ACCESS(emuTrks.mVInt, "trk_phi_int") .at(j) &&
	   ACCESS(unpTrks.mVInt, "unp_trk_pt_int")  .at(i) == ACCESS(emuTrks.mVInt, "trk_pt_int")  .at(j) ) {
	INSERT(unpTrks.mVInt, "unp_trk_emu_match_exact", i, 1);
	INSERT(emuTrks.mVInt, "trk_unp_match_exact",     j, 1);
      }

    } // End loop: for (int j = 0; j < nEmu; j++)
  } // End loop: for (int i = 0; i < nUnp; i++)

} // End function: void UnpEmuTrkDR::Match()
