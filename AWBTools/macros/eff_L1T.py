#! /usr/bin/env python

########################################################
## eff_L1T.py   
## A script to find L1T efficiency from L1Ntuples
## By Andrew Brinkerhoff
##
########################################################

print '------> Setting Environment'

import sys
import math
from subprocess import Popen,PIPE
from ROOT import *
import numpy as np
from array import *
import Helper as h

print '------> Importing Root File'

## Configuration settings
USE_EMUL = False    ## Use emulated L1T muons instead of unpacked
MAX_FILE = 3        ## Maximum number of input files (use "-1" for unlimited)
MAX_EVT  = -1       ## Maximum number of events to process
PRT_EVT  = 1000     ## Print every Nth event

REQ_BX0    = True  ## Require L1T muon to be in BX 0
REQ_uGMT   = True  ## Require a final uGMT candidate, not just a TF muon
REQ_HLT    = True  ## Require tag muon to be matched to unprescaled HLT muon
## REQ_Z      = False ## Require tag and probe muon to satisfy 81 < mass < 101 GeV (not yet implemented - AWB 25.04.2019)

MAX_dR  = 0.4   ## Maximum dR for L1T-offline matching
TAG_ISO = 0.1   ## Maximum relative isolation for tag muon
TAG_PT  = 26    ## Minimum offline pT for tag muon
PRB_PT  = 26    ## Minimum offline pT for probe muon
TRG_PT  = 22    ## Minimum L1T pT for probe muon


## L1NTuple branches
evt_tree  = TChain('l1EventTree/L1EventTree')
reco_tree = TChain('l1MuonRecoTree/Muon2RecoTree')
if not USE_EMUL:
    L1_tree = TChain('l1UpgradeTree/L1UpgradeTree')
    tf_tree = TChain('l1UpgradeTfMuonTree/L1UpgradeTfMuonTree')
else:
    L1_tree = TChain('l1UpgradeEmuTree/L1UpgradeTree')
    tf_tree = TChain('l1UpgradeTfMuonEmuTree/L1UpgradeTfMuonTree')


# dir1 = '/eos/cms/store/group/dpg_trigger/comm_trigger/L1Trigger/safarzad/2017/Collision2017-wRECO-l1t-integration-v96p2_updatedHFSF/'
# dir2 = 'SingleMuon/crab_Collision2017-wRECO-l1t-integration-v96p2_updatedHFSF__SingleMuon/170628_124037/0001/'
# run_str = '_297606'

# dir1 = '/eos/cms/store/user/treis/data/i96p0/SingleMuon/crab_20170622_184540/170622_164616/0000/'
# dir2 = ''
# run_str = '_2017B'

## Full list of 2018 SingleMuon data files: https://indico.cern.ch/event/806643/contributions/3362250
dir1 = '/eos/cms/store/user/arkadios/L1Ntpl/SingleMuon/SingleMuon_2018D_v2_runRange_324729to325172/181129_232407/0000/'
dir2 = ''
run_str = '_2018D'


## Load input files
print dir1+dir2
nFiles = 0
for in_file_name in Popen(['ls', dir1+dir2], stdout=PIPE).communicate()[0].split():
    if not '.root' in in_file_name: continue
    file_name = '%s%s%s' % (dir1, dir2, in_file_name)
    nFiles   += 1
    print '  * Loading file #%d: %s' % (nFiles, in_file_name)
    evt_tree.Add(file_name)
    reco_tree.Add(file_name)
    L1_tree.Add(file_name)
    tf_tree.Add(file_name)
    if nFiles == MAX_FILE: break


## Trigger settings
trig_WP = {}
trig_WP['SingleMu']  = [12, 13, 14, 15]
trig_WP['SingleMu7'] = [11, 12, 13, 14, 15]
trig_WP['DoubleMu']  = [8, 9, 10, 11, 12, 13, 14, 15]
trig_WP['MuOpen']    = [4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]

trig_TF = {}
trig_TF['uGMT'] = [0.00, 2.40]
trig_TF['BMTF'] = [0.00, 0.83]
trig_TF['OMTF'] = [0.83, 1.24]
trig_TF['EMTF'] = [1.24, 2.40]


## Choose level of probe matching

## L1Upgrade__tfMuon.tfMuon.Bx == 0
if REQ_BX0: BX0_str = '_BX0'
else: BX0_str = ''

## L1Upgrade__tfMuon matches L1Upgrade.muon (Bx, HwQual <--> Qual, HwEta <--> lEta, HwPt <--> lEt) 
if REQ_uGMT: uGMT_str = '_uGMT'
else: uGMT_str = ''

## Muon.hlt_isomu == 1 && Muon.hlt_isoDeltaR < 0.1 for probe.  And Muon.iso < 0.1 && Muon.pt < TAG_PT in denom.
if REQ_HLT: HLT_str = '_HLT'
else: HLT_str = ''

## Invariant mass of tag and probe muon pair must be between 81 and 101 GeV
## if REQ_Z: Z_str = '_Zmass'  (not yet implemented - AWB 25.04.2019) 
## else: Z_str = ''
Z_str = ''

## Histogram filename
out_file = TFile('plots/L1T_eff_Pt%d%s%s%s%s%s.root' % (TRG_PT, run_str, BX0_str, uGMT_str, HLT_str, Z_str), 'recreate')


## ================ Histograms ======================
scale_pt_temp = [0, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 18, 20, 22, 25, 30, 35, 45, 60, 75, 100, 140, 150]
scale_pt  = array('f', scale_pt_temp)
max_pt = scale_pt_temp[len(scale_pt_temp) - 1] - 0.01

eta_bins = [100, -2.5, 2.5]
phi_bins = [72, -180, 180]

h_pt  = {}
h_eta = {}
h_phi = {}
h_pt_trg  = {}
h_eta_trg = {}
h_phi_trg = {}
h_eta_trg_vs_PF = {}
h_phi_trg_vs_PF = {}
for TF in trig_TF.keys():
    h_pt [TF] = TH1D('h_pt_%s' % TF,  '', len(scale_pt_temp)-1,  scale_pt)
    h_eta[TF] = TH1D('h_eta_%s' % TF, '', eta_bins[0], eta_bins[1], eta_bins[2])
    h_phi[TF] = TH1D('h_phi_%s' % TF, '', phi_bins[0], phi_bins[1], phi_bins[2])

    for WP in trig_WP.keys():
        key = '%s_%s' % (TF, WP)
        h_pt_trg [key] = TH1D('h_pt_%s' % key,  '', len(scale_pt_temp)-1,  scale_pt)
        h_eta_trg[key] = TH1D('h_eta_%s' % key, '', eta_bins[0], eta_bins[1], eta_bins[2])
        h_phi_trg[key] = TH1D('h_phi_%s' % key, '', phi_bins[0], phi_bins[1], phi_bins[2])

        h_eta_trg_vs_PF[key] = TH2D('h_eta_%s_vs_PF' % key, '', eta_bins[0], eta_bins[1], eta_bins[2], eta_bins[0], eta_bins[1], eta_bins[2])
        h_phi_trg_vs_PF[key] = TH2D('h_phi_%s_vs_PF' % key, '', phi_bins[0], phi_bins[1], phi_bins[2], phi_bins[0], phi_bins[1], phi_bins[2])

h_nProbes_vs_nProbes = TH2D('h_nTags_vs_nProbes', '', 5, -0.5, 4.5, 5, -0.5, 4.5)


## ================================================

# Loop over over events in TFile
for iEvt in range(evt_tree.GetEntries()):
    if MAX_EVT > 0 and iEvt > MAX_EVT: break
    if iEvt % PRT_EVT is 0: print 'Event #', iEvt
    
    evt_tree.GetEntry(iEvt)
    # if not (evt_tree.Event.run == 273725 or evt_tree.Event.run == 273730):
    #     continue
    L1_tree.GetEntry(iEvt)
    tf_tree.GetEntry(iEvt)
    reco_tree.GetEntry(iEvt)

    ## uGMT muon tree
    uGMT_tree = L1_tree.L1Upgrade

    TFs = {}  ## Track-finder trees by name
    TFs['BMTF']  = tf_tree.L1UpgradeBmtfMuon
    TFs['OMTF']  = tf_tree.L1UpgradeOmtfMuon
    TFs['EMTF']  = tf_tree.L1UpgradeEmtfMuon

    if reco_tree.Muon.nMuons < 2: continue

    ## Lists of tag and probe RECO muon indices
    iTags, iProbes = [], []
    ## Lists of track-finder indices matching tag and probe muons
    tfTags, tfProbes, tfTrigs = {}, {}, {}
    for iTF in ['BMTF', 'OMTF', 'EMTF']:
        tfTags[iTF], tfProbes[iTF], tfTrigs[iTF] = {}, {}, {}

    ##########################################################
    ###  Loop over RECO muons to find all valid tag muons  ###
    ##########################################################
    for iTag in range(reco_tree.Muon.nMuons):

        ## Compute tag muon coordinates at 2nd station, require to be valid
        recoEta = reco_tree.Muon.etaSt2[iTag]
        recoPhi = reco_tree.Muon.phiSt2[iTag]
        if (recoEta < -99 or recoPhi < -99): continue

        recoAbsEta = abs(recoEta)
        recoIsMed = np.array([-99], dtype=np.bool)
        recoIsMed = reco_tree.Muon.isMediumMuon[iTag]

        ## Require tag muon to pass Muon POG medium ID
        if not recoIsMed: continue
        ## Require tag muon to be matched to HLT single muon trigger
        if REQ_HLT and reco_tree.Muon.hlt_isomu[iTag]    != 1:   continue
        if REQ_HLT and reco_tree.Muon.hlt_isoDeltaR[iTag] > 0.1: continue
        ## Require tag muon to pass pT and isolation cuts
        if reco_tree.Muon.pt[iTag]  < TAG_PT:  continue
        if reco_tree.Muon.iso[iTag] > TAG_ISO: continue

        ## Find track-finder muons which could be matched to the tag muon

        ## Loop over all tracks in each track-finder
        for iTF in ['BMTF', 'OMTF', 'EMTF']:
            ## Loop over the tracks in each track-finder
            for iTrk in range(TFs[iTF].nTfMuons):
                ## Require track-finder to fire with SingleMuon quality (>= 12)
                if not TFs[iTF].tfMuonHwQual[iTrk] in trig_WP['SingleMu']: continue
                ## Require track-finder pT to be >= offline cut - 4 GeV
                if    (TFs[iTF].tfMuonHwPt[iTrk] - 1)*0.5 < TAG_PT - 4.01: continue
                ## Scaling from hardware eta to integer value
                trk_eta = TFs[iTF].tfMuonHwEta[iTrk]*0.010875
                ## Scaling from degrees to radians
                trk_phi = TFs[iTF].tfMuonGlobalPhi[iTrk]*3.14159/180.
                ## For some reason, it seems this is filled in a buggy way and needs to be rescaled - AWB 22.04.2019
                trk_phi *= (1.0/1.6)
                ## Wrap-around values > pi
                if (trk_phi > 3.14159): trk_phi -= 2*3.14159
                ## Require track with be within specified dR of RECO muon
                if h.CalcDR( trk_eta, trk_phi, recoEta, recoPhi ) > MAX_dR: continue

                ## If all requirements are met, we have an L1 track for this tag muon
                if not iTag in iTags:
                    iTags.append(iTag)
                if not iTag in tfTags[iTF].keys():
                    tfTags[iTF][iTag] = [iTrk]
                else:
                    tfTags[iTF][iTag].append(iTrk)

    ## End loop: for iTag in range(reco_tree.Muon.nMuons):


    ## Quit the event if there are no tag muons
    if len(iTags) == 0: continue


    ############################################################
    ###  Loop over RECO muons to find all valid probe muons  ###
    ############################################################
    for iProbe in range(reco_tree.Muon.nMuons):

        ## Compute tag muon coordinates at 2nd station, require to be valid
        recoEta = reco_tree.Muon.etaSt2[iProbe]
        recoPhi = reco_tree.Muon.phiSt2[iProbe]
        if (recoEta < -99 or recoPhi < -99): continue

        recoPhiDeg = recoPhi * 180. / 3.14159
        recoAbsEta = abs(recoEta)
        recoPt = min(reco_tree.Muon.pt[iProbe], max_pt)
        recoIsMed = np.array([-99], dtype=np.bool)
        recoIsMed = reco_tree.Muon.isMediumMuon[iProbe]

        ## Require probe muon to pass Muon POG medium ID
        if not recoIsMed:   continue
        ## Require probe muon to pass minimum pT cut
        if recoPt < PRB_PT: continue

        ## Try to find at least on valid tag muon for this probe
        xTag = -1
        ## Loop over tag muon candidates
        for iTag in iTags:
            ## Make sure tag and probe are not the same muon
            if iTag == iProbe: continue
            ## Require tag and probe muons to be well separated
            tagEta = reco_tree.Muon.etaSt2[iTag]
            tagPhi = reco_tree.Muon.phiSt2[iTag]
            if h.CalcDR( tagEta, tagPhi, recoEta, recoPhi ) < 2*MAX_dR: continue
            ## If tag passes requirements, store its index and quit the loop
            xTag = iTag
            break
        ## End loop: for iTag in iTags

        ## Valid probe only if there is a corresponding tag
        if xTag < 0: continue
        else: iProbes.append(iProbe)

        ## Fill denominator distributions for the probe muon in uGMT
        h_pt ['uGMT'].Fill(recoPt)
        h_eta['uGMT'].Fill(recoEta)
        h_phi['uGMT'].Fill(recoPhiDeg)

        ## Count number of fired triggers from each track-finder, and the uGMT
        nTrig = {}
        for iTF in ['uGMT', 'BMTF', 'OMTF', 'EMTF']:
            nTrig[iTF] = {}
            for WP in trig_WP.keys():
                nTrig[iTF][WP] = 0

        ## Mapping from L1Upgrade__tfMuon to L1Upgrade.muon : Bx <--> Bx, HwQual <--> Qual, HwEta <--> lEta, HwPt <--> lEt

        ## Loop over track-finder regions for efficiency plots
        for iTF in ['BMTF', 'OMTF', 'EMTF']:

            ## Fill denominator distribution for eta for all track-finders
            h_eta[iTF].Fill(recoEta)
            ## For other distributions, require RECO muon to fall in correct eta range
            if recoAbsEta > trig_TF[iTF][0] and recoAbsEta < trig_TF[iTF][1]:
                h_pt [iTF].Fill(recoPt)
                h_phi[iTF].Fill(recoPhiDeg)
            ## If muon is not in track-finder eta range, skip loop over tracks
            else: continue


            ## Loop over tracks in all track-finders, to handle overlap regions
            for jTF in ['BMTF', 'OMTF', 'EMTF']:
                for jTrk in range(TFs[jTF].nTfMuons):

                    ## Global eta and phi coordinates from track-finder hardware values
                    ## Scaling from hardware eta to integer value
                    trk_eta = TFs[jTF].tfMuonHwEta[jTrk]*0.010875
                    ## Scaling from degrees to radians
                    trk_phi = TFs[jTF].tfMuonGlobalPhi[jTrk]*3.14159/180.
                    ## For some reason, it seems this is filled in a buggy way and needs to be rescaled - AWB 22.04.2019
                    trk_phi *= (1.0/1.6)
                    ## Wrap-around values > pi
                    if (trk_phi > 3.14159): trk_phi -= 2*3.14159

                    ## Require track with be within specified dR of RECO muon
                    if h.CalcDR( trk_eta, trk_phi, recoEta, recoPhi ) > MAX_dR: continue
                    ## Require track to be in the specified bunch crossing
                    if REQ_BX0 and TFs[jTF].tfMuonBx[jTrk] != 0: continue

                    uGMT_match = False
                    ## Loop over uGMT muons to find a match
                    for kTrk in range(uGMT_tree.nMuons):
                        if ( TFs[jTF].tfMuonBx   [jTrk] == uGMT_tree.muonBx  [kTrk] and
                             TFs[jTF].tfMuonHwEta[jTrk] == uGMT_tree.muonIEta[kTrk] and
                             TFs[jTF].tfMuonHwPt [jTrk] == uGMT_tree.muonIEt [kTrk] ): uGMT_match = True
                            # print 'TF eta = %.3f, phi = %.3f' % (TFs[jTF].tfMuonHwEta[jTrk]*0.010875, TFs[jTF].tfMuonGlobalPhi[jTrk]*3.14159/180.)
                            # print 'MT eta = %.3f, phi = %.3f' % (uGMT_tree.muonEta[kTrk], uGMT_tree.muonPhi[kTrk])
                            # if (uGMT_tree.muonPhi[kTrk] > 0): print 'TF / MT ratio = %.4f' % (TFs[jTF].tfMuonGlobalPhi[jTrk]*(3.14159/180.) / uGMT_tree.muonPhi[kTrk])
                    ## Require track-finder track to be matched to a uGMT track
                    if REQ_uGMT and not uGMT_match: continue


                    ## Loop over trigger working-points for efficiency measurements
                    for WP in trig_WP.keys():

                        ## Require track to pass quality cut for working-point
                        if not TFs[jTF].tfMuonHwQual[jTrk] in trig_WP[WP]: continue
                        ## Require track to pass pT cut for working-point
                        if    (TFs[jTF].tfMuonHwPt[jTrk] - 1)*0.5 < TRG_PT - 0.01: continue

                        ## Count the tracks matching this probe muon and passing this working point
                        nTrig['uGMT'][WP] += 1
                        nTrig   [jTF][WP] += 1

                        ## Don't need to fill any plots if we already found a matching track in this track-finder
                        if nTrig[jTF][WP] > 1: continue

                        ## Fill numerator distribution for eta
                        h_eta_trg      ['%s_%s' % (jTF, WP)].Fill(recoEta)
                        h_eta_trg_vs_PF['%s_%s' % (jTF, WP)].Fill(recoEta, trk_eta)

                        ## For the remainder of the plots, only fill them if we haven't already
                        if nTrig['uGMT'][WP] > 1: continue

                        h_pt_trg       ['%s_%s' % (iTF, WP)].Fill(recoPt)
                        h_phi_trg      ['%s_%s' % (iTF, WP)].Fill(recoPhiDeg)
                        h_phi_trg_vs_PF['%s_%s' % (iTF, WP)].Fill(recoPhiDeg, trk_phi*180/3.14159)

                        h_pt_trg       ['uGMT_%s' % WP ].Fill(recoPt)
                        h_eta_trg      ['uGMT_%s' % WP ].Fill(recoEta)
                        h_phi_trg      ['uGMT_%s' % WP ].Fill(recoPhiDeg)
                        h_eta_trg_vs_PF['uGMT_%s' % WP ].Fill(recoEta, trk_eta)
                        h_phi_trg_vs_PF['uGMT_%s' % WP ].Fill(recoPhiDeg, trk_phi*180/3.14159)

                    ## End loop: for WP in trig_WP.keys()
                ## End loop: for jTrk in range(TFs[jTF].nTfMuons)
            ## End loop: for jTF in ['BMTF', 'OMTF', 'EMTF']:
        ## End loop: for iTF in ['BMTF', 'OMTF', 'EMTF']:
    ## End loop: for iProbe in range(reco_tree.Muon.nMuons):

    h_nTags_vs_nProbes.Fill(len(iProbes), len(iTags))

## End loop: for iEvt in range(evt_tree.GetEntries()):



############################################################
###  Write output file with histograms and efficiencies  ###
############################################################

out_file.cd()

nWP = 0
for WP in trig_WP.keys():
    nWP += 1
    print '\n***********************************'
    print '*** %s_%d%s%s%s%s efficiency ***' % (WP, TRG_PT, BX0_str, uGMT_str, HLT_str, Z_str)
    print '***********************************'
    for TF in trig_TF.keys():
        key = '%s_%s' % (TF, WP)
        print '%s: %.1f +/- %.1f%%' % ( TF, 100 * h_phi_trg[key].Integral() / h_phi[TF].Integral(), 
                                        (100 * h_phi_trg[key].Integral() / h_phi[TF].Integral()) * 
                                        math.sqrt(h_phi[TF].Integral()) / h_phi[TF].Integral() )

        if nWP == 1: h_pt[TF].Write() 
        h_pt_trg[key].Write() 
        h_pt_trg[key].Divide(h_pt[TF])
        h_pt_trg[key].SetName(h_pt_trg[key].GetName()+'_eff')
        h_pt_trg[key].Write()

        if nWP == 1: h_eta[TF].Write() 
        h_eta_trg[key].Write() 
        h_eta_trg[key].Divide(h_eta[TF])
        h_eta_trg[key].SetName(h_eta_trg[key].GetName()+'_eff')
        h_eta_trg[key].Write()

        if nWP == 1: h_phi[TF].Write() 
        h_phi_trg[key].Write() 
        h_phi_trg[key].Divide(h_phi[TF])
        h_phi_trg[key].SetName(h_phi_trg[key].GetName()+'_eff')
        h_phi_trg[key].Write()

        h_eta_trg_vs_PF[key].Write()
        h_phi_trg_vs_PF[key].Write()

    ## End loop: for TF in trig_TF.keys()
## End loop: for WP in trig_WP.keys()

h_nTags_vs_nProbes.Write()


del out_file
