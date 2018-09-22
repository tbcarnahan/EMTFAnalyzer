
pwd_cmd="/bin/pwd"
run_dir=`${pwd_cmd}`
echo "run_dir = ${run_dir}"

bsub -q 8nh ${run_dir}/batch/launchers/sub_00.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_01.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_02.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_03.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_04.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_05.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_06.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_07.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_08.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_09.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_10.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_11.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_12.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_13.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_14.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_15.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_16.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_17.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_18.sh
bsub -q 8nh ${run_dir}/batch/launchers/sub_19.sh

