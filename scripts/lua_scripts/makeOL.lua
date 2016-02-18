
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


--   Extra functions
function getTab(n)
     local str = ""
     for i = 0, n do str = str .. "   " end
     return str    
end

function addOLTask(olTasks, agent, exp, vIndex)
     --   Create a new command
     local olTask = { agent = agent, exp = exp, vIndex = {} }
     for _, v in ipairs(vIndex) do table.insert(olTask.vIndex, v) end
     table.insert(olTasks, olTask)
     
     
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
     if (not noMoreTask) then addOLTask(olTasks, agent, exp, vIndex) end
end

function getOLCommand(task)
     --   Program
     local programStr = "./BBRL-DDS --offline_learning \\\n"


     --   Agent
     local agentStr = getTab(1) .. "--agent " .. task.agent.name .. " \\\n"
     for i, v in ipairs(task.agent.params) do
          if (i > 1 and v.opt ~= "") then
               agentStr = agentStr .. " \\\n"
          end
          if (v.opt ~= "") then agentStr = agentStr .. getTab(2) end
          
          agentStr = agentStr .. v.opt
          
          if (v.values[task.vIndex[i]] == "$T") then
               agentStr = agentStr .. " " .. task.exp.T
          else
               agentStr = agentStr .. " " .. v.values[task.vIndex[i]]
          end
     end
     if (#task.agent.params > 0) then agentStr = agentStr .. " \\\n" end

     
     --   MDP distribution
     local mdpDistribStr = getTab(1) .. "--mdp_distribution "
     mdpDistribStr = mdpDistribStr .. "\"DirMultiDistribution\" \\\n"
     mdpDistribStr = mdpDistribStr .. getTab(2) .. "--mdp_distribution_file "
     mdpDistribStr = mdpDistribStr .. "\"data/distributions/"
                     .. task.exp.priorFile .. "\" \\\n"


     --   OL options
     local agentName = task.agent.name
     for i, v in ipairs(task.vIndex) do
          if (task.agent.params[i].labels) then
               agentName = agentName .. "-" .. task.agent.params[i].labels[v]
          elseif (task.agent.params[i].values[v] == "$T") then
               agentName = agentName .. "-" .. task.exp.T
          else
               agentName = agentName .. "-" .. task.agent.params[i].values[v]
          end
     end
     
     local olOptionsStr = ""
     for _, opt in ipairs(task.agent.olOptions) do
          opt = opt:gsub("$PRIOR", task.exp.prior)
          opt = opt:gsub("$AGENT_STR", agentName)
          olOptionsStr = olOptionsStr .. getTab(1) .. opt .. " \\\n"          
     end


     --   Output command
     local outputFileName = "data/agents/" .. task.exp.prior
     outputFileName = outputFileName .. "-" .. agentName
     outputFileName = outputFileName .. ".dat"
     
     local outputStr = getTab(1)
     outputStr = outputStr .. "--output \"" .. outputFileName .. "\""


     --   Output file name (compression case)
     for _, opt in ipairs(task.agent.olOptions) do
          if (opt == "--compress_output") then
               outputFileName = outputFileName .. ".zz"
          end
     end


     --   return command
     return programStr .. agentStr .. mdpDistribStr .. olOptionsStr .. outputStr,
            outputFileName
end


--   Main program
args = {...}
     --   Generate the tasks
local olTasks = {}
for _, exp in ipairs(experiments) do
     for _, agent in ipairs(agents) do
          local vIndex = {}
          for _, p in ipairs(agent.params) do table.insert(vIndex, 1) end
     
          addOLTask(olTasks, agent, exp, vIndex)
     end
end


     --   Order by memory usage
local order = function(a, b)
     local aNum =
          tonumber(string.sub(a.agent.memory.ol, 1, #a.agent.memory.ol - 1))
     local bNum =
          tonumber(string.sub(b.agent.memory.ol, 1, #b.agent.memory.ol - 1))

     return (aNum < bNum)
end

table.sort(olTasks, order)


     --   Generate the corresponding commands
local submitJobs = {}
print()
if (#args >= 2 and (args[2] == "create" or args[2] == "submit")) then
     print("cd scripts/")
     print()
end
for _, task in ipairs(olTasks) do
     local command, outputFileName = getOLCommand(task)
     
          --   Normal output
     if (#args == 0 or not gridComputing[args[1]]) then
          print(command)
          print("")

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
          local jobName = "OL-" .. task.exp.prior .. agentStr
          if (#args >= 2 and args[2] == "create") then
               print("if [ ! -f " .. jobName .. ".sh ]; then")
               print("./new_job.sh"
                    .. " \"" .. gc.name .. "\""
                    .. " \"" .. jobName .. "\""
                    .. " \"" .. gc.nbCores .. "\""
                    .. " \"" .. task.agent.memory.ol .. "\""
                    .. " \"" .. task.agent.duration.ol .. "\""
                    .. " \"" .. gc.path .. "\""
                    .. " \"" .. gc.libPath .. "\""
                    .. " \\\n"
                    .. command .. " \\\n"
                    .. " > \"" .. jobName .. ".sh\"")
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
