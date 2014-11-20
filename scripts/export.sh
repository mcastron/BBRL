
#!/bin/bash


#    For each test distribution...

for t in GC OGC PGC UGC; do

     #    Accurate case (prior = test) --> table
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

          #    Generate the table
cmd="./TinyBRL-export --latex \\"$'\n'$cmd" --output \"data/export/table$t.tex\""
eval "$cmd"

     
     #    Inaccurate case (prior != test) --> graphic
cmd=""
     
for l in GC OGC PGC UGC; do

          #    OPPSDSAgent
for s in 3 4 5; do
cmd=$cmd$'\t'
cmd=$cmd"--agent OPPSDSAgent --agent_file \"data/agents/opps-ds[F[$s],$l].dat.zz\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/exp[$t;opps-ds[F[$s],$l]].dat.zz\" \\"
cmd=$cmd$'\n'
done

          #    BAMCPAgent
for K in 1 10 25 50 100 250 500; do
cmd=$cmd$'\t'
cmd=$cmd"--agent BAMCPAgent --agent_file \"data/agents/bamcp[$K,$l].dat\" \\"
cmd=$cmd$'\n\t'
cmd=$cmd"--experiment --experiment_file \"data/results/exp[$t;bamcp[$K,$l]].dat.zz\" \\"
cmd=$cmd$'\n'
done

done

          #    Generate the graphic
cmd="./TinyBRL-export --matlab \\"$'\n'$cmd" --output \"data/export/graph$t.m\""
eval "$cmd"

done
