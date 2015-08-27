
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
--        -- The path to 'BBRL' folder
--        path = "<BBRL path>",
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
     SLURM_CECI =
     {
          name = "SLURM",
          user = "mcastron",
          libPath = "$LD_LIBRARY_PATH:/u/mcastron/lib",
          path = "/home/ulg/sysmod/mcastron/home/BBRL/",
          nbCores = 1,
          submitCommand = "sbatch"
     },

     SLURM_AZURA =
     {
          name = "SLURM",
          user = "kira",
          libPath = "$LD_LIBRARY_PATH",
          path = "/home/kira/BBRL/",
          nbCores = 1,
          submitCommand = "sbatch"
     }
}

return gridComputing
