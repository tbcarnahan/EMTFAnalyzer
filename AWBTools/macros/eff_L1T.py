#! /usr/bin/env python

########################################################
## eff_L1T.py   
## A script to find L1T efficiency from L1Ntuples
## By David Curry
##
########################################################

print '------> Setting Environment'

import sys
import math
from ROOT import *
import numpy as np
from array import *
from collections import Counter
from eff_modules import *

## Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]

print '------> Importing Root File'

req_BX0 = True
req_uGMT = False
req_L1_dEta = True
req_HLT = False

# filename = 'root://eoscms.cern.ch//eos/cms/store/user/dcurry/L1T/SingleMuon_2016B_merge_519.root'
# run_str = '_pre_273725'
# filename = '/afs/cern.ch/work/a/abrinke1/public/EMTF/Run2016B/NTuples/2016_05_24/integration-v59p0_273725_SingleMuon/L1Ntuples.root'
filename = '/afs/cern.ch/work/a/abrinke1/public/EMTF/Run2016B/NTuples/2016_05_28/integration-v59p0_273730_SingleMuon/L1Ntuple.root'
run_str = '_273725_273730'

file = TFile.Open(filename)

L1_tree = file.Get("l1UpgradeTree/L1UpgradeTree")
tf_tree = file.Get("l1UpgradeTfMuonTree/L1UpgradeTfMuonTree")
evt_tree  = file.Get("l1EventTree/L1EventTree")
reco_tree = file.Get("l1MuonRecoTree/Muon2RecoTree") 

# trig_WP = 'SingleMu'
trig_WP = 'DoubleMu'
# trig_WP = 'DoubleMuNo7'
# trig_WP = 'DoubleMuNo10'
# trig_WP = 'DoubleMuNo12'
# trig_WP = 'DoubleMuNo1012'
# trig_WP = 'DoubleMuEtaR'
# trig_WP = 'MuOpen'

pt_cut = 10
probe_ptCut = 12
if 'SingleMu' in trig_WP: qual_cut = [12, 13, 14, 15]
if 'DoubleMu' in trig_WP: qual_cut = [8, 9, 10, 11, 12, 13, 14, 15]
if 'DoubleMuNo7' in trig_WP: qual_cut = [8, 9, 10, 12, 13, 14, 15]
if 'DoubleMuNo10' in trig_WP: qual_cut = [8, 9, 11, 12, 13, 14, 15]
if 'DoubleMuNo12' in trig_WP: qual_cut = [9, 10, 11, 12, 13, 14, 15]
if 'DoubleMuNo1012' in trig_WP: qual_cut = [9, 11, 12, 13, 14, 15]
if 'DoubleMuEtaR' in trig_WP: qual_cut = [8, 9, 10, 11, 12, 13, 14, 15]
if 'MuOpen' in trig_WP: qual_cut = [4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]

## Choose level of probe matching

## L1Upgrade__tfMuon.tfMuon.Bx == 0
if req_BX0: BX0_str = '_BX0'
else: BX0_str = ''

## L1Upgrade__tfMuon matches L1Upgrade.muon (Bx, HwQual <--> Qual, HwEta <--> lEta, HwPt <--> lEt) 
if req_uGMT: uGMT_str = '_uGMT'
else: uGMT_str = ''

## abs( (L1Upgrade__tfMuon.tfMuonHwEta*0.010875) - Muon.eta ) < 0.3
if req_L1_dEta: dEta_str = '_dEta'
else: dEta_str = ''

## Muon.hlt_isomu == 1 && Muon.hlt_isoDeltaR < 0.3 for probe.  And Muon.iso < 0.1 && Muon.pt < 27 in denom.
if req_HLT: HLT_str = '_HLT'
else: HLT_str = ''

## Histogram filename
newfile = TFile('plots/L1T_eff_%s_%d%s%s%s%s%s.root' % (trig_WP, pt_cut, run_str, dEta_str, BX0_str, uGMT_str, HLT_str), 'recreate')

count = Counter()

## ================ Histograms ======================
scale_pt_temp = [0, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 18, 20, 22, 25, 30, 35, 45, 60, 75, 100, 140, 150]
scale_pt  = array('f', scale_pt_temp)
max_pt = scale_pt_temp[len(scale_pt_temp) - 1] - 0.01

eta_bins = [100, -2.5, 2.5]

heta = TH1F('heta', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt = TH1F('hpt', '', len(scale_pt_temp)-1,  scale_pt)

heta_trig = TH1F('heta_trig', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_trig = TH1F('hpt_trig', '', len(scale_pt_temp)-1,  scale_pt)

heta_bmtf = TH1F('heta_bmtf', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_bmtf = TH1F('hpt_bmtf', '', len(scale_pt_temp)-1,  scale_pt)
heta_omtf = TH1F('heta_omtf', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_omtf = TH1F('hpt_omtf', '', len(scale_pt_temp)-1,  scale_pt)
heta_emtf = TH1F('heta_emtf', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_emtf = TH1F('hpt_emtf', '', len(scale_pt_temp)-1,  scale_pt)

heta_bmtf_trig = TH1F('heta_bmtf_trig', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_bmtf_trig = TH1F('hpt_bmtf_trig', '', len(scale_pt_temp)-1,  scale_pt)
heta_omtf_trig = TH1F('heta_omtf_trig', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_omtf_trig = TH1F('hpt_omtf_trig', '', len(scale_pt_temp)-1,  scale_pt)
heta_emtf_trig = TH1F('heta_emtf_trig', '', eta_bins[0], eta_bins[1], eta_bins[2])
hpt_emtf_trig = TH1F('hpt_emtf_trig', '', len(scale_pt_temp)-1,  scale_pt)

heta_gmt_DM = TH1F('heta_gmt_DM', '', 131, 99.5, 230.5)
heta_gmt_SM = TH1F('heta_gmt_SM', '', 131, 99.5, 230.5)
heta_gmt_SM_7 = TH1F('heta_gmt_SM_7', '', 131, 99.5, 230.5)
heta_gmt_SM_7_10 = TH1F('heta_gmt_SM_7_10', '', 131, 99.5, 230.5)
heta_gmt_SM_7_12 = TH1F('heta_gmt_SM_7_12', '', 131, 99.5, 230.5)
heta_gmt_etaR = TH1F('heta_gmt_etaR', '', 131, 99.5, 230.5)

hist_list = [ heta, heta_trig, hpt, hpt_trig,\
                  heta_bmtf, heta_bmtf_trig, hpt_bmtf, hpt_bmtf_trig,\
                  heta_omtf, heta_omtf_trig, hpt_omtf, hpt_omtf_trig,\
                  heta_emtf, heta_emtf_trig, hpt_emtf, hpt_emtf_trig ]

## ================================================

# Loop over over events in TFile
for iEvt in range(evt_tree.GetEntries()):

    # ## For testing
    # if iEvt > 10000: break
    if iEvt % 1000 is 0: print 'Event #', iEvt
    
    evt_tree.GetEntry(iEvt)
    if not (evt_tree.Event.run == 273725 or evt_tree.Event.run == 273730):
        continue
    L1_tree.GetEntry(iEvt)
    tf_tree.GetEntry(iEvt)
    reco_tree.GetEntry(iEvt)

    uGMT_tree = L1_tree.L1Upgrade
    bmtf_tree = tf_tree.L1UpgradeBmtfMuon
    omtf_tree = tf_tree.L1UpgradeOmtfMuon
    emtf_tree = tf_tree.L1UpgradeEmtfMuon

    if reco_tree.Muon.nMuons < 2: continue

    # === Look for a tag muon ===
    isTag, isProbe = False, False
    isBMTFtag, isOMTFtagIn, isOMTFtagOut, isEMTFtag = False, False, False, False
    isBMTFprobe, isOMTFprobeIn, isOMTFprobeOut, isEMTFprobe = False, False, False, False
    isTrigger_emtf, isTrigger_omtfIn, isTrigger_omtfOut, isTrigger_bmtf = False, False, False, False

    for iTag in range(reco_tree.Muon.nMuons):

        recoAbsEta = abs(reco_tree.Muon.eta[iTag])
        recoIsMed = np.array([-99], dtype=np.bool)
        recoIsMed = reco_tree.Muon.isMediumMuon[iTag]
        
        if reco_tree.Muon.hlt_isomu[iTag] != 1: continue
        if reco_tree.Muon.hlt_isoDeltaR[iTag] > 0.3: continue
        if not recoIsMed: continue
        if reco_tree.Muon.pt[iTag] < 27: continue
        if reco_tree.Muon.iso[iTag] > 0.1: continue

        if recoAbsEta > 1.0: 
            
            ## Did the tag trigger
            for iTrk in range(emtf_tree.nTfMuons):
                if emtf_tree.tfMuonHwQual[iTrk] < 12: continue
                if (emtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < 15.99: continue
                isBMTFtag = True
                bmtf_iTag = iTag
                
        if recoAbsEta < 1.0: 
            
            for iTrk in range(bmtf_tree.nTfMuons):
                if bmtf_tree.tfMuonHwQual[iTrk] < 12: continue
                if (bmtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < 15.99: continue
                isEMTFtag = True
                emtf_iTag = iTag
                
        if recoAbsEta > 1.4: 
            
            for iTrk in range(emtf_tree.nTfMuons):
                if emtf_tree.tfMuonHwQual[iTrk] < 12: continue
                if (emtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < 15.99: continue
                isOMTFtagIn = True
                omtfIn_iTag = iTag

        if recoAbsEta < 0.6: 
            
            for iTrk in range(bmtf_tree.nTfMuons):
                if bmtf_tree.tfMuonHwQual[iTrk] < 12: continue
                if (bmtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < 15.99: continue
                isOMTFtagOut = True
                omtfOut_iTag = iTag

    for iProbe in range(reco_tree.Muon.nMuons):

        recoEta = reco_tree.Muon.eta[iProbe]
        recoAbsEta = abs(recoEta)
        recoPt = min(reco_tree.Muon.pt[iProbe], max_pt)
        recoIsMed = np.array([-99], dtype=np.bool)
        recoIsMed = reco_tree.Muon.isMediumMuon[iProbe]

        if recoPt < probe_ptCut: continue
        if not recoIsMed: continue
        # if reco_tree.Muon.iso[iProbe] > 0.1: continue

        # ## This cuts both numerator and denominator ... not right - AWB 25.05.16
        # if req_HLT:
        #     if reco_tree.Muon.hlt_isomu[iProbe] != 1 or reco_tree.Muon.hlt_isoDeltaR[iProbe] > 0.3: continue
        #     if recoPt < 27 or reco_tree.Muon.iso[iProbe] > 0.1: continue 

## L1Upgrade__tfMuon matches L1Upgrade.muon (Bx, HwQual <--> Qual, HwEta <--> lEta, HwPt <--> lEt) 

        if isBMTFtag and recoAbsEta < 0.8:
            if isBMTFprobe: continue
            isBMTFprobe = True
            heta.Fill(recoEta)
            hpt.Fill(recoPt)
            heta_bmtf.Fill(recoEta)
            hpt_bmtf.Fill(recoPt)
            heta_omtf.Fill(recoEta)
            if iProbe == bmtf_iTag: 
                print 'Weird error in isBMTFtag: iTag = %d, iProbe = %d' % (chosen_iTag, iProbe)
            
            # If TandP exist look for trigger (fill numerator) 
            for iTrk in range(bmtf_tree.nTfMuons):
                if not bmtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (bmtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(bmtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if req_BX0 and bmtf_tree.tfMuonBx[iTrk] != 0: continue
                
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (bmtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and
                        bmtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        bmtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_trig.Fill(recoEta)
                hpt_trig.Fill(recoPt)
                heta_bmtf_trig.Fill(recoEta)
                hpt_bmtf_trig.Fill(recoPt)
                isTrigger_bmtf = True
                break
                
            ## Also check for OMTFIn tracks
            for iTrk in range(omtf_tree.nTfMuons):
                if not omtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (omtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(omtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if abs(omtf_tree.tfMuonHwEta[iTrk]*0.010875) > 1.0: continue
                if req_BX0 and omtf_tree.tfMuonBx[iTrk] != 0: continue
                
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (omtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and 
                        omtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        omtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_omtf_trig.Fill(recoEta)

                if not isTrigger_bmtf: 
                    heta_trig.Fill(recoEta)
                    hpt_trig.Fill(recoPt)
                    isTrigger_bmtf = True
                break


        if isEMTFtag and recoAbsEta > 1.2: 
            if isEMTFprobe: continue
            isEMTFprobe = True
            heta.Fill(recoEta)
            hpt.Fill(recoPt)
            heta_emtf.Fill(recoEta)
            hpt_emtf.Fill(recoPt)
            heta_omtf.Fill(recoEta)
            if iProbe == emtf_iTag: print 'Weird error in isEMTFtag: iTag = %d, iProbe = %d' % (chosen_iTag, iProbe)

            emtf_trigger = False
            omtf_trigger = False
            for iTrk in range(emtf_tree.nTfMuons):
                if not emtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (emtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(emtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue

                # if 'DoubleMuEtaR' in trig_WP and (emtf_tree.tfMuonHwQual[iTrk] == 8 or emtf_tree.tfMuonHwQual[iTrk] == 10):
                #     if abs(emtf_tree.tfMuonHwEta[iTrk]) < 161 or abs(emtf_tree.tfMuonHwEta[iTrk]) > 169:
                #         if abs(emtf_tree.tfMuonHwEta[iTrk]) < 199 or abs(emtf_tree.tfMuonHwEta[iTrk]) > 202:
                #             continue

                if (not req_BX0) or (emtf_tree.tfMuonBx[iTrk] == 0):
                    if emtf_tree.tfMuonHwQual[iTrk] >= 8:
                        heta_gmt_DM.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    if emtf_tree.tfMuonHwQual[iTrk] >= 12:
                        heta_gmt_SM.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    if emtf_tree.tfMuonHwQual[iTrk] >= 11:
                        heta_gmt_SM_7.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                        heta_gmt_etaR.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    if emtf_tree.tfMuonHwQual[iTrk] >= 11 or emtf_tree.tfMuonHwQual[iTrk] == 10:
                        heta_gmt_SM_7_10.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    if emtf_tree.tfMuonHwQual[iTrk] >= 11 or emtf_tree.tfMuonHwQual[iTrk] == 8:
                        heta_gmt_SM_7_12.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    if emtf_tree.tfMuonHwQual[iTrk] == 10: ## Mode 10
                        if abs(emtf_tree.tfMuonHwEta[iTrk]) > 142 and abs(emtf_tree.tfMuonHwEta[iTrk]) < 149:
                            heta_gmt_etaR.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                        if abs(emtf_tree.tfMuonHwEta[iTrk]) > 159 and abs(emtf_tree.tfMuonHwEta[iTrk]) < 171:
                            heta_gmt_etaR.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    if emtf_tree.tfMuonHwQual[iTrk] == 8: ## Mode 12
                        if abs(emtf_tree.tfMuonHwEta[iTrk]) > 156 and abs(emtf_tree.tfMuonHwEta[iTrk]) < 171:
                            heta_gmt_etaR.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                        if abs(emtf_tree.tfMuonHwEta[iTrk]) > 198 and abs(emtf_tree.tfMuonHwEta[iTrk]) < 203:
                            heta_gmt_etaR.Fill(abs(emtf_tree.tfMuonHwEta[iTrk]))
                    
                if 'DoubleMuEtaR' in trig_WP and emtf_tree.tfMuonHwQual[iTrk] == 10: ## Mode 10
                    if abs(emtf_tree.tfMuonHwEta[iTrk]) < 143 or abs(emtf_tree.tfMuonHwEta[iTrk]) > 148:
                        if abs(emtf_tree.tfMuonHwEta[iTrk]) < 160 or abs(emtf_tree.tfMuonHwEta[iTrk]) > 170:
                            continue

                if 'DoubleMuEtaR' in trig_WP and emtf_tree.tfMuonHwQual[iTrk] == 8: ## Mode 12
                    if abs(emtf_tree.tfMuonHwEta[iTrk]) < 157 or abs(emtf_tree.tfMuonHwEta[iTrk]) > 170:
                        if abs(emtf_tree.tfMuonHwEta[iTrk]) < 199 or abs(emtf_tree.tfMuonHwEta[iTrk]) > 202:
                            continue

                if req_BX0 and emtf_tree.tfMuonBx[iTrk] != 0: continue

                emtf_trigger = True
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (emtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and
                        emtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        emtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_trig.Fill(recoEta)
                hpt_trig.Fill(recoPt)
                heta_emtf_trig.Fill(recoEta)
                hpt_emtf_trig.Fill(recoPt)
                isTrigger_emtf = True
                break

            ## Also check for OMTFOut tracks
            for iTrk in range(omtf_tree.nTfMuons):
                if not omtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (omtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(omtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if abs(omtf_tree.tfMuonHwEta[iTrk]*0.010875) < 1.0: continue
                if req_BX0 and omtf_tree.tfMuonBx[iTrk] != 0: continue

                omtf_trigger = True
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (omtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and 
                        omtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        omtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True

                if req_uGMT and not uGMT_match: continue

                heta_omtf_trig.Fill(recoEta)

                if not isTrigger_emtf:
                    heta_trig.Fill(recoEta)
                    hpt_trig.Fill(recoPt)
                    isTrigger_emtf = True
                break

            # if (emtf_trigger or omtf_trigger) and not isTrigger_emtf:
            #     print '\nIn event %d, EMTF / OMTF / uGMT trigger = %s / %s / %s' % (evt_tree.Event.event, emtf_trigger, omtf_trigger, isTrigger_emtf) 
            #     print '******* EMTF tracks *******'
            #     for iTrk in range(emtf_tree.nTfMuons):
            #         print 'BX %d, HwEta %d, HwPhi %d, HwQual %d, HwPt %d, dEta %.2f' % ( emtf_tree.tfMuonBx[iTrk], emtf_tree.tfMuonHwEta[iTrk],
            #                                                                              emtf_tree.tfMuonHwPhi[iTrk], emtf_tree.tfMuonHwQual[iTrk],
            #                                                                              emtf_tree.tfMuonHwPt[iTrk], emtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta )
            #     print '******* OMTF tracks *******'
            #     for iTrk in range(omtf_tree.nTfMuons):
            #         print 'BX %d, HwEta %d, HwPhi %d, HwQual %d, HwPt %d, dEta %.2f' % ( omtf_tree.tfMuonBx[iTrk], omtf_tree.tfMuonHwEta[iTrk],
            #                                                                              omtf_tree.tfMuonHwPhi[iTrk], omtf_tree.tfMuonHwQual[iTrk],
            #                                                                              omtf_tree.tfMuonHwPt[iTrk], omtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta )
            #     print '******* uGMT tracks *******'
            #     for iTrk in range(uGMT_tree.nMuons):
            #         print 'BX %d, HwEta %d, HwPhi %d, HwQual %d, HwPt %d, dEta %.2f' % ( uGMT_tree.muonBx[iTrk], uGMT_tree.muonIEta[iTrk],
            #                                                                              uGMT_tree.muonIPhi[iTrk], uGMT_tree.muonQual[iTrk],
            #                                                                              uGMT_tree.muonIEt[iTrk], uGMT_tree.muonIEta[iTrk]*0.010875 - recoEta )

        if isOMTFtagIn and recoAbsEta > 0.8 and recoAbsEta < 1.0: 
            if isOMTFprobeIn: continue
            isOMTFprobeIn = True
            heta.Fill(recoEta)
            hpt.Fill(recoPt)
            heta_omtf.Fill(recoEta)
            hpt_omtf.Fill(recoPt)
            heta_bmtf.Fill(recoEta)
            if iProbe == omtfIn_iTag: print 'Weird error in isOMTFtagIn: iTag = %d, iProbe = %d' % (chosen_iTag, iProbe)

            for iTrk in range(omtf_tree.nTfMuons):
                if not omtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (omtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(omtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if req_BX0 and omtf_tree.tfMuonBx[iTrk] != 0: continue
                
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (omtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and 
                        omtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        omtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_trig.Fill(recoEta)
                hpt_trig.Fill(recoPt)
                heta_omtf_trig.Fill(recoEta)
                hpt_omtf_trig.Fill(recoPt)
                isTrigger_omtfIn = True
                break

            ## Also check for BMTF tracks
            for iTrk in range(bmtf_tree.nTfMuons):
                if not bmtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (bmtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(bmtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if req_BX0 and bmtf_tree.tfMuonBx[iTrk] != 0: continue
                
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (bmtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and 
                        bmtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        bmtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_bmtf_trig.Fill(recoEta)

                if not isTrigger_omtfIn:
                    heta_trig.Fill(recoEta)
                    hpt_trig.Fill(recoPt)
                    isTrigger_omtfIn = True
                break

        if isOMTFtagOut and recoAbsEta > 1.0 and recoAbsEta < 1.2: 
            if isOMTFprobeOut: continue
            isOMTFprobeOut = True
            heta.Fill(recoEta)
            hpt.Fill(recoPt)
            heta_omtf.Fill(recoEta)
            hpt_omtf.Fill(recoPt)
            heta_emtf.Fill(recoEta)
            if iProbe == omtfOut_iTag: print 'Weird error in isOMTFtagOut: iTag = %d, iProbe = %d' % (chosen_iTag, iProbe)

            for iTrk in range(omtf_tree.nTfMuons):
                if not omtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (omtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(omtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if req_BX0 and omtf_tree.tfMuonBx[iTrk] != 0: continue
                
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (omtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and 
                        omtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        omtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_trig.Fill(recoEta)
                hpt_trig.Fill(recoPt)
                heta_omtf_trig.Fill(recoEta)
                hpt_omtf_trig.Fill(recoPt)
                isTrigger_omtfOut = True
                break

            ## Also check for EMTF tracks
            for iTrk in range(emtf_tree.nTfMuons):
                if not emtf_tree.tfMuonHwQual[iTrk] in qual_cut: continue
                if (emtf_tree.tfMuonHwPt[iTrk] - 1)*0.5 < pt_cut - 0.01: continue
                if req_L1_dEta and abs(emtf_tree.tfMuonHwEta[iTrk]*0.010875 - recoEta) > 0.3: continue
                if req_BX0 and emtf_tree.tfMuonBx[iTrk] != 0: continue
                
                uGMT_match = False
                for jTrk in range(uGMT_tree.nMuons):
                    if (emtf_tree.tfMuonBx[iTrk] == uGMT_tree.muonBx[jTrk] and 
                        emtf_tree.tfMuonHwEta[iTrk] == uGMT_tree.muonIEta[jTrk] and 
                        emtf_tree.tfMuonHwPt[iTrk] == uGMT_tree.muonIEt[jTrk]): uGMT_match = True
                if req_uGMT and not uGMT_match: continue

                heta_emtf_trig.Fill(recoEta)

                if not isTrigger_omtfOut:
                    heta_trig.Fill(recoEta)
                    hpt_trig.Fill(recoPt)
                    isTrigger_omtfOut = True
                break


# Save the Hists


#c1 = TCanvas('c1')
#c1.cd()
#heta.Draw('AP')

#c2 = TCanvas('c2')
#c2.cd()
#heta_trigger.Draw()

#c3 = TCanvas('c3')
#c3.cd()
#tg_eta = TGraphAsymmErrors(heta_trigger, heta, '')
#tg_eta.Draw()

print '***********************************'
print '*** %s_%d%s%s%s%s efficiency ***' % (trig_WP, pt_cut, dEta_str, BX0_str, uGMT_str, HLT_str)
print '***********************************'
print 'uGMT: %.1f +/- %.1f%%' % (100 * heta_trig.Integral() / heta.Integral(), 
                               (100 * heta_trig.Integral() / heta.Integral()) * math.sqrt(heta.Integral())/heta.Integral() )
print 'BMTF: %.1f +/- %.1f%%' % (100 * hpt_bmtf_trig.Integral() / hpt_bmtf.Integral(), 
                               (100 * hpt_bmtf_trig.Integral() / hpt_bmtf.Integral()) * math.sqrt(hpt_bmtf.Integral())/hpt_bmtf.Integral() )
print 'OMTF: %.1f +/- %.1f%%' % (100 * hpt_omtf_trig.Integral() / hpt_omtf.Integral(), 
                               (100 * hpt_omtf_trig.Integral() / hpt_omtf.Integral()) * math.sqrt(hpt_omtf.Integral())/hpt_omtf.Integral() )
print 'EMTF: %.1f +/- %.1f%%' % (100 * hpt_emtf_trig.Integral() / hpt_emtf.Integral(), 
                               (100 * hpt_emtf_trig.Integral() / hpt_emtf.Integral()) * math.sqrt(hpt_emtf.Integral())/hpt_emtf.Integral() )

newfile.cd()

heta.Write() 
heta_trig.Write() 
hpt.Write() 
hpt_trig.Write()
heta_trig.Divide(heta)
heta_trig.SetName('heta_eff')
heta_trig.Write()
hpt_trig.Divide(hpt)
hpt_trig.SetName('hpt_eff')
hpt_trig.Write()

heta_bmtf.Write() 
heta_bmtf_trig.Write() 
hpt_bmtf.Write() 
hpt_bmtf_trig.Write()
heta_bmtf_trig.Divide(heta_bmtf)
heta_bmtf_trig.SetName('heta_bmtf_eff')
heta_bmtf_trig.Write()
hpt_bmtf_trig.Divide(hpt_bmtf)
hpt_bmtf_trig.SetName('hpt_bmtf_eff')
hpt_bmtf_trig.Write()

heta_omtf.Write() 
heta_omtf_trig.Write() 
hpt_omtf.Write() 
hpt_omtf_trig.Write()
heta_omtf_trig.Divide(heta_omtf)
heta_omtf_trig.SetName('heta_omtf_eff')
heta_omtf_trig.Write()
hpt_omtf_trig.Divide(hpt_omtf)
hpt_omtf_trig.SetName('hpt_omtf_eff')
hpt_omtf_trig.Write()

heta_emtf.Write() 
heta_emtf_trig.Write() 
hpt_emtf.Write() 
hpt_emtf_trig.Write() 
heta_emtf_trig.Divide(heta_emtf)
heta_emtf_trig.SetName('heta_emtf_eff')
heta_emtf_trig.Write()
hpt_emtf_trig.Divide(hpt_emtf)
hpt_emtf_trig.SetName('hpt_emtf_eff')
hpt_emtf_trig.Write()

heta_gmt_DM.Write()   
heta_gmt_SM.Write()   
heta_gmt_SM_7.Write()   
heta_gmt_SM_7_10.Write()
heta_gmt_SM_7_12.Write()
heta_gmt_etaR.Write()   

# for hist in hist_list:
#     if isinstance(hist, list):
#         newfile.mkdir('%s' % hist[0].GetName()).cd()
#         for i in hist: i.Write()
#     else: hist.Write()

del newfile

# Analysis Results
print count


#raw_input('Press return to continue...')
