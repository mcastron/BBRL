
#	$1:		Computer (NIC3 or DRAGON1)
#	$2:		Name of the job
#	$3:		Number of cores
#	$4:		Memory usage (M/-)
#	$5:		Duration ('hh:mm:ss')
#	$6:		Program
#	$7..:	The arguments of the program

#	Prints out the help if there is not the correct number of arguments
if [ $# = 0 ]; then
	echo -en "\n\t./new_job.sh [COMPUTER] [JOBNAME] [NCORES] [MEMORY] [DURATION] [EXEC] [ARG1] ..\n\n"
	exit
fi

#	NIC3 case
if [ "$1" = "NIC3" ]; then
	#	Set the 'highmem' flag if needed
	highmemFlag="false"
	if [ $4 -ge 3800 ]; then
		highmemFlag="true"
	fi
	
	
	echo "#!/bin/sh"
	echo "#$ -N $2"
	echo "#$ -l h_vmem=$4M"
	echo "#$ -l highmem=$highmemFlag"
	echo "#$ -l h_rt=$5"
	echo "#$ -pe snode $3"
	echo "#$ -cwd"
	#echo "#$ -m beas -M m.castronovo@ulg.ac.be"
	echo "#$ -m a -M m.castronovo@ulg.ac.be"
	echo ""
	echo "source /etc/profile.d/modules.sh"
	echo "source /etc/profile.d/segi.sh"
	echo ""
	echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/u/mcastron/lib"
	echo ""
	echo "cd /u/mcastron/home/TinyBRL/"
	
	
	command="$6"
	i=1
	for arg in "$@"
	do
		if [ $i -gt 6 ]; then
			command=$command" \""$arg"\""
		fi
		i=$((i+1))
	done
	
	
	echo "$command"
fi


#	DRAGON1 case
if [ "$1" = "DRAGON1" ]; then	
	echo "#!/bin/sh"
	echo "#SBATCH --job-name=$2"
	echo "#SBATCH --output=$2.out"
	echo "#SBATCH --mem-per-cpu=$4"
	echo ""
	echo "#SBATCH --time=$5"
	echo ""
	echo "#SBATCH --ntasks=1"
	echo "#SBATCH --cpus-per-task=$3"
	echo "#SBATCH --ntasks-per-node=1"
	echo ""
	echo "#SBATCH --mail-user=m.castronovo@ulg.ac.be"
	#echo "#SBATCH --mail-type=ALL"
	echo "#SBATCH --mail-type=FAIL"
	echo ""
	echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ulg/sysmod/mcastron/lib"
	echo ""
	echo "cd /home/ulg/sysmod/mcastron/home/TinyBRL/"
	
	
	command="$6"
	i=1
	for arg in "$@"
	do
		if [ $i -gt 6 ]; then
			command=$command" \""$arg"\""
		fi
		i=$((i+1))
	done
	
	
	echo "$command"
fi
