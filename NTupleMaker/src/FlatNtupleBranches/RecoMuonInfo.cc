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

void RecoMuonInfo::Fill(L1Analysis::L1AnalysisRecoMuon2 & recoMuon){
	INSERT(mInts, "nRecoMuons", ACCESS(mVFlt, "reco_pt").size() + 1);
	INSERT(mVFlt, "reco_pt", recoMuon.getData()->pt.at(1) );
	INSERT(mVFlt, "reco_eta", recoMuon.getData()->eta[1] );
	INSERT(mVFlt, "reco_phi", recoMuon.getData()->phi[1] );
	INSERT(mVInt, "reco_charge", recoMuon.getData()->charge[1] );
	INSERT(mVInt, "reco_loose", recoMuon.getData()->isLooseMuon[1] );
	INSERT(mVInt, "reco_medium", recoMuon.getData()->isMediumMuon[1] );
	INSERT(mVInt, "reco_tight", recoMuon.getData()->isTightMuon[1] );
	INSERT(mVFlt, "reco_St1_eta", recoMuon.getData()->etaSt1[1] );//initial -9999
	INSERT(mVFlt, "reco_St1_phi", recoMuon.getData()->phiSt1[1] );
	INSERT(mVFlt, "reco_St2_eta", recoMuon.getData()->etaSt2[1] );
	INSERT(mVFlt, "reco_St2_phi", recoMuon.getData()->phiSt2[1] );
}
