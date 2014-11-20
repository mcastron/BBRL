
#!/bin/bash

for l in OGC PGC UGC; do

#    BAMCPAgent
for K in 1 15 50 100; do
./TinyBRL-DDS --offline_learning \
               --agent BAMCPAgent --max_depth 250 --K $K \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/bamcp($l, $K).dat"
done

#    OPPSDSAgent (e-greedy)
./TinyBRL-DDS --offline_learning \
               --agent OPPSDSAgent \
                    --n_draws 10 --c 1000.0 \
                    --agent_factory EGreedyAgentFactory --epsilon 0.0 1.0 --counters 1 25 \
                    --n_samples 5000 \
                    --discount_factor 0.95 \
                    --horizon_limit 250 \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/opps-ds($l, egreedy).dat"

#    OPPSDSAgent (soft-max)
./TinyBRL-DDS --offline_learning \
               --agent OPPSDSAgent \
                    --n_draws 10 --c 1000.0 \
                    --agent_factory SoftMaxAgentFactory --tau 0.01 100.0 --counters 1 25 \
                    --n_samples 5000 \
                    --discount_factor 0.95 \
                    --horizon_limit 250 \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/opps-ds($l, soft-max).dat"

#    OPPSDSAgent (vdbe e-greedy)
./TinyBRL-DDS --offline_learning \
               --agent OPPSDSAgent \
                    --n_draws 10 --c 1000.0 \
                    --agent_factory VDBEEGreedyAgentFactory --sigma 0.01 100.0 --delta 0.0 0.99 --ini_epsilon 0.0 1.0 --counters 1 25 \
                    --n_samples 5000 \
                    --discount_factor 0.95 \
                    --horizon_limit 250 \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/opps-ds($l, vdbe egreedy).dat"

#    OPPSCSAgent (e-greedy)
./TinyBRL-DDS --offline_learning \
               --agent OPPSCSAgent \
                    --n_draws 10 --c 1000.0 \
                    --agent_factory EGreedyAgentFactory --epsilon 0.0 1.0 --counters 1 25 \
                    --discount_factor 0.95 \
                    --horizon_limit 250 \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/opps-cs($l, egreedy).dat"

#    OPPSCSAgent (soft-max)
./TinyBRL-DDS --offline_learning \
               --agent OPPSCSAgent \
                    --n_draws 10 --c 1000.0 \
                    --agent_factory SoftMaxAgentFactory --tau 0.01 100.0 --counters 1 25 \
                    --discount_factor 0.95 \
                    --horizon_limit 250 \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/opps-cs($l, soft-max).dat"

#    OPPSCSAgent (vdbe e-greedy)
./TinyBRL-DDS --offline_learning \
               --agent OPPSCSAgent \
                    --n_draws 10 --c 1000.0 \
                    --agent_factory VDBEEGreedyAgentFactory --sigma 0.01 100.0 --delta 0.0 0.99 --ini_epsilon 0.0 1.0 --counters 1 25 \
                    --discount_factor 0.95 \
                    --horizon_limit 250 \
               --mdp_distribution "DirMultiDistribution" --mdp_distribution_file "data/distributions/$l-distrib.dat" \
               --output "data/agents/opps-cs($l, vdbe egreedy).dat"

done
