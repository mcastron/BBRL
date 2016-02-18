
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
 },


 --   Soft-max ------------------------------------------------------------
 {
      name = "SoftMaxAgent",
      params =
      {
           {
                opt = "--tau",
                values =
                {
                     0.05, 0.10, 0.20, 0.33, 0.50, 1.0, 2.0, 3.0, 5.0, 25.0
                }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "1000M" },
      duration = { ol = "01:00:00", re = "01:00:00" }
 },


 --   OPPS-DS (low) -------------------------------------------------------
 {
      name = "OPPSDSAgent", -- 12, 43
      params =
      {
           {
                opt = "--formula_set --formula_set_file",
                values =
                {
                     "\"data/formula_sets/F2.dat\"",
                     "\"data/formula_sets/F3.dat\""
                },
                labels = { "F2", "F3" }
           },
           {
                opt = "--n_draws",
                values =
                {
                     50, 500, 1250, 2500, 5000, 10000, 100000, 1000000
                }
           },
           {
                opt = "--horizon_limit",
                values = { "$T" }
           }
           
      },
      olOptions =
      {
           "--c 660.0",
           "--variables 3 QMean QSelf QUniform",
           "--discount_factor 0.95",
           "--compress_output"
      },
      memory = { ol = "2000M", re = "1000M" },
      duration = { ol = "120:00:00", re = "01:00:00" }
 },


 --   OPPS-DS (mid) -------------------------------------------------------
 {
      name = "OPPSDSAgent", -- 226, 1208
      params =
      {
           {
                opt = "--formula_set --formula_set_file",
                values =
                {
                     "\"data/formula_sets/F4.dat\"",
                     "\"data/formula_sets/F5.dat\""
                },
                labels = { "F4", "F5" }
           },
           {
                opt = "--n_draws",
                values =
                {
                    1250, 2500, 5000, 10000, 100000, 1000000
                }
           },
           {
                opt = "--horizon_limit",
                values = { "$T" }
           }
           
      },
      olOptions =
      {
           "--c 660.0",
           "--variables 3 QMean QSelf QUniform",
           "--discount_factor 0.95",
           "--compress_output"
      },
      memory = { ol = "4000M", re = "1000M" },
      duration = { ol = "120:00:00", re = "01:00:00" }
 },


 --   OPPS-DS (high) ------------------------------------------------------
 {
      name = "OPPSDSAgent", -- 7350
      params =
      {
           {
                opt = "--formula_set --formula_set_file",
                values =
                {
                     "\"data/formula_sets/F6.dat\""
                },
                labels = { "F6" }
           },
           {
                opt = "--n_draws",
                values =
                {
                     7500, 10000, 100000, 1000000
                }
           },
           {
                opt = "--horizon_limit",
                values = { "$T" }
           }
           
      },
      olOptions =
      {
           "--c 660.0",
           "--variables 3 QMean QSelf QUniform",
           "--discount_factor 0.95",
           "--compress_output"
      },
      memory = { ol = "16000M", re = "1000M" },
      duration = { ol = "120:00:00", re = "01:00:00" }
 },
 
 
 --   BAMCP (low) ---------------------------------------------------------
 {
      name = "BAMCPAgent",
      params =
      {
           {
                opt = "--K",
                values = { 1, 500, 1250, 2500 }
           },
           {
                opt = "--D",
                values = { 15, 25, 50 }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "2000M" },
      duration = { ol = "01:00:00", re = "60:00:00" }
 },
 
 
 --   BAMCP (high) --------------------------------------------------------
 {
      name = "BAMCPAgent",
      params =
      {
           {
                opt = "--K",
                values = { 5000, 10000, 25000 }
           },
           {
                opt = "--D",
                values = { 15, 25, 50 }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "4000M" },
      duration = { ol = "01:00:00", re = "120:00:00" }
 },


 --   BFS3 (low) ----------------------------------------------------------
 {
      name = "BFS3Agent",
      params =
      {
           {
                opt = "--K",
                values = { 1, 500, 1250 }
           },
           {
                opt = "--C",
                values = { 2, 5, 10, 15 }
           },
           {
                opt = "--D",
                values = { 15, 25, 50 }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "2000M" },
      duration = { ol = "01:00:00", re = "60:00:00" }
 },


 --   BFS3 (high) ---------------------------------------------------------
 {
      name = "BFS3Agent",
      params =
      {
           {
                opt = "--K",
                values = { 2500, 5000, 10000 }
           },
           {
                opt = "--C",
                values = { 2, 5, 10, 15 }
           },
           {
                opt = "--D",
                values = { 15, 25, 50 }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "4000M" },
      duration = { ol = "01:00:00", re = "120:00:00" }
 },


 --   SBOSS (low) ---------------------------------------------------------
 {
      name = "SBOSSAgent",
      params =
      {
           {
                opt = "--epsilon",
                values =
                {
                     1.0, 1e-1, 1e-2, 1e-3
                }
           },
           {
                opt = "--delta",
                values =
                {
                     9, 7, 5, 3, 1, 1e-1, 1e-2, 1e-3
                }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "2000M" },
      duration = { ol = "01:00:00", re = "60:00:00" }
 },


 --   SBOSS (high) ---------------------------------------------------------
 {
      name = "SBOSSAgent",
      params =
      {
           {
                opt = "--epsilon",
                values =
                {
                     1e-4, 1e-5, 1e-6
                }
           },
           {
                opt = "--delta",
                values =
                {
                     1e-4, 1e-5, 1e-6
                }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "4000M" },
      duration = { ol = "01:00:00", re = "120:00:00" }
 },


 --   BEB -----------------------------------------------------------------
 {
      name = "BEBAgent",
      params =
      {
           {
                opt = "--beta",
                values = { 0.25, 0.5, 1, 1.5, 2, 2.5, 3, 4, 8, 16 }
           }
      },
      olOptions = { "--compress_output" },
      memory = { ol = "1000M", re = "1000M" },
      duration = { ol = "01:00:00", re = "01:00:00" }
 },


     --   ANN -----------------------------------------------------------------
     {
          name = "ANNAgent",
          params =
          {
               {
                    opt = "--hidden_layers",
                    values =
                    {
                         "1 10",       "1 25",    "1 50",
                         "2 10 10",    "2 25 25",
                         "3 10 10 10",
                    },
                    labels =
                    {
                         "1_10",       "1_25",    "1_50",
                         "2_10_10",    "2_25_25",
                         "3_10_10_10",
                    }
               },
               {
                    opt = "--learning_rate",
                    values =
                    {
                         1e-2, 1e-4, 1e-6, 1e-8
                    }
               },
               {
                    opt = "--epoch_range",
                    values =
                    {
                         5, 10, 25, 50
                    }
               },
               {
                    opt = "--n_mdps",
                    values =
                    {
                         500, 1000, 2500, 5000
                    }
               },
               {
                    opt = "--horizon_limit",
                    values = { "$T" }
               }
          },
          olOptions =
          {
               "--decreasing_learning_rate",
               "--max_epoch 1000000",
               "--base_agent OptimalAgent",
               "--discount_factor 0.95",
               "--model_file \"data/agents/$PRIOR-$AGENT_STR(model).dat\"",
               "--compress_output"
          },
          memory = { ol = "2000M", re = "1000M" },
          duration = { ol = "24:00:00", re = "01:00:00" }
     }
}


return agents
