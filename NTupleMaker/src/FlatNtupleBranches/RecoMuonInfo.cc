//Added by Wei Shi

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
	INSERT(mInts, "nRecoMuons", 0);
}

void RecoMuonInfo::Fill(const L1Analysis::L1AnalysisRecoMuon2 & recoMuon){
	INSERT(mInts, "nRecoMuons", ACCESS(mInts, "nRecoMuons") + 1 );
	INSERT(mVFlt, "reco_pt", recoMuon.pt.at(1) );
	INSERT(mVFlt, "reco_eta", recoMuon.eta[1] );
	INSERT(mVFlt, "reco_phi", recoMuon.phi[1] );
	INSERT(mVInt, "reco_charge", recoMuon.charge[1] );
	INSERT(mVInt, "reco_loose", recoMuon.isLooseMuon[1] );
	INSERT(mVInt, "reco_medium", recoMuon.isMediumMuon[1] );
	INSERT(mVInt, "reco_tight", recoMuon.isTightMuon[1] );
	INSERT(mVFlt, "reco_St1_eta", recoMuon.etaSt1[1] );//initial -9999
	INSERT(mVFlt, "reco_St1_phi", recoMuon.phiSt1[1] );
	INSERT(mVFlt, "reco_St2_eta", recoMuon.etaSt2[1] );
	INSERT(mVFlt, "reco_St2_phi", recoMuon.phiSt2[1] );
}
