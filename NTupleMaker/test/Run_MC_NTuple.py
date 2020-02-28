
# ## Basic (but usually unnecessary) imports
# import os
# import sys
# import commands

import subprocess

import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Phase2C8_timing_layer_bar_cff import Phase2C8_timing_layer_bar

process = cms.Process('NTUPLE', Phase2C8_timing_layer_bar)

## Import standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi') ## What does this module do? - AWB 05.09.16
process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff') ## What does this module do? - AWB 05.09.16
process.load('Configuration.StandardSequences.MagneticField_cff') ## Different than in data ("MagneticField_AutoFromDBCurrent_cff"?)
#process.load('Configuration.StandardSequences.SimL1EmulatorRepack_FullMC_cff') ## Different than in data
process.load('Configuration.StandardSequences.RawToDigi_cff') ## Different than in data ("RawToDigi_Data_cff"?)
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi')

## CSCTF digis, phi / pT LUTs?
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")

## Import RECO muon configurations
process.load("RecoMuon.TrackingTools.MuonServiceProxy_cff")
process.load("RecoMuon.TrackingTools.MuonTrackLoader_cff")

## Message Logger and Event range
#process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))

## Global Tags
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

## Default parameters for firmware version, pT LUT XMLs, and coordinate conversion LUTs
process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_cff')

readFiles = cms.untracked.vstring()
process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring(
        'file:/eos/uscms/store/user/dildick/SingleMu/crab_SingleMu_DIGI_L1_20190822/190823_043416/0000/step2_1.root',
        'file:/eos/uscms/store/user/dildick/SingleMu/crab_SingleMu_DIGI_L1_20190822/190823_043416/0000/step2_20.root',
        'file:/eos/uscms/store/user/dildick/SingleMu/crab_SingleMu_DIGI_L1_20190822/190823_043416/0000/step2_30.root',
        'file:/eos/uscms/store/user/dildick/SingleMu/crab_SingleMu_DIGI_L1_20190822/190823_043416/0000/step2_40.root',
        'file:/eos/uscms/store/user/dildick/SingleMu/crab_SingleMu_DIGI_L1_20190822/190823_043416/0000/step2_50.root',
        'file:/eos/uscms/store/user/dildick/SingleMu/crab_SingleMu_DIGI_L1_20190822/190823_043416/0000/step2_60.root'
    ),
)

###################
###  NTuplizer  ###
###################
process.load('EMTFAnalyzer.NTupleMaker.FlatNtuple_cfi')
process.load('EMTFAnalyzer.NTupleMaker.PtLutInput_cfi')

process.FlatNtupleMC.emtfHitTag = cms.InputTag("simEmtfDigis","","L1")
process.FlatNtupleMC.emtfTrackTag = cms.InputTag("simEmtfDigis","","L1")

process.PtLutInputMC.emtfHitTag = cms.InputTag("simEmtfDigis","","L1")
process.PtLutInputMC.emtfTrackTag = cms.InputTag("simEmtfDigis","","L1")

process.Analysis = cms.Sequence(process.FlatNtupleMC)# * process.PtLutInputMC)
process.Analysis_step = cms.Path(process.Analysis)
process.endjob_step = cms.EndPath(process.endOfProcess)

## NTuple output File
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('EMTF_MC_NTuple_SingleMu_20200219.root')
    )

# Schedule definition
process.schedule = cms.Schedule(process.Analysis_step,process.endjob_step)
