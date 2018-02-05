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
                                recoMuonTag  = cms.InputTag("muons"),
                                )
