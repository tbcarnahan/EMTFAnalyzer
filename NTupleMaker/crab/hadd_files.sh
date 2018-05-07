#!/bin/bash

data_tag=$1 ## String to match CMS primary datasets
job_tag=$2  ## Tag of the particular crab job


eos_cmd="/afs/cern.ch/project/eos/installation/scripts/bin/eos.select"
hadd_cmd="/cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_10_0_0/external/slc6_amd64_gcc630/bin/hadd -O -f"

eos_pre="root://eoscms.cern.ch/"
eos_dir="/eos/cms/store/user/abrinke1/EMTF/Emulator/ntuples/"
tmp_dir="/afs/cern.ch/work/a/abrinke1/tmp4"
max_add=40

# ## Clean up tmp directory
`rm $tmp_dir/NTuple_*.root`
`rm $tmp_dir/tuple_*.root`

## hadd command for all files matching data_tag
hadd_all_str="$hadd_cmd $tmp_dir/NTuple_${data_tag}_${job_tag}.root"


for dir1 in `$eos_cmd ls ${eos_dir}`; do

    if test "${dir1#*${data_tag}*}" != "$dir1"; then
    	echo "#####################################################################################"
    	echo "Found directory that matches ${data_tag}: ${eos_dir}$dir1"
    	echo "#####################################################################################"
    	echo ""
    fi
    if test "${dir1#*${data_tag}*}" == "$dir1"; then
	continue
    fi


    for dir2 in `$eos_cmd ls ${eos_dir}$dir1`; do

    	## Create new hadded files
    	for dir3 in `$eos_cmd ls ${eos_dir}$dir1/$dir2`; do

    	    ## Only look at single crab job tag
    	    if test "${dir2#${job_tag}}" == "$dir2"; then
    	    	continue
    	    fi

    	    ## Find most recent crab subission
    	    last_ver=$dir3
    	    for dir3a in `$eos_cmd ls ${eos_dir}$dir1/$dir2`; do
    		if test "${dir3a#*root}" == "$dir3a"; then
    		    last_ver=$dir3a
    		fi
    	    done
	    
    	    ## Assume only the most recent submission is the only valid one
    	    if test "${dir3#*$last_ver}" == "$dir3"; then
    		echo "#####################################################################################"
    		echo "Skipping old submission: ${eos_dir}$dir1/$dir2/$dir3"
    		echo "#####################################################################################"
    		echo ""
    		continue
    	    fi
	    
    	    for dir4 in `$eos_cmd ls ${eos_dir}$dir1/$dir2/$dir3`; do

    		nFiles=0
    		for fName in `$eos_cmd ls ${eos_dir}$dir1/$dir2/$dir3/$dir4`; do
    		    if test "${fName#*.root}" != "$fName"; then
    			let "nFiles += 1"
    		    fi
    		done
    		echo "*************************************************************************************"
    		echo "$dir1/$dir2/$dir3/$dir4 has $nFiles files"
    		echo "*************************************************************************************"
    		echo ""

    		nTot=0
    		nOut=0
    		used_files=""
    		while [ $nTot -lt $nFiles ]; do

    		    hadd_str="$hadd_cmd $tmp_dir/NTuple_$nOut.root"
    		    nAdded=0
    		    for fName in `$eos_cmd ls ${eos_dir}$dir1/$dir2/$dir3/$dir4`; do
    			if test "${used_files#*$fName}" != "$used_files"; then
    			    continue
    			fi
    			if test "${fName#*.root}" != "$fName"; then
    			    if [ "$nAdded" -lt $max_add ]; then
    				`xrdcp ${eos_pre}${eos_dir}$dir1/$dir2/$dir3/$dir4/$fName $tmp_dir`
    				hadd_str="$hadd_str $tmp_dir/$fName"
    				used_files="$used_files $fName"
    				let "nAdded += 1"
    				let "nTot += 1"
    			    fi
    			fi
    		    done  ## Closes for loop over max_add files
		    echo ""
		    echo "$hadd_str"
    		    `$hadd_str`
    		    echo ""
    		    echo "  * Wrote out $tmp_dir/NTuple_$nOut.root"
    	            ## Remove existing hadded files
    		    echo "Removing existing ${eos_dir}$dir1/$dir2/NTuple_$nOut.root"
    		    `$eos_cmd rm ${eos_dir}$dir1/$dir2/NTuple_$nOut.root`
		    echo ""
    		    `xrdcp $tmp_dir/NTuple_$nOut.root ${eos_pre}${eos_dir}$dir1/$dir2`
    		    `rm $tmp_dir/tuple_*.root`
    		    `mv $tmp_dir/NTuple_$nOut.root $tmp_dir/NTuple_${dir1}_${nOut}.root`
    		    echo ""
		    hadd_all_str="${hadd_all_str} $tmp_dir/NTuple_${dir1}_${nOut}.root"
    		    let "nOut += 1"
    		done  ## Closes while loop over all files
    	    done  ## Closes for loop over dir4 ("0000")
    	done  ## Closes for loop over dir3 (crab timestamp)
    done ## Closes for loop over dir2 (sample name)
done  ## Closes for loop over dir1 (DAS name)

## hadd of all files matching data_tag and job_tag
echo ""
echo "${hadd_all_str}"
`${hadd_all_str}`
echo ""
echo "  * Wrote out $tmp_dir/NTuple_${data_tag}_${job_tag}.root"
echo ""

## Remove existing hadd of all files matching data_tag
echo "Removing existing ${eos_dir}HADD/NTuple_${data_tag}_${job_tag}.root"
`$eos_cmd rm ${eos_dir}HADD/NTuple_${data_tag}_${job_tag}.root`
echo ""

`xrdcp $tmp_dir/NTuple_${data_tag}_${job_tag}.root ${eos_pre}${eos_dir}HADD/`
`rm $tmp_dir/NTuple_*.root`
