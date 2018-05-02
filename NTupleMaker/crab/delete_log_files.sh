#!/bin/bash

# data_tag=$1 ## String to match CMS primary datasets
# job_tag=$2  ## Tag of the particular crab job

eos_cmd="/afs/cern.ch/project/eos/installation/scripts/bin/eos.select"

eos_pre="root://eoscms.cern.ch/"
# eos_dir="/store/group/phys_higgs/HiggsExo/H2Mu/UF/ntuples/Moriond17/Mar13/"
eos_dir="/store/user/abrinke1/EMTF/Emulator/ntuples/"

# ## Remove existing hadd of all files matching data_tag
# echo "Removing existing ${eos_dir}HADD/NTuple_${data_tag}_${job_tag}.root"
# `$eos_cmd rm ${eos_dir}HADD/NTuple_${data_tag}_${job_tag}.root`
# echo ""

for dir1 in `$eos_cmd ls ${eos_dir}`; do

    # if test "${dir1#*${data_tag}*}" != "$dir1"; then
    # 	echo "#####################################################################################"
    # 	echo "Found directory that matches ${data_tag}: ${eos_dir}$dir1"
    # 	echo "#####################################################################################"
    # 	echo ""
    # fi
    # if test "${dir1#*${data_tag}*}" == "$dir1"; then
    # 	continue
    # fi

    for dir2 in `$eos_cmd ls ${eos_dir}$dir1`; do

	# if test "${dir2#*${job_tag}*}" != "$dir2"; then
    	#     echo "#####################################################################################"
    	#     echo "Found directory that matches ${job_tag}: ${eos_dir}$dir1/$dir2"
    	#     echo "#####################################################################################"
    	#     echo ""
	# fi
	# if test "${dir2#*${job_tag}*}" == "$dir2"; then
	#     continue
	# fi

	echo ""
	echo "#####################################################################################"
	echo "Found directory that matches ${data_tag}: ${eos_dir}$dir1/$dir2"
	echo "#####################################################################################"
	echo ""

    	for dir3 in `$eos_cmd ls ${eos_dir}$dir1/$dir2`; do

	    if test "${dir3#*.root*}" != "$dir3"; then
		continue
	    fi

    	    for dir4 in `$eos_cmd ls ${eos_dir}$dir1/$dir2/$dir3`; do
    		for logFile in `$eos_cmd ls ${eos_dir}$dir1/$dir2/$dir3/$dir4/log`; do

		    if test "${logFile#*log.tar.gz*}" == "$logFile"; then
			continue
		    fi

    		    echo "Removing existing ${eos_dir}$dir1/$dir2/$dir3/$dir4/log/$logFile"
		    `$eos_cmd rm ${eos_dir}$dir1/$dir2/$dir3/$dir4/log/$logFile`

		done # End loop over log files
    	    done # End loop over "0001", "0001", etc.
    	done # End loop over job date/time

    done # End loop over job_tag (or simply all sub-directories)
done # End loop over data_tag (or simply all sub-directories)
