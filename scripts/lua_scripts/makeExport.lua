
local agents        = require "data.agents"
local gridComputing = require "data.gridComputing"

local experiments_accurate   = require "data.experiments_accurate"
local experiments_inaccurate = require "data.experiments_inaccurate"
local experiments_options    = require "data.experiments_options"
for _, exp in ipairs(experiments_accurate) do
     exp.expFile = exp.exp .. exp.N .. "-exp.dat.zz"
end
for _, exp in ipairs(experiments_inaccurate) do
     exp.expFile = exp.exp .. exp.N .. "-exp.dat.zz"
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
     local olOptionsStr = ""
     for _, opt in ipairs(task.agent.olOptions) do
          olOptionsStr = olOptionsStr .. getTab(1) .. opt .. " \\\n"          
     end


     --   Output command
     local outputFileName = "data/agents/" .. task.exp.prior
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
     for _, opt in ipairs(task.agent.olOptions) do
          if (opt == "--compress_output") then
               outputFileName = outputFileName .. ".zz"
          end
     end


     --   return command
     return programStr .. agentStr .. mdpDistribStr .. olOptionsStr .. outputStr,
            outputFileName
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

for i, experiments in ipairs({ experiments_accurate, experiments_inaccurate }) do
     experiments.options = experiments_options

          --   Generate the RE/OL tasks (to get output file names)
     local reTasks = {}
     for _, exp in ipairs(experiments) do
          for _, agent in ipairs(agents) do
               local vIndex = {}
               for _, p in ipairs(agent.params) do table.insert(vIndex, 1) end
          
               addRETask(reTasks, agent, exp, vIndex)
          end
     end
     
     local olTasks = {}
     for _, exp in ipairs(experiments) do
          for _, agent in ipairs(agents) do
               local vIndex = {}
               for _, p in ipairs(agent.params) do table.insert(vIndex, 1) end
          
               addOLTask(olTasks, agent, exp, vIndex)
          end
     end
     
     
          --   Generate the corresponding commands
     local olOutput = {}
     for _, task in ipairs(olTasks) do
          local _, outputFileName = getOLCommand(task, experiments.options)
          table.insert(olOutput, outputFileName)
     end
     
     local reOutput = {}
     for _, task in ipairs(reTasks) do
          local _, outputFileName = getRECommand(task, experiments.options)
          table.insert(reOutput, outputFileName)
     end
     
     
          --   Generate the export command    
     print()
     print("./BBRL-export \\")
     
     local count = 0
     for _, exp in ipairs(experiments) do
          for i = 1, #olTasks do
               if (exp.expFile == reTasks[i].exp.expFile) then count = count + 1
               end
          end
     end
     
     if (count >= 100) then
          local olFileName = "export-agent-file-list(" .. i .. ").txt";
          local reFileName = "export-exp-file-list(" .. i .. ").txt";
          
          local olFile = io.open("../" .. olFileName, "w")
          local reFile = io.open("../" .. reFileName, "w")
          
          for _, exp in ipairs(experiments) do
               for i = 1, #olTasks do
                    if (exp.expFile == reTasks[i].exp.expFile) then
                         local agentName = olTasks[i].agent.name
                    
                         olFile:write(agentName .. "\n" .. olOutput[i] .. "\n")
                         reFile:write(reOutput[i] .. "\n")
                    end
               end
          end
          
          print(getTab(1) .. "--agent_files \"scripts/" .. olFileName .. "\" \\")
          print(getTab(1) .. "--experiment_files \"scripts/" .. reFileName .. "\" \\")
     
     else
          for _, exp in ipairs(experiments) do
               local prefix = exp.exp .. "-" .. exp.prior .. "-"
                        .. experiments.shortName .. "-"

               for i = 1, #olTasks do
                   if (exp.expFile == reTasks[i].exp.expFile) then     
                        local agentName = olTasks[i].agent.name
                        print(getTab(1) .. "--agent " .. agentName .. " \\")
                        print(getTab(2) .. "--agent_file \""
                              .. olOutput[i] .. "\" \\")

                        print(getTab(1) .. "--experiment \\")
                        print(getTab(2) .. "--experiment_file \""
                              .. reOutput[i] .. "\" \\")
                   end
               end
          end
     end     

     
     if (i == 1) then print(getTab(1) .. "--suffix \"accurate\"")
     else             print(getTab(1) .. "--suffix \"inaccurate\"")
     end
end
