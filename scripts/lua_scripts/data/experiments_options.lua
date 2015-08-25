
-- ============================================================================
--   This script describes the options to use when performing an experiment.
--
--   ...
--   -- The number of threads to use
--   "--n_threads <number of threads>",
--
--   -- If specified, compress the output files
--   "--compress_output",
--
--   -- If specified, ensures that the agents do not have access to the
--   -- transition matrix of the unknown MDP
--   -- (shouldn't be used with 'OptimalAgent')
--   "--safe_simulations",
--
--   -- Defines the frequency at which the progression of the current
--   -- experiment is displayed (in seconds)
--   "--refresh_frequency <refresh frenquency>"
--
--   -- Defines the frequency at which a backup file is created (in seconds)
--   "--backup_frequency <backup frequency>"
-- ============================================================================
local experiments_options =
{
     "--n_threads 1",
     "--compress_output",
     "--safe_simulations",
     "--refresh_frequency 60",
     "--backup_frequency 900"
}


return experiments_options
