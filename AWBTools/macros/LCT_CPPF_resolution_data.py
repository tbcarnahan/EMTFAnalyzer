#! /usr/bin/env python

from ROOT import *

MAX_FILE = 1
MAX_EVT  = 2000
PRT_EVT  = 100

def main():

###################
## Initialize files
###################

    file_names = []
    store  = 'root://eoscms.cern.ch//store/user/abrinke1/EMTF/Emulator/ntuples/'
    in_dir = 'SingleMuon/FlatNtuple_Run_2018B_v1_2018_07_04_SingleMuon_2018_emul/'
    for i in range(10):
        if (i >= MAX_FILE): break
        file_names.append(store+in_dir+'NTuple_%d.root' % i)
        print 'Opening file: '+store+in_dir+file_names[i]
        
    in_chains = []
    for i in range(len(file_names)):
        in_chains.append( TChain('FlatNtupleData/tree') )
        in_chains[i].Add( file_names[i] )
        
    out_file = TFile('plots/LCT_CPPF_resolution.root', 'recreate')
    png_dir  = 'plots/png/LCT_CPPF_resolution_data/'

    
#############
## Histograms
#############

    ph_bins = [131, -65.5, 65.5]
    th_bCSC = [ 19,  -9.5,  9.5]
    th_bRPC = [ 35, -17.5, 17.5]

    ## [station,ring] for CSC and RPC chambers to compare
    CSC_CSC = [ [[1,1],[2,1]], [[2,1],[3,1]], [[3,1],[4,1]],
                [[1,2],[2,2]], [[2,2],[3,2]], [[3,2],[4,2]],
                [[1,3],[2,2]], [[1,4],[2,1]] ]
    CSC_RPC = [ [[1,2],[1,2]], [[2,2],[2,2]],
                [[3,2],[3,2]], [[4,2],[4,2]],
                [[3,2],[3,3]], [[4,2],[4,3]] ]
                                
    h_CSC_ph   = {} ## CSC LCT phi resolution
    h_CSC_th   = {} ## CSC LCT theta resolution
    h_RPC_ph_d = {} ## Unpacked CPPF digis in data
    h_RPC_th_d = {}
    h_RPC_ph_e = {} ## Emulated CPPF digis from RPC hits
    h_RPC_th_e = {}

    for end in ['p', 'm']: ## Positive and negative endcaps
        for SR in CSC_CSC:
            h_CSC_ph['%s%d_%d' % (end, SR[0][0], SR[0][1])] = TH1D( 'h_CSC%s_ph_%d_%d' % (end, SR[0][0], SR[0][1]),
                                                                    'CSC%s LCT #Delta#phi(%d/%d - %d/%d)' % (end, SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                                    ph_bins[0], ph_bins[1], ph_bins[2] )
            h_CSC_th['%s%d_%d' % (end, SR[0][0], SR[0][1])] = TH1D( 'h_CSC%s_th_%d_%d' % (end, SR[0][0], SR[0][1]),
                                                                    'CSC%s LCT #Delta#theta(%d/%d - %d/%d)' % (end, SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                                    th_bCSC[0], th_bCSC[1], th_bCSC[2] )
            
        for SR in CSC_RPC:
            h_RPC_ph_d['%s%d_%d' % (end, SR[1][0], SR[1][1])] = TH1D( 'h_RPC%s_ph_%d_%d_data' % (end, SR[1][0], SR[1][1]),
                                                                      'Unpacked CPPF%s digi #Delta#phi(RPC %d/%d - CSC %d/%d)' % (end, SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                                      ph_bins[0], ph_bins[1], ph_bins[2] )
            h_RPC_th_d['%s%d_%d' % (end, SR[1][0], SR[1][1])] = TH1D( 'h_RPC%s_th_%d_%d_data' % (end, SR[1][0], SR[1][1]),
                                                                      'Unpacked CPPF%s digi #Delta#theta(RPC %d/%d - CSC %d/%d)' % (end, SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                                      th_bRPC[0], th_bRPC[1], th_bRPC[2] )
            h_RPC_ph_e['%s%d_%d' % (end, SR[1][0], SR[1][1])] = TH1D( 'h_RPC%s_ph_%d_%d_emul' % (end, SR[1][0], SR[1][1]),
                                                                      'Emulated CPPF%s digi #Delta#phi(RPC %d/%d - CSC %d/%d)' % (end, SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                                      ph_bins[0], ph_bins[1], ph_bins[2] )
            h_RPC_th_e['%s%d_%d' % (end, SR[1][0], SR[1][1])] = TH1D( 'h_RPC%s_th_%d_%d_emul' % (end, SR[1][0], SR[1][1]),
                                                                      'Emulated CPPF%s digi #Delta#theta(RPC %d/%d - CSC %d/%d)' % (end, SR[1][0], SR[1][1], SR[0][0], SR[0][1]),
                                                                      th_bRPC[0], th_bRPC[1], th_bRPC[2] )
                
                    
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
                if (ch.reco_pt                 [iReco] < 30): continue
                if (ch.reco_trig_ID            [iReco] >= 1
                    and ch.nRecoMuonsTrig              <= 1): continue
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

                    iSect = ch.hit_sector_index[iHit]
                    iStat = ch.hit_station     [iHit]
                    iRing = ch.hit_ring        [iHit]
                    iCham = ch.hit_chamber     [iHit]
                    iPh   = ch.hit_phi_int     [iHit]
                    iTh   = ch.hit_theta_int   [iHit]

                    ## Loop over all unpacked hits in the event in the corresponding chamber
                    for jHit in range(nHits):
                        if (iHit == jHit): continue
                        if (ch.hit_BX[jHit] != 0 and ch.hit_BX[jHit] != -1): continue ## Unpacked CPPF digis may be 1 BX early

                        jSect = ch.hit_sector_index[jHit]
                        jStat = ch.hit_station     [jHit]
                        jRing = ch.hit_ring        [jHit]
                        jCham = ch.hit_chamber     [jHit]
                        jPh   = ch.hit_phi_int     [jHit]
                        jTh   = ch.hit_theta_int   [jHit]

                        if (ch.hit_endcap[jHit] == 1): end = 'p'
                        else:                          end = 'm'

                        ## Require same EMTF sector and overlapping chamber
                        if (jSect != iSect): continue
                        if (iStat == 1 and (iRing % 3) == 1): ## ME1/1 and ME1/4 have twice as many chambers as ME2/1
                            if (jCham != (iCham + 1) / 2): continue
                        else:
                            if (jCham != iCham): continue

                        if (ch.hit_isCSC[jHit] == 1 and ch.hit_BX[jHit] == 0):
                            for SR in CSC_CSC:
                                if (iStat == SR[0][0] and iRing == SR[0][1] and jStat == SR[1][0] and jRing == SR[1][1]):

                                    h_CSC_ph['%s%d_%d' % (end, SR[0][0], SR[0][1])].Fill( max(ph_bins[1]+0.1, min(ph_bins[2]-0.1, jPh - iPh) ) )
                                    h_CSC_th['%s%d_%d' % (end, SR[0][0], SR[0][1])].Fill( max(th_bRPC[1]+0.1, min(th_bRPC[2]-0.1, jTh - iTh) ) )

                        if (ch.hit_isRPC[jHit] == 1):
                            for SR in CSC_RPC:
                                if (iStat == SR[0][0] and iRing == SR[0][1] and jStat == SR[1][0] and jRing == SR[1][1]):

                                    h_RPC_ph_d['%s%d_%d' % (end, SR[1][0], SR[1][1])].Fill( max(ph_bins[1]+0.1, min(ph_bins[2]-0.1, jPh - iPh) ) )
                                    h_RPC_th_d['%s%d_%d' % (end, SR[1][0], SR[1][1])].Fill( max(th_bRPC[1]+0.1, min(th_bRPC[2]-0.1, jTh - iTh) ) )

                    ## End loop over all unpacked hits in event (jHit)

                    ## Loop over all emulated hits in the event in the corresponding chamber
                    for kHit in range(nSimHits):
                        if (iHit == kHit): continue
                        if (ch.sim_hit_BX[kHit] != 0): continue

                        kSect = ch.sim_hit_sector_index[kHit]
                        kStat = ch.sim_hit_station     [kHit]
                        kRing = ch.sim_hit_ring        [kHit]
                        kCham = ch.sim_hit_chamber     [kHit]
                        kPh   = ch.sim_hit_phi_int     [kHit]
                        kTh   = ch.sim_hit_theta_int   [kHit]

                        if (ch.sim_hit_endcap[kHit] == 1): end = 'p'
                        else:                              end = 'm'

                        ## Require same EMTF sector and chamber
                        if (kSect != iSect): continue
                        if (kCham != iCham): continue
                        
                        if (ch.sim_hit_isRPC[kHit] == 1):
                            for SR in CSC_RPC:
                                if (iStat == SR[0][0] and iRing == SR[0][1] and kStat == SR[1][0] and kRing == SR[1][1]):

                                    h_RPC_ph_e['%s%d_%d' % (end, SR[1][0], SR[1][1])].Fill( max(ph_bins[1]+0.1, min(ph_bins[2]-0.1, kPh - iPh) ) )
                                    h_RPC_th_e['%s%d_%d' % (end, SR[1][0], SR[1][1])].Fill( max(th_bRPC[1]+0.1, min(th_bRPC[2]-0.1, kTh - iTh) ) )

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
            hist = h_CSC_ph['%s%d_%d' % (end, SR[0][0], SR[0][1])]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_CSC_th['%s%d_%d' % (end, SR[0][0], SR[0][1])]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
        for SR in CSC_RPC:
            hist = h_RPC_ph_d['%s%d_%d' % (end, SR[1][0], SR[1][1])]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_RPC_th_d['%s%d_%d' % (end, SR[1][0], SR[1][1])]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_RPC_ph_e['%s%d_%d' % (end, SR[1][0], SR[1][1])]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')
            hist = h_RPC_th_e['%s%d_%d' % (end, SR[1][0], SR[1][1])]
            hist.Write()
            hist.Draw()
            canv.SaveAs(png_dir+hist.GetName()+'.png')


    del out_file


if __name__ == '__main__':
    main()
