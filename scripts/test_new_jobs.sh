

#    FORMULA SETS GENERATION
for s in 3 4 5; do
./TinyBRL-DDS --formula_set_generation --n_variables 3 \
     --tokens 15 ADD2 SUB MUL2 DIV MIN2 MAX2 SQRT LN ABS 1 2 3 5 7 9 \
     --max_size $s \
     --reduce --n_points 100 --points_range 0.0 100.0 \
     --compress_output \
     --output "data/formula_sets/F[$s].dat"
done


#    OFFLINE LEARNING
for l in GC OGC PGC UGC; do
     
     #    OPPS-DS
for s in 3 4 5; do
./new_job.sh DRAGON1 "OL-$l-OPPSDS-$s" 1 8000M 120:00:00 \
./TinyBRL-DDS --offline_learning \
     --agent OPPSDSAgent \
          --n_draws 500000 --c 1000.0 \
          --formula_set --formula_set_file "data/formula_sets/F[$s].dat.zz" \
          --discount_factor 0.95 --horizon_limit 250 \
     --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
     --compress_output \
     --output "data/agents/opps-ds[F[$s],$l].dat" > ol-$l-oppsds-$s.sh
done

     #    BAMCP
for K in 1 10 25 50 100 250 500; do
./new_job.sh DRAGON1 OL-$l-BAMCP-$K 1 16000M 120:00:00 \
./TinyBRL-DDS --offline_learning \
     --agent BAMCPAgent --K $K \
     --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
     --output "data/agents/bamcp[$K,$l].dat" > ol-$l-bamcp-$K.sh
done

done


for l in GC OGC PGC UGC; do
for s in 3 4 5; do
sbatch ./ol-$l-oppsds-$s.sh
done
done

for l in GC OGC PGC UGC; do
for K in 1 10 25 50 100 250 500; do
sbatch ./ol-$l-bamcp-$K.sh
done
done




#    NEW EXPERIMENTS
./TinyBRL-DDS --new_experiment \
     --name "GChain" \
     --mdp_distribution DirMultiDistribution --mdp_distribution_file "data/distributions/GC-distrib.dat" \
     --n_mdps 10000 --n_simulations_per_mdp 1 \
     --discount_factor 0.95 --horizon_limit 250 \
     --safe_simulations --compress_output \
     --output "data/experiments/GC[10000].dat"

./TinyBRL-DDS --new_experiment \
     --name "Optimistic GChain" \
     --mdp_distribution DirMultiDistribution --mdp_distribution_file "data/distributions/OGC-distrib.dat" \
     --n_mdps 10000 --n_simulations_per_mdp 1 \
     --discount_factor 0.95 --horizon_limit 250 \
     --safe_simulations --compress_output \
     --output "data/experiments/OGC[10000].dat"

./TinyBRL-DDS --new_experiment \
     --name "Pessimistic GChain" \
     --mdp_distribution DirMultiDistribution --mdp_distribution_file "data/distributions/PGC-distrib.dat" \
     --n_mdps 10000 --n_simulations_per_mdp 1 \
     --discount_factor 0.95 --horizon_limit 250 \
     --safe_simulations --compress_output \
     --output "data/experiments/PGC[10000].dat"

./TinyBRL-DDS --new_experiment \
     --name "Uniform GChain" \
     --mdp_distribution DirMultiDistribution --mdp_distribution_file "data/distributions/UGC-distrib.dat" \
     --n_mdps 10000 --n_simulations_per_mdp 1 \
     --discount_factor 0.95 --horizon_limit 250 \
     --safe_simulations --compress_output \
     --output "data/experiments/UGC[10000].dat"


#    RUN EXPERIMENTS
for l in GC OGC PGC UGC; do
for t in GC OGC PGC UGC; do

     #    OPPS-DS
for s in 3 4 5; do
./new_job.sh DRAGON1 RE-$t-$l-OPPSDS-$s 1 8000M 120:00:00 \
./TinyBRL-DDS --run_experiment \
     --experiment --experiment_file "data/experiments/$t[10000].dat.zz" \
     --agent OPPSDSAgent --agent_file "data/agents/opps-ds[F[$s],$l].dat.zz" \
     --n_threads 1 --compress_output \
     --output "data/results/exp[$t;opps-ds[F[$s],$l]].dat" \
     --refresh_frequency 900000 \
     --backup_frequency 3600000 > re-$t-$l-oppsds-$s.sh
done

     #    BAMCP
for K in 1 10 25 50 100 250 500; do
./new_job.sh DRAGON1 RE-$t-$l-BAMCP-$K 1 16000M 120:00:00 \
./TinyBRL-DDS --run_experiment \
     --experiment --experiment_file "data/experiments/$t[10000].dat.zz" \
     --agent BAMCPAgent --agent_file "data/agents/bamcp[$K,$l].dat" \
     --n_threads 1 --compress_output \
     --output "data/results/exp[$t;bamcp[$K,$l]].dat" \
     --refresh_frequency 900000 \
     --backup_frequency 3600000 > re-$t-$l-bamcp-$K.sh
done

done
done



     #    OPPS-DS
for l in GC OGC PGC UGC; do
for t in GC OGC PGC UGC; do
for s in 3 4 5; do
sbatch ./re-$t-$l-oppsds-$s.sh
done
done
done

     #    BAMCP
for K in 500 250 100 50 25 10 1; do
for l in GC OGC PGC UGC; do
for t in GC OGC PGC UGC; do
sbatch ./re-$t-$l-bamcp-$K.sh
done
done
done
