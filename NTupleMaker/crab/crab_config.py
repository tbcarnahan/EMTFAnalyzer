## Before running:
## source /cvmfs/cms.cern.ch/crab3/crab.csh
## voms-proxy-init --voms cms --valid 168:00 (for 7-day validity)
## crab submit -c crab_config.py
## crab status -d logs/crab_EMTF_MuGun_2016_12_13_v1/

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
# config.General.requestName = 'EMTF_MuGun_2017_02_14'
config.General.requestName = 'EMTF_JPsi_2017_09_04'
# config.General.requestName = 'EMTF_ZeroBias0_2017_02_14_slim'
config.General.workArea = 'logs'
config.General.transferOutputs = True  ## Do output root files
config.General.transferLogs = True
# config.General.instance = 'preprod'  ## Fix for CMSSW_10_1_0, obsolete as of Apr. 30 - AWB 05.04.18

config.section_('JobType')
config.JobType.psetName = 'RunTrackFinder_MC_NTuple.py'
# config.JobType.psetName = 'RunTrackFinder_data_NTuple.py'
## Must be the same as the output file in process.TFileService in config.JobType.psetName python file
config.JobType.outputFiles = ['tuple.root']
config.JobType.pluginName = 'Analysis'  ## Is this necessary? - AWB 01.03.16

config.section_('Data')
config.Data.inputDBS = 'global'

# config.Data.inputDataset = '/SingleMu_Pt1To1000_FlatRandomOneOverPt/RunIISpring16DR80-NoPURAW_NZS_withHLT_80X_mcRun2_asymptotic_v14-v1/GEN-SIM-RAW'
config.Data.inputDataset = '/JPsiToMuMu_Pt20to120_EtaPhiRestricted-pythia8-gun/RunIISpring16DR80-NoPURAW_NZS_withHLT_80X_mcRun2_asymptotic_v14-v1/GEN-SIM-RAW'
# config.Data.inputDataset = '/ZeroBiasIsolatedBunch0/Run2016H-v1/RAW'

config.Data.useParent = False
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob =  5  ## Should take 2 - 4 hours  ##  10
config.Data.totalUnits = 1400 ## 1369 files in total      ## 1400
# config.Data.splitting = 'LumiBased'
# config.Data.unitsPerJob =  20 ## ~15k events per LS, ~450k events / hour
# config.Data.totalUnits = 1000 ## ~9k mode 15 tracks with pT > 30 GeV

config.Data.outLFNDirBase = '/store/user/abrinke1/EMTF/Emulator/ntuples/'
config.Data.publication = False
config.Data.outputDatasetTag = 'NoRPC_NewPatt_NewMatch_NewGhost_v3'
# config.Data.outputDatasetTag = 'RPC'
# config.Data.outputDatasetTag = 'Slim'
# config.Data.outputDatasetTag = 'Slim_RPC'

config.section_('User')

config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
