
-- ============================================================================
--   This script describes the options to use when executing the scripts on
--   SLURM machine(s).
--
--   A SLURM configuration is described as follows:
--
--   ...
--   -- An identifier to use as a parameter of the 'run.sh' script
--   -- (when creating the scripts of commands)
--   <SLURM NAME>
--   {
--        name = "SLURM",
--
--        -- The user name on the SLURM machine
--        user = "<user name>",
--
--        -- The path to the libraries
--        libPath = "<library path>",
--
--        -- The path to 'TinyBRL' folder
--        path = "<TinyBRL path>",
--
--        -- The number of cores to reserve for each job
--        nbCores = <number of core per job>,
--
--        submitCommand = "sbatch"
--   },
--   ...
-- ============================================================================
local gridComputing =
{
     SLURM_TEST =
     {
          name = "SLURM",
          user = "user",
          libPath = "$LD_LIBRARY_PATH",
          path = "/home/user/TinyBRL/",
          nbCores = 1,
          submitCommand = "sbatch"
     }
}

return gridComputing
