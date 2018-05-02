#!/bin/bash

data_tag=$1 ## String to match CMS primary datasets
job_tag=$2  ## Tag of the particular crab job

eos_cmd="/afs/cern.ch/project/eos/installation/scripts/bin/eos.select"

eos_pre="root://eoscms.cern.ch/"
eos_dir="/store/user/abrinke1/EMTF/Emulator/ntuples/"
tmp_dir="/afs/cern.ch/work/a/abrinke1/tmp"

NNN=0
while [ $NNN -lt 25 ]; do
    `xrdcp ${eos_pre}${eos_dir}$data_tag/$job_tag/NTuple_$NNN.root $tmp_dir/`
    # `xrdcp ${eos_pre}${eos_dir}$data_tag/$job_tag/NTuple_$NNN.root $tmp_dir/NTUPLE_$NNN.root`
    let "NNN += 1"
done