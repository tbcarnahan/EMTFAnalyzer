## 11.02.16: Copied from https://raw.githubusercontent.com/dcurry09/EMTF8/master/L1Trigger/L1TMuonEndCap/test/runMuonEndCap.py

import FWCore.ParameterSet.Config as cms
import os
import sys
import commands
from Configuration.StandardSequences.Eras import eras

process = cms.Process('L1TMuonEmulation')

## Import standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryExtended2016Reco_cff') ## Is this appropriate for 2015 data/MC? - AWB 18.04.16
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff') ## Will this work on 0T data? - AWB 18.04.16
process.load('Configuration.StandardSequences.RawToDigi_Data_cff') ## Will this work for MC? - AWB 18.04.16
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

# ## Extra tools
# process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",ignoreTotal = cms.untracked.int32(1))

## CSCTF digis, phi / pT LUTs?
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")

## Import RECO muon configurations
process.load("RecoMuon.TrackingTools.MuonServiceProxy_cff")
process.load("RecoMuon.TrackingTools.MuonTrackLoader_cff")

## Lumi JSON tools
import FWCore.PythonUtilities.LumiList as LumiList
# process.source.lumisToProcess = LumiList.LumiList(filename = 'goodList.json').getVLuminosityBlockRange()

## Message Logger and Event range
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))

process.options = cms.untracked.PSet(
    # SkipEvent = cms.untracked.vstring('ProductNotFound')
)

## Global Tags
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '') ## Good for 2015/2016 data/MC? - AWB 18.04.16
# process.GlobalTag = GlobalTag(process.GlobalTag, 'GR_P_V56', '') ## Used for anything? - AWB 18.04.16

## Event Setup Producer
process.load('L1Trigger.L1TMuonEndCap.fakeEmtfParams_cff') ## Why does this file have "fake" in the name? - AWB 18.04.16
process.esProd = cms.EDAnalyzer("EventSetupRecordDataGetter",
                                toGet = cms.VPSet(
        ## Apparently L1TMuonEndcapParamsRcd doesn't exist in CondFormats/DataRecord/src/ (Important? - AWB 18.04.16)
        cms.PSet(record = cms.string('L1TMuonEndcapParamsRcd'),
                 data = cms.vstring('L1TMuonEndcapParams'))
        ),
                                verbose = cms.untracked.bool(True)
                                )


readFiles = cms.untracked.vstring()
secFiles  = cms.untracked.vstring()
process.source = cms.Source(
    'PoolSource',
    fileNames = readFiles,
    secondaryFileNames= secFiles
    #, eventsToProcess = cms.untracked.VEventRange('201196:265380261')                                                                              
    )

readFiles.extend([
        
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/0202038A-7D27-E611-8819-02163E0144E2.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/10D443D9-BC27-E611-8566-02163E014587.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/14D2E712-7E27-E611-A930-02163E01366C.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/28A36D96-8527-E611-A4E7-02163E013680.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/2A318B3C-8227-E611-A1A4-02163E0134CF.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/346A0FAC-9127-E611-8D6A-02163E012449.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/501DC2B8-7B27-E611-90E7-02163E011EA7.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/8A11646E-7827-E611-884B-02163E0141B9.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/CCCE669F-7C27-E611-A133-02163E011A45.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/D8702DF0-7A27-E611-9E1F-02163E01422C.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/E0118C6A-7B27-E611-9D27-02163E014485.root',
        'root://eoscms.cern.ch//eos/cms/store/data/Run2016B/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/274/198/00000/F68FBFFB-8127-E611-A7A6-02163E013891.root',
        
        ])

# secFiles.extend([
#         'root://eoscms.cern.ch//eoscms//eos/cms/store/data/Run2015B/SingleMuon/RAW/v1/000/251/168/00000/382EE8DB-2825-E511-B3E0-02163E013597.root'
#         ])


process.content = cms.EDAnalyzer("EventContentAnalyzer")
process.dumpED = cms.EDAnalyzer("EventContentAnalyzer")
process.dumpES = cms.EDAnalyzer("PrintEventSetupContent")

# ## EMTF Emulator
process.load('EventFilter.L1TRawToDigi.emtfStage2Digis_cfi')
process.load('L1Trigger.L1TMuonEndCap.simEmtfDigis_cfi') 
process.simEmtfDigis.CSCInput = cms.InputTag('emtfStage2Digis') ## Can also use ('csctfDigis') or ('simCscTriggerPrimitiveDigis', 'MPCSORTED')
process.simEmtfDigis.CSCInputBxShift = cms.untracked.int32(0)
## process.simEmtfDigis.CSCInput = cms.InputTag('csctfDigis') 
process.simEmtfDigis.RPCInput = cms.InputTag('muonRPCDigis') ## Can also use ('csctfDigis') or ('simCscTriggerPrimitiveDigis', 'MPCSORTED')

## NTuplizer
process.ntuple = cms.EDAnalyzer('NTupleMaker',
                                muonsTag     = cms.InputTag("muons"),               ## RECO muons
                                genTag       = cms.InputTag("genParticles"),        ## GEN muons
                                cscSegTag    = cms.InputTag("cscSegments"),         ## RECO CSC segments
                                ## rpcClustTag  = cms.InputTag("muonRPCDigis"),        ## Use these (or RecHits?) like CSC segments
                                csctfTag     = cms.InputTag("csctfDigis"),          ## CSCTF output tracks
                                cscTPTag     = cms.InputTag("csctfDigis"),          ## CSCTF input LCTs
                                emtfTag      = cms.InputTag("simEmtfDigis", ""),    ## EMTF emulator output tracks
                                emtfTPTag    = cms.InputTag("simEmtfDigis", "CSC"), ## EMTF input LCTs
                                emtfTPTagRPC = cms.InputTag("simEmtfDigis", "RPC"), ## EMTF input RPC clusters
                                unp_emtfTag  = cms.InputTag("emtfStage2Digis"),     ## Unpacked EMTF output tracks
                                ## leg_gmtTag   = cms.InputTag("gtDigis"),             ## Legacy GMT tracks (2015 only)
                                isMC         = cms.untracked.int32(0),
                                NoTagAndProbe= cms.untracked.bool(True),
                                printLevel   = cms.untracked.int32(0), 
                                outputDIR   = cms.string('')
                                )

process.L1TMuonSeq = cms.Sequence(
    process.csctfDigis + ## Necessary for legacy studies, or if you use csctfDigis as input
    process.muonRPCDigis +
    ## process.esProd + ## What do we loose by not having this? - AWB 18.04.16
    process.emtfStage2Digis +
    process.simEmtfDigis +
    process.ntuple
    )

process.L1TMuonPath = cms.Path(
    process.L1TMuonSeq
    )

## Output File
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("EMTF_NTuple_ZMu_274198_debug.root")
    )

# outCommands = cms.untracked.vstring('keep *')
outCommands = cms.untracked.vstring(
    'keep *EMTF*_*_*_*',
    'keep *_*_*EMTF*_*',
    'keep *_*csctf*_*_*',
    'keep l1tMuonBXVector_*_*_*',
    'keep l1tRegionalMuonCandBXVector_*_*_*'
    )


process.out = cms.OutputModule("PoolOutputModule", 
                               fileName = cms.untracked.string("EMTF_Tree_ZMu_274198_debug.root"),
                               outputCommands = outCommands
                               )

process.output_step = cms.EndPath(process.out)

process.schedule = cms.Schedule(process.L1TMuonPath)

process.schedule.extend([process.output_step])

## What does this do? Necessary? - AWB 29.04.16
from SLHCUpgradeSimulations.Configuration.muonCustoms import customise_csc_PostLS1
process = customise_csc_PostLS1(process)
