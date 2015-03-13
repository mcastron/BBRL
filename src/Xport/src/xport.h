
#ifndef XPORT_H
#define XPORT_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	xport
	\author 		Castronovo Michael
	
	\brief		A collection of general tools for exporting results.
	
	\date		2015-03-13
*/
// ===========================================================================
namespace xport
{
     // ======================================================================
     //   Enum definitions
     // ======================================================================
     /**
          \enum     StrField
          \brief    Refer to a string field of 'AgentData'.
     */
     enum StrField
     {
          CLASS_NAME, NAME, EXP_NAME
     };


     /**
          \enum     StrFilterCondition
          \brief    A filtering condition on a string field of 'AgentData'.
     */
     enum StrFilterCondition
     {
          STR_EQUAL, STR_NOT_EQUAL, STR_SUBSTR, STR_NOT_SUBSTR
     };


     /**
          \enum     DblField
          \brief    Refer to a double field of 'AgentData'.
     */
     enum DblField
     {
          OFFLINE_TIME, ONLINE_TIME, MEAN, LOWER_BOUND, UPPER_BOUND
     };


     /**
          \enum     DblFilterCondition
          \brief    A filtering condition on a double field of 'AgentData'.
     */
     enum DblFilterCondition
     {
          DBL_BELOW, DBL_BELOW_EQUAL, DBL_EQUAL, DBL_ABOVE_EQUAL, DBL_ABOVE
     };


     // ======================================================================
     //   Classes
	// ======================================================================
	/**
          \class    AgentData
          \brief    Store the data related to an agent tested on an
                    experiment.
	*/
	class AgentData
     {
          public:
               // ============================================================
               //   Public Constructor
               // ============================================================
               /**
                    \brief    Constructor
               */
               AgentData() : dsrList(std::vector<double>()) {}     
               
               
               // ============================================================
               //   Public methods
               // ============================================================
               /**
                    \brief         Load the data of 'agent'.
                    
                    \param[agent   The data to load.
               */
               void loadAgent(const Agent* agent)
               {
                    className   = agent->getExportClassName();
                    name        = agent->getExportName();
                    offlineTime = agent->getOfflineTime();
               }
               
               
               /**
                    \brief         Load the data of 'exp'.
                    
                    \param[exp     The data to load.
               */
               void loadExperiment(const Experiment* exp)
               {
                    expName     = exp->getName();
                    
                    onlineTime  = (exp->getTimeElapsed()
                                  / (double) exp->getNbOfMDPs());

                    dsrList = exp->computeDSRList();

                    pair<double, double> CI95;
                    CI95 = utils::statistics::computeCI95<double>(dsrList);

                    mean       = ((CI95.first + CI95.second) / 2.0);
                    lowerBound = CI95.first;
                    upperBound = CI95.second;
               }
               
               
               /**
                    \brief    Return the DSR list.
                    
                    \return   The DSR list.
               */
               std::vector<double> getDSRList() const { return dsrList; }


               /**
                    \brief    Return the value of field 'field'.

                    \return   The value of field 'field.
               */
               std::string getField(StrField field) const
               {
                    switch (field)
                    {
                         case CLASS_NAME: return className;
                         case NAME:       return name;
                         case EXP_NAME:   return expName;
                    }
               }


               /**
                    \brief    Return the value of field 'field'.

                    \return   The value of field 'field.
               */
               double getField(DblField field) const
               {
                    switch (field)
                    {
                         case OFFLINE_TIME: return offlineTime;
                         case ONLINE_TIME:  return onlineTime;
                         case MEAN:         return mean;
                         case LOWER_BOUND:  return lowerBound;
                         case UPPER_BOUND:  return upperBound;
                    }
               }


          private:
               // ============================================================
               //   Private attributes
               // ============================================================
               /**
                    \brief    The class, agent and experiment names.
               */
               std::string className, name, expName;

               
               /**
                    \brief    The offline/online cpu times.
               */
               double offlineTime, onlineTime;


               /**
                    \brief    The list of discounted sum of rewards.
               */
               std::vector<double> dsrList;


               /**
                    \brief    The mean score discounted sum of rewards observed.
               */
               double mean;


               /**
                    \brief    The lower/upper bound on 'mean' (CI at 95%).
               */
               double lowerBound, upperBound;
     };


     // ======================================================================
     //   Functions
	// ======================================================================
	/**
          \brief         Return the data of the agents referenced by 'argc' and
                         'argv'.

          \param[argc    The size of 'argv'.
          \param[argv    The parameters' list.
          
          \return        The data of the agents referenced by 'argc' and 'argv'.
	*/
	std::vector<AgentData> getAgentDataList(int argc, char* argv[]);


     /**
          \brief         Return the list of 'AgentData's values taken by the
                         given field (no repetition).

          \param[field   A string field.
          
          \return        The list of 'AgentData' values taken by the given
                         field (no repetition).
     */
     std::vector<std::string> getList(
               const std::vector<AgentData>& agentDataList, StrField field);


     /**
          \brief               Filter the 'AgentData's by returning those whose
                               field 'field' matches the condition 'condition'
                               when compared to 'cStr'.

          \param[agentDataList The data to filter.
          \param[field         The field to analyse.
          \param[condition     The condition to check.
          \param[cStr          The comparison value.
          
          \return              Return the 'AgentData' whose field 'field'
                               matches the condition 'condition' when compared
                               to 'cStr'.
     */
     std::vector<AgentData> filter(const std::vector<AgentData>& agentDataList,
	                              StrField field,
	                              StrFilterCondition condition,
	                              string cStr);


     /**
          \brief               Filter the 'AgentData's by returning those whose
                               field 'field' matches the condition 'condition'
                               when compared to 'cStr'.

          \param[agentDataList The data to filter.
          \param[field         The field to analyse.
          \param[condition     The condition to check.
          \param[value         The comparison value.
          
          \return              Return the 'AgentData' whose field 'field'
                               matches the condition 'condition' when compared
                               to 'value'.
     */
     std::vector<AgentData> filter(const std::vector<AgentData>& agentDataList,
                                   DblField field,
	                              DblFilterCondition condition, double value);


     /**
          \brief               Return the index of the 'AgentData's minimizing 
                               field 'field', '-1' if 'agentDataList' is empty.

          \param[agentDataList The data.
          \param[field         The field to check.

          \return              The index of the 'AgentData' minimizing field
                               'field'.    
     */
     int getMin(const std::vector<AgentData>& agentDataList, DblField field);


     /**
          \brief               Return the index of the 'AgentData's maximizing 
                               field 'field', '-1' if 'agentDataList' is empty.

          \param[agentDataList The data.
          \param[field         The field to check.

          \return              The index of the 'AgentData' maximizing field
                               'field'.    
     */
     int getMax(const std::vector<AgentData>& agentDataList, DblField field);


     /**
          \brief                Return a sorted vector of the 'AgentData's
                                with respect to field 'field'.
                                (complexity: O(n^2))

          \param[agentDataList  The data.
          \param[field          The field to check.
          \param[ascendingOrder Sort in ascending if true, in descending order
                                else.
     */
     std::vector<AgentData> sort(const std::vector<AgentData>& agentDataList,
                                 DblField field, bool ascendingOrder = true);
}

#endif
