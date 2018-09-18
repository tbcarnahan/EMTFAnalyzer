#!/bin/bash

crab_cmd="/cvmfs/cms.cern.ch/crab3/slc6_amd64_gcc493/cms/crabclient/3.3.1809-comp/bin/crab"

# datasetTag="FlatNtuple_Run_306154_2018_05_15_SingleMu_2018_emul_really"
# datasetTag="FlatNtuple_Run_306091_2018_05_07_ZB1_2017_emul_dBX"
# datasetTag="FlatNtuple_Run_2018B_v1_2018_09_11_SingleMuon_2018_emul_90X_v1_coord"
datasetTag="FlatNtuple_Run_2018B_v1_2018_09_18_SingleMuon_2018_emul_102X_ReReco_v1_coord"
# datasetTag="FlatNtuple_Run_2018B_v1_2018_07_04_ZeroBias_2018_emul"

declare -a datasets=(
    # "/SingleMuon/Run2017F-PromptReco-v1/AOD---${datasetTag}"
    # "/ZeroBias1/Run2017F-PromptReco-v1/AOD---${datasetTag}"
    "/SingleMuon/Run2018B-PromptReco-v1/AOD---${datasetTag}"
    # "/ZeroBias/Run2018B-PromptReco-v1/AOD---${datasetTag}"
)

for dataset in ${datasets[@]}; do
    echo ""
    echo "About to execute crab submit -c crab_config_auto_RAW.py --inputDataset ${dataset%%---*} --outputDatasetTag ${datasetTag} --requestName ${dataset##*---}"
    echo ""
    inputDataset="${dataset%%---*}"
    requestName="${dataset##*---}"
    `perl -pi -e "s{MYARGS.inputDataset}{'${inputDataset}'}g" crab_config_auto_RAW.py`
    `perl -pi -e "s/MYARGS.outputDatasetTag/'${datasetTag}'/g" crab_config_auto_RAW.py`
    `perl -pi -e "s/MYARGS.requestName/'${requestName}'/g" crab_config_auto_RAW.py`
    echo ""
    echo "About to execute command: ${crab_cmd} submit -c crab_config_auto_RAW.py"
    echo "${crab_cmd} submit -c crab_config_auto_RAW.py"
    `${crab_cmd} submit -c crab_config_auto_RAW.py`
    echo "Submitted!"
    `cp crab_config_auto_RAW.py crab_config_last_submit.py`
    `perl -pi -e "s{'${inputDataset}'}{MYARGS.inputDataset}g" crab_config_auto_RAW.py`
    `perl -pi -e "s/'${datasetTag}'/MYARGS.outputDatasetTag/g" crab_config_auto_RAW.py`
    `perl -pi -e "s/'${requestName}'/MYARGS.requestName/g" crab_config_auto_RAW.py`
    echo ""
done
