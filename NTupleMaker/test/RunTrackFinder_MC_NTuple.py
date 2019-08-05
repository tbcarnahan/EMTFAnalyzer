
# ## Basic (but usually unnecessary) imports
# import os
# import sys
# import commands

import subprocess

import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reL1T', eras.Phase2)

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
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))

## Global Tags
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2023_realistic_v3', '')

## Default parameters for firmware version, pT LUT XMLs, and coordinate conversion LUTs
process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_cff')

readFiles = cms.untracked.vstring()
process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring(['/store/mc/PhaseIITDRSpring19DR/Mu_FlatPt2to100-pythia8-gun/GEN-SIM-DIGI-RAW/NoPU_106X_upgrade2023_realistic_v3-v1/60000/E0D5C6A5-B855-D14F-9124-0B2C9B28D0EA.root'])
    )

#eos_cmd = '/afs/cern.ch/project/eos/installation/pro/bin/eos.select'

#in_dir_name = '/store/user/abrinke1/EMTF/MC/SingleMu_Pt1To1000_FlatRandomOneOverPt/'
# in_dir_name = '/store/user/abrinke1/EMTF/MC/JPsiToMuMu_Pt20to120_EtaPhiRestricted-pythia8-gun/'

'''
for in_file_name in subprocess.check_output([eos_cmd, 'ls', in_dir_name]).splitlines():
    if not ('.root' in in_file_name): continue
    readFiles.extend( cms.untracked.vstring(in_dir_name+in_file_name) )
'''

process.dumpED = cms.EDAnalyzer("EventContentAnalyzer")
process.dumpES = cms.EDAnalyzer("PrintEventSetupContent")

# Path and EndPath definitions

# ## Defined in Configuration/StandardSequences/python/SimL1EmulatorRepack_FullMC_cff.py
# process.L1RePack_step = cms.Path(process.SimL1Emulator)
#SimL1Emulator_AWB = cms.Sequence(process.unpackRPC+process.unpackCSC)
#process.L1RePack_step = cms.Path(SimL1Emulator_AWB)

## Defined in Configuration/StandardSequences/python/RawToDigi_cff.py
## Includes L1TRawToDigi, defined in L1Trigger/Configuration/python/L1TRawToDigi_cff.py
# process.raw2digi_step = cms.Path(process.RawToDigi)

#process.simCscTriggerPrimitiveDigis.CSCComparatorDigiProducer = cms.InputTag('unpackCSC', 'MuonCSCComparatorDigi')
#process.simCscTriggerPrimitiveDigis.CSCWireDigiProducer       = cms.InputTag('unpackCSC', 'MuonCSCWireDigi')

process.load('L1Trigger.L1TMuonEndCap.simEmtfDigis_cfi')

process.simEmtfDigis.verbosity       = cms.untracked.int32(0)
###################
###  NTuplizer  ###
###################
process.ntuple = cms.EDAnalyzer('PtLutInput',
                                isMC          = cms.bool(True),
                                genMuonTag    = cms.InputTag("genParticles"),  ## GEN muons
                                emtfHitTag    = cms.InputTag("simEmtfDigis"),  ## EMTF input LCTs
                                emtfTrackTag  = cms.InputTag("simEmtfDigis"),  ## EMTF emulator output tracks
                                )

RawToDigi_AWB = cms.Sequence(#process.simCscTriggerPrimitiveDigis +
                             #process.muonCSCDigis +
                             #process.muonRPCDigis +
                             #process.csctfDigis +
                             process.simEmtfDigis +
                             process.ntuple)
process.raw2digi_step = cms.Path(RawToDigi_AWB)

## Defined in Configuration/StandardSequences/python/EndOfProcess_cff.py
process.endjob_step = cms.EndPath(process.endOfProcess)

# process.L1TMuonSeq = cms.Sequence(
#     process.muonCSCDigis + ## Unpacked CSC LCTs from TMB
#     process.csctfDigis + ## Necessary for legacy studies, or if you use csctfDigis as input
#     process.muonRPCDigis +
#     ## process.esProd + ## What do we loose by not having this? - AWB 18.04.16
#     process.emtfStage2Digis +
#     process.simEmtfDigis
#     ## process.ntuple
#     )

# process.L1TMuonPath = cms.Path(
#     process.L1TMuonSeq
#     )

out_dir_name = '/afs/cern.ch/work/a/abrinke1/public/EMTF/Analyzer/ntuples/'
out_dir_name = ''

## NTuple output File
process.TFileService = cms.Service(
    "TFileService",
    # fileName = cms.string("EMTF_MC_NTuple_Tau3Mu.root")
    fileName = cms.string(out_dir_name+'EMTF_MC_NTuple_SingleMu_2019_07_30.root')
    # fileName = cms.string('EMTF_MC_NTuple_SingleMu_RPC_test.root')
    # fileName = cms.string(out_dir_name+'EMTF_MC_NTuple_JPsi_RPC_1_file.root')
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
