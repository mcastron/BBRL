BBRL
=======
![alt tag](https://travis-ci.org/mcastron/BBRL.svg)

BBRL (**B**enchmarking tools for **B**ayesian **R**einforcement **L**earning) is a C++ open-source library used to compare Bayesian reinforcement learning algorithms.
For more information about my research, visit my [website](http://www.montefiore.ulg.ac.be/~mcastron/index.html).

1.4
 - Bug fix: 'SBOSSAgent' parsing was parsing '--delta' by using '--C' flag instead.
 - Bug fix: 'IExperiment' was not measuring the time elapsed anymore.
 - Bug fix: 'FormulaAgentFactory' was not deserialized correctly.
 - Minor fix: 'OPPSCSAgent' output name had a typing error.
 - Minor fix: 'IExperiment:checkIntegrity()' was not implemented correctly.
 - Added 'Formula:getStr()' method (used to represent 'nicely' the formula when exported in Latex, Gnuplot, ...). 'FormulaAgent:getName()' modified to use this function instead of 'Formula:getRPNStr()'.
 - Minor changes in Agents name ('getName()' methods).
 - Added method 'Agent::getExportName()' and ’Agent::getExportClassName()'.
 - 'xport' namespace added: provide methods to load, filter and sort the results of the experiments.
 - 'utils::gnuplot' namespace added: provide methods to create gnuplot graphs.
 - 'utils::latex' namespace added: provide methods to create latex tables.
 - 'BBRL-export' has drastically changed. The user will only provide all the data, and BBRL-export will create a neat Latex report. More information on the corresponding wiki page.

1.3.2
 - Minor fix: 'FormulaAgentFactory' was printing out the tested formula.
 - Bug fix: 'FormulaAgentFactory' was not registered with 'Serializable::checkIn(...)' (which is necessary during the deserialization of an 'OPPSCSAgent').

1.3.1
 - 'BBRL-DDS' will stop if the output file already exists (useful not to perform the same tests several times).
 - 'BBRL-DDS' detects if a backup already exists when running an experiment (based on output file name) and starts from it if it exists.
 - Added 'pause()' and 'resume()' methods to 'Chrono'.
 - It is now possible to restart a 'Chrono' as paused (see 'Chrono::restart()').
 - Added the flag 'saveTraj' in the constructor of 'IExperiment' and 'dds::Experiment', allowing to enable/disable the saving of the trajectories.
 - Added 'getRList()' method to 'IExperiment'.
 - Added 'computeDSRList()' in 'rl_utils::simulation'.
 - Added 'src/RLUtils/CMakeLists.txt' and 'src/RLUtils/src/rl_utils.cpp'
 - Added option '--save_trajectories' to 'BBRL-DDS' (for '--new_experiment').
 - Added option '--variables' to 'FormulaAgent' and 'OPPSDSAgent', which allows to select which variable to use in the formulas.
 - Added 'FormulaAgentFactory' (for searching through polynomial formulas).
 - Clarification about the call of 'init()' method after calling 'deserialize()' for 'EGreedyAgentFactory', 'SoftMaxAgentFactory' & 'VDBEEGreedyAgentFactory' (see headers).
 - Handle properly exceptions in 'utils::UCT', 'dds::opps::UCT',  'dds::OPPSCSAgent::learnOffline_aux()', 'dds::OPPSDSAgent::learnOffline_aux()' and in 'main.cpp'.
 - Added 'getNbParam()' method to 'AgentFactory'.
 - Added namespace 'algorithm' in namespace 'utils'.
 - Moved 'utils::UCB1' & 'utils::UCT' in namespace 'utils::algorithm'
 - Implemented Sto-SOO algorithm in 'utils::algorithm::StoSOO'.
 - Added option '--seed' to 'BBRL-DDS' (and 'BBRL-custom'), allowing to choose the seed used by the RNG.
 - 'OPPSCSAgent' now uses 'StoSOO' optimization algorithm instead of 'UCT' (command-line options have been updated appropriately).
 - Updated the documentation file of 'BBRL-DDS'.

1.3
 - Added support of gnuplot output in 'main-export.cpp'.
 - Updated the documentation file of 'BBRL-export'.
 - Updated the documentation file of 'BBRL-DDS'.
 - Added 'BFS3Agent’, 'SBOSSAgent' and ‘BEBAgent’.
 - Added parameter '--D' for BAMCPAgent.
 - Corrected bug in 'MDPUtils::valueIteration()' (wrong call of ‘resize()’ for vector 'Q').

1.2.3
 - Moved 'FormulaVector' in 'utils::formula'.
 - Minor changes in headers' comments to fit Doxygen syntax.

1.2.2
 - Minor change in ‘BBRL-DDS’ and ‘BBRL-export’ manuals.

1.2.1
 - Minor change in ‘BBRL-DDS’ manual.

1.2
 - Switch 'MDPException' to 'AgentException' in Agents (and its derived classes)
 - Add 'AgentException' and 'MDPException' to 'dds::simulate()' function.
 - Allow the UCB1 algorithm to discard any arm for which an std::exception has been thrown when it has been drawn.
 - Added an optional method 'freeData()' to the Agents. This method will be called when the Agent has finished to interact with the current MDP, offering the opportunity to free unnecessary data (useful when dealing with OPPS-DS which has to keep an instance of all Agents of the set of candidate strategies).
 - Revision on error case in 'learningOffline_aux()' of several agents (when an Agent only support Dirchlet distributions as a prior).
 - Options '--backup_frequency' and '--refresh_frequency' are expressed in seconds (it was in milliseconds previously).
 - Move 'Formula' package into 'utils' package ('Formula' is not serializable anymore).
 - Minor change in agents' names for 'OPPSDSAgent' & 'OPPSCSAgent'.
 - Split the variables used by 'FormulaAgent' in indepedent classes: 'QMean', 'QSelf', 'QUniform' (integrated in dds namespace).

1.1
 - Remove 'counters' parameter for:    EGreedyAgentFactory/SoftMaxAgentFactory/VDBEEGreedyAgentFactory. Those AgentFactory uses the mean model of the distribution used to initialize them instead of generating it through counters.
 - Add 'Formula' package. Provide management of formulas, obtained by combining operators, variables and constants (see 'Formula.h' for more details).
 - Complete header description for pointer management (if an object is passed by pointer, the destructor description will state if this object will be released in the process).
 - Modfied how the time is rendered in latex tables generated by 'BBRL-export' (the time is rounded to make reading easier).
 - FormulaAgent added (only support formulas using 3 variables).
 - Add a new mode to 'BBRL-DDS' (--formula_set_generation). Used to generate a set of formulas (e.g. for FormulaAgent).
 - OPPS-DS modified to identify the best agent in a discrete set of FormulaAgent (defined through a set of formulas using 3 variables).

1.0.1
 - Memory optimization for 'BBRL-export'.
 - Minor bug fixed in Agents names. (BAMCPAgent/EGreedyAgent/SoftMaxAgent/VDBEEGreedyAgent)
 - Add support of short names for MDPDistributions (IMDPDistribution, MDPDistribution, DirMultiDistribution).
 - Add usage of short distribution names in AgentFactories. (EGreedyAgentFactory/SoftMaxAgentFactory/VDBEEGreedyAgentFactory)

1.0
 - Initial release
