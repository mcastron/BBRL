
local agents        = require "data.agents"
local gridComputing = require "data.gridComputing"

local experiments_accurate   = require "data.experiments_accurate"
local experiments_inaccurate = require "data.experiments_inaccurate"
local experiments_options    = require "data.experiments_options"

local experiments = { options = experiments_options }
for _, exp in ipairs(experiments_accurate) do
     table.insert(experiments, exp)
end
for _, exp in ipairs(experiments_inaccurate) do
     table.insert(experiments, exp)
end
for _, exp in ipairs(experiments) do
     exp.expFile = exp.exp .. exp.N .. "-exp.dat.zz"
end


--   Extra functions
function getTab(n)
     local str = ""
     for i = 0, n do str = str .. "   " end
     return str    
end

function addRETask(reTasks, agent, exp, vIndex)
     --   Create a new command
     local olTask = { agent = agent, exp = exp, vIndex = {} }
     for _, v in ipairs(vIndex) do table.insert(olTask.vIndex, v) end
     table.insert(reTasks, olTask)
     
     
     --   Exit if no parameters
     if (#vIndex == 0) then return end
     
     
     --   Go to the next set of values
     vIndex[#vIndex] = vIndex[#vIndex] + 1
     
     local noMoreTask = false
     local i = #vIndex
     while (i > 0) do
          if (i == 1 and vIndex[1] > #agent.params[1].values) then
               noMoreTask = true
               break
          end
     
          if (vIndex[i] > #agent.params[i].values) then
               vIndex[i] = 1
               vIndex[i - 1] = vIndex[i - 1] + 1
          else break
          end
          
          i = i - 1
     end
     
     
     --   Add the next command(s) if possible
     if (not noMoreTask) then addRETask(reTasks, agent, exp, vIndex) end
end

function getRECommand(task, options)
     --   Program
     local programStr = "./BBRL-DDS --run_experiment \\\n"


     --   Experiment
     local expStr = getTab(1) .. "--experiment \\\n"
     expStr = expStr .. getTab(2) .. "--experiment_file \""
     expStr = expStr .. "data/experiments/" .. task.exp.expFile .. "\" \\\n"


     --   Agent
     local agentStr = getTab(1) .. "--agent " .. task.agent.name .. " \\\n"
     agentStr = agentStr .. getTab(2)
     agentStr = agentStr .. "--agent_file \"data/agents/" .. task.exp.prior
     agentStr = agentStr .. "-" .. task.agent.name
     for i, v in ipairs(task.agent.params) do          
          if (v.labels) then
               agentStr = agentStr .. "-" .. v.labels[task.vIndex[i]]
          elseif (v.values[task.vIndex[i]] == "$T") then
               agentStr = agentStr .. "-" .. task.exp.T
          else
               agentStr = agentStr .. "-" .. v.values[task.vIndex[i]]
          end
     end
     
     local compressInput = false
     for _, opt in ipairs(task.agent.olOptions) do
          if (opt == "--compress_output") then
               compressInput = true
          end
     end
     
     if (compressInput) then agentStr = agentStr .. ".dat.zz\" \\\n"   
     else                    agentStr = agentStr .. ".dat\" \\\n"
     end 


     --   Experiment options
     local optionsStr = ""
     for _, opt in ipairs(options) do
          optionsStr = optionsStr .. getTab(1) .. opt .. " \\\n"
     end


     --   Output command
     local outputFileName = "data/results/" .. task.exp.exp .. task.exp.N
     outputFileName = outputFileName .. "-" .. task.exp.prior
     outputFileName = outputFileName .. "-" .. task.agent.name
     for i, v in ipairs(task.vIndex) do
          if (task.agent.params[i].labels) then
               outputFileName = outputFileName .. "-"
                                .. task.agent.params[i].labels[v]
          elseif (task.agent.params[i].values[v] == "$T") then
               outputFileName = outputFileName .. "-"
                                .. task.exp.T
          else
               outputFileName = outputFileName .. "-"
                                .. task.agent.params[i].values[v]
          end
     end
     outputFileName = outputFileName .. ".dat"
     
     local outputStr = getTab(1)
     outputStr = outputStr .. "--output \"" .. outputFileName .. "\""


     --   Output file name (compression case)
     for _, opt in ipairs(options) do
          if (opt == "--compress_output") then
               outputFileName = outputFileName .. ".zz"
          end
     end


     --   return command
     return programStr .. expStr .. agentStr .. optionsStr .. outputStr,
            outputFileName
end


--   Main program
args = {...}
     --   Generate the tasks
local reTasks = {}
for _, exp in ipairs(experiments) do
     for _, agent in ipairs(agents) do
          local vIndex = {}
          for _, p in ipairs(agent.params) do table.insert(vIndex, 1) end
     
          addRETask(reTasks, agent, exp, vIndex)
     end
end

     --   Order by memory usage
local order = function(a, b)
     local aNum =
          tonumber(string.sub(a.agent.memory.re, 1, #a.agent.memory.re - 1))
     local bNum =
          tonumber(string.sub(b.agent.memory.re, 1, #b.agent.memory.re - 1))

     return (aNum < bNum)
end

table.sort(reTasks, order)


     --   Generate the corresponding commands
local submitJobs = {}
print()
if (#args >= 2 and (args[2] == "create" or args[2] == "submit")) then
     print("cd scripts/")
     print()
end
for _, task in ipairs(reTasks) do
     local command, outputFileName = getRECommand(task, experiments.options)
     
          --   Normal output
     if (#args == 0 or not gridComputing[args[1]]) then
          print(command)
          print()

         --    Grid computing output 
     else
          local gc = gridComputing[args[1]]
     
          local agentStr = ""
          agentStr = agentStr .. "-" .. task.agent.name
          for i, v in ipairs(task.vIndex) do
               if (task.agent.params[i].labels) then
                    agentStr = agentStr .. "-" .. task.agent.params[i].labels[v]
               elseif (task.agent.params[i].values[v] == "$T") then
                    agentStr = agentStr .. "-" .. task.exp.T
               else
                    agentStr = agentStr .. "-" .. task.agent.params[i].values[v]
               end
          end
          
               --   Grid computing output (create)
          local jobName = "RE-" .. task.exp.exp .. task.exp.N .. "-" .. task.exp.prior
                          .. agentStr
          if (#args >= 2 and args[2] == "create") then
               print("if [ ! -f " .. jobName .. ".sh ]; then")
               print("./new_job.sh"
                    .. " \"" .. gc.name .. "\""
                    .. " \"" .. jobName .. "\""
                    .. " \"" .. gc.nbCores .. "\""
                    .. " \"" .. task.agent.memory.re .. "\""
                    .. " \"" .. task.agent.duration.re .. "\""
                    .. " \"" .. gc.path .. "\""
                    .. " \"" .. gc.libPath .. "\""
                    .. " \\\n"
                    .. command .. " \\\n"
                    .. " > " .. jobName .. ".sh")
               print("fi\n")
          end
          
          table.insert(submitJobs,
                       {
                         command = gc.submitCommand .. " ./" .. jobName
                                   .. ".sh",
                         jobName = jobName,
                         outputFileName = outputFileName
                       })

     end
end


               --   Grid computing output (submit)
if (#args >= 2 and args[2] == "submit") then
     local gc = gridComputing[args[1]]

     print("jobList=$(squeue -u " .. gc.user
           .. " -o \"%.7i %.9P %.100j %.8u %.2t %.10M %.6D %R\""
           .. " | awk -F \" \" '{print $3}')")

     for _, submitJob in ipairs(submitJobs) do
           print("if [ ! -f " .. gc.path
                 .. submitJob.outputFileName .. " ]; then")
           print("runTest=true; "
                 .. "for name in $jobList; do "
                 .. "if [[ $name == \"" .. submitJob.jobName
                 .. "\" ]]; then "
                 .. "runTest=false; fi; done")
            
           print("if $runTest; then " .. submitJob.command .. "; fi")
           print("fi")
           print()
     end
     print()
end
if (#args >= 2 and (args[2] == "create" or args[2] == "submit")) then
     print("cd ../")
     print()
end
