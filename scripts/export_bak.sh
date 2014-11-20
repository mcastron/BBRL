
#!/bin/bash


#    Accurate prior
for t in GC OGC PGC UGC; do
cmd=""

     #    OPPSDSAgent
for s in 3 4 5; do
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSDSAgent --agent_file \"data/agents/opps-ds[F[$s],$t].dat.zz\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/exp[$t;opps-ds[F[$s],$t]].dat.zz\" \\"
cmd=$cmd$'\n'
done     
     
     #    BAMCPAgent
for K in 1 10 25 50 100 250 500; do
cmd=$cmd$'\t'
cmd=$cmd"--agent BAMCPAgent --agent_file \"data/agents/bamcp[$K,$t].dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/exp[$t;bamcp[$K,$t]].dat.zz\" \\"
cmd=$cmd$'\n'
done

latexCmd="./TinyBRL-export --latex \\"$'\n'$cmd" --output \"data/export/tablesAcc$t.tex\""
matlabCmd="./TinyBRL-export --matlab \\"$'\n'$cmd" --output \"data/export/doGraphsAcc$t.m\""

eval "$latexCmd"
eval "$matlabCmd"

done


#    Inaccurate prior
for t in GC OGC PGC UGC; do
cmd=""

     #    OPPSDSAgent
for l in GC OGC PGC UGC; do

if [ "$l" = "$t" ]
then
     continue
fi

for s in 3 4 5; do
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSDSAgent --agent_file \"data/agents/opps-ds[F[$s],$l].dat.zz\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/exp[$t;opps-ds[F[$s],$l]].dat.zz\" \\"
cmd=$cmd$'\n'
done

done
     
     #    BAMCPAgent
for l in GC OGC PGC UGC; do

if [ "$l" = "$t" ]
then
     continue
fi

for K in 1 10 25 50 100 250 500; do
cmd=$cmd$'\t'
cmd=$cmd"--agent BAMCPAgent --agent_file \"data/agents/bamcp[$K,$l].dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/exp[$t;bamcp[$K,$l]].dat.zz\" \\"
cmd=$cmd$'\n'
done

done

     #    Export
latexCmd="./TinyBRL-export --latex \\"$'\n'$cmd" --output \"data/export/tablesInacc$t.tex\""
matlabCmd="./TinyBRL-export --matlab \\"$'\n'$cmd" --output \"data/export/doGraphsInacc$t.m\""

eval "$latexCmd"
eval "$matlabCmd"

done

