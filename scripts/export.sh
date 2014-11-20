
#!/bin/bash


cmd=""

for l in OGC PGC UGC; do
for t in OGC PGC UGC; do

#    BAMCP
for K in 1 15 50 100; do
cmd=$cmd$'\t'
cmd=$cmd"--agent BAMCPAgent --agent_file \"data/agents/bamcp($l, $K).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-bamcp($l, $K).dat\" \\"
cmd=$cmd$'\n'
done

#    OPPSDSAgent (e-greedy)
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSDSAgent --agent_file \"data/agents/opps-ds($l, egreedy).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-opps-ds($l, egreedy).dat\" \\"
cmd=$cmd$'\n'

#    OPPSDSAgent (soft-max)
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSDSAgent --agent_file \"data/agents/opps-ds($l, soft-max).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-opps-ds($l, soft-max).dat\" \\"
cmd=$cmd$'\n'

#    OPPSDSAgent (vdbe e-greedy)
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSDSAgent --agent_file \"data/agents/opps-ds($l, vdbe egreedy).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-opps-ds($l, vdbe egreedy).dat\" \\"
cmd=$cmd$'\n'

#    OPPSCSAgent (e-greedy)
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSCSAgent --agent_file \"data/agents/opps-cs($l, egreedy).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-opps-cs($l, egreedy).dat\" \\"
cmd=$cmd$'\n'

#    OPPSCSAgent (soft-max)
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSCSAgent --agent_file \"data/agents/opps-cs($l, soft-max).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-opps-cs($l, soft-max).dat\" \\"
cmd=$cmd$'\n'

#    OPPSCSAgent (vdbe e-greedy)
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSCSAgent --agent_file \"data/agents/opps-cs($l, vdbe egreedy).dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/$t-opps-cs($l, vdbe egreedy).dat\" \\"
cmd=$cmd$'\n'

done
done

latexCmd="./TinyBRL-export --latex \\"$'\n'$cmd" --output \"data/export/latex.tex\""
matlabCmd="./TinyBRL-export --matlab \\"$'\n'$cmd" --output \"data/export/doGraph.m\""

eval "$latexCmd"
eval "$matlabCmd"
