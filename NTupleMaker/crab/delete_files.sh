#!/bin/bash

data_tag=$1 ## String to match CMS primary datasets
job_tag=$2  ## Tag of the particular crab job

eos_cmd="/afs/cern.ch/project/eos/installation/scripts/bin/eos.select"

eos_pre="root://eoscms.cern.ch/"
eos_dir="/store/user/abrinke1/EMTF/Emulator/ntuples/"

for dir1 in `$eos_cmd ls ${eos_dir}`; do

    if test "${dir1#*${data_tag}*}" == "$dir1"; then
	continue
    fi

    for dir2 in `$eos_cmd ls ${eos_dir}$dir1`; do
	
    	## Only look at single crab job tag
    	if test "${dir2#${job_tag}}" == "$dir2"; then
    	    continue
    	fi

        echo "About to delete directory ${eos_dir}${dir1}/${dir2}"
    	`$eos_cmd rm -r ${eos_dir}${dir1}/${dir2}`

    done ## Closes for loop over dir2 (sample name)
done  ## Closes for loop over dir1 (DAS name)
