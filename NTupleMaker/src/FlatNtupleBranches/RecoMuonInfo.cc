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

void RecoMuonInfo::Fill(float reco_pt, float reco_eta, float reco_phi, int reco_charge, 
			int reco_loose, int reco_medium, int reco_tight, 
			float reco_St1_eta, float reco_St1_phi,
		        float reco_St2_eta, float reco_St2_phi){
	INSERT(mInts, "nRecoMuons", ACCESS(mVFlt, "reco_pt").size() + 1);
	INSERT(mVFlt, "reco_pt", reco_pt );
	INSERT(mVFlt, "reco_eta", reco_eta );
	INSERT(mVFlt, "reco_phi", reco_phi );
	INSERT(mVInt, "reco_charge", reco_charge );
	INSERT(mVInt, "reco_loose", reco_loose );
	INSERT(mVInt, "reco_medium", reco_medium );
	INSERT(mVInt, "reco_tight", reco_tight );
        INSERT(mVFlt, "reco_St1_eta", reco_St1_eta );
        INSERT(mVFlt, "reco_St1_phi", reco_St1_phi );
	INSERT(mVFlt, "reco_St2_eta", reco_St2_eta );
	INSERT(mVFlt, "reco_St2_phi", reco_St2_phi );
}
