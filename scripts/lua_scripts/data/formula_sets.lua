
-- ============================================================================
--   This script describes the formula sets to generate for OPPS agents.
--
--   A formula set is described as follows:
--
--   ...
--   {
--        -- The name of the formula set
--        name = "<name>",
--
--        -- The number of variables to consider
--        nVariables = <number of variables>,
--
--        -- The list of tokens to consider (operators and constants)
--        tokens =
--        {
--             "<token1>", ..., "<tokenk>"
--        },
--
--        -- The maximal size of a formula
--        -- (= the maximal number of elements to be combined)
--        maxSize = <maximal size of a formula>,
--
--        -- The number of points to consider when comparing two formula.
--        -- More points you define, less likely the program will consider
--        -- two different formula to be equivalent.
--        nPoints = <number of points>,
--
--        -- The ranges of values taken by the formulas
--        -- (used to discard formulas which will perform forbidden operations)
--        pointsRange = { <min value>, <max value> }
--   
--   },
--   ...
-- ============================================================================
local formula_sets =
{
     --   F2 --------------------------------------------------------------
     {
          name = "F2",
          nVariables = 3,
          tokens = { "ABS", "LN", "SQRT", "INV", "OPP",
                     "SUB", "DIV", "ADD2", "MUL2", "MIN2", "MAX2" },
          maxSize = 2,
          nPoints = 1000,
          pointsRange = { -100.0, 100.0 }
     },

     --   F3 --------------------------------------------------------------
     {
          name = "F3",
          nVariables = 3,
          tokens = { "ABS", "LN", "SQRT", "INV", "OPP",
                     "SUB", "DIV", "ADD2", "MUL2", "MIN2", "MAX2" },
          maxSize = 3,
          nPoints = 1000,
          pointsRange = { -100.0, 100.0 }
     },

     --   F4 --------------------------------------------------------------
     {
          name = "F4",
          nVariables = 3,
          tokens = { "ABS", "LN", "SQRT", "INV", "OPP",
                     "SUB", "DIV", "ADD2", "MUL2", "MIN2", "MAX2" },
          maxSize = 4,
          nPoints = 1000,
          pointsRange = { -100.0, 100.0 }
     },

     --   F5 --------------------------------------------------------------
     {
          name = "F5",
          nVariables = 3,
          tokens = { "ABS", "LN", "SQRT", "INV", "OPP",
                     "SUB", "DIV", "ADD2", "MUL2", "MIN2", "MAX2" },
          maxSize = 5,
          nPoints = 1000,
          pointsRange = { -100.0, 100.0 }
     },

     --   F6 --------------------------------------------------------------
     {
          name = "F6",
          nVariables = 3,
          tokens = { "ABS", "LN", "SQRT", "INV", "OPP",
                     "SUB", "DIV", "ADD2", "MUL2", "MIN2", "MAX2" },
          maxSize = 6,
          nPoints = 1000,
          pointsRange = { -100.0, 100.0 }
     }
}


return formula_sets
