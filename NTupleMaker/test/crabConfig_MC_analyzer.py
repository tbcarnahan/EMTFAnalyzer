# -*- coding: utf-8 -*-
#CRAB configuration to do processing of Monte Carlo. Inputs a dataset from previous step of processing chain.
#Note: Filenames in psetName must match filenames of input dataset.

from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'CRAB3_Mu_FlatPt1to1000-pythia8-gun_NTuples'
config.General.transferLogs = True
config.General.transferOutputs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'Run3_MC_NTuple.py'
config.JobType.allowUndistributedCMSSW = True
#config.JobType.numCores = 2
#config.JobType.maxMemoryMB = 3000   #Use approx (1+1*ncores)GB

#4M OneOverPt 1-10GeV NoPU
#config.Data.inputDataset = '/SingleMu_Run3_Pt1to10OneOverPt_noPU/madecaro-CRAB3_SingleMu_Run3_Pt1to10OneOverPt_noPU_GEN_SIM_DIGI_L1_4M_Run3CCLUTConfig-e5149589855f0dfe291be51e5f0bd796/USER'

#12M 1-1000GeV Flat NoPU
config.Data.inputDataset = '/Mu_FlatPt1to1000-pythia8-gun/madecaro-CRAB3_Mu_FlatPt1to1000-pythia8-gun_MiniAOD-NoPU_110X__DIGI_L1-Run3CCLUT-e5149589855f0dfe291be51e5f0bd796/USER'

config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = True
config.Data.outputDatasetTag = 'CRAB3_SingleMu_1to1000Flat_Run3CCLUT_NTuples'

config.Site.storageSite = 'T3_US_FNALLPC'

 
