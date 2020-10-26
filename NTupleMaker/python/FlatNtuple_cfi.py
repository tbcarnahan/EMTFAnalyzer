import FWCore.ParameterSet.Config as cms

FlatNtupleMC = cms.EDAnalyzer('FlatNtuple',
                              isMC             = cms.bool(True),
                              isReco           = cms.bool(False),
                              skimTrig         = cms.bool(False), # Discard events with too few triggering muons
                              skimEmtf         = cms.bool(False), # Discard events with no EMTF tracks in BX 0
                              skimPair         = cms.bool(False), # Discard events with no RECO muon pairs
                              genMuonTag       = cms.InputTag("genParticles"),
                              emtfHitTag       = cms.InputTag("simEmtfDigisRun3CCLUT"),
                              emtfTrackTag     = cms.InputTag("simEmtfDigisRun3CCLUT"),
                              #emtfHitTag       = cms.InputTag("GEMEMTFMatcher"),
                              #emtfTrackTag     = cms.InputTag("GEMEMTFMatcher"),
                              emtfUnpTrackTag  = cms.InputTag(""),

                              cscSegmentTag    = cms.InputTag("cscSegments"),
                              recoMuonTag      = cms.InputTag("muons"),
                              recoVertexTag    = cms.InputTag("offlinePrimaryVertices"),
                              recoBeamSpotTag  = cms.InputTag("offlineBeamSpot"),
                              trigEvent        = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                              muonTriggers     = cms.vstring("HLT_IsoMu27", "HLT_IsoTkMu27", "HLT_Mu50"),
                              cppfDigiTag      = cms.InputTag("simEmtfDigisData"),
                              cppfUnpDigiTag   = cms.InputTag("emtfStage2Digis"),
                              emtfSimHitTag    = cms.InputTag("simEmtfDigisSimHit"),

                              ## option to exclude GEM and iRPC hits from the NTuple-making!
                              ignoreME0 = cms.bool(True),
                              ignoreGE11 = cms.bool(False),
                              ignoreGE21 = cms.bool(True),
                              ignoreRE31 = cms.bool(True),
                              ignoreRE41 = cms.bool(True),
                              ignoreDT = cms.bool(True),

                              # RECO muon extrapolation to 1st station
                              muProp1st = cms.PSet(
                                  useTrack          = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker', 'muon', 'global'
                                  useState          = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
                                  useSimpleGeometry = cms.bool(True),
                                  useStation2       = cms.bool(False),
                              ),

                              # RECO muon extrapolation to 2nd station
                              muProp2nd = cms.PSet(
                                  useTrack          = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker', 'muon', 'global'
                                  useState          = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
                                  useSimpleGeometry = cms.bool(True),
                                  useStation2       = cms.bool(True),
                                  fallbackToME1     = cms.bool(False),
                              ),

                              )

FlatNtupleData = cms.EDAnalyzer('FlatNtuple',
                                isMC             = cms.bool(False),
                                isReco           = cms.bool(True),
                                skimTrig         = cms.bool(False), # Discard events with too few triggering muons
                                skimEmtf         = cms.bool(False), # Discard events with no EMTF tracks in BX 0
                                skimPair         = cms.bool(False), # Discard events with no RECO muon pairs

                                genMuonTag       = cms.InputTag(""),
                                cscSegmentTag    = cms.InputTag("cscSegments"),
                                recoMuonTag      = cms.InputTag("muons"),
                                recoVertexTag    = cms.InputTag("offlinePrimaryVertices"),
                                recoBeamSpotTag  = cms.InputTag("offlineBeamSpot"),
                                trigEvent        = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
                                muonTriggers     = cms.vstring("HLT_IsoMu27", "HLT_IsoTkMu27", "HLT_Mu50"),
                                cppfDigiTag      = cms.InputTag("simEmtfDigisData"),
                                cppfUnpDigiTag   = cms.InputTag("emtfStage2Digis"),
                                emtfHitTag       = cms.InputTag("simEmtfDigisData"),
                                emtfSimHitTag    = cms.InputTag("simEmtfDigisDataSimHit"),
                                emtfTrackTag     = cms.InputTag("simEmtfDigisData"),
                                emtfUnpTrackTag  = cms.InputTag("emtfStage2Digis"),

                                # RECO muon extrapolation to 1st station
                                muProp1st = cms.PSet(
                                  useTrack          = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker', 'muon', 'global'
                                  useState          = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
                                  useSimpleGeometry = cms.bool(True),
                                  useStation2       = cms.bool(False),
                                  ),

                                # RECO muon extrapolation to 2nd station
                                muProp2nd = cms.PSet(
                                  useTrack          = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker', 'muon', 'global'
                                  useState          = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
                                  useSimpleGeometry = cms.bool(True),
                                  useStation2       = cms.bool(True),
                                  fallbackToME1     = cms.bool(False),
                                  ),
                                )
