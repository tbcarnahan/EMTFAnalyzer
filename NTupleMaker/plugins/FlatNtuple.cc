// Make NTuples for input to BDT training
// Runs off of the new 2017 EMTF emultor
// Andrew Brinkerhoff - 27.06.17

#include "EMTFAnalyzer/NTupleMaker/plugins/FlatNtuple.h"

// Constructor
FlatNtuple::FlatNtuple(const edm::ParameterSet& iConfig):
  nEventsProc_(0), nEventsSel_(0),
  muProp1st_(iConfig.getParameter<edm::ParameterSet>("muProp1st")), // Propagate RECO muon coordinates to 1st muon station
  muProp2nd_(iConfig.getParameter<edm::ParameterSet>("muProp2nd"))  // Propagate RECO muon coordinates to 2nd muon station
{
  // Output file
  edm::Service<TFileService> fs;
  out_tree      = fs->make<TTree>("tree",    "FlatNtupleTree");
  out_tree_meta = fs->make<TTree>("metadata","FlatNtupleMeta");

  // Config parameters
  isMC     = iConfig.getParameter<bool>("isMC");
  isReco   = iConfig.getParameter<bool>("isReco");
  skimTrig = iConfig.getParameter<bool>("skimTrig");
  skimEmtf = iConfig.getParameter<bool>("skimEmtf");
  skimPair = iConfig.getParameter<bool>("skimPair");

  // Expert station config parameters
  ignoreGE11_ = iConfig.getParameter<bool>("ignoreGE11");
  ignoreGE21_ = iConfig.getParameter<bool>("ignoreGE21");
  ignoreRE31_ = iConfig.getParameter<bool>("ignoreRE31");
  ignoreRE41_ = iConfig.getParameter<bool>("ignoreRE41");
  ignoreDT_ = iConfig.getParameter<bool>("ignoreDT");
  ignoreME0_ = iConfig.getParameter<bool>("ignoreME0");

  // Input collections
  if (isMC)   GenMuon_token      = consumes<std::vector<reco::GenParticle>> (iConfig.getParameter<edm::InputTag>("genMuonTag"));
  if (isReco) CSCSeg_token       = consumes<CSCSegmentCollection>           (iConfig.getParameter<edm::InputTag>("cscSegmentTag"));
  if (isReco) RecoMuon_token     = consumes<reco::MuonCollection>           (iConfig.getParameter<edm::InputTag>("recoMuonTag"));
  if (isReco) RecoVertex_token   = consumes<reco::VertexCollection>         (iConfig.getParameter<edm::InputTag>("recoVertexTag"));
  if (isReco) RecoBeamSpot_token = consumes<reco::BeamSpot>                 (iConfig.getParameter<edm::InputTag>("recoBeamSpotTag"));
  if (isReco) TrigEvent_token    = consumes<trigger::TriggerEvent>          (iConfig.getParameter<edm::InputTag>("trigEvent"));

  // User defined settings
  if (isReco) muonTriggers_ = iConfig.getParameter< std::vector<std::string> > ("muonTriggers");

  CPPFDigi_token     = consumes<l1t::CPPFDigiCollection>     (iConfig.getParameter<edm::InputTag>("cppfDigiTag"));
  CPPFUnpDigi_token  = consumes<l1t::CPPFDigiCollection>     (iConfig.getParameter<edm::InputTag>("cppfUnpDigiTag"));
  EMTFHit_token      = consumes<std::vector<l1t::EMTFHit>>   (iConfig.getParameter<edm::InputTag>("emtfHitTag"));
  EMTFSimHit_token   = consumes<std::vector<l1t::EMTFHit>>   (iConfig.getParameter<edm::InputTag>("emtfSimHitTag"));
  EMTFTrack_token    = consumes<std::vector<l1t::EMTFTrack>> (iConfig.getParameter<edm::InputTag>("emtfTrackTag"));
  EMTFUnpTrack_token = consumes<std::vector<l1t::EMTFTrack>> (iConfig.getParameter<edm::InputTag>("emtfUnpTrackTag"));
  GEMCoPad_token = consumes<GEMCoPadDigiCollection> (iConfig.getParameter<edm::InputTag>("gemCoPadTag"));

} // End FlatNtuple::FlatNtuple

// Destructor
FlatNtuple::~FlatNtuple() {}

// Called once per run
void FlatNtuple::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  std::cout << "\nInside FlatNtuple::beginRun()" << std::endl;

  if (not isReco) return;

  bool changed = true;
  if (not hltConfig_.init(iRun, iSetup, "HLT", changed)) {
    std::cout << "\n\nTrying to set up HLT, could not!!!" << std::endl;
    std::cout << "Quitting.\n\n" << std::endl;
    assert(false);
  }

  const boost::regex rgx("_v[0-9]+");

  for (unsigned i = 0; i < hltConfig_.size(); i++) {
    std::string trigName = hltConfig_.triggerName(i);
    std::string trigNameStripped = boost::regex_replace(trigName, rgx, "", boost::match_default | boost::format_sed);

    for (unsigned j = 0; j < muonTriggers_.size(); j++) {
      if (trigNameStripped == muonTriggers_.at(j)) {
	std::cout << "\nTrigger #" << i << ": " << trigName << "(" << trigNameStripped << ") matches " << muonTriggers_.at(j) << std::endl;
	trigNames_    .push_back(trigName);
	trigModLabels_.push_back(hltConfig_.moduleLabels(i).at(hltConfig_.size(i) - 2));
	std::cout << "  * Module index " << hltConfig_.size(i) - 2 << " returns label " << trigModLabels_.back() << std::endl;
      }
    }
  }

  if (trigNames_.size() == 0 || trigNames_.size() > muonTriggers_.size()) {
    std::cout << "\nFound " << trigNames_.size() << " triggers matching: ";
    for (unsigned i = 0; i < muonTriggers_.size(); i++) {
      std::cout << muonTriggers_.at(i) << " or ";
    } std::cout << "\n" << std::endl;
    for (unsigned i = 0; i < trigNames_.size(); i++) {
      std::cout << "  * Trigger #" << i << ": " << trigNames_.at(i) << std::endl;
    }
    assert(false);
  }

} // End FlatNtuple::beginRun()


// Called once per run
void FlatNtuple::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  std::cout << "\nInside FlatNtuple::endRun()\n" << std::endl;
}


// Called once per event
void FlatNtuple::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  nEventsProc_ += 1;

  // std::cout << "\nCalling analyze" << std::endl;
  edm::Handle<std::vector<reco::GenParticle>> genMuons;
  if (isMC)   iEvent.getByToken(GenMuon_token, genMuons);
  edm::Handle<CSCSegmentCollection> cscSegs;
  if (isReco) iEvent.getByToken(CSCSeg_token, cscSegs);
  edm::Handle<reco::MuonCollection> recoMuons;
  if (isReco) iEvent.getByToken(RecoMuon_token, recoMuons);
  edm::Handle<reco::VertexCollection> recoVertices;
  if (isReco) iEvent.getByToken(RecoVertex_token, recoVertices);
  edm::Handle<reco::BeamSpot> recoBeamSpot;
  if (isReco) iEvent.getByToken(RecoBeamSpot_token, recoBeamSpot);
  edm::Handle<trigger::TriggerEvent> trigEvent;
  if (isReco) iEvent.getByToken(TrigEvent_token, trigEvent);


  edm::Handle<l1t::CPPFDigiCollection> cppfDigis;
  iEvent.getByToken(CPPFDigi_token, cppfDigis);
  edm::Handle<l1t::CPPFDigiCollection> cppfUnpDigis;
  iEvent.getByToken(CPPFUnpDigi_token, cppfUnpDigis);
  edm::Handle<std::vector<l1t::EMTFHit>> emtfHits;
  iEvent.getByToken(EMTFHit_token, emtfHits);
  edm::Handle<std::vector<l1t::EMTFHit>> emtfSimHits;
  iEvent.getByToken(EMTFSimHit_token, emtfSimHits);
  edm::Handle<std::vector<l1t::EMTFTrack>> emtfTracks;
  iEvent.getByToken(EMTFTrack_token, emtfTracks);
  edm::Handle<std::vector<l1t::EMTFTrack>> emtfUnpTracks;
  iEvent.getByToken(EMTFUnpTrack_token, emtfUnpTracks);
  edm::Handle<GEMCoPadDigiCollection> gemCoPadsH;
  iEvent.getByToken(GEMCoPad_token, gemCoPadsH);
  const GEMCoPadDigiCollection& gemCoPads = *gemCoPadsH.product();

  edm::ESHandle<CSCGeometry> cscGeom;
  iSetup.get<MuonGeometryRecord>().get(cscGeom);

  edm::ESHandle<GEMGeometry> gemGeom;
  iSetup.get<MuonGeometryRecord>().get(gemGeom);

  // Reset branch values
  eventInfo.Reset();
  genMuonInfo.Reset();
  emtfHitInfo.Reset();
  emtfSimHitInfo.Reset();
  emtfTrackInfo.Reset();
  emtfUnpTrackInfo.Reset();
  cscSegInfo.Reset();
  recoMuonInfo.Reset();
  recoPairInfo.Reset();

  // ignore hits we are not interested in
  emtfHitInfo.ignoreGE11 = ignoreGE11_;
  emtfHitInfo.ignoreGE21 = ignoreGE21_;
  emtfHitInfo.ignoreRE31 = ignoreRE31_;
  emtfHitInfo.ignoreRE41 = ignoreRE41_;
  emtfHitInfo.ignoreDT = ignoreDT_;
  emtfHitInfo.ignoreME0 = ignoreME0_;

  emtfTrackInfo.ignoreGE11 = ignoreGE11_;
  emtfTrackInfo.ignoreGE21 = ignoreGE21_;
  emtfTrackInfo.ignoreRE31 = ignoreRE31_;
  emtfTrackInfo.ignoreRE41 = ignoreRE41_;
  emtfTrackInfo.ignoreDT = ignoreDT_;
  emtfTrackInfo.ignoreME0 = ignoreME0_;

  // std::cout << "About to fill event info" << std::endl;
  // Fill event info
  eventInfo.Fill(iEvent);


  // std::cout << "About to fill CSC segment info" << std::endl;
  // Fill CSC segment info
  if ( isReco && cscSegs.isValid() ) {
    for (CSCSegmentCollection::const_iterator iter = cscSegs->begin(); iter != cscSegs->end(); iter++) {
      cscSegInfo.Fill(*iter, cscGeom);
    }
  }
  else if (isReco) {
    std::cout << "ERROR: could not get cscSegs from event!!!" << std::endl;
    return;
  }

  // std::cout << "About to fill RECO muon info" << std::endl;
  // Fill RECO muon info
  if ( isReco && recoMuons.isValid() && recoVertices.isValid() && trigEvent.isValid() ) {
    // Set up muon propagator for this event
    muProp1st_.init(iSetup);
    muProp2nd_.init(iSetup);
    // Loop over RECO muons
    for ( reco::MuonCollection::const_iterator mu = recoMuons->begin(); mu != recoMuons->end(); ++mu ) {
      recoMuonInfo.Fill( *mu, (*recoVertices)[0], recoBeamSpot, trigEvent, trigModLabels_,
			 muProp1st_, muProp2nd_, MIN_RECO_ETA, MAX_RECO_ETA );
    }
  }
  else if (isReco) {
    std::cout << "ERROR: could not get recoMuons, recoVertices, or trigEvent from event!!!" << std::endl;
    std::cout << "recoMuons = " << recoMuons.isValid() << ", recoVertices = " << recoVertices.isValid()
	      << ", trigEvent = " << trigEvent.isValid() << std::endl;
    return;
  }
  if (skimTrig && ACCESS(recoMuonInfo.mInts, "nRecoMuonsTrig")    < 2
               && ACCESS(recoMuonInfo.mInts, "nRecoMuonsTrigCen") < 1) return;


  // std::cout << "About to fill RECO muon pair info" << std::endl;
  // Fill RECO muon pair info
  recoPairInfo.Fill(recoMuonInfo);
  if (skimPair && ACCESS(recoPairInfo.mInts, "nRecoPairsFwd") < 1) return;

  // std::cout << "About to fill GEN muon info" << std::endl;
  // Fill RECO muon info
  if ( isMC && genMuons.isValid() ) {
    for (reco::GenParticle genMuon: *genMuons) {
      if (abs(genMuon.pdgId()) != 13) continue; // Must be a muon
      if ( (fabs(genMuon.eta()) > MIN_GEN_ETA) && (fabs(genMuon.eta()) < MAX_GEN_ETA) ) {
	genMuonInfo.Fill(genMuon);
      }
    } // End for (reco::GenParticle genMuon: *genMuons)
  }
  else if (isMC) {
    std::cout << "ERROR: could not get genMuons from event!!!" << std::endl;
    return;
  }
  // Skip event if there are no GEN muons within acceptance
  if (isMC && ACCESS(genMuonInfo.mInts, "nGenMuons") < 1) {
    return;
  }


  // here, need to do the association with GEM hits
  std::vector<l1t::EMTFHit> associatedGEMCoPads;
  if ( emtfHits.isValid() ) {
    for (l1t::EMTFHit emtfHit: *emtfHits) {
      // ME1/1 stubs!
      if (emtfHit.Is_CSC() == 1 and
          emtfHit.Station() == 1 and
          emtfHit.Ring() == 1) {
        // ME1/1 detid
        const auto& cscId = emtfHit.CSC_DetId();
        CSCDetId key_id(cscId.endcap(), cscId.station(), cscId.ring(), cscId.chamber(), 3);


        // ME1/1 chamber
        const auto& cscChamber = cscGeom->chamber(cscId);

        // CSC GP
        const auto& lct = emtfHit.CreateCSCCorrelatedLCTDigi();
        float fractional_strip = lct.getFractionalStrip();
        const auto& layer_geo = cscChamber->layer(3)->geometry();
        // LCT::getKeyWG() also starts from 0
        float wire = layer_geo->middleWireOfGroup(lct.getKeyWG() + 1);
        const LocalPoint& csc_intersect = layer_geo->intersectionOfStripAndWire(fractional_strip, wire);
        const GlobalPoint& csc_gp = cscGeom->idToDet(key_id)->surface().toGlobal(csc_intersect);

        // corresponding GE1/1 detid
        const GEMDetId gemId(cscId.zendcap(),
                             1,
                             1,
                             0,
                             cscId.chamber(),
                             0);

        // copad collection
        const auto& co_pads_in_det = gemCoPads.get(gemId);

        // best copad
        GEMCoPadDigi best;

        // at most the width of an ME11 chamber
        float minDPhi = 0.18;
        // loop on the GEM coincidence pads
        // find the closest matching one
        for (auto it = co_pads_in_det.first; it != co_pads_in_det.second; ++it) {
          // pick the first layer in the copad!
          const auto& copad = (*it);

          const GEMDetId gemCoId(cscId.zendcap(),
                                 1,
                                 1,
                                 1,
                                 cscId.chamber(),
                                 copad.roll());

          const LocalPoint& gem_lp = gemGeom->etaPartition(gemCoId)->centreOfPad(copad.pad(1));
          const GlobalPoint& gem_gp = gemGeom->idToDet(gemCoId)->surface().toGlobal(gem_lp);
          float currentDPhi = reco::deltaPhi(float(csc_gp.phi()), float(gem_gp.phi()));
          if (currentDPhi < minDPhi) {
            best = copad;
            minDPhi = currentDPhi;
          }
        }
        // create a new EMTFHit with the
        // best matching coincidence pad
        l1t::EMTFHit bestEMTFHit;
        bestEMTFHit.set_subsystem(3);
        bestEMTFHit.SetGEMDetId(gemId);
        bestEMTFHit.set_roll(best.roll());
        bestEMTFHit.set_strip(best.pad(1));
        associatedGEMCoPads.push_back(bestEMTFHit);
      }
    }
  }

  // make a combined collection
  l1t::EMTFHitCollection mergedHits;
  mergedHits.insert(std::end(mergedHits), std::begin(*emtfHits), std::end(*emtfHits));
  mergedHits.insert(std::end(mergedHits), std::begin(associatedGEMCoPads), std::end(associatedGEMCoPads));

  std::cout << "About to fill EMTF hit branches" << std::endl;
  // Fill EMTF hit branches
  if ( emtfHits.isValid() ) {
    for (l1t::EMTFHit emtfHit: mergedHits) {
      emtfHitInfo.Fill(emtfHit);
    } // End for (l1t::EMTFHit emtfHit: *emtfHits)
  }
  else {
    std::cout << "ERROR: could not get emtfHits from event!!!" << std::endl;
    return;
  }


  /*
  std::cout << "About to fill EMTF simHit branches" << std::endl;
  // Fill EMTF simHit branches
  if ( emtfSimHits.isValid() ) {
    for (l1t::EMTFHit emtfSimHit: *emtfSimHits) {
      emtfSimHitInfo.Fill(emtfSimHit);
    } // End for (l1t::EMTFHit emtfSimHit: *emtfSimHits)
  }
  else {
    std::cout << "ERROR: could not get emtfSimHits from event!!!" << std::endl;
    return;
  }
  */

  std::cout << "About to fill EMTF track branches" << std::endl;
  // Fill EMTF track branches
  if ( emtfTracks.isValid() ) {
    for (l1t::EMTFTrack emtfTrk: *emtfTracks) {
      emtfTrackInfo.Fill(emtfTrk, emtfHitInfo);
    }
  } // End for (l1t::EMTFTrack emtfTrk: *emtfTracks)
  else {
    std::cout << "ERROR: could not get emtfTracks from event!!!" << std::endl;
    return;
  }


  std::cout << "About to fill unpacked EMTF track branches" << std::endl;
  if (not isMC) {
    // Fill Unpacked EMTF track branches
    if ( emtfUnpTracks.isValid() ) {
      for (l1t::EMTFTrack emtfTrk: *emtfUnpTracks) {
        emtfUnpTrackInfo.Fill(emtfTrk, emtfHitInfo);
      } // End for (l1t::EMTFTrack emtfTrk: *emtfUnpTracks)
    }
    else {
      std::cout << "ERROR: could not get emtfUnpTracks from event!!!" << std::endl;
      return;
    }
  }
  if (skimEmtf && ACCESS(emtfTrackInfo.mInts,    "nTracksBX0")    == 0
               && ACCESS(emtfUnpTrackInfo.mInts, "nUnpTracksBX0") == 0) return;


  // std::cout << "About to match EMTF hits to simulated hits" << std::endl;
  // Match EMTF hits to simulated hits (and visa-versa)
  simUnpHit.Match(emtfHitInfo, emtfSimHitInfo);


  // std::cout << "About to match CSC RECO segments to CSC LCTs" << std::endl;
  // Match CSC RECO segments to CSC LCTs (and visa-versa)
  lctSeg.Match(cscSegInfo, emtfHitInfo);

  // std::cout << "About to match emulated EMTF tracks to RECO muons" << std::endl;
  // Match emulated EMTF tracks to RECO muons (and visa-versa)
  recoTrkDR.Match(recoMuonInfo, emtfTrackInfo, MIN_RECO_ETA, MAX_RECO_ETA, MAX_RECO_TRK_MATCH_DR);

  // std::cout << "About to match unpacked EMTF tracks to RECO muons" << std::endl;
  // Match unpacked EMTF tracks to RECO muons (and visa-versa)
  recoUnpTrkDR.Match(recoMuonInfo, emtfUnpTrackInfo, MIN_RECO_ETA, MAX_RECO_ETA, MAX_RECO_TRK_MATCH_DR);

  // std::cout << "About to match EMTF unpacked and emulated tracks" << std::endl;
  // Match unpacked and emulated EMTF tracks
  unpEmuTrkDR.Match(emtfUnpTrackInfo, emtfTrackInfo, MAX_UNP_EMU_MATCH_DR);

  // Check that size of all vectors is consistent
  genMuonInfo.CheckSize();
  emtfHitInfo.CheckSize();
  emtfSimHitInfo.CheckSize();
  emtfTrackInfo.CheckSize();
  emtfUnpTrackInfo.CheckSize();
  cscSegInfo.CheckSize();
  recoMuonInfo.CheckSize();
  recoPairInfo.CheckSize();

  // std::cout << "About to fill output tree" << std::endl;
  nEventsSel_ += 1;
  out_tree->Fill();

  // std::cout << "All done with this event!\n" << std::endl;
  return;

} // End FlatNtuple::analyze


// Called once per job before starting event loop
void FlatNtuple::beginJob() {

  eventInfo.Initialize();
  genMuonInfo.Initialize();
  emtfHitInfo.Initialize();
  emtfSimHitInfo.Initialize();
  emtfTrackInfo.Initialize();
  emtfUnpTrackInfo.Initialize();
  cscSegInfo.Initialize();
  recoMuonInfo.Initialize();
  recoPairInfo.Initialize();

  ////////////////////////////////////////////////
  ////   WARNING!!! CONSTRUCTION OF STRUCTS   ////
  ////////////////////////////////////////////////
  // All variables in the struct must have the same length
  // e.g. Int_t (I) and Float_t (F) have 4 bytes, Long64_t (L) and Double_t (D) have 8 bytes
  // https://twiki.cern.ch/twiki/bin/view/Main/RootNotes#Conventions_and_Types
  // https://root.cern.ch/root/html534/guides/users-guide/Trees.html#adding-a-branch-to-hold-a-list-of-variables

  if (not isMC) {
    for (auto & it : eventInfo.mInts)  out_tree->Branch(it.first, (int*)       &it.second);
    for (auto & it : eventInfo.mLongs) out_tree->Branch(it.first, (long long*) &it.second);
  }

  if (isMC) {
    for (auto & it : genMuonInfo.mInts) out_tree->Branch(it.first, (int*) &it.second);
    for (auto & it : genMuonInfo.mVFlt) out_tree->Branch(it.first, (std::vector<float>*) &it.second);
    for (auto & it : genMuonInfo.mVInt) out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
  }

  for (auto & it : emtfHitInfo.mInts) out_tree->Branch(it.first, (int*) &it.second);
  for (auto & it : emtfHitInfo.mVFlt) out_tree->Branch(it.first, (std::vector<float>*) &it.second);
  for (auto & it : emtfHitInfo.mVInt) out_tree->Branch(it.first, (std::vector<int>*)   &it.second);

  for (auto & it : emtfSimHitInfo.mInts) out_tree->Branch(it.first, (int*) &it.second);
  for (auto & it : emtfSimHitInfo.mVFlt) out_tree->Branch(it.first, (std::vector<float>*) &it.second);
  for (auto & it : emtfSimHitInfo.mVInt) out_tree->Branch(it.first, (std::vector<int>*)   &it.second);

  for (auto & it : emtfTrackInfo.mInts)  out_tree->Branch(it.first, (int*) &it.second);
  for (auto & it : emtfTrackInfo.mVFlt)  out_tree->Branch(it.first, (std::vector<float>*) &it.second);
  for (auto & it : emtfTrackInfo.mVInt)  out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
  for (auto & it : emtfTrackInfo.mVVInt) out_tree->Branch(it.first, (std::vector<std::vector<int> >*) &it.second);

  if (isReco) {
    for (auto & it : cscSegInfo.mInts) out_tree->Branch(it.first, (int*) &it.second);
    for (auto & it : cscSegInfo.mVFlt) out_tree->Branch(it.first, (std::vector<float>*) &it.second);
    for (auto & it : cscSegInfo.mVInt) out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
  }

  if (isReco) {
    for (auto & it : recoMuonInfo.mInts)  out_tree->Branch(it.first, (int*) &it.second);
    for (auto & it : recoMuonInfo.mVFlt)  out_tree->Branch(it.first, (std::vector<float>*) &it.second);
    for (auto & it : recoMuonInfo.mVInt)  out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
  }

  if (isReco) {
    for (auto & it : recoPairInfo.mInts)  out_tree->Branch(it.first, (int*) &it.second);
    for (auto & it : recoPairInfo.mVFlt)  out_tree->Branch(it.first, (std::vector<float>*) &it.second);
    for (auto & it : recoPairInfo.mVInt)  out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
  }

  if (not isMC) {
    for (auto & it : emtfUnpTrackInfo.mInts)  out_tree->Branch(it.first, (int*) &it.second);
    for (auto & it : emtfUnpTrackInfo.mVFlt)  out_tree->Branch(it.first, (std::vector<float>*) &it.second);
    for (auto & it : emtfUnpTrackInfo.mVInt)  out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
    for (auto & it : emtfUnpTrackInfo.mVVInt) out_tree->Branch(it.first, (std::vector<std::vector<int> >*) &it.second);
  }

} // End FlatNtuple::beginJob

// Called once per job after ending event loop
void FlatNtuple::endJob() {
  out_tree_meta->Branch("nEventsProc", &nEventsProc_, "nEventsProc/I");
  out_tree_meta->Branch("nEventsSel",  &nEventsSel_,  "nEventsSel/I");

  out_tree_meta->Fill();
}
