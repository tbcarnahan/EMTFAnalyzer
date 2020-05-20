// ROOT includes
#include "TTree.h"
#include "TFile.h"

// So vectors can be written out in the tree branch
#include "TROOT.h"
#include <vector>

// CMSSW includes
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// FWCore
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h" // Necessary for DEFINE_FWK_MODULE

// Output branches
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EventInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/GenMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFSimHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFTrackInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFUnpTrackInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/CSCSegInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/RecoMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/RecoPairInfo.h"

// Object matchers
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/RecoTrkDR.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/RecoUnpTrkDR.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/UnpEmuTrkDR.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/SimUnpHit.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/LCTSeg.h"

// CSC segment geometry
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

// RECO muons
#include "DataFormats/MuonReco/interface/MuonFwd.h"

// RECO vertices
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// RECO muon track extrapolation
#include "MuonAnalysis/MuonAssociators/interface/PropagateToMuon.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

// HLT configuration
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

// GEM Copads
#include "DataFormats/GEMDigi/interface/GEMCoPadDigiCollection.h"

class FlatNtuple : public edm::EDAnalyzer {

 public:

  // Constructor/destructor
  explicit FlatNtuple(const edm::ParameterSet&);
  ~FlatNtuple();

  // Default parameters
  const float MIN_GEN_ETA  = 1.0;
  const float MAX_GEN_ETA  = 2.5;
  const float MIN_RECO_ETA = 0.0;
  const float MAX_RECO_ETA = 3.0;
  const float MAX_RECO_TRK_MATCH_DR = 0.5;
  const float MAX_UNP_EMU_MATCH_DR  = 0.15;

  ///////////////////////////////////
  ///  Output branch information  ///
  ///////////////////////////////////
  EventInfo        eventInfo;
  GenMuonInfo      genMuonInfo;
  RecoMuonInfo     recoMuonInfo;
  RecoPairInfo     recoPairInfo;
  CSCSegInfo       cscSegInfo;
  EMTFHitInfo      emtfHitInfo;
  EMTFSimHitInfo   emtfSimHitInfo;
  EMTFTrackInfo    emtfTrackInfo;
  EMTFUnpTrackInfo emtfUnpTrackInfo;

  /////////////////////////
  ///  Object matchers  ///
  /////////////////////////
  RecoTrkDR    recoTrkDR;
  RecoUnpTrkDR recoUnpTrkDR;
  UnpEmuTrkDR  unpEmuTrkDR;
  SimUnpHit    simUnpHit;
  LCTSeg       lctSeg;

  // Output tree
  TTree * out_tree;
  TTree * out_tree_meta;

 private:

  // Inherited from EDAnalyzer

  virtual void beginJob();
  virtual void beginRun(const edm::Run&,   const edm::EventSetup&);
  virtual void analyze (const edm::Event&, const edm::EventSetup&);
  virtual void endRun  (const edm::Run&,   const edm::EventSetup&);
  virtual void endJob  ();

  // File in/out
  edm::Service<TFileService> fs;

  // Config parameters
  bool isMC, isReco, skimTrig, skimEmtf, skimPair;

  // Expert station config parameters
  bool ignoreME0_;
  bool ignoreGE11_;
  bool ignoreGE21_;
  bool ignoreRE31_;
  bool ignoreRE41_;
  bool ignoreDT_;

  // User defined settings
  std::vector<std::string> muonTriggers_;  // List of unprescale SingleMuon triggers

  // HLT trigger matching
  std::vector<std::string> trigNames_;      // HLT triggers matching the desired names
  std::vector<std::string> trigModLabels_;  // HLT 3rd-to-last module label for each trigger

  // EDM Tokens
  edm::EDGetTokenT<std::vector<reco::GenParticle>>         GenMuon_token;
  edm::EDGetTokenT<CSCSegmentCollection>                   CSCSeg_token;
  edm::EDGetTokenT<reco::MuonCollection>                   RecoMuon_token;
  edm::EDGetTokenT<reco::VertexCollection>                 RecoVertex_token;
  edm::EDGetTokenT<reco::BeamSpot>                         RecoBeamSpot_token;
  edm::EDGetTokenT<trigger::TriggerEvent>                  TrigEvent_token;
  edm::EDGetTokenT<std::vector<l1t::CPPFDigi>>             CPPFDigi_token;
  edm::EDGetTokenT<std::vector<l1t::CPPFDigi>>             CPPFUnpDigi_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFHit>>              EMTFHit_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFHit>>              EMTFSimHit_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFTrack>>            EMTFTrack_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFTrack>>            EMTFUnpTrack_token;
  edm::EDGetTokenT<GEMCoPadDigiCollection>                 GEMCoPad_token;

  // Event counters for metadata
  int nEventsProc_, nEventsSel_;

  PropagateToMuon muProp1st_;
  PropagateToMuon muProp2nd_;

  HLTConfigProvider hltConfig_;
}; // End class FlatNtuple public edm::EDAnalyzer

// Define as a plugin
DEFINE_FWK_MODULE(FlatNtuple);
