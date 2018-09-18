## Before running:
## source /cvmfs/cms.cern.ch/crab3/crab.csh
## voms-proxy-init --voms cms --valid 168:00 (for 7-day validity)
## crab submit -c crab_config_RAW.py

print '\n\nRunning with inputDataset = %s, outputDatasetTag = %s, requestName = %s\n\n' % (MYARGS.inputDataset, MYARGS.outputDatasetTag, MYARGS.outputDatasetTag)

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
# config.General.requestName = 'FlatNtuple_Run_302674_2017_09_27_Nom9'
config.General.requestName = MYARGS.outputDatasetTag
config.General.workArea = 'logs'
config.General.transferOutputs = True  ## Do output root files
config.General.transferLogs = True
# config.General.instance = 'preprod'  ## Fix for CMSSW_10_1_0, obsolete as of Apr. 30 - AWB 05.04.18

config.section_('JobType')
config.JobType.psetName = 'RunTrackFinder_data_NTuple.py'
## Must be the same as the output file in process.TFileService in config.JobType.psetName python file
config.JobType.outputFiles = ['tuple.root']
config.JobType.pluginName = 'Analysis'  ## Is this necessary? - AWB 01.03.16
# config.JobType.maxMemoryMB = 2500

config.section_('Data')
config.Data.inputDBS = 'global'

config.Data.inputDataset = MYARGS.inputDataset

config.Data.runRange = '321988'

config.Data.useParent = True
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob =    1 ## ~1k events per LS, ~1k events / minute
config.Data.totalUnits =  9999
# config.Data.lumiMask = 'data/Run_306091_tracker_on.txt'
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-322057_13TeV_PromptReco_Collisions18_JSON.txt'

config.Data.outLFNDirBase = '/store/user/abrinke1/EMTF/Emulator/ntuples/'
config.Data.publication = False
config.Data.outputDatasetTag = MYARGS.outputDatasetTag

config.section_('User')

config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
