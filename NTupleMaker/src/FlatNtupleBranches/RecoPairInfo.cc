
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoPairInfo.h"

void RecoPairInfo::Initialize() {
  for (auto & str : ints)  mInts .insert( std::pair<TString, int>(str, DINT) );
  for (auto & str : vFlt)  mVFlt .insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt)  mVInt .insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void RecoPairInfo::Reset(){
  for (auto & it : mInts)  it.second = DINT;
  for (auto & it : mVFlt)  it.second.clear();
  for (auto & it : mVInt)  it.second.clear();
  INSERT(mInts, "nRecoPairs", 0);
  INSERT(mInts, "nRecoPairsFwd", 0);
}



void RecoPairInfo::Fill( const RecoMuonInfo & recoMuons ) {

  // std::cout << "\nEntering RecoPairInfo.cc" << std::endl;

  const std::map<TString, std::vector<int>   > * iReco = &(recoMuons.mVInt);
  const std::map<TString, std::vector<float> > * fReco = &(recoMuons.mVFlt);

  const float MASS_MUON = 0.105658367;
  const int nReco = ACCESS(recoMuons.mInts, "nRecoMuons");

  TLorentzVector mu1_vec;
  TLorentzVector mu2_vec;
  TLorentzVector pair_vec;

  float dR_nom;
  float dEta_nom;
  float dTheta_nom;
  float dPhi_nom;
  float dR_St1;
  float dEta_St1;
  float dTheta_St1;
  float dPhi_St1;
  float dR_St2;
  float dEta_St2;
  float dTheta_St2;
  float dPhi_St2;

  // Fill properties of opposite-sign RECO muon pairs
  for (int i = 0; i < nReco; i++) {

    int   mu1_charge    = ACCESS(*iReco, "reco_charge")   .at(i);
    float mu1_dZ_PV     = ACCESS(*fReco, "reco_dZ_PV")    .at(i);
    float mu1_pt        = ACCESS(*fReco, "reco_pt")       .at(i);
    float mu1_eta_nom   = ACCESS(*fReco, "reco_eta")      .at(i);
    float mu1_theta_nom = ACCESS(*fReco, "reco_theta")    .at(i);
    float mu1_phi_nom   = ACCESS(*fReco, "reco_phi")      .at(i) * M_PI / 180.;
    float mu1_eta_St1   = ACCESS(*fReco, "reco_eta_St1")  .at(i);
    float mu1_theta_St1 = ACCESS(*fReco, "reco_theta_St1").at(i);
    float mu1_phi_St1   = ACCESS(*fReco, "reco_phi_St1")  .at(i) * M_PI / 180.;
    float mu1_eta_St2   = ACCESS(*fReco, "reco_eta_St2")  .at(i);
    float mu1_theta_St2 = ACCESS(*fReco, "reco_theta_St2").at(i);
    float mu1_phi_St2   = ACCESS(*fReco, "reco_phi_St2")  .at(i) * M_PI / 180.;

    mu1_vec.SetPtEtaPhiM( mu1_pt, mu1_eta_nom, mu1_phi_nom, MASS_MUON );

    for (int j = i+1; j < nReco; j++) {

      int   mu2_charge    = ACCESS(*iReco, "reco_charge")   .at(j);
      float mu2_dZ_PV     = ACCESS(*fReco, "reco_dZ_PV")    .at(j);
      float mu2_pt        = ACCESS(*fReco, "reco_pt")       .at(j);
      float mu2_eta_nom   = ACCESS(*fReco, "reco_eta")      .at(j);
      float mu2_theta_nom = ACCESS(*fReco, "reco_theta")    .at(j);
      float mu2_phi_nom   = ACCESS(*fReco, "reco_phi")      .at(j) * M_PI / 180.;
      float mu2_eta_St1   = ACCESS(*fReco, "reco_eta_St1")  .at(j);
      float mu2_theta_St1 = ACCESS(*fReco, "reco_theta_St1").at(j);
      float mu2_phi_St1   = ACCESS(*fReco, "reco_phi_St1")  .at(j) * M_PI / 180.;
      float mu2_eta_St2   = ACCESS(*fReco, "reco_eta_St2")  .at(j);
      float mu2_theta_St2 = ACCESS(*fReco, "reco_theta_St2").at(j);
      float mu2_phi_St2   = ACCESS(*fReco, "reco_phi_St2")  .at(j) * M_PI / 180.;

      mu2_vec.SetPtEtaPhiM( mu2_pt, mu2_eta_nom, mu2_phi_nom, MASS_MUON );

      if (mu1_charge * mu2_charge != -1)     continue;  // Only keep opposite-sign pairs
      if (abs(mu2_dZ_PV - mu1_dZ_PV) > 0.25) continue;  // Only keep pairs with a consistent vertex

      INSERT(mInts, "nRecoPairs", ACCESS(mInts, "nRecoPairs") + 1);
      if ( (abs(mu1_eta_nom) > 1.2 || abs(mu1_eta_St1) > 1.2 || abs(mu1_eta_St2) > 1.2) &&
	   (abs(mu2_eta_nom) > 1.2 || abs(mu2_eta_St1) > 1.2 || abs(mu2_eta_St2) > 1.2) ) {
	INSERT(mInts, "nRecoPairsFwd", ACCESS(mInts, "nRecoPairsFwd") + 1);
      }


      dR_nom     = calc_dR  (mu1_eta_nom, mu1_phi_nom, mu2_eta_nom, mu2_phi_nom);
      dEta_nom   = mu2_eta_nom - mu1_eta_nom;
      dTheta_nom = mu2_theta_nom - mu1_theta_nom;
      dPhi_nom   = calc_dPhi(mu1_phi_nom, mu2_phi_nom) * 180. / M_PI;

      if (abs(mu1_eta_St1) < 3.0 && abs(mu2_eta_St1) < 3.0) {
	dR_St1     = calc_dR  (mu1_eta_St1, mu1_phi_St1, mu2_eta_St1, mu2_phi_St1);
	dEta_St1   = mu2_eta_St1 - mu1_eta_St1;
	dTheta_St1 = mu2_theta_St1 - mu1_theta_St1;
	dPhi_St1   = calc_dPhi(mu1_phi_St1, mu2_phi_St1) * 180. / M_PI;
      } else {
	dR_St1     = DFLT;
	dEta_St1   = DFLT;
	dTheta_St1 = DFLT;
	dPhi_St1   = DFLT;
      }
	
      if (abs(mu1_eta_St2) < 3.0 && abs(mu2_eta_St2) < 3.0) {
	dR_St2     = calc_dR  (mu1_eta_St2, mu1_phi_St2, mu2_eta_St2, mu2_phi_St2);
	dEta_St2   = mu2_eta_St2 - mu1_eta_St2;
	dTheta_St2 = mu2_theta_St2 - mu1_theta_St2;
	dPhi_St2   = calc_dPhi(mu1_phi_St2, mu2_phi_St2) * 180. / M_PI;
      } else {
	dR_St2     = DFLT;
	dEta_St2   = DFLT;
	dTheta_St2 = DFLT;
	dPhi_St2   = DFLT;
      }

      pair_vec = mu1_vec + mu2_vec;

      INSERT(mVInt, "recoPair_iReco1", i);
      INSERT(mVInt, "recoPair_iReco2", j);

      INSERT(mVFlt, "recoPair_p",     pair_vec.P());
      INSERT(mVFlt, "recoPair_pt",    pair_vec.Pt());
      INSERT(mVFlt, "recoPair_pz",    pair_vec.Pz());
      INSERT(mVFlt, "recoPair_eta",   pair_vec.Eta());
      INSERT(mVFlt, "recoPair_theta", pair_vec.Theta() * 180. / M_PI);
      INSERT(mVFlt, "recoPair_phi",   pair_vec.Phi() * 180. / M_PI);
      INSERT(mVFlt, "recoPair_mass",  pair_vec.M());

      INSERT(mVFlt, "recoPair_dR",         dR_nom);
      INSERT(mVFlt, "recoPair_dEta",       dEta_nom);
      INSERT(mVFlt, "recoPair_dTheta",     dTheta_nom);
      INSERT(mVFlt, "recoPair_dPhi",       dPhi_nom);
      INSERT(mVFlt, "recoPair_dR_St1",     dR_St1);
      INSERT(mVFlt, "recoPair_dEta_St1",   dEta_St1);
      INSERT(mVFlt, "recoPair_dTheta_St1", dTheta_St1);
      INSERT(mVFlt, "recoPair_dPhi_St1",   dPhi_St1);
      INSERT(mVFlt, "recoPair_dR_St2",     dR_St2);
      INSERT(mVFlt, "recoPair_dEta_St2",   dEta_St2);
      INSERT(mVFlt, "recoPair_dTheta_St2", dTheta_St2);
      INSERT(mVFlt, "recoPair_dPhi_St2",   dPhi_St2);

    } // End loop over nReco (j)
  } // End loop over nReco (i)

  // std::cout << "Finished with RecoPairInfo.cc\n" << std::endl;

} // End function: void RecoPairInfo::Fill()
