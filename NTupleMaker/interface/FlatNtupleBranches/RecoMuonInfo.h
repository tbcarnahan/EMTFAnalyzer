#ifndef FlatNtupleBranchesRecoMuonInfo_h
#define FlatNtupleBranchesRecoMuonInfo_h

//-------------------------------------------------------------------------------
// Create by Wei Shi
//-------------------------------------------------------------------------------
// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// RECO muon class
#include "L1Trigger/L1TNtuples/interface/MuonID.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/MuonReco/interface/MuonEnergy.h"
#include "DataFormats/MuonReco/interface/MuonTime.h"
#include "CondFormats/AlignmentRecord/interface/TrackerSurfaceDeformationRcd.h"

//vertices bp
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "L1Trigger/L1TNtuples/interface/L1AnalysisRecoVertexDataFormat.h"

// track extrapolation
#include "MuonAnalysis/MuonAssociators/interface/PropagateToMuon.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

////////////////////////////////
///  RECO muon  information  ///
////////////////////////////////
struct RecoMuonInfo {
  std::vector<TString> ints = {{"nRecoMuons"}};
  std::vector<TString> vFlt = {{"reco_pt", "reco_eta", "reco_phi", "reco_St1_eta", "reco_St1_phi", "reco_St2_eta", "reco_St2_phi"}};
  std::vector<TString> vInt = {{"reco_charge", "reco_loose", "reco_medium", "reco_tight"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  void Fill(reco::MuonCollection::const_iterator it, edm::Handle<reco::VertexCollection> vertices);
};

#endif
