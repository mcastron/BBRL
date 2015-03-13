
#include "xport.h"

using namespace std;
using namespace utils;


// ===========================================================================
//	Functions
// ===========================================================================
vector<xport::AgentData> xport::getAgentDataList(int argc, char* argv[])
{
     vector<xport::AgentData> agentDataList;


     //   1.   Parse the Agents
     int iAgent = 0;
     while (true)
     {
          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
               ++iAgent;
   
          if ((iAgent + 3) >= (argc - 1)) { break; }
   
          int argcBis = 5;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iAgent++];
          argvBis[2] = argv[iAgent++];
          argvBis[3] = argv[iAgent++];
          argvBis[4] = argv[iAgent++];


          Agent* agent;
          try { agent = Agent::parse(argcBis, argvBis); }
          catch (parsing::ParsingException&) { delete[] argvBis; break; }
          delete[] argvBis;

          
          xport::AgentData agentData;
          agentData.loadAgent(agent);
          agentDataList.push_back(agentData);
          
          delete agent;
     }
     
     
     //   2.   Parse the Experiments
     int iExp = 0, nExp = 0;
     while (iExp < (argc - 1))
     {
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               ++iExp;
          
          if ((iExp + 1) >= (argc - 1)) { break; }
          
          int argcBis = 4;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iExp++];
          argvBis[2] = argv[iExp++];
          argvBis[3] = argv[iExp++];


          Experiment* exp;
          try { exp = Experiment::parse(argcBis, argvBis); }
          catch (parsing::ParsingException&) { delete[] argvBis; break; }
          delete[] argvBis;

          
          agentDataList[nExp].loadExperiment(exp);
          ++nExp;
          
          delete exp;
     }
     
     
     //   3.   Return
     return agentDataList;
}


vector<xport::AgentData> xport::filter(
                                const vector<xport::AgentData>& agentDataList,
                                xport::StrField field,
                                xport::StrFilterCondition condition,
	                           string cStr)
{
     vector<xport::AgentData> filteredList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          const xport::AgentData& data = agentDataList[i];
          string fieldV = data.getField(field);
          switch (condition)
          {
               case STR_EQUAL:
                    if (fieldV == cStr) { filteredList.push_back(data); }
                    break;

               case STR_NOT_EQUAL:
                    if (fieldV != cStr) { filteredList.push_back(data); }
                    break;

               case STR_SUBSTR:
                    if (fieldV.find(cStr) != std::string::npos)
                         filteredList.push_back(data);
                    break;

               case STR_NOT_SUBSTR:
                    if (fieldV.find(cStr) == std::string::npos)
                         filteredList.push_back(data);
                    break;
          }
     }
     return filteredList;
}


vector<string> xport::getList(const vector<xport::AgentData>& agentDataList,
                              xport::StrField field)
{
     vector<string> list;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          const xport::AgentData& data = agentDataList[i];
          string fieldV = data.getField(field);
          
          bool found = false;
          for (unsigned int j = 0; j < list.size(); ++j)
               if (fieldV == list[j]) { found = true; break; }
          
          if (!found) { list.push_back(fieldV); }
     }
     return list;
}


vector<xport::AgentData> xport::filter(
                                const vector<xport::AgentData>& agentDataList,
                                xport::DblField field,
	                           xport::DblFilterCondition condition,
	                           double value)
{
     vector<xport::AgentData> filteredList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          const xport::AgentData& data = agentDataList[i];
          double fieldV = data.getField(field);
          switch (condition)
          {
               case DBL_BELOW:
                    if (fieldV < value)  { filteredList.push_back(data); }
                    break;

               case DBL_BELOW_EQUAL:
                    if (fieldV <= value) { filteredList.push_back(data); }
                    break;

               case DBL_EQUAL:
                    if (fieldV == value) { filteredList.push_back(data); }
                    break;

               case DBL_ABOVE_EQUAL:
                    if (fieldV >= value) { filteredList.push_back(data); }
                    break;

               case DBL_ABOVE:
                    if (fieldV > value)  { filteredList.push_back(data); }
                    break;
          }
     }
     return filteredList;
}


int xport::getMin(const vector<xport::AgentData>& agentDataList,
                           xport::DblField field)
{
     int min = -1;
     double minV;
     for (unsigned int i = 0; i < agentDataList.size(); i++)
     {
          const xport::AgentData& data = agentDataList[i];
          double fieldV = data.getField(field);
          
          if ((min == -1) || (fieldV < minV))
          {
               min = i;
               minV = fieldV;
          }
     }
     return min;
}


int xport::getMax(const vector<AgentData>& agentDataList,
                           xport::DblField field)
{
     int max = -1;
     double maxV;
     for (unsigned int i = 0; i < agentDataList.size(); i++)
     {
          const xport::AgentData& data = agentDataList[i];
          double fieldV = data.getField(field);
          
          if ((max == -1) || (fieldV > maxV))
          {
               max = i;
               maxV = fieldV;
          }
     }
     return max;
}


vector<xport::AgentData> xport::sort(
                              const vector<xport::AgentData>& agentDataList,
                              xport::DblField field, bool ascendingOrder)
{
     vector<xport::AgentData> sortedList, cAgentDataList = agentDataList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          unsigned int s;
          if (ascendingOrder) { s = xport::getMin(cAgentDataList, field); }
          else                { s = xport::getMax(cAgentDataList, field); }
          
          sortedList.push_back(cAgentDataList[s]);
          cAgentDataList[s] = cAgentDataList.back();
          cAgentDataList.pop_back();
     }
     return sortedList;
}
