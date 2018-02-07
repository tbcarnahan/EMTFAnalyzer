import FWCore.ParameterSet.Config as cms

FlatNtupleMC = cms.EDAnalyzer('FlatNtuple',
                              isMC          = cms.bool(True),
                              genMuonTag    = cms.InputTag("genParticles"),
                              emtfHitTag    = cms.InputTag("simEmtfDigisMC"),
                              emtfTrackTag  = cms.InputTag("simEmtfDigisMC"),
                              emtfUnpTrackTag  = cms.InputTag(""),
                              )

FlatNtupleData = cms.EDAnalyzer('FlatNtuple',
                                isMC          = cms.bool(False),
                                genMuonTag    = cms.InputTag(""),
                                emtfHitTag    = cms.InputTag("simEmtfDigisData"),
                                emtfTrackTag  = cms.InputTag("simEmtfDigisData"),
                                emtfUnpTrackTag  = cms.InputTag("emtfStage2DigisData"),
                                recoMuTag  = cms.InputTag("muons"),
                                verticesTag = cms.InputTag("offlinePrimaryVertices"),
                                # muon track extrapolation to 1st station
                                muProp1st = cms.PSet(
                                  useTrack = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker', 'muon', 'global'
                                  useState = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
                                  useSimpleGeometry = cms.bool(True),
                                  useStation2 = cms.bool(False),
                                ),
                                # muon track extrapolation to 2nd station
                                muProp2nd = cms.PSet(
                                  useTrack = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker', 'muon', 'global'
                                  useState = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
                                  useSimpleGeometry = cms.bool(True),
                                  useStation2 = cms.bool(True),
                                  fallbackToME1 = cms.bool(False),
                                ),
                               )
