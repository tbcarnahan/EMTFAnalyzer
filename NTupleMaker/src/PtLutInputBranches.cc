
#include "EMTFAnalyzer/NTupleMaker/interface/PtLutInputBranches.hh"

void GenMuonBranch::Initialize() {
  nMuons = 0;
  for (unsigned int i = 0; i < N_GEN; i++) {
    pt[i]     = DFLT; eta[i] = DFLT;
    theta[i]  = DFLT; phi[i] = DFLT;
    charge[i] = DINT;
  }
}

void EMTFHitBranch::Initialize() {
  nHits = 0;
  for (unsigned int i = 0; i < N_HIT; i++) {
    eta[i]     = DFLT; theta[i]        = DFLT; phi[i]     = DFLT; phi_loc[i] = DFLT;
    eta_int[i] = DINT; theta_int[i]    = DINT; phi_int[i] = DINT; endcap[i]  = DINT;
    sector[i]  = DINT; sector_index[i] = DINT; station[i] = DINT; ring[i]    = DINT;
    CSC_ID[i]  = DINT; chamber[i]      = DINT; FR[i]      = DINT; pattern[i] = DINT;
    roll[i]    = DINT; subsector[i]    = DINT; isRPC[i]   = DINT; vetoed[i]  = DINT;
    BX[i]      = DINT;
  }
}

void EMTFTrackBranch::Initialize() {
  nTracks = 0;
  for (unsigned int i = 0; i < N_TRK; i++) {
    pt[i]     = DFLT; eta[i]     = DFLT; theta[i]        = DFLT; phi[i]     = DFLT; phi_loc[i] = DFLT;
    pt_int[i] = DINT; eta_int[i] = DINT; theta_int[i]    = DINT; phi_int[i] = DINT; BX[i]      = DINT;
    endcap[i] = DINT; sector[i]  = DINT; sector_index[i] = DINT; mode[i]    = DINT; charge[i]  = DINT;

    nHits[i] = 0; nRPC[i] = 0;
    for (unsigned int j = 0; j < 4; j++) {
      hit_eta[i][j]     = DFLT; hit_theta[i][j]        = DFLT; hit_phi[i][j]     = DFLT; hit_phi_loc[i][j] = DFLT;
      hit_eta_int[i][j] = DINT; hit_theta_int[i][j]    = DINT; hit_phi_int[i][j] = DINT; hit_endcap[i][j]  = DINT;
      hit_sector[i][j]  = DINT; hit_sector_index[i][j] = DINT; hit_station[i][j] = DINT; hit_ring[i][j]    = DINT;
      hit_CSC_ID[i][j]  = DINT; hit_chamber[i][j]      = DINT; hit_FR[i][j]      = DINT; hit_pattern[i][j] = DINT;
      hit_roll[i][j]    = DINT; hit_subsector[i][j]    = DINT; hit_isRPC[i][j]   = DINT; hit_vetoed[i][j]  = DINT;
      hit_BX[i][j]      = DINT;
    }
  }
}

void GenMuonBranch::Fill(unsigned int i, reco::GenParticle genMuon) {
  float _theta = genMuon.theta() * 180. / PI;
  if (_theta > 180) _theta -= 360;
  if (_theta >  90) _theta  = 180. - _theta;
  float _phi = genMuon.phi() * 180. / PI;
  if (_phi > 180) _phi -= 360;

  nMuons    = i+1;
  pt[i]     = genMuon.pt();
  eta[i]    = genMuon.eta();
  theta[i]  = _theta;
  phi[i]    = _phi;
  charge[i] = genMuon.charge();
}

void EMTFHitBranch::Fill(unsigned int i, L1TMuonEndCap::EMTFHitExtra emtfHit) {
  int   _eta_int = calc_GMT_eta_from_theta(emtfHit.theta_fp, emtfHit.endcap);
  float _phi_loc = (emtfHit.phi_fp / 60.0) - 22.0;
  float _phi     = _phi_loc + 15.0 + (emtfHit.pc_sector - 1)*60.0;
  if (_phi > 180) _phi -= 360;
  
  nHits           = i+1;
  eta[i]          = _eta_int * 0.010875;
  theta[i]        = emtfHit.theta_fp*0.285 + 8.5;
  phi[i]          = _phi;
  phi_loc[i]      = _phi_loc;
  eta_int[i]      = _eta_int;
  theta_int[i]    = emtfHit.theta_fp;
  phi_int[i]      = emtfHit.phi_fp;
  endcap[i]       = (emtfHit.endcap == 1 ? 1 : -1);
  sector[i]       = emtfHit.sector;
  sector_index[i] = (emtfHit.endcap == 1 ? emtfHit.pc_sector : emtfHit.pc_sector + 6);
  station[i]      = emtfHit.station;
  ring[i]         = emtfHit.ring;
  CSC_ID[i]       = emtfHit.csc_ID;
  chamber[i]      = emtfHit.chamber;
  FR[i]           = calc_FR_bit(emtfHit.station, emtfHit.ring, emtfHit.chamber);
  pattern[i]      = emtfHit.pattern;
  roll[i]         = emtfHit.roll;
  subsector[i]    = emtfHit.subsector;
  isRPC[i]        = (emtfHit.subsystem == 2 ? 1 : 0);
  vetoed[i]       = emtfHit.vetoed;
  BX[i]           = emtfHit.bx;
}

void EMTFTrackBranch::Fill(unsigned int i, L1TMuonEndCap::EMTFTrackExtra emtfTrk) {
  float _phi_loc = (emtfTrk.phi_int / 60.0) - 22.0;
  float _phi     = _phi_loc + 15.0 + (emtfTrk.sector - 1)*60.0;
  if (_phi > 180) _phi -= 360;

  nTracks         = i+1;
  pt[i]           = emtfTrk.pt;
  eta[i]          = emtfTrk.gmt_eta * 0.010875;
  theta[i]        = emtfTrk.theta_int*0.285 + 8.5;
  phi[i]          = _phi;
  phi_loc[i]      = _phi_loc;
  eta_int[i]      = emtfTrk.gmt_eta;
  theta_int[i]    = emtfTrk.theta_int;
  phi_int[i]      = emtfTrk.phi_int;
  BX[i]           = emtfTrk.bx;
  endcap[i]       = (emtfTrk.endcap == 1 ? 1 : -1);
  sector[i]       = emtfTrk.sector;
  sector_index[i] = (emtfTrk.endcap == 1 ? emtfTrk.sector : emtfTrk.sector + 6);
  mode[i]         = emtfTrk.mode;
  std::cout << "Mode for track " << i << " assigned as " << mode[i] << " (" << emtfTrk.mode << ")" << std::endl;
  charge[i]       = (emtfTrk.gmt_charge == 1 ? -1 : 1);

  int _nHits = 0;
  int _nRPC  = 0;
  for (const auto& trk_hit : emtfTrk.xhits) {
    _nHits += 1;
    unsigned int j = trk_hit.station - 1;
    assert(j < 4);

    int   _eta_int_hit = calc_GMT_eta_from_theta(trk_hit.theta_fp, trk_hit.endcap);
    float _phi_loc_hit = (trk_hit.phi_fp / 60.0) - 22.0;
    float _phi_hit     = _phi_loc_hit + 15.0 + (trk_hit.pc_sector - 1)*60.0;
    if (_phi_hit > 180) _phi_hit -= 360;
  
    hit_eta[i][j]          = _eta_int_hit * 0.010875;
    hit_theta[i][j]        = trk_hit.theta_fp*0.285 + 8.5;
    hit_phi[i][j]          = _phi_hit;
    hit_phi_loc[i][j]      = _phi_loc_hit;
    hit_eta_int[i][j]      = _eta_int_hit;
    hit_theta_int[i][j]    = trk_hit.theta_fp;
    hit_phi_int[i][j]      = trk_hit.phi_fp;
    hit_endcap[i][j]       = (trk_hit.endcap == 1 ? 1 : -1);
    hit_sector[i][j]       = trk_hit.sector;
    hit_sector_index[i][j] = (trk_hit.endcap == 1 ? trk_hit.pc_sector : trk_hit.pc_sector + 6);
    hit_station[i][j]      = trk_hit.station;
    hit_ring[i][j]         = trk_hit.ring;
    hit_CSC_ID[i][j]       = trk_hit.csc_ID;
    hit_chamber[i][j]      = trk_hit.chamber;
    hit_FR[i][j]           = calc_FR_bit(trk_hit.station, trk_hit.ring, trk_hit.chamber);
    hit_pattern[i][j]      = trk_hit.pattern;
    hit_roll[i][j]         = trk_hit.roll;
    hit_subsector[i][j]    = trk_hit.subsector;
    hit_isRPC[i][j]        = (trk_hit.subsystem == 2 ? 1 : 0);
    hit_vetoed[i][j]       = trk_hit.vetoed;
    hit_BX[i][j]           = trk_hit.bx;
    if (trk_hit.subsystem == 2) _nRPC += 1;
  }
  nHits[i] = _nHits;
  nRPC[i]  = _nRPC;
}
