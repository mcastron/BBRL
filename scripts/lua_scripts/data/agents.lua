
-- ============================================================================
--   This script describes the agents which have to be considered in the
--   experiments.
--
--   An agent is described as follows:
--
--   ...
--   {
--        -- Name of the agent class
--        name = "<agent class name>",
--
--        -- List of the parameters
--        params =                 
--        {
--             {
--                  --   Option flag
--                  opt = "--<opt>",
--                  
--                  --   The values to be tested
--                  values =
--                  {
--                       <v1>, ..., <vn>
--                  }
--             }
--        },
--
--        --   List of flags to use for offline learning
--        olOptions = { "--compress_output" },
--
--        --   (SLURM only) Memory limit for offline and online phases (in Mo)
--        memory = { ol = "<memory>M", re = "<memory>M" },
--
--        --   (SLURM only) Time limit for offline and online phases
--        duration = { ol = "<HH>:<MM>:<SS>", re = "<HH>:<MM>:<SS>" }
--   },
--   ...
--
-- ============================================================================
local agents =
{
     --   Random --------------------------------------------------------------
     {
          name = "RandomAgent",
          params = {},
          olOptions = {},
          memory = { ol = "1000M", re = "1000M" },
          duration = { ol = "01:00:00", re = "01:00:00" }
     },


     --   e-Greedy ------------------------------------------------------------
     {
          name = "EGreedyAgent",
          params =
          {
               {
                    opt = "--epsilon",
                    values =
                    {
                         0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0
                    }
               }
          },
          olOptions = { "--compress_output" },
          memory = { ol = "1000M", re = "1000M" },
          duration = { ol = "01:00:00", re = "01:00:00" }
     }
}


return agents
