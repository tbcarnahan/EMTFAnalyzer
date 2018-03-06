
// Make NTuples for input to BDT training
// Runs off of the new 2017 EMTF emultor
// Andrew Brinkerhoff - 27.06.17

#include "EMTFAnalyzer/NTupleMaker/plugins/FlatNtuple.h"

// Constructor
FlatNtuple::FlatNtuple(const edm::ParameterSet& iConfig):
  muProp1st_(iConfig.getParameter<edm::ParameterSet>("muProp1st")), // Propagate RECO muon coordinates to 1st muon station
  muProp2nd_(iConfig.getParameter<edm::ParameterSet>("muProp2nd"))  // Propagate RECO muon coordinates to 2nd muon station
{
  // Output file
  edm::Service<TFileService> fs;
  out_tree = fs->make<TTree>("tree","FlatNtupleTree");
  
  // Config parameters  
  isMC   = iConfig.getParameter<bool>("isMC");
  isReco = iConfig.getParameter<bool>("isReco");
  
  // Input collections
  if (isMC)   GenMuon_token      = consumes<std::vector<reco::GenParticle>> (iConfig.getParameter<edm::InputTag>("genMuonTag"));
  if (isReco) RecoMuon_token     = consumes<reco::MuonCollection>           (iConfig.getParameter<edm::InputTag>("recoMuonTag"));
  if (isReco) RecoVertex_token   = consumes<reco::VertexCollection>         (iConfig.getParameter<edm::InputTag>("recoVertexTag"));
  if (isReco) RecoBeamSpot_token = consumes<reco::BeamSpot>                 (iConfig.getParameter<edm::InputTag>("recoBeamSpotTag"));
  
  EMTFHit_token      = consumes<std::vector<l1t::EMTFHit>>   (iConfig.getParameter<edm::InputTag>("emtfHitTag"));
  EMTFSimHit_token   = consumes<std::vector<l1t::EMTFHit>>   (iConfig.getParameter<edm::InputTag>("emtfSimHitTag"));
  EMTFTrack_token    = consumes<std::vector<l1t::EMTFTrack>> (iConfig.getParameter<edm::InputTag>("emtfTrackTag"));
  EMTFUnpTrack_token = consumes<std::vector<l1t::EMTFTrack>> (iConfig.getParameter<edm::InputTag>("emtfUnpTrackTag"));
  
} // End FlatNtuple::FlatNtuple

// Destructor
FlatNtuple::~FlatNtuple() {}


// Called once per event
void FlatNtuple::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // std::cout << "\nCalling analyze" << std::endl;
  edm::Handle<std::vector<reco::GenParticle>> genMuons;
  if (isMC)   iEvent.getByToken(GenMuon_token, genMuons);
  edm::Handle<reco::MuonCollection> recoMuons;
  if (isReco) iEvent.getByToken(RecoMuon_token, recoMuons);
  edm::Handle<reco::VertexCollection> recoVertices;
  if (isReco) iEvent.getByToken(RecoVertex_token, recoVertices);
  edm::Handle<reco::BeamSpot> recoBeamSpot;
  if (isReco) iEvent.getByToken(RecoBeamSpot_token, recoBeamSpot);

  
  edm::Handle<std::vector<l1t::EMTFHit>> emtfHits;
  iEvent.getByToken(EMTFHit_token, emtfHits);
  edm::Handle<std::vector<l1t::EMTFHit>> emtfSimHits;
  iEvent.getByToken(EMTFSimHit_token, emtfSimHits);
  edm::Handle<std::vector<l1t::EMTFTrack>> emtfTracks;
  iEvent.getByToken(EMTFTrack_token, emtfTracks);
  edm::Handle<std::vector<l1t::EMTFTrack>> emtfUnpTracks;
  iEvent.getByToken(EMTFUnpTrack_token, emtfUnpTracks);
  
  // Reset branch values
  eventInfo.Reset();
  genMuonInfo.Reset();
  emtfHitInfo.Reset();
  emtfSimHitInfo.Reset();
  emtfTrackInfo.Reset();
  emtfUnpTrackInfo.Reset();
  recoMuonInfo.Reset();
  
  // std::cout << "About to fill event info" << std::endl;	
  // Fill event info
  eventInfo.Fill(iEvent);
  

  // std::cout << "About to fill RECO muon info" << std::endl;	
  // Fill RECO muon info
  if ( isReco && recoMuons.isValid() && recoVertices.isValid() ) {
    // Set up muon propagator for this event
    muProp1st_.init(iSetup);
    muProp2nd_.init(iSetup);
    // Loop over RECO muons
    for ( reco::MuonCollection::const_iterator mu = recoMuons->begin(); mu != recoMuons->end(); ++mu ) {
      recoMuonInfo.Fill( *mu, (*recoVertices)[0], recoBeamSpot, muProp1st_, muProp2nd_, MIN_RECO_ETA, MAX_RECO_ETA );
    }
  }
  else if (isReco) {
    std::cout << "ERROR: could not get recoMuons or recoVertices from event!!!" << std::endl;
    return;
  }
  
  
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


  // std::cout << "About to fill EMTF hit branches" << std::endl;
  // Fill EMTF hit branches
  if ( emtfHits.isValid() ) {
    for (l1t::EMTFHit emtfHit: *emtfHits) {
      emtfHitInfo.Fill(emtfHit);
    } // End for (l1t::EMTFHit emtfHit: *emtfHits)
  }
  else {
    std::cout << "ERROR: could not get emtfHits from event!!!" << std::endl;
    return;
  }

  
  // std::cout << "About to fill EMTF simHit branches" << std::endl;
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

  
  // std::cout << "About to fill EMTF track branches" << std::endl;
  bool passesSingleMu16 = false;
  // Fill EMTF track branches
  if ( emtfTracks.isValid() ) {
    for (l1t::EMTFTrack emtfTrk: *emtfTracks) {
      emtfTrackInfo.Fill(emtfTrk, emtfHitInfo);
      if ( (emtfTrk.Mode() == 7 || emtfTrk.Mode() == 11 || emtfTrk.Mode() > 12) &&
	   emtfTrk.Pt() >= 16 ) passesSingleMu16 = true;
    }
  } // End for (l1t::EMTFTrack emtfTrk: *emtfTracks)
  else {
    std::cout << "ERROR: could not get emtfTracks from event!!!" << std::endl;
    return;
  }


  // std::cout << "About to fill unpacked EMTF track branches" << std::endl;
  if (not isMC) {
    // Fill Unpacked EMTF track branches
    if ( emtfUnpTracks.isValid() ) {
      for (l1t::EMTFTrack emtfTrk: *emtfUnpTracks) {
	emtfUnpTrackInfo.Fill(emtfTrk, emtfHitInfo);
	if ( (emtfTrk.Mode() == 7 || emtfTrk.Mode() == 11 || emtfTrk.Mode() > 12) &&
	     emtfTrk.Pt() >= 16 ) passesSingleMu16 = true;
      } // End for (l1t::EMTFTrack emtfTrk: *emtfUnpTracks)
    }  
    else {
      std::cout << "ERROR: could not get emtfUnpTracks from event!!!" << std::endl;
      return;
    }
  }


  // std::cout << "About to match EMTF hits to simulated hits" << std::endl;
  // Match EMTF hits to simulated hits (and visa-versa)
  simUnpHit.Match(emtfHitInfo, emtfSimHitInfo);

  // std::cout << "About to match EMTF tracks to RECO muons" << std::endl;
  // Match emulated EMTF tracks to RECO muons (and visa-versa)
  recoTrkDR.Match(recoMuonInfo, emtfTrackInfo, MIN_RECO_ETA, MAX_RECO_ETA, MAX_RECO_TRK_MATCH_DR);

  // std::cout << "About to match EMTF unpacked and emulated tracks" << std::endl;
  // Match unpacked and emulated EMTF tracks
  unpEmuTrkDR.Match(emtfUnpTrackInfo, emtfTrackInfo, MAX_UNP_EMU_MATCH_DR);

  
  // std::cout << "About to fill output tree" << std::endl;
  if (passesSingleMu16 || true) { // No filter for now
    out_tree->Fill();
  }
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
  recoMuonInfo.Initialize();
	
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
  
  for (auto & it : recoMuonInfo.mInts)  out_tree->Branch(it.first, (int*) &it.second);
  for (auto & it : recoMuonInfo.mVFlt)  out_tree->Branch(it.first, (std::vector<float>*) &it.second);
  for (auto & it : recoMuonInfo.mVInt)  out_tree->Branch(it.first, (std::vector<int>*)   &it.second);

  if (not isMC) {
    for (auto & it : emtfUnpTrackInfo.mInts)  out_tree->Branch(it.first, (int*) &it.second);
    for (auto & it : emtfUnpTrackInfo.mVFlt)  out_tree->Branch(it.first, (std::vector<float>*) &it.second);
    for (auto & it : emtfUnpTrackInfo.mVInt)  out_tree->Branch(it.first, (std::vector<int>*)   &it.second);
    for (auto & it : emtfUnpTrackInfo.mVVInt) out_tree->Branch(it.first, (std::vector<std::vector<int> >*) &it.second);
  }

} // End FlatNtuple::beginJob

// Called once per job after ending event loop
void FlatNtuple::endJob() {}

