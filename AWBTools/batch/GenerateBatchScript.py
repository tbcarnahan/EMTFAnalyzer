#! /usr/bin/env python

import sys
import os

## Main function executed by ./batch/GenerateBatchScript.py
def main():

    sub_files = [] ## Separate submission script for each job

    for i in range(20):

        launcher_name = 'batch/launchers/sub_%02d.sh' % i
        sub_files.append( open(launcher_name, 'w') )

        ## run_macro  = "\nroot -b -l -q '/afs/cern.ch/user/a/abrinke1/EMTFAnalyzer/CMSSW_10_2_5/src/EMTFAnalyzer/AWBTools/macros/EMTF_efficiency.C({"
        run_macro  = "\nroot -b -l -q '/afs/cern.ch/user/a/abrinke1/EMTFAnalyzer/CMSSW_10_2_5/src/EMTFAnalyzer/AWBTools/macros/LCT_efficiency_ME11.C({"
        ## run_macro += '"FlatNtuple_Run_2018D_v1_2018_09_18_SingleMuon_2018_emul_ph_lut_v2_coord/NTuple_%d.root", ' % i
        run_macro += '"FlatNtuple_Run_2018D_v1_2018_09_19_SingleMuon_2018_emul_102X_ReReco_v1_321988_bugFix/NTuple_%d.root"}' % i
        run_macro += ', "_%02d"' % i
        run_macro += ")'"

        sub_files[-1].write('\nrun_dir="/afs/cern.ch/user/a/abrinke1/EMTFAnalyzer/CMSSW_10_2_5/src/EMTFAnalyzer/AWBTools"')
        sub_files[-1].write('\ncd ${run_dir}')
        sub_files[-1].write('\neval `scramv1 runtime -sh`')
        sub_files[-1].write(run_macro)
        os.chmod(sub_files[-1].name, 0o777)

        print 'Wrote file %s' % launcher_name

## End function: main()
    
if __name__ == '__main__':
    main()
