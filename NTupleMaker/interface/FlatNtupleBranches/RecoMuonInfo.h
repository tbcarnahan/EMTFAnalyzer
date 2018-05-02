#ifndef FlatNtupleBranchesRecoMuonInfo_h
#define FlatNtupleBranchesRecoMuonInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// RECO muons
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

// RECO vertex
#include "DataFormats/VertexReco/interface/Vertex.h"

// Muon propator
#include "MuonAnalysis/MuonAssociators/interface/PropagateToMuon.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

// HLT trigger
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Math/interface/deltaR.h"

// HLT trigger name matching
#include <boost/regex.hpp>


////////////////////////////////
///  RECO muon  information  ///
////////////////////////////////

struct RecoMuonInfo {
  std::vector<TString> ints = {{"nRecoMuons", "nRecoMuonsFwd", "nRecoMuonsTrig", "nRecoMuonsTrigCen"}};
  std::vector<TString> vFlt = {{"reco_pt", "reco_eta", "reco_eta_St1", "reco_eta_St2", 
				"reco_theta", "reco_theta_St1", "reco_theta_St2", 
				"reco_phi", "reco_phi_St1", "reco_phi_St2",
                                "reco_dR_match_dEta", "reco_dR_match_dPhi", "reco_dR_match_dR",
				"reco_iso", "reco_d0_BS", "reco_dZ_BS", "reco_d0_PV", "reco_dZ_PV",
				"reco_trig_dR"}};
  std::vector<TString> vInt = {{"reco_ID_soft", "reco_ID_loose", "reco_ID_medium", "reco_ID_tight",
				"reco_ID_PF", "reco_ID_tracker", "reco_ID_stand", "reco_ID_global",
				"reco_ID_station", "reco_ID_nStations", "reco_charge",
				"reco_dR_match_iTrk", "reco_dR_match_nTrk", "reco_dR_match_unique",
				"reco_trig_ID"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  void Fill(const reco::Muon mu, const reco::Vertex vertex,
	    const edm::Handle<reco::BeamSpot>& beamSpotHandle,
	    const edm::Handle<trigger::TriggerEvent>& trigEvent,
	    std::vector<std::string> trigModLabels,
	    PropagateToMuon muProp1st, PropagateToMuon muProp2nd,
	    const float min_eta, const float max_eta);

  void HltMatch( const reco::Muon muon,
		 const edm::Handle<trigger::TriggerEvent>& trigEvent,
		 std::vector<std::string> trigModLabels,
		 const double _muon_trig_dR, const double _muon_trig_pt );
  
};

#endif
