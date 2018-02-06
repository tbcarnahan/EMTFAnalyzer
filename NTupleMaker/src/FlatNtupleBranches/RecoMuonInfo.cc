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

void RecoMuonInfo::Fill(L1Analysis::L1AnalysisRecoMuon2DataFormat* recoMuonData){
	INSERT(mInts, "nRecoMuons", ACCESS(mVFlt, "reco_pt").size() + 1);
	INSERT(mVFlt, "reco_pt", recoMuonData->pt );
	INSERT(mVFlt, "reco_eta", recoMuonData->eta[1] );
	INSERT(mVFlt, "reco_phi", recoMuonData->phi[1] );
	INSERT(mVInt, "reco_charge", recoMuonData()->charge[1] );
	INSERT(mVInt, "reco_loose", recoMuonData()->isLooseMuon[1] );
	INSERT(mVInt, "reco_medium", recoMuonData()->isMediumMuon[1] );
	INSERT(mVInt, "reco_tight", recoMuonData()->isTightMuon[1] );
	INSERT(mVFlt, "reco_St1_eta", recoMuonData()->etaSt1[1] );//initial -9999
	INSERT(mVFlt, "reco_St1_phi", recoMuonData()->phiSt1[1] );
	INSERT(mVFlt, "reco_St2_eta", recoMuonData()->etaSt2[1] );
	INSERT(mVFlt, "reco_St2_phi", recoMuonData()->phiSt2[1] );
}
