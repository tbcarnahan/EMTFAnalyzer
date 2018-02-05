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

void RecoMuonInfo::Fill(const L1Analysis::L1AnalysisRecoMuon2DataFormat & recoMuon){
	INSERT(mInts, "nRecoMuons", ACCESS(mInts, "nRecoMuons") + 1 );
	INSERT(mVFlt, "reco_pt", *recoMuon.pt );
	INSERT(mVFlt, "reco_eta", recoMuon[2].eta );
	INSERT(mVFlt, "reco_phi", &recoMuon.phi );
	INSERT(mVInt, "reco_charge", recoMuon.charge );
	INSERT(mVInt, "reco_loose", recoMuon.isLooseMuon );
	INSERT(mVInt, "reco_medium", recoMuon.isMediumMuon );
	INSERT(mVInt, "reco_tight", recoMuon.isTightMuon );
	INSERT(mVFlt, "reco_St1_eta", recoMuon.etaSt1 );//initial -9999
	INSERT(mVFlt, "reco_St1_phi", recoMuon.phiSt1 );
	INSERT(mVFlt, "reco_St2_eta", recoMuon.etaSt2 );
	INSERT(mVFlt, "reco_St2_phi", recoMuon.phiSt2 );
}
