
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFSimHitInfo.h"

void EMTFSimHitInfo::Initialize() {
  for (auto & str : ints)  mInts.insert( std::pair<TString, int>(str, DINT) );
  for (auto & str : vFlt)  mVFlt.insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt)  mVInt.insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void EMTFSimHitInfo::Reset() {
  for (auto & it : mInts) it.second = DINT;
  for (auto & it : mVFlt) it.second.clear();
  for (auto & it : mVInt) it.second.clear();
  INSERT(mInts, "nSimHits", 0);
  INSERT(mInts, "nSimHitsCSC", 0);
  INSERT(mInts, "nSimHitsRPC", 0);
  INSERT(mInts, "nSimHitsBX0", 0);
  INSERT(mInts, "nSimHitsCSCBX0", 0);
  INSERT(mInts, "nSimHitsRPCBX0", 0);
}

void EMTFSimHitInfo::Fill(const l1t::EMTFHit & emtfHit) {
  // std::cout << "Filling EMTFSimHitInfo" << std::endl;

  INSERT(mInts, "nSimHits", ACCESS(mInts, "nSimHits") + 1 );
  if (emtfHit.Is_CSC() == 1) 
    INSERT(mInts, "nSimHitsCSC", ACCESS(mInts, "nSimHitsCSC") + 1 );
  if (emtfHit.Is_RPC() == 1) 
    INSERT(mInts, "nSimHitsRPC", ACCESS(mInts, "nSimHitsRPC") + 1 );
  if (emtfHit.BX() == 0) {
    INSERT(mInts, "nSimHitsBX0", ACCESS(mInts, "nSimHitsBX0") + 1 );
    if (emtfHit.Is_CSC() == 1) 
      INSERT(mInts, "nSimHitsCSCBX0", ACCESS(mInts, "nSimHitsCSCBX0") + 1 );
    if (emtfHit.Is_RPC() == 1) 
      INSERT(mInts, "nSimHitsRPCBX0", ACCESS(mInts, "nSimHitsRPCBX0") + 1 );
  }

  INSERT(mVFlt, "sim_hit_eta",          emtf::calc_eta_from_theta_deg( emtfHit.Theta(), emtfHit.Endcap() ) );
  INSERT(mVFlt, "sim_hit_theta",        emtfHit.Theta() );
  INSERT(mVFlt, "sim_hit_phi",          emtfHit.Phi_glob() );
  INSERT(mVFlt, "sim_hit_phi_loc",      emtfHit.Phi_loc() );
  INSERT(mVInt, "sim_hit_eta_int",      emtf::calc_eta_GMT( emtf::calc_eta_from_theta_deg( emtfHit.Theta(), emtfHit.Endcap() ) ) );
  INSERT(mVInt, "sim_hit_theta_int",    emtfHit.Theta_fp() );
  INSERT(mVInt, "sim_hit_phi_int",      emtfHit.Phi_fp() );
  INSERT(mVInt, "sim_hit_endcap",       emtfHit.Endcap() );
  INSERT(mVInt, "sim_hit_sector",       emtfHit.Sector() );
  INSERT(mVInt, "sim_hit_sector_index", emtfHit.Sector_idx() );
  INSERT(mVInt, "sim_hit_station",      emtfHit.Station() );
  INSERT(mVInt, "sim_hit_ring",         emtfHit.Ring() );
  INSERT(mVInt, "sim_hit_CSC_ID",       ( emtfHit.Is_RPC() ? DINT : emtfHit.CSC_ID()) );
  INSERT(mVInt, "sim_hit_chamber",      emtfHit.Chamber() );
  assert( calc_FR_bit(emtfHit.Station(), emtfHit.Ring(), emtfHit.Chamber()) == isFront(emtfHit.Station(), emtfHit.Ring(), emtfHit.Chamber(), emtfHit.Subsystem()) );
  // INSERT(mVInt, "sim_hit_FR",           calc_FR_bit(emtfHit.Station(), emtfHit.Ring(), emtfHit.Chamber()) );
  INSERT(mVInt, "sim_hit_FR",           isFront( emtfHit.Station(), emtfHit.Ring(), emtfHit.Chamber(), emtfHit.Subsystem() ) );
  INSERT(mVInt, "sim_hit_pattern",      ( emtfHit.Is_RPC() ? DINT : emtfHit.Pattern()) );
  INSERT(mVInt, "sim_hit_quality",      ( emtfHit.Is_RPC() ? DINT : emtfHit.Quality()) );
  // INSERT(mVInt, "sim_hit_alct_quality", ( emtfHit.Is_RPC() ? DINT : emtfHit.ALCT_quality()) );
  // INSERT(mVInt, "sim_hit_clct_quality", ( emtfHit.Is_RPC() ? DINT : emtfHit.CLCT_quality()) );
  INSERT(mVInt, "sim_hit_alct_quality", DINT );
  INSERT(mVInt, "sim_hit_clct_quality", DINT );
  INSERT(mVInt, "sim_hit_roll",         (!emtfHit.Is_RPC() ? DINT : emtfHit.Roll()) );
  INSERT(mVInt, "sim_hit_subsector",    emtfHit.Subsector() );
  INSERT(mVInt, "sim_hit_isCSC",        emtfHit.Is_CSC() );
  INSERT(mVInt, "sim_hit_isRPC",        emtfHit.Is_RPC() );
  INSERT(mVInt, "sim_hit_valid",        emtfHit.Valid() );
  INSERT(mVInt, "sim_hit_BX",           emtfHit.BX() );
  INSERT(mVInt, "sim_hit_strip",        ( emtfHit.Is_RPC() ? DINT : emtfHit.Strip()) );
  INSERT(mVInt, "sim_hit_strip_hi",     (!emtfHit.Is_RPC() ? DINT : emtfHit.Strip_hi()) );
  INSERT(mVInt, "sim_hit_strip_low",    (!emtfHit.Is_RPC() ? DINT : emtfHit.Strip_low()) );
  INSERT(mVInt, "sim_hit_wire",         ( emtfHit.Is_RPC() ? DINT : emtfHit.Wire()) );
  INSERT(mVInt, "sim_hit_neighbor",     emtfHit.Neighbor() );
  // std::cout << "Filled EMTFSimHitInfo" << std::endl;
} // End function: EMTFSimHitInfo::Fill(const l1t::EMTFHit & emtfHit)
