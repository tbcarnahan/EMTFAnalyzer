
# ## Basic (but usually unnecessary) imports
# import os
# import sys
# import commands

import subprocess

import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_cff import Run3

process = cms.Process('NTUPLE', Run3)

## Import standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi') ## What does this module do? - AWB 05.09.16
process.load('Configuration.StandardSequences.GeometryRecoDB_cff') ## What does this module do? - AWB 05.09.16
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
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')

## Default parameters for firmware version, pT LUT XMLs, and coordinate conversion LUTs
#process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_cff')

readFiles = cms.untracked.vstring()
process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring(
        'file:step2bis.root'
    ),
)

###################
###  NTuplizer  ###
###################
process.load('EMTFAnalyzer.NTupleMaker.FlatNtuple_cfi')
process.load('EMTFAnalyzer.NTupleMaker.PtLutInput_cfi')

process.FlatNtupleMC.emtfHitTag = cms.InputTag("simEmtfDigis","","ReL1")
process.FlatNtupleMC.emtfTrackTag = cms.InputTag("simEmtfDigis","","ReL1")

process.PtLutInputMC.emtfHitTag = cms.InputTag("simEmtfDigis","","ReL1")
process.PtLutInputMC.emtfTrackTag = cms.InputTag("simEmtfDigis","","ReL1")

process.Analysis = cms.Sequence(process.FlatNtupleMC)# * process.PtLutInputMC)
process.Analysis_step = cms.Path(process.Analysis)
process.endjob_step = cms.EndPath(process.endOfProcess)

## NTuple output File
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('EMTF_MC_NTuple_SingleMu_20200522.root')
    )

# Schedule definition
process.schedule = cms.Schedule(process.Analysis_step,process.endjob_step)
