
local agents        = require "data.agents"
local gridComputing = require "data.gridComputing"
local formulaSets   = require "data.formula_sets"

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


--   Main program
     --   Generate the commands
           -- Formulas sets
print()
for _, fSet in ipairs(formulaSets) do
     print("./TinyBRL-DDS --formula_set_generation \\")
     print(getTab(1) .. "--n_variables " .. fSet.nVariables .. " \\")
     
     tokensStr = "--tokens " .. #(fSet.tokens)
     for _, token in ipairs(fSet.tokens) do
          tokensStr = tokensStr .. " " .. token
     end
     print(getTab(1) .. tokensStr .. " \\")
     
     print(getTab(1) .. "--max_size " .. fSet.maxSize .. " \\")
     print(getTab(1) .. "--reduce --n_points " .. fSet.nPoints
           .. " --points_range " .. fSet.pointsRange[1]
           .. " " .. fSet.pointsRange[2] .. " \\")
     print(getTab(1) .. "--output \"data/formula_sets/"
           .. fSet.name .. ".dat\"")
     print()
end

          -- Experiments
print()
for _, exp in ipairs(experiments) do
     alreadyDone = false
     for _, expBis in ipairs(experiments) do
          if (expBis == exp) then break end
          
          if (expBis.exp == exp.exp
                    and expBis.testFile == exp.testFile
                    and expBis.N == exp.N
                    and expBis.gamma == exp.gamma
                    and expBis.T == exp.T) then
               alreadyDone = true
               break
          end
     end
     
     if (not alreadyDone) then
          print("./TinyBRL-DDS --new_experiment \\")
          print(getTab(1) .. "--name \"" .. exp.exp .. " Experiment\" \\")
          print(getTab(1) .. "--mdp_distribution \"DirMultiDistribution\" \\")
          print(getTab(2) .. "--mdp_distribution_file \"data/distributions/"
                .. exp.testFile .. "\" \\")
          print(getTab(1) .. "--n_mdps " .. exp.N .. " \\")
          print(getTab(1) .. "--n_simulations_per_mdp 1 \\")
          print(getTab(1) .. "--discount_factor " .. exp.gamma .. " \\")
          print(getTab(1) .. "--horizon_limit " .. exp.T .. " \\")
          print(getTab(1) .. "--compress_output \\")
          print(getTab(1) .. "--output \"data/experiments/"
                .. exp.exp .. exp.N .. "-exp.dat" .. "\"")
          print()
     end
end
