#ifndef FlatNtupleBranchesRecoMuonInfo_h
#define FlatNtupleBranchesRecoMuonInfo_h

//-------------------------------------------------------------------------------
// Adapted from cmssw/L1Trigger/L1TNtuples/interface/L1AnalysisRecoMuon2.h
// Wei Shi
//-------------------------------------------------------------------------------
// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/JetID.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoMuonDataFormat.h"

//muons
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/MuonReco/interface/MuonEnergy.h"
#include "DataFormats/MuonReco/interface/MuonTime.h"
#include "CondFormats/AlignmentRecord/interface/TrackerSurfaceDeformationRcd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"


//vertices bp
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/RecoVertexDataFormat.h"

// track extrapolation
#include "MuonAnalysis/MuonAssociators/interface/PropagateToMuon.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

  class RecoMuonInfo
  {
  public:
    RecoMuonInfo(const edm::ParameterSet& pset);
    ~RecoMuonInfo();
    
    void init(const edm::EventSetup &eventSetup);

    //void Print(std::ostream &os = std::cout) const;
    void SetMuon(const edm::Event& event,
                 const edm::EventSetup& setup,
                 const edm::Handle<reco::MuonCollection> muons,
                 const edm::Handle<reco::VertexCollection> vertices,
                 unsigned maxMuon);

    RecoMuonDataFormat * getData() {return &recoMuon_;}
    void Reset() {recoMuon_.Reset();}

  private :
    RecoMuonDataFormat recoMuon_;

    PropagateToMuon muPropagator1st_;
    PropagateToMuon muPropagator2nd_;
  }; 

#endif
