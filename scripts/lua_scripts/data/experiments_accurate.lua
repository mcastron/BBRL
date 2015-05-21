
-- ============================================================================
--   This script describes the experiments to perform for the 'accurate' case
--   (prior distribution = test distribution)
--
--   An experiment is described as follows:
--
--   ...
--   {
--        -- Short name of the prior distribution
--        prior = "<prior name>",
--
--        -- Prior distribution file in 'data/distributions/'
--        priorFile = "<prior file>",
--
--        -- Short name of the test distribution
--        exp = "<exp name>",
--
--        -- Test distribution file in 'data/distributions/'
--        expFile = "<test file>",
--
--        -- The number of MDPs to draw from the test distribution
--        N = <number of MDPs>,
--
--        -- The discount factor
--        gamma = <discount factor>,
--
--        -- The horizon limit
--        T = <horizon limit>
--   },
--   ...
--
--
--   You also have to associate a short name to this set of experiments
--
--   shortName = <short name>
-- ============================================================================
local experiments =
{
     {
          prior = "GC",   priorFile = "GC-distrib.dat",
          exp   = "GC",   testFile  = "GC-distrib.dat",
          N = 500, gamma = 0.95, T = 250
     },
     {
          prior = "GDL",  priorFile = "GDL-distrib.dat",
          exp   = "GDL",  testFile  = "GDL-distrib.dat",
          N = 500, gamma = 0.95, T = 250
     },
     {
          prior = "Grid", priorFile = "Grid-distrib.dat",
          exp   = "Grid", testFile  = "Grid-distrib.dat",
          N = 500, gamma = 0.95, T = 250
     },
     
     shortName = "accurate"
}


return experiments
 