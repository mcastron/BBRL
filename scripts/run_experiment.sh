
#!/bin/bash

for l in OGC PGC UGC; do
for t in OGC PGC UGC; do

#    BAMCP
for K in 1 15 50 100; do
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent BAMCPAgent --agent_file "data/agents/bamcp($l, $K).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-bamcp($l, $K).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000
done

#    OPPSDSAgent (e-greedy)
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent OPPSDSAgent --agent_file "data/agents/opps-ds($l, egreedy).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-opps-ds($l, egreedy).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000

#    OPPSDSAgent (soft-max)
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent OPPSDSAgent --agent_file "data/agents/opps-ds($l, soft-max).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-opps-ds($l, soft-max).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000

#    OPPSDSAgent (vdbe e-greedy)
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent OPPSDSAgent --agent_file "data/agents/opps-ds($l, vdbe egreedy).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-opps-ds($l, vdbe egreedy).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000

#    OPPSCSAgent (e-greedy)
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent OPPSCSAgent --agent_file "data/agents/opps-cs($l, egreedy).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-opps-cs($l, egreedy).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000

#    OPPSCSAgent (soft-max)
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent OPPSCSAgent --agent_file "data/agents/opps-cs($l, soft-max).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-opps-cs($l, soft-max).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000

#    OPPSCSAgent (vdbe e-greedy)
./TinyBRL-DDS --run_experiment \
          --experiment --experiment_file "data/experiments/$t-exp.dat" \
          --agent OPPSCSAgent --agent_file "data/agents/opps-cs($l, vdbe egreedy).dat" \
          --n_threads 1 \
          --compress-output \
          --output "data/results/$t-opps-cs($l, vdbe egreedy).dat" \
          --refresh_frequency 1000 \
          --backup_frequency 15000

done
done
