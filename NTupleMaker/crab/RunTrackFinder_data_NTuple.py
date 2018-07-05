# 11.02.16: Copied from https://raw.githubusercontent.com/dcurry09/EMTF8/master/L1Trigger/L1TMuonEndCap/test/runMuonEndCap.py

import FWCore.ParameterSet.Config as cms
import os
import sys
import commands
import subprocess
from Configuration.StandardSequences.Eras import eras

process = cms.Process('L1TMuonEmulation')

# ## Verbose printouts of plugins
# process.add_(cms.Service("PrintLoadingPlugins"))

## Import standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
## Geometry : see http://cmslxr.fnal.gov/source/Configuration/Geometry/python/
# process.load('Configuration.Geometry.GeometryExtended2016Reco_cff') ## Is this appropriate for 2015 data/MC? - AWB 18.04.16
process.load('Configuration.Geometry.GeometryExtended2018Reco_cff')   ## Is this appropriate for 2018 data?    - AWB 19.06.18
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff') ## Will this work on 0T data? - AWB 18.04.16
process.load('Configuration.StandardSequences.RawToDigi_Data_cff') ## Will this work for MC? - AWB 18.04.16
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

# ## Extra tools
# process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
#                                         ignoreTotal = cms.untracked.int32(1),
#                                         monitorPssAndPrivate = cms.untracked.bool(True))
# process.Tracer = cms.Service("Tracer")

# ## CSCTF digis, phi / pT LUTs?
# process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
# process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")

## Import RECO muon configurations
process.load("RecoMuon.TrackingTools.MuonServiceProxy_cff")
process.load("RecoMuon.TrackingTools.MuonTrackLoader_cff")

## Lumi JSON tools
import FWCore.PythonUtilities.LumiList as LumiList
# process.source.lumisToProcess = LumiList.LumiList(filename = 'goodList.json').getVLuminosityBlockRange()

## Message Logger and Event range
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))

process.options = cms.untracked.PSet(
    # SkipEvent = cms.untracked.vstring('ProductNotFound')
)

# ## Global Tags
# from Configuration.AlCa.GlobalTag import GlobalTag
# # process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
# process.GlobalTag = GlobalTag(process.GlobalTag, '102X_dataRun2_v3', '')

## Global Tags
process.GlobalTag.globaltag = '101X_dataRun2_HLT_v7'

# from Configuration.AlCa.GlobalTag import GlobalTag as gtCustomise
# process.GlobalTag.globaltag = gtCustomise(process.GlobalTag, 'auto:run2_data', '')

# from Configuration.AlCa.autoCond_condDBv2 import autoCond
# process.GlobalTag.globaltag = cms.string( autoCond['run2_data'] )


# ## Default parameters for firmware version, pT LUT XMLs, and coordinate conversion LUTs
# process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_cff') 
# # process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_2017_data_cff') 
# # process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_2016_data_cff') 

## Un-comment out this line to choose the GlobalTag settings rather than fakeEmtfParams settings
## Comment out this line to use default FW version rather than true FW version in data
## Update: seems to have no effect one way or the other in re-emulating 2017 data - AWB 26.04.18
process.es_prefer_GlobalTag = cms.ESPrefer("PoolDBESSource","GlobalTag")

## What is this supposed to do?  Causes segfault when re-emulating 2017 data - AWB 26.04.18
# process.es_prefer_GlobalTag = cms.ESPrefer("PoolDBESSource","emtfParamsSource")


readFiles = cms.untracked.vstring()
secFiles  = cms.untracked.vstring()
process.source = cms.Source(
    'PoolSource',
    fileNames = readFiles,
    secondaryFileNames= secFiles
    # eventsToProcess = cms.untracked.VEventRange('317626:30320773')
    )


process.content = cms.EDAnalyzer("EventContentAnalyzer")
process.dumpED = cms.EDAnalyzer("EventContentAnalyzer")
process.dumpES = cms.EDAnalyzer("PrintEventSetupContent")


## Re-emulate CSC LCTs to get full ALCT+CLCT info
process.load('L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi')
process.cscTriggerPrimitiveDigis.CSCComparatorDigiProducer = cms.InputTag('muonCSCDigis', 'MuonCSCComparatorDigi')
process.cscTriggerPrimitiveDigis.CSCWireDigiProducer       = cms.InputTag('muonCSCDigis', 'MuonCSCWireDigi')
process.cscTriggerPrimitiveDigis.commonParam = cms.PSet(
    isTMB07 = cms.bool(True), ## Default
    isMTCC = cms.bool(False), ## Default
    isSLHC = cms.bool(False), ## Default
    smartME1aME1b = cms.bool(False), ## Default
    gangedME1a = cms.bool(False), ## Changed - why is this not default?
    disableME1a = cms.bool(False), ## Default
    disableME42 = cms.bool(False), ## Default
    alctClctOffset = cms.uint32(1), ## Default
)

## EMTF Emulator
process.load('EventFilter.L1TRawToDigi.emtfStage2Digis_cfi')
process.load('L1Trigger.L1TMuonEndCap.simEmtfDigis_cfi')

process.simEmtfDigisData.verbosity  = cms.untracked.int32(0)
process.simEmtfDigisData.CPPFEnable = cms.bool(True)

# process.simEmtfDigisData.FWConfig = cms.bool(True)

## Planned 2018 settings
process.simEmtfDigisData.FWConfig = cms.bool(False)
process.simEmtfDigisData.BXWindow = cms.int32(2)
process.simEmtfDigisData.spTBParams16.ThetaWindowZone0 = cms.int32(4)
process.simEmtfDigisData.spTBParams16.BugAmbigThetaWin = cms.bool(False)
process.simEmtfDigisData.spTBParams16.TwoStationSameBX = cms.bool(True)
process.simEmtfDigisData.spPAParams16.ModeQualVer      = cms.int32(2)

# ## Early 2018 actual settings (through end of April at least)
# process.simEmtfDigisData.FWConfig = cms.bool(False)
# process.simEmtfDigisData.BXWindow = cms.int32(2)
# process.simEmtfDigisData.spTBParams16.ThetaWindowZone0 = cms.int32(8)
# process.simEmtfDigisData.spTBParams16.BugAmbigThetaWin = cms.bool(False)
# process.simEmtfDigisData.spTBParams16.TwoStationSameBX = cms.bool(False)
# process.simEmtfDigisData.spPAParams16.ModeQualVer      = cms.int32(1)


## EMTF Emulator with re-emulated CSC LCTs and clustered RPC hits as input
process.simEmtfDigisDataSimHit = process.simEmtfDigisData.clone()

process.simEmtfDigisDataSimHit.CSCInput = cms.InputTag('cscTriggerPrimitiveDigis','MPCSORTED') ## Re-emulated CSC LCTs
process.simEmtfDigisDataSimHit.CSCInputBXShift = cms.int32(-8) ## Only for re-emulated CSC LCTs (vs. -6 default)
process.simEmtfDigisDataSimHit.CPPFEnable = cms.bool(False)



###################
###  NTuplizer  ###
###################

process.load('EMTFAnalyzer.NTupleMaker.FlatNtuple_cfi')
process.FlatNtupleData.skimTrig = cms.bool(False)
process.FlatNtupleData.skimEmtf = cms.bool(False)
process.FlatNtupleData.isReco   = cms.bool(True)

RawToDigi_AWB = cms.Sequence(
    process.muonRPCDigis             + ## Unpacked RPC hits from RPC PAC
    process.muonCSCDigis             + ## Unpacked CSC LCTs (and raw strip and wire?) from TMB
    process.cscTriggerPrimitiveDigis + ## To get re-emulated CSC LCTs
    # process.csctfDigis               + ## Necessary for legacy studies, or if you use csctfDigis as input
    process.emtfStage2Digis          + 
    process.simEmtfDigisData         + 
    process.simEmtfDigisDataSimHit   +
    process.FlatNtupleData
    )

process.raw2digi_step = cms.Path(RawToDigi_AWB)

## Defined in Configuration/StandardSequences/python/EndOfProcess_cff.py
process.endjob_step = cms.EndPath(process.endOfProcess)

# out_dir_name = '/afs/cern.ch/work/a/abrinke1/public/EMTF/Analyzer/ntuples/'
out_dir_name = './'

## NTuple output File
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('tuple.root')
    )


# outCommands = cms.untracked.vstring('keep *')
outCommands = cms.untracked.vstring(

    'keep recoMuons_muons__*',
    'keep *Gen*_*_*_*',
    'keep *_*Gen*_*_*',
    'keep *gen*_*_*_*',
    'keep *_*gen*_*_*',
    'keep CSCDetIdCSCCorrelatedLCTDigiMuonDigiCollection_*_*_*', ## muonCSCDigis
    'keep RPCDetIdRPCDigiMuonDigiCollection_*_*_*', ## muonRPCDigis
    #'keep CSCCorrelatedLCTDigiCollection_muonCSCDigis_*_*',
    #'keep *_*_*muonCSCDigis*_*',
    #'keep *_*_*_*muonCSCDigis*',
    'keep *_csctfDigis_*_*',
    'keep *_emtfStage2Digis_*_*',
    'keep *_simEmtfDigis_*_*',
    'keep *_simEmtfDigisMC_*_*',
    'keep *_gmtStage2Digis_*_*',
    'keep *_simGmtStage2Digis_*_*',

    )

# process.treeOut = cms.OutputModule("PoolOutputModule", 
#                                    # fileName = cms.untracked.string("EMTF_MC_Tree_RelValNuGun_UP15_1k.root"),
#                                    # fileName = cms.untracked.string("EMTF_MC_Tree_tau_to_3_mu_RPC_debug.root"),
#                                    fileName = cms.untracked.string(out_dir_name+'EMTF_MC_Tree_SingleMu_noRPC_300k.root'),
#                                    outputCommands = outCommands
#                                    )

# process.treeOut_step = cms.EndPath(process.treeOut) ## Keep output tree - AWB 08.07.16

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.endjob_step)
# process.schedule = cms.Schedule(process.L1RePack_step,process.raw2digi_step,process.endjob_step,process.treeOut_step)

# process.output_step = cms.EndPath(process.treeOut)
# process.schedule = cms.Schedule(process.L1TMuonPath)
# process.schedule.extend([process.output_step])

# ## What does this do? Necessary? - AWB 29.04.16
# from SLHCUpgradeSimulations.Configuration.muonCustoms import customise_csc_PostLS1
# process = customise_csc_PostLS1(process)
