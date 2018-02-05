//Adapted from cmssw/L1Trigger/L1TNtuples/src/L1AnalysisRecoMuon2.cc
//Wei Shi

#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonInfo.h"
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include <DataFormats/PatCandidates/interface/Muon.h>
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/MuonID.h"

using namespace std;
using namespace muon;

RecoMuonInfo::RecoMuonInfo(const edm::ParameterSet& pset) :
  muPropagator1st_(pset.getParameter<edm::ParameterSet>("muProp1st")),
  muPropagator2nd_(pset.getParameter<edm::ParameterSet>("muProp2nd"))
{
}


RecoMuonInfo::~RecoMuonInfo()
{
}

void RecoMuonInfo::SetMuon(const edm::Event& event,
					      const edm::EventSetup& setup,
					      edm::Handle<reco::MuonCollection> muons,
					      edm::Handle<reco::VertexCollection> vertices, 
                unsigned maxMuon)
{

  recoMuon_.nMuons=0;
  
  for(reco::MuonCollection::const_iterator it=muons->begin();
      it!=muons->end() && recoMuon_.nMuons < maxMuon;
      ++it) {

    recoMuon_.pt.push_back(it->pt()); 
    recoMuon_.eta.push_back(it->eta());
    recoMuon_.phi.push_back(it->phi());
    recoMuon_.charge.push_back(it->charge());
    
    //RecoMu quality
    //check isLooseMuon
    bool flagLoose = isLooseMuonCustom(*it);
    recoMuon_.isLooseMuon.push_back(flagLoose);

    //check isMediumMuon
     bool flagMedium = isMediumMuonCustom(*it);
    recoMuon_.isMediumMuon.push_back(flagMedium);

    //check isTightMuon
    bool flagTight = false;
    if (vertices.isValid())
      flagTight = isTightMuonCustom(*it, (*vertices)[0]);
    recoMuon_.isTightMuon.push_back(flagTight);

    recoMuon_.nMuons++;

    // extrapolation of track coordinates
    TrajectoryStateOnSurface stateAtMuSt1 = muPropagator1st_.extrapolate(*it);
    if (stateAtMuSt1.isValid()) {
      recoMuon_.etaSt1.push_back(stateAtMuSt1.globalPosition().eta());
      recoMuon_.phiSt1.push_back(stateAtMuSt1.globalPosition().phi());
    } else {
      recoMuon_.etaSt1.push_back(-9999);
      recoMuon_.phiSt1.push_back(-9999);
    }

    TrajectoryStateOnSurface stateAtMuSt2 = muPropagator2nd_.extrapolate(*it);
    if (stateAtMuSt2.isValid()) {
      recoMuon_.etaSt2.push_back(stateAtMuSt2.globalPosition().eta());
      recoMuon_.phiSt2.push_back(stateAtMuSt2.globalPosition().phi());
    } else {
      recoMuon_.etaSt2.push_back(-9999);
      recoMuon_.phiSt2.push_back(-9999);
    }
  }
}

void RecoMuonInfo::init(const edm::EventSetup &eventSetup)
{
  muPropagator1st_.init(eventSetup);
  muPropagator2nd_.init(eventSetup);
}
