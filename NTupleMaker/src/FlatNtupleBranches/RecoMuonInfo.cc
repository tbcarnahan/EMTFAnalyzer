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

void RecoMuonInfo::Fill(reco::Muon & it, <reco::VertexCollection> vertices){
	INSERT(mInts, "nRecoMuons", ACCESS(mVFlt, "reco_pt").size() + 1);
	INSERT(mVFlt, "reco_pt", it->pt() );
	INSERT(mVFlt, "reco_eta", it->eta() );
	INSERT(mVFlt, "reco_phi", it->phi() );
	INSERT(mVInt, "reco_charge", it->charge() );
	//check isLooseMuon
        bool flagLoose = isLooseMuonCustom(*it);
	if (flagLoose) INSERT(mVInt, "reco_loose", 1 );

     	//check isMediumMuon
     	bool flagMedium = isMediumMuonCustom(*it);
    	if (flagMedium) INSERT(mVInt, "reco_medium", 1 );
      
    	//check isTightMuon
    	bool flagTight = false;
    	if (vertices.isValid()) flagTight = isTightMuonCustom(*it, (*vertices)[0]);
	if (flagTight) INSERT(mVInt, "reco_tight", 1 );
	
	// extrapolation of track coordinates
    	TrajectoryStateOnSurface stateAtMuSt1 = muPropagator1st_.extrapolate(*it);
    	if (stateAtMuSt1.isValid()) {
		INSERT(mVFlt, "reco_St1_eta", stateAtMuSt1.globalPosition().eta() );
		INSERT(mVFlt, "reco_St1_phi", stateAtMuSt1.globalPosition().phi() );
    	} else {
		INSERT(mVFlt, "reco_St1_eta", -9999 );
		INSERT(mVFlt, "reco_St1_phi", -9999 );
    	}

    	TrajectoryStateOnSurface stateAtMuSt2 = muPropagator2nd_.extrapolate(*it);
    	if (stateAtMuSt2.isValid()) {
		INSERT(mVFlt, "reco_St2_eta", stateAtMuSt2.globalPosition().eta() );
		INSERT(mVFlt, "reco_St2_phi", stateAtMuSt2.globalPosition().phi() );
    	} else {
		INSERT(mVFlt, "reco_St2_eta", -9999 );
		INSERT(mVFlt, "reco_St2_phi", -9999 );
    	}
	
}
