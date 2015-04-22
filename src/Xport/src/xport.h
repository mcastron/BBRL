
#ifndef XPORT_H
#define XPORT_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	xport
	\author 		Castronovo Michael
	
	\brief		A collection of general tools for exporting results.
	
	\date		2015-04-14
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
         \class    AgentDataException
         \brief    Exception
    */
    class AgentDataException: public std::exception
     {
     	public:
     		// ============================================================
     		//	Public Constructors/Destructors
     		// ============================================================
     		/**
     			\brief 		Constructor.
     			
     			\param[opt	The option concerned by the error.
     		*/
     		AgentDataException(std::string opt)
     		{
                    msg = "Error while parsing option \'" + opt + "\'\n";
     		}
     		
     		
     		/**
     		     \brief    Destructor.
     		*/
     		virtual ~AgentDataException() throw() {}
     
     
     		// ============================================================
     		//	Public methods
     		// ============================================================
     		/**
     			\brief		Returns the error message.
     			
     			\return		The error messIAge.
     		*/
     		virtual const char* what() const throw()
     		{
     		   return msg.c_str();
     		}
     	
     	
     	private:
     		// ============================================================
     		//	Private attributes
     		// ============================================================
     		/**
     			\brief	The error message.
     		*/
     		std::string msg;
     };

	
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
               AgentData() : agentLoaded(false), expLoaded(false),
                             dsrList(std::vector<double>()) {}     
               
               
               // ============================================================
               //   Public methods
               // ============================================================
               /**
                    \brief         Load the data of 'agent'.
                    
                    \param[agent   The data to load.
               */
               void loadAgent(const Agent* agent) throw (AgentDataException)
               {
                    if (agentLoaded)
                    {
                         string msg;
                         msg += "An agent has already been loaded in this ";
                         msg += "'AgentData'!";
                         
                         throw AgentDataException(msg);
                    }

                    
                    className   = agent->getExportClassName();
                    name        = agent->getExportName();
                    offlineTime = agent->getOfflineTime();
                    
                    agentLoaded = true;
               }


               /**
                    \brief         Load the data of 'exp'.
                    
                    \param[exp     The data to load.
               */
               void loadExperiment(const Experiment* exp)
                                                  throw (AgentDataException)
               {
                    if (expLoaded)
                    {
                         string msg;
                         msg += "An experiment has already been loaded in ";
                         msg += "this 'AgentData'!";
                         
                         throw AgentDataException(msg);
                    }
                    
                    
                    expName     = exp->getName();
                    
                    onlineTime  = (exp->getTimeElapsed()
                                  / (double) exp->getNbOfMDPs());

                    dsrList = exp->computeDSRList();

                    pair<double, double> CI95;
                    CI95 = utils::statistics::computeCI95<double>(dsrList);

                    mean       = ((CI95.first + CI95.second) / 2.0);
                    lowerBound = CI95.first;
                    upperBound = CI95.second;
                    
                    expLoaded = true;
               }
               
               
               /**
                    \brief    Return true if an agent has already been loaded,
                              false else.

                    \return   True if an agent has alread been loaded, false
                              else.
               */
               bool isAgentLoaded() const { return agentLoaded; }

               
               /**
                    \brief    Return true if an experiment has already been
                              loaded, false else.

                    \return   True if an experiment has already been loaded,
                              false else.
               */
               bool isExperimentLoaded() const { return expLoaded; }
               
               
               /**
                    \brief    Return the DSR list.
                    
                    \return   The DSR list.
               */
               std::vector<double> getDSRList() const
                                                  throw (AgentDataException)
               {
                    if (!agentLoaded)
                    {
                         string msg;
                         msg += "An agent has not been loaded yet!";
                         
                         throw AgentDataException(msg);
                    }
                    
                    if (!expLoaded)
                    {
                         string msg;
                         msg += "An experiment has not been loaded yet!";
                         
                         throw AgentDataException(msg);
                    }


                    return dsrList;
               }


               /**
                    \brief    Return the value of field 'field'.

                    \return   The value of field 'field.
               */
               std::string getField(StrField field) const
                                                  throw (AgentDataException)
               {
                    if (!agentLoaded)
                    {
                         string msg;
                         msg += "An agent has not been loaded yet!";
                         
                         throw AgentDataException(msg);
                    }
                    
                    if (!expLoaded)
                    {
                         string msg;
                         msg += "An experiment has not been loaded yet!";
                         
                         throw AgentDataException(msg);
                    }

                    
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
                                                  throw (AgentDataException)
               {
                    if (!agentLoaded)
                    {
                         string msg;
                         msg += "An agent has not been loaded yet!";
                         
                         throw AgentDataException(msg);
                    }
                    
                    if (!expLoaded)
                    {
                         string msg;
                         msg += "An experiment has not been loaded yet!";
                         
                         throw AgentDataException(msg);
                    }

                    
                    switch (field)
                    {
                         case OFFLINE_TIME: return offlineTime;
                         case ONLINE_TIME:  return onlineTime;
                         case MEAN:         return mean;
                         case LOWER_BOUND:  return lowerBound;
                         case UPPER_BOUND:  return upperBound;
                    }
               }
               
               
               /**
                    \brief    Return true if 'x' is the same AgentData,
                              false else.
                    
                    \param[x  An AgentData
                    
                    \return   True if 'x' is the same AgentData, false else.
               */
               bool operator==(const AgentData& x) const
               {
                    bool sameDSR = true;
                    if (dsrList.size() != (x.dsrList).size())
                         sameDSR = false;

                    else
                    {
                         for (unsigned int i = 0; i < dsrList.size(); ++i)
                         {
                              if (dsrList[i] != (x.dsrList)[i])
                              {
                                   
                                   sameDSR = false;
                                   break;
                              }
                         }
                    }
                    
                    return (className == x.className
                              && name == x.name
                              && expName == x.expName
                              && offlineTime == x.offlineTime
                              && onlineTime == x.onlineTime
                              && sameDSR
                              && mean == x.mean
                              && lowerBound == x.lowerBound
                              && upperBound == x.upperBound);
               }
               


          private:
               // ============================================================
               //   Private attributes
               // ============================================================
               /**
                    \brief    True if an agent has been loaded, false else.
               */
               bool agentLoaded;
               
               
               /**
                    \brief    True if an experiment has been loaded, false
                              else.
               */
               bool expLoaded;


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


     // -----------------------------------------------------------------------
     //   getList()
     // -----------------------------------------------------------------------
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
          \brief         Return the list of 'AgentData's values taken by the
                         given field (no repetition).
                         
                         Limited to the 'AgentData's whose indexes are in
                         'list'.

          \param[list    The indexes of the 'AgentData's to consider.
                         (optional)
          \param[field   A string field.
          
          \return        The list of 'AgentData' values taken by the given
                         field (no repetition).
     */
     std::vector<std::string> getList(
               const vector<unsigned int>& list,
               const std::vector<AgentData>& agentDataList, StrField field);


     // -----------------------------------------------------------------------
     //   filter()
     // -----------------------------------------------------------------------
     /**
          \brief               Filter the 'AgentData's by returning the indexes
                               of those whose field 'field' matches the
                               condition 'condition' when compared to 'cStr'.

          \param[agentDataList The data to filter.
          \param[field         The field to analyse.
          \param[condition     The condition to check.
          \param[cStr          The comparison value.
          
          \return              Return the indexes of those whose field 'field'
                               matches the condition 'condition' when compared
                               to 'cStr'.
     */
     std::vector<unsigned int> filter(
               const std::vector<AgentData>& agentDataList,
	          StrField field, StrFilterCondition condition, string cStr);


     /**
          \brief               Filter the 'AgentData's by returning the indexes
                               of those whose field 'field' matches the
                               condition 'condition' when compared to 'cStr'.
                               
                               Limited to the 'AgentData's whose indexes are in
                               'list'.

          \param[list          The indexes of the 'AgentData's to consider.
          \param[agentDataList The data to filter.
          \param[field         The field to analyse.
          \param[condition     The condition to check.
          \param[cStr          The comparison value.
          
          \return              Return the indexes of those whose field 'field'
                               matches the condition 'condition' when compared
                               to 'cStr'.
     */
     std::vector<unsigned int> filter(
               const vector<unsigned int>& list,
               const std::vector<AgentData>& agentDataList,
	          StrField field, StrFilterCondition condition, string cStr);


     /**
          \brief               Filter the 'AgentData's by returning the indexes
                               of those whose field 'field' matches the
                               condition 'condition' when compared to 'value'.

          \param[agentDataList The data to filter.
          \param[field         The field to analyse.
          \param[condition     The condition to check.
          \param[value         The comparison value.
          
          \return              Return the indexes of those whose field 'field'
                               matches the condition 'condition' when compared
                               to 'value'.
     */
     std::vector<unsigned int> filter(
               const std::vector<AgentData>& agentDataList,
               DblField field, DblFilterCondition condition, double value);


     /**
          \brief               Filter the 'AgentData's by returning the indexes
                               of those whose field 'field' matches the
                               condition 'condition' when compared to 'value'.

                               Limited to the 'AgentData's whose indexes are in
                               'list'.

          \param[list          The indexes of the 'AgentData's to consider.
          \param[agentDataList The data to filter.
          \param[field         The field to analyse.
          \param[condition     The condition to check.
          \param[value         The comparison value.
          
          \return              Return the indexes of those whose field 'field'
                               matches the condition 'condition' when compared
                               to 'value'.
     */
     std::vector<unsigned int> filter(
               const vector<unsigned int>& list,
               const std::vector<AgentData>& agentDataList,
               DblField field, DblFilterCondition condition, double value);


     // -----------------------------------------------------------------------
     //   getMin()
     // -----------------------------------------------------------------------
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
          \brief               Return the index of the 'AgentData's minimizing 
                               field 'field', '-1' if 'agentDataList' is empty.
                               
                               Limited to the 'AgentData's whose indexes are in
                               'list'.

          \param[list          The indexes of the 'AgentData's to consider.
          \param[agentDataList The data.
          \param[field         The field to check.

          \return              The index of the 'AgentData' minimizing field
                               'field'.    
     */
     int getMin(const vector<unsigned int>& list,
                const std::vector<AgentData>& agentDataList, DblField field);
     
     
     /**
          \brief    DO NOT USE (memory optimization for 'sort()').
     */
     int getMin(const std::vector<const AgentData*>& agentDataList,
                DblField field);



     // -----------------------------------------------------------------------
     //   getMax()
     // -----------------------------------------------------------------------
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
          \brief               Return the index of the 'AgentData's maximizing 
                               field 'field', '-1' if 'agentDataList' is empty.
                               
                               Limited to the 'AgentData's whose indexes are in
                               'list'.

          \param[list          The indexes of the 'AgentData's to consider.
          \param[agentDataList The data.
          \param[field         The field to check.

          \return              The index of the 'AgentData' maximizing field
                               'field'.    
     */
     int getMax(const vector<unsigned int>& list,
                const std::vector<AgentData>& agentDataList, DblField field);
     
     
     /**
          \brief    DO NOT USE (memory optimization for 'sort()').
     */
     int getMax(const std::vector<const AgentData*>& agentDataList,
                DblField field);



     // -----------------------------------------------------------------------
     //   sort()
     // -----------------------------------------------------------------------
     /**
          \brief                Return a sorted vector of the index of the
                                'AgentData's with respect to field 'field'.
                                (complexity: O(n^2))

          \param[agentDataList  The data.
          \param[field          The field to check.
          \param[ascendingOrder Sort in ascending if true, in descending order
                                else.
     */
     std::vector<unsigned int> sort(
               const std::vector<AgentData>& agentDataList,
               DblField field, bool ascendingOrder = true);


     /**
          \brief                Return a sorted vector of the index of the
                                'AgentData's with respect to field 'field'.
                                (complexity: O(n^2))
                                
                                Limited to the 'AgentData's whose indexes are
                                in 'list'.

          \param[list           The indexes of the 'AgentData's to consider.
          \param[agentDataList  The data.
          \param[field          The field to check.
          \param[ascendingOrder Sort in ascending if true, in descending order
                                else.
     */
     std::vector<unsigned int> sort(
               const std::vector<unsigned int>& list,
               const std::vector<AgentData>& agentDataList,
               DblField field, bool ascendingOrder = true);
}

#endif
