#! /usr/bin/env python

# import sys
# import math
from ROOT import *
# import numpy as np
# from array import *
# # from eff_modules import *

MAX_FILE =    1
MAX_EVT  =  100
PRT_EVT  =    1

def main():

###################
## Initialize files
###################

    # file_name = 'root://eoscms.cern.ch//store/user/abrinke1/EMTF/Emulator/ntuples/SingleMuon/FlatNtuple_Run_2018B_v1_2018_07_04_SingleMuon_2018_emul/NTuple_0.root'
    # in_file   = TFile.Open(file_name)
    # in_tree   = in_file.Get('FlatNtupleData/tree')
    
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
        
    out_file = TFile('plots/Read_FlatNtuples.root', 'recreate')

    
#############
## Histograms
#############

    eta_bins = [50, -2.5, 2.5]

    h_trk_eta = TH1D('h_trk_eta', 'EMTF emulated track #eta', eta_bins[0], eta_bins[1], eta_bins[2])

                    
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

            nUnpHits = int(ch.nHits)
            nEmuHits = int(ch.nSimHits)

            nUnpTrks = int(ch.nUnpTracks)
            nEmuTrks = int(ch.nTracks)

            print 'Event has %d unpacked (%d emulated) hits, %d unpacked (%d emulated) tracks' % (nUnpHits, nEmuHits, nUnpTrks, nEmuTrks)

            for iTrk in range(nEmuTrks):
                print '  - Emulated track #%d has eta %.2f' % (iTrk+1, ch.trk_eta[iTrk])
                h_trk_eta.Fill(ch.trk_eta[iTrk])
                
        ## End loop over events in chain (jEvt)
    ## End loop over chains (ch)


######################
## Save the histograms
######################

    out_file.cd()

    h_trk_eta.Write()

    del out_file


if __name__ == '__main__':
    main()
