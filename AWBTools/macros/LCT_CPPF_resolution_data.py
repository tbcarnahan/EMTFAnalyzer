#! /usr/bin/env python

import os
from ROOT import *
gROOT.SetBatch(True)

MAX_FILE = 20
MAX_EVT  = 10000000
PRT_EVT  = 1000

LABEL = 'ph_lut_v2_coord'
# LABEL = '102X_ReReco_v1_321988_coord'
# LABEL = '102X_ReReco_v1_321988_bugFix'

def main():

###################
## Initialize files
###################

    file_names = []
    store  = 'root://eoscms.cern.ch//store/user/abrinke1/EMTF/Emulator/ntuples/'
    in_dir = 'SingleMuon/FlatNtuple_Run_2018D_v1_2018_09_18_SingleMuon_2018_emul_%s/' % LABEL
    # in_dir = 'SingleMuon/FlatNtuple_Run_2018D_v1_2018_09_19_SingleMuon_2018_emul_%s/' % LABEL
    for i in range(MAX_FILE):
        file_names.append(store+in_dir+'NTuple_%d.root' % i)
        print 'Opening file: '+file_names[i]
        
    in_chains = []
    for i in range(len(file_names)):
        in_chains.append( TChain('FlatNtupleData/tree') )
        in_chains[i].Add( file_names[i] )
        
    out_file = TFile('plots/LCT_CPPF_resolution_data_%s_%dk.root'  % (LABEL, (MAX_EVT / 1000)), 'recreate')
    png_dir  = 'plots/png/LCT_CPPF_resolution_data_%s_%dk/' % (LABEL, (MAX_EVT / 1000))
    # out_file = TFile('plots/LCT_CPPF_resolution_data_seg_%s_%dk.root'  % (LABEL, (MAX_EVT / 1000)), 'recreate')
    # png_dir  = 'plots/png/LCT_CPPF_resolution_data_seg_%s_%dk/' % (LABEL, (MAX_EVT / 1000))
    # out_file = TFile('plots/LCT_CPPF_resolution_data_sim_%s_%dk.root'  % (LABEL, (MAX_EVT / 1000)), 'recreate')
    # png_dir  = 'plots/png/LCT_CPPF_resolution_data_sim_%s_%dk/' % (LABEL, (MAX_EVT / 1000))

    try:
        os.makedirs(png_dir)
    except:
        print '%s already exists' % png_dir

    
#############
## Histograms
#############

    # ph_bins = [5401, -0.5, 5400.5]
    th_bins = [ 128, -0.5,  128.5]

    dPh_bins = [131, -65.5, 65.5]
    dTh_bCSC = [ 19,  -9.5,  9.5]
    dTh_bRPC = [ 35, -17.5, 17.5]

    ## [station,ring] for CSC and [station,ring,roll] for RPC chambers to compare
    CSC_CSC = [ [[1,1],[2,1]], [[2,1],[3,1]], [[3,1],[4,1]],
                [[1,1],[3,1]], [[1,1],[4,1]], [[2,1],[4,1]],
                [[1,2],[2,2]], [[2,2],[3,2]], [[3,2],[4,2]],
                [[1,2],[3,2]], [[1,2],[4,2]], [[2,2],[4,2]],
                [[1,3],[2,2]], [[1,4],[2,1]],
                [[1,3],[3,2]], [[1,4],[3,1]], [[1,4],[4,1]] ]
    CSC_RPC = [ [[1,2],[1,2,1]], [[2,2,1],[2,2,1]],
                [[3,2],[3,2,1]], [[4,2,1],[4,2,1]],
                [[3,2],[3,3,1]], [[4,2,1],[4,3,1]],
                [[1,2],[1,2,2]], [[2,2],[2,2,2]],
                [[3,2],[3,2,2]], [[4,2],[4,2,2]],
                [[3,2],[3,3,2]], [[4,2],[4,3,2]],
                [[1,2],[1,2,3]], [[2,2],[2,2,3]],
                [[3,2],[3,2,3]], [[4,2],[4,2,3]], ## Compare RE3/2/3 to ME3/2 and RE4/2/3 to ME4/2
                [[2,1],[3,2,3]], [[3,1],[4,2,3]], ## Compare RE3/2/3 to ME2/1 and RE4/2/3 to ME3/1
                [[3,2],[3,3,3]], [[4,2],[4,3,3]] ]

    h_CSC_dPh   = {} ## CSC LCT phi resolution
    h_CSC_dTh   = {} ## CSC LCT theta resolution
    h_RPC_dPh_d = {} ## Unpacked CPPF digis in data
    h_RPC_dTh_d = {}
    h_RPC_dPh_e = {} ## Emulated CPPF digis from RPC hits
    h_RPC_dTh_e = {}

    # h_CSC_ph   = {} ## CSC LCT phi 2D
    # h_CSC_th   = {} ## CSC LCT theta 2D
    # h_RPC_ph_d = {} ## Unpacked CPPF digis in data
    # h_RPC_th_d = {}
    # h_RPC_ph_e = {} ## Emulated CPPF digis from RPC hits
    # h_RPC_th_e = {}

    for end in [['p','+'], ['m','-']]: ## Positive and negative endcaps
        for SR in CSC_CSC:
            SR_ID = '%d_%d_%d_%d' % (SR[0][0], SR[0][1], SR[1][0], SR[1][1])

            h_CSC_dTh['%s%s' % (end[0], SR_ID)] = TH1F( 'h_CSC%s_dTh_%s' % (end[0], SR_ID),
                                                        'CSC%s LCT #Delta#theta(%d/%d - %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                        dTh_bCSC[0], dTh_bCSC[1], dTh_bCSC[2] )
            h_CSC_dPh['%s%s' % (end[0], SR_ID)] = TH1F( 'h_CSC%s_dPh_%s' % (end[0], SR_ID),
                                                        'CSC%s LCT #Delta#phi(%d/%d - %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                        dPh_bins[0], dPh_bins[1], dPh_bins[2] )

            # h_CSC_th['%s%s' % (end[0], SR_ID)] = TH2F( 'h_CSC%s_th_%s' % (end[0], SR_ID),
            #                                            'CSC%s LCT #theta : %d/%d vs. %d/%d' % (end[1], SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
            #                                            th_bins[0], th_bins[1], th_bins[2], th_bins[0], th_bins[1], th_bins[2] )
            # h_CSC_ph['%s%s' % (end[0], SR_ID)] = TH2F( 'h_CSC%s_ph_%s' % (end[0], SR_ID),
            #                                            'CSC%s LCT #phi : %d/%d vs. %d/%d' % (end[1], SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
            #                                            ph_bins[0], ph_bins[1], ph_bins[2], ph_bins[0], ph_bins[1], ph_bins[2] )

            
        for SR in CSC_RPC:
            SR_ID = '%d_%d_%d_%d_%d' % (SR[0][0], SR[0][1], SR[1][0], SR[1][1], SR[1][2])

            h_RPC_dTh_d['%s%s' % (end[0], SR_ID)] = TH1F( 'h_RPC%s_dTh_%s_data' % (end[0], SR_ID),
                                                          'Unpacked CPPF%s digi #Delta#theta(RPC %d/%d/%d - CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
                                                          dTh_bRPC[0], dTh_bRPC[1], dTh_bRPC[2] )
            h_RPC_dPh_d['%s%s' % (end[0], SR_ID)] = TH1F( 'h_RPC%s_dPh_%s_data' % (end[0], SR_ID),
                                                          'Unpacked CPPF%s digi #Delta#phi(RPC %d/%d/%d - CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
                                                          dPh_bins[0], dPh_bins[1], dPh_bins[2] )
            h_RPC_dTh_e['%s%s' % (end[0], SR_ID)] = TH1F( 'h_RPC%s_dTh_%s_emul' % (end[0], SR_ID),
                                                          'Emulated CPPF%s digi #Delta#theta(RPC %d/%d/%d - CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
                                                          dTh_bRPC[0], dTh_bRPC[1], dTh_bRPC[2] )
            h_RPC_dPh_e['%s%s' % (end[0], SR_ID)] = TH1F( 'h_RPC%s_dPh_%s_emul' % (end[0], SR_ID),
                                                          'Emulated CPPF%s digi #Delta#phi(RPC %d/%d/%d - CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
                                                          dPh_bins[0], dPh_bins[1], dPh_bins[2] )

            # h_RPC_th_d['%s%s' % (end[0], SR_ID)] = TH2F( 'h_RPC%s_th_%s_data' % (end[0], SR_ID),
            #                                              'Unpacked CPPF%s digi #theta : RPC %d/%d/%d vs. CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
            #                                              th_bins[0], th_bins[1], th_bins[2], th_bins[0], th_bins[1], th_bins[2] )
            # h_RPC_th_e['%s%s' % (end[0], SR_ID)] = TH2F( 'h_RPC%s_th_%s_emul' % (end[0], SR_ID),
            #                                              'Emulated CPPF%s digi #theta : RPC %d/%d/%d vs. CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
            #                                              th_bins[0], th_bins[1], th_bins[2], th_bins[0], th_bins[1], th_bins[2] )
            # h_RPC_ph_d['%s%s' % (end[0], SR_ID)] = TH2F( 'h_RPC%s_ph_%s_data' % (end[0], SR_ID),
            #                                              'Unpacked CPPF%s digi #phi : RPC %d/%d/%d vs. CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
            #                                              ph_bins[0], ph_bins[1], ph_bins[2], ph_bins[0], ph_bins[1], ph_bins[2] )
            # h_RPC_ph_e['%s%s' % (end[0], SR_ID)] = TH2F( 'h_RPC%s_ph_%s_emul' % (end[0], SR_ID),
            #                                              'Emulated CPPF%s digi #phi : RPC %d/%d/%d vs. CSC %d/%d)' % (end[1], SR[1][0], SR[1][1], SR[1][2], SR[0][0], SR[0][1]),
            #                                              ph_bins[0], ph_bins[1], ph_bins[2], ph_bins[0], ph_bins[1], ph_bins[2] )


#############
## Event loop
#############
                
    iEvt = -1
    for ch in in_chains:
        
        if iEvt > MAX_EVT: break
                
        for jEvt in range(ch.GetEntries()):
            iEvt += 1
            
            if iEvt > MAX_EVT: break
            if iEvt % PRT_EVT is 0: print 'Event #', iEvt

            ch.GetEntry(jEvt)

            nReco = int(ch.nRecoMuons)

            ## Loop over offline RECO muons
            for iReco in range(nReco):
                
                ## Require a good-quality, high-pT muon with tag-and-probe selection,
                ##   uniquely matched to an EMTF track
                if (ch.reco_ID_medium          [iReco] != 1): continue
                if (ch.reco_pt                 [iReco] < 40): continue
                # if (ch.reco_trig_ID            [iReco] >= 1
                #     and ch.nRecoMuonsTrig              <= 1): continue
                if (ch.reco_dR_match_emu_unique[iReco] != 1): continue

                iTrk = ch.reco_dR_match_emu_iTrk[iReco]
                if (ch.trk_BX[iTrk] != 0): continue

                nHits    = ch.nHits
                nSimHits = ch.nSimHits
                nTrkHits = ch.trk_nHits[iTrk]

                ## Loop over CSC LCTs in the track, requiring BX = 0
                for iTrkHit in range(nTrkHits):
                    iHit = ch.trk_iHit[iTrk][iTrkHit]
                    if (ch.hit_BX   [iHit] != 0): continue
                    if (ch.hit_isCSC[iHit] != 1): continue
                    if (ch.hit_match_iSeg[iHit] < 0): continue

                    iSect = ch.hit_sector_index[iHit]
                    iStat = ch.hit_station     [iHit]
                    iRing = ch.hit_ring        [iHit]
                    iCham = ch.hit_chamber     [iHit]
                    iPh   = ch.hit_phi_int     [iHit]
                    iTh   = ch.hit_theta_int   [iHit]
                    # iPh   = ch.seg_phi[ch.hit_match_iSeg[iHit]] * 60
                    # iTh   = ch.seg_theta[ch.hit_match_iSeg[iHit]] * 16
                    # iPh   = ch.hit_phi_sim     [iHit] * 60
                    # iTh   = ch.hit_theta_sim   [iHit] * 16

                    ## Loop over all unpacked hits in the event in the corresponding chamber
                    for jHit in range(nHits):
                        if (iHit == jHit): continue
                        if (ch.hit_BX[jHit] != 0 and ch.hit_BX[jHit] != -1): continue ## Unpacked CPPF digis may be 1 BX early
                        if (ch.hit_match_iSeg[jHit] < 0): continue

                        jSect = ch.hit_sector_index[jHit]
                        jStat = ch.hit_station     [jHit]
                        jRing = ch.hit_ring        [jHit]
                        jRoll = ch.hit_roll        [jHit]
                        jCham = ch.hit_chamber     [jHit]
                        jPh   = ch.hit_phi_int     [jHit]
                        jTh   = ch.hit_theta_int   [jHit]
                        # jPh   = ch.seg_phi[ch.hit_match_iSeg[jHit]] * 60
                        # jTh   = ch.seg_theta[ch.hit_match_iSeg[jHit]] * 16
                        # jPh   = ch.hit_phi_sim     [jHit] * 60
                        # jTh   = ch.hit_theta_sim   [jHit] * 16

                        if (ch.hit_endcap[jHit] == 1): end = 'p'
                        else:                          end = 'm'

                        ## Require same EMTF sector and overlapping chamber
                        if (jSect != iSect): continue
                        if (iStat == 1 and (iRing % 3) == 1): ## ME1/1 and ME1/4 have twice as many chambers as ME2/1
                            if (jCham != (iCham + 1) / 2): continue
                        elif (jStat >= 3 and jRing == 2 and jRoll == 3): ## RE3/2/3 and RE4/2/3 have twice as many chambers as ME3/1 and ME4/1
                            if (jCham != (iCham + 1) / 2): continue
                        else:
                            if (jCham != iCham): continue

                        if (ch.hit_isCSC[jHit] == 1 and ch.hit_BX[jHit] == 0):
                            for SR in CSC_CSC:
                                SR_ID = '%s%d_%d_%d_%d' % (end, SR[0][0], SR[0][1], SR[1][0], SR[1][1])
                                if (iStat == SR[0][0] and iRing == SR[0][1] and jStat == SR[1][0] and jRing == SR[1][1]):

                                    h_CSC_dPh[SR_ID].Fill( max(dPh_bins[1]+0.1, min(dPh_bins[2]-0.1, jPh - iPh) ) )
                                    h_CSC_dTh[SR_ID].Fill( max(dTh_bRPC[1]+0.1, min(dTh_bRPC[2]-0.1, jTh - iTh) ) )

                                    # h_CSC_ph[SR_ID].Fill( iPh, jPh )
                                    # h_CSC_th[SR_ID].Fill( iTh, jTh )

                        if (ch.hit_isRPC[jHit] == 1):
                            for SR in CSC_RPC:
                                SR_ID = '%s%d_%d_%d_%d_%d' % (end, SR[0][0], SR[0][1], SR[1][0], SR[1][1], SR[1][2])

                                if (iStat == SR[0][0] and iRing == SR[0][1] and jStat == SR[1][0] and jRing == SR[1][1] and jRoll == SR[1][2]):

                                    h_RPC_dPh_d[SR_ID].Fill( max(dPh_bins[1]+0.1, min(dPh_bins[2]-0.1, jPh - iPh) ) )
                                    h_RPC_dTh_d[SR_ID].Fill( max(dTh_bRPC[1]+0.1, min(dTh_bRPC[2]-0.1, jTh - iTh) ) )

                                    # h_RPC_ph_d[SR_ID].Fill( iPh, jPh )
                                    # h_RPC_th_d[SR_ID].Fill( iTh, jTh )

                    ## End loop over all unpacked hits in event (jHit)

                    ## Loop over all emulated hits in the event in the corresponding chamber
                    for kHit in range(nSimHits):
                        if (iHit == kHit): continue
                        if (ch.sim_hit_BX[kHit] != 0): continue

                        kSect = ch.sim_hit_sector_index[kHit]
                        kStat = ch.sim_hit_station     [kHit]
                        kRing = ch.sim_hit_ring        [kHit]
                        kRoll = ch.sim_hit_roll        [kHit]
                        kCham = ch.sim_hit_chamber     [kHit]
                        kPh   = ch.sim_hit_phi_int     [kHit]
                        kTh   = ch.sim_hit_theta_int   [kHit]

                        if (ch.sim_hit_endcap[kHit] == 1): end = 'p'
                        else:                              end = 'm'

                        ## Require same EMTF sector and chamber
                        if (kSect != iSect): continue
                        if (kStat >= 3 and kRing == 2 and kRoll == 3): ## RE3/2/3 and RE4/2/3 have twice as many chambers as ME3/1 and ME4/1
                            if (kCham != (iCham + 1) / 2): continue
                        else:
                            if (kCham != iCham): continue

                        if (ch.sim_hit_isRPC[kHit] == 1):
                            for SR in CSC_RPC:
                                SR_ID = '%s%d_%d_%d_%d_%d' % (end, SR[0][0], SR[0][1], SR[1][0], SR[1][1], SR[1][2])

                                if (iStat == SR[0][0] and iRing == SR[0][1] and kStat == SR[1][0] and kRing == SR[1][1] and kRoll == SR[1][2]):

                                    h_RPC_dPh_e[SR_ID].Fill( max(dPh_bins[1]+0.1, min(dPh_bins[2]-0.1, kPh - iPh) ) )
                                    h_RPC_dTh_e[SR_ID].Fill( max(dTh_bRPC[1]+0.1, min(dTh_bRPC[2]-0.1, kTh - iTh) ) )

                                    # h_RPC_ph_e[SR_ID].Fill( iPh, kPh )
                                    # h_RPC_th_e[SR_ID].Fill( iTh, kTh )

                    ## End loop over all emulated hits in event (kHit)

                ## End loop over hits in EMTF track (iTrkHit)
            ## End loop over RECO muons (iReco)
                
        ## End loop over events in chain (jEvt)
    ## End loop over chains (ch)


######################
## Save the histograms
######################

    out_file.cd()

    canv = TCanvas('canv')
    canv.cd()

    for end in ['p', 'm']: ## Positive and negative endcaps
        for SR in CSC_CSC:
            SR_ID = '%s%d_%d_%d_%d' % (end, SR[0][0], SR[0][1], SR[1][0], SR[1][1])

            hist = h_CSC_dPh[SR_ID]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_CSC_dTh[SR_ID]
            hist.Write()
            hist.Draw()

            # canv.SaveAs(png_dir+hist.GetName()+'.png')
            # hist = h_CSC_ph[SR_ID]
            # hist.Write()
            # hist.Draw('colz')
            # canv.SaveAs(png_dir+hist.GetName()+'.png')
            # hist = h_CSC_th[SR_ID]
            # hist.Write()
            # hist.Draw('colz')
            # canv.SaveAs(png_dir+hist.GetName()+'.png')

        for SR in CSC_RPC:
            SR_ID = '%s%d_%d_%d_%d_%d' % (end, SR[0][0], SR[0][1], SR[1][0], SR[1][1], SR[1][2])

            hist = h_RPC_dPh_d[SR_ID]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_RPC_dTh_d[SR_ID]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_RPC_dPh_e[SR_ID]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_RPC_dTh_e[SR_ID]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')

            # hist = h_RPC_ph_d[SR_ID]
            # hist.Write()
            # hist.Draw('colz')
            # canv.SaveAs(png_dir+hist.GetName()+'.png')
            # hist = h_RPC_th_d[SR_ID]
            # hist.Write()
            # hist.Draw('colz')
            # canv.SaveAs(png_dir+hist.GetName()+'.png')
            # hist = h_RPC_ph_e[SR_ID]
            # hist.Write()
            # hist.Draw('colz')
            # canv.SaveAs(png_dir+hist.GetName()+'.png')
            # hist = h_RPC_th_e[SR_ID]
            # hist.Write()
            # hist.Draw('colz')
            # canv.SaveAs(png_dir+hist.GetName()+'.png')


    del out_file


if __name__ == '__main__':
    main()
