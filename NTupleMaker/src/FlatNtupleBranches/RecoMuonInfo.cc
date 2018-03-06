
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonInfo.h"

void RecoMuonInfo::Initialize() {
  for (auto & str : ints)  mInts .insert( std::pair<TString, int>(str, DINT) );
  for (auto & str : vFlt)  mVFlt .insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt)  mVInt .insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void RecoMuonInfo::Reset(){
  for (auto & it : mInts)  it.second = DINT;
  for (auto & it : mVFlt)  it.second.clear();
  for (auto & it : mVInt)  it.second.clear();
  INSERT(mInts, "nRecoMuons"   , 0);
  INSERT(mInts, "nRecoMuonsFwd", 0);
}

void RecoMuonInfo::Fill(const reco::Muon mu, const reco::Vertex vertex,
			const edm::Handle<reco::BeamSpot>& beamSpotHandle,
			PropagateToMuon muProp1st, PropagateToMuon muProp2nd,
			const float min_eta, const float max_eta) {

  // Propagate muon phi and eta coordinates to muon chambers
  float eta_St1   = -999.;
  float theta_St1 = -999.;
  float phi_St1   = -999.;
  float eta_St2   = -999.;
  float theta_St2 = -999.;
  float phi_St2   = -999.;
  
  TrajectoryStateOnSurface station1 = muProp1st.extrapolate(mu);
  if (station1.isValid()) {
    eta_St1   = station1.globalPosition().eta();
    theta_St1 = emtf::range_theta_deg( emtf::calc_theta_deg( eta_St1 ) );
    phi_St1   = station1.globalPosition().phi();
    phi_St1   = emtf::rad_to_deg( phi_St1 );
  }
  
  TrajectoryStateOnSurface station2 = muProp2nd.extrapolate(mu);
  if (station2.isValid()) {
    eta_St2   = station2.globalPosition().eta();
    theta_St2 = emtf::range_theta_deg( emtf::calc_theta_deg( eta_St2 ) );
    phi_St2   = station2.globalPosition().phi();
    phi_St2   = emtf::rad_to_deg( phi_St2 );
  }
  
  // RECO muon selection cuts
  if ( mu.pt() < 0.5 ) return;
  if ( abs(mu.eta())   < min_eta                      &&
       ( (abs(eta_St1) < min_eta) || (eta_St1 < -3) ) &&
       ( (abs(eta_St2) < min_eta) || (eta_St2 < -3) )  ) return;
  if ( abs(mu.eta()) > max_eta &&
       abs(eta_St1)  > max_eta &&
       abs(eta_St2)  > max_eta  ) return;
  if ( not (muon::isLooseMuon(mu) || muon::isSoftMuon(mu, vertex) || mu.isStandAloneMuon()) ) return;
  
  INSERT(mInts, "nRecoMuons", ACCESS(mInts, "nRecoMuons") + 1);
  if ( not ( abs(mu.eta())   < 1.2                      &&
	     ( (abs(eta_St1) < 1.2) || (eta_St1 < -3) ) &&
	     ( (abs(eta_St2) < 1.2) || (eta_St2 < -3) )  ) ) {
    INSERT(mInts, "nRecoMuonsFwd", ACCESS(mInts, "nRecoMuonsFwd") + 1);
  }

  float relIso = mu.pfIsolationR04().sumChargedHadronPt;
  relIso += std::max( 0., mu.pfIsolationR04().sumNeutralHadronEt + mu.pfIsolationR04().sumPhotonEt - 0.5*mu.pfIsolationR04().sumPUPt );

  reco::Track track;
  if      ( mu.isGlobalMuon()     ) track = *(mu.globalTrack() );
  else if ( mu.isTrackerMuon()    ) track = *(mu.innerTrack()  );
  else if ( mu.isStandAloneMuon() ) track = *(mu.outerTrack()  );
  else {
    std::cout << "\nERROR: The muon is NOT global NOR tracker NOR standalone?!?\n";
    return;
  }

  // Info on muon ID: https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMuonAnalysis
  //                  https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2
  INSERT(mVInt, "reco_ID_soft",      muon::isSoftMuon(mu, vertex) );
  INSERT(mVInt, "reco_ID_loose",     muon::isLooseMuon(mu) );
  INSERT(mVInt, "reco_ID_medium",    muon::isMediumMuon(mu) );
  INSERT(mVInt, "reco_ID_tight",     muon::isTightMuon(mu, vertex) );
  INSERT(mVInt, "reco_ID_PF",        mu.isPFMuon() );
  INSERT(mVInt, "reco_ID_tracker",   mu.isTrackerMuon() );
  INSERT(mVInt, "reco_ID_stand",     mu.isStandAloneMuon() );
  INSERT(mVInt, "reco_ID_global",    mu.isGlobalMuon() );
  // "OneStationTight" and "numberOfMatches" are properties of Tracker muons,
  //  not Global or StandAlone muons which are not Tracker muons
  INSERT(mVInt, "reco_ID_station",   muon::isGoodMuon(mu, muon::TMOneStationTight) );
  INSERT(mVInt, "reco_ID_nStations", mu.numberOfMatches() );

  INSERT(mVInt, "reco_charge",     mu.charge() );
  INSERT(mVFlt, "reco_pt",         mu.pt() );
  INSERT(mVFlt, "reco_eta",        mu.eta() );
  INSERT(mVFlt, "reco_eta_St1",    eta_St1 );
  INSERT(mVFlt, "reco_eta_St2",    eta_St2 );
  INSERT(mVFlt, "reco_theta",      emtf::range_theta_deg( emtf::calc_theta_deg( mu.eta() ) ) );
  INSERT(mVFlt, "reco_theta_St1",  theta_St1 );
  INSERT(mVFlt, "reco_theta_St2",  theta_St2 );
  INSERT(mVFlt, "reco_phi",        emtf::rad_to_deg( mu.phi() ) );
  INSERT(mVFlt, "reco_phi_St1",    phi_St1 );
  INSERT(mVFlt, "reco_phi_St2",    phi_St2 );

  INSERT(mVFlt, "reco_iso",   relIso );
  INSERT(mVFlt, "reco_d0_BS", track.dxy( beamSpotHandle->position() ) );
  INSERT(mVFlt, "reco_dZ_BS", track.dz ( beamSpotHandle->position() ) );
  INSERT(mVFlt, "reco_d0_PV", track.dxy( vertex.position() ) );
  INSERT(mVFlt, "reco_dZ_PV", track.dz ( vertex.position() ) );

}
