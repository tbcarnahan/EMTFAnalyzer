
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleMatchers/UnpEmuTrkDR.h"

void UnpEmuTrkDR::Match( EMTFUnpTrackInfo & unpTrks, EMTFTrackInfo & emuTrks, const float max_match_dR ) {

  const float NOMATCH = -999.;
	
  const int nUnp = ACCESS(unpTrks.mInts, "nUnpTracks");
  const int nEmu = ACCESS(emuTrks.mInts, "nTracks");
  
  std::vector<int>   unpBX (nUnp, NOMATCH);
  std::vector<float> unpEta(nUnp, NOMATCH);
  std::vector<float> unpPhi(nUnp, NOMATCH);
  std::vector<int>   emuBX (nEmu, NOMATCH);
  std::vector<float> emuEta(nEmu, NOMATCH);
  std::vector<float> emuPhi(nEmu, NOMATCH);
  std::vector<std::vector<int>   > dBX_matrix (nUnp, std::vector<int>  (nEmu, NOMATCH));
  std::vector<std::vector<float> > dEta_matrix(nUnp, std::vector<float>(nEmu, NOMATCH));
  std::vector<std::vector<float> > dPhi_matrix(nUnp, std::vector<float>(nEmu, NOMATCH));
  std::vector<std::vector<float> > dR_matrix  (nUnp, std::vector<float>(nEmu, NOMATCH));

  const std::map<TString, std::vector<int>   > * iUnp = &(unpTrks.mVInt);
  const std::map<TString, std::vector<int>   > * iEmu = &(emuTrks.mVInt);
  const std::map<TString, std::vector<float> > * fUnp = &(unpTrks.mVFlt);
  const std::map<TString, std::vector<float> > * fEmu = &(emuTrks.mVFlt);

  // Find dR, dEta, and dPhi between all unpacker-emulator pairs
  for (int i = 0; i < nUnp; i++) {

    unpBX [i] = ACCESS(*iUnp, "unp_trk_BX") .at(i);
    unpEta[i] = ACCESS(*fUnp, "unp_trk_eta").at(i);
    unpPhi[i] = ACCESS(*fUnp, "unp_trk_phi").at(i) * M_PI / 180.;

    for (int j = 0; j < nEmu; j++) {

      emuBX [i] = ACCESS(*iEmu, "trk_BX") .at(j);
      emuEta[j] = ACCESS(*fEmu, "trk_eta").at(j);
      emuPhi[j] = ACCESS(*fEmu, "trk_phi").at(j) * M_PI / 180.;

      dBX_matrix [i][j] = emuBX[j]  - unpBX[i];
      dEta_matrix[i][j] = emuEta[j] - unpEta[i];
      dPhi_matrix[i][j] = calc_dPhi(unpPhi[i], emuPhi[j]) * 180. / M_PI;
      dR_matrix  [i][j] = calc_dR(unpEta[i], unpPhi[i], emuEta[j], emuPhi[j]);
    } // End loop over nEmu (j)

  } // End loop over nUnp (i)


  // Find closest EMTF track to each RECO muon
  for (int i = 0; i < nUnp; i++) {
    int jMin   = -1;
    float min_dR = max_match_dR;
    for (int j = 0; j < nEmu; j++) {
      if (dR_matrix[i][j] < min_dR && abs(dBX_matrix[i][j]) < 2) {
	jMin = j;
	min_dR = dR_matrix[i][j];
      }
    }

    if (jMin >= 0) {
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_iEmu", jMin);
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_dBX",  dBX_matrix [i][jMin]);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dEta", dEta_matrix[i][jMin]);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dPhi", dPhi_matrix[i][jMin]);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dR",   dR_matrix[i][jMin]);
    } else {
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_iEmu", DINT);
      INSERT(unpTrks.mVInt, "unp_trk_emu_match_dBX",  DINT);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dEta", DFLT);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dPhi", DFLT);
      INSERT(unpTrks.mVFlt, "unp_trk_emu_match_dR",   DFLT);
    }
    INSERT(unpTrks.mVInt, "unp_trk_emu_match_unique", 0);
    INSERT(unpTrks.mVInt, "unp_trk_emu_match_exact",  0);
      
  } // End loop: for (int i = 0; i < nUnp; i++)

  // Find closest RECO muon to each EMTF track
  for (int j = 0; j < nEmu; j++) {
    int iMin   = -1;
    float min_dR = max_match_dR;
    for (int i = 0; i < nUnp; i++) {
      if (dR_matrix[i][j] < min_dR && abs(dBX_matrix[i][j]) < 2) {
	iMin = i;
	min_dR = dR_matrix[i][j];
      }
    }

    if (iMin >= 0) {
      INSERT(emuTrks.mVInt, "trk_unp_match_iUnp", iMin);
      INSERT(emuTrks.mVInt, "trk_unp_match_dBX",  dBX_matrix [iMin][j]);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dEta", dEta_matrix[iMin][j]);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dPhi", dPhi_matrix[iMin][j]);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dR",   dR_matrix[iMin][j]);
    } else {
      INSERT(emuTrks.mVInt, "trk_unp_match_iUnp", DINT);
      INSERT(emuTrks.mVInt, "trk_unp_match_dBX",  DINT);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dEta", DFLT);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dPhi", DFLT);
      INSERT(emuTrks.mVFlt, "trk_unp_match_dR",   DFLT);
    }
    INSERT(emuTrks.mVInt, "trk_unp_match_unique", 0);
    INSERT(emuTrks.mVInt, "trk_unp_match_exact",  0);

  } // End loop: for (int j = 0; j < nEmu; j++)


  // Loop over both, check for reciprocal and exact matches
  for (int i = 0; i < nUnp; i++) {
    for (int j = 0; j < nEmu; j++) {

      if ( ACCESS(unpTrks.mVInt, "unp_trk_emu_match_iEmu").at(i) == j &&
	   ACCESS(emuTrks.mVInt, "trk_unp_match_iUnp")    .at(j) == i ) {
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
