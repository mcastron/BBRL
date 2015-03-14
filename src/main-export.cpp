
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"
#include "Xport/XportLib.h"

using namespace std;
using namespace dds;
using namespace utils;
using namespace utils::gnuplot;
using namespace utils::latex;
using namespace utils::statistics;
using namespace xport;


// ===========================================================================
/**	
	\brief	A simple command-line program for TinyBRL library
	
	\author	Castronovo Michael

	\date	2015-03-14
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Main function
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
     init();

     
     //   Retrieve the data
     const vector<AgentData> agentDataList = getAgentDataList(argc, argv);
     const vector<string>    expStrList    = getList(agentDataList, EXP_NAME);


     //   OFFLINE/SCORE graph -------------------------------------------------
     DblField xField = OFFLINE_TIME;
     DblField yField = MEAN;
     DblField lBField = LOWER_BOUND;
     DblField uBField = UPPER_BOUND;
     unsigned int fontSize = 12;
     string xLabel = "Offline time bound per trajectory (in m)";
     string yLabel = "Mean score";
     string output = "data/export/graph-offline";
     double xCoeff = (1.0 / 1000.0 / 60.0);
     
     //   For each experiment, create a graph
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<AgentData> cAgentDataList;
          cAgentDataList = filter(agentDataList, EXP_NAME, STR_EQUAL, expName);
          if (cAgentDataList.empty()) { continue; }


          //   Retrieve the maximal 'x' value
          double xMax = xCoeff *
               agentDataList[getMax(agentDataList, xField)].getField(xField);


          //   Retrieve the minimal and maximal 'y' values
          double yMin =
               agentDataList[getMin(agentDataList, yField)].getField(yField);

          double yMax =
               agentDataList[getMax(agentDataList, yField)].getField(yField);


          //   For each type of agent, create a curve
          vector<string> classStrList;         
          classStrList = getList(cAgentDataList, CLASS_NAME);
          vector<vector<pair<double, double> > > data, bounds;
          for (unsigned int j = 0; j < classStrList.size(); ++j)
          {
               string className = classStrList[j];


               //   Apply a filter to get the data related to the current
               //   type of agent
               vector<AgentData> ccAgentDataList =
                         filter(cAgentDataList, CLASS_NAME, STR_EQUAL,
                                className);


               //   For each sample, create a point
               vector<pair<double, double> > dataV, boundsV;               
               vector<AgentData> sortedList =
                         sort(ccAgentDataList, xField);

               for (unsigned int k = 0; k < sortedList.size(); ++k)
               {
                    double x = sortedList[k].getField(xField) * xCoeff;
                    double y = sortedList[k].getField(yField);
                    dataV.push_back(pair<double, double>(x, y));
               }
               
               
               //   Remove noise
               dataV = removeNoise(dataV, (500 * xCoeff), NFT_MAX);

               //   Retrieve the bounds of each point
               for (unsigned int p = 0; p < dataV.size(); ++p)
               {
                    unsigned int k = 0;
                    for (; k < sortedList.size(); ++k)
                    {
                         double x = sortedList[k].getField(xField) * xCoeff;
                         double y = sortedList[k].getField(yField);
                         if ((x == dataV[p].first) && (y == dataV[p].second))
                              break;
                    }

                    boundsV.push_back(pair<double, double>(
                              sortedList[k].getField(lBField),
                              sortedList[k].getField(uBField)));
               }
               
               
               //   Add the (0, 0) point
               dataV.push_back(pair<double, double>(0, 0));
               boundsV.push_back(pair<double, double>(0, 0));
               for (unsigned int i = 0; i < dataV.size(); ++i)
               {
                    pair<double, double> tmp = dataV[i];
                    dataV[i] = dataV.back();
                    dataV[dataV.size() - 1] = tmp;

                    tmp = boundsV[i];
                    boundsV[i] = boundsV.back();
                    boundsV[boundsV.size() - 1] = tmp;
               }
               
               
               //   Add a point for the last 'x' value (if necessary)
               if (xMax > dataV.back().first)
               {
                    double y = dataV.back().second;
                    dataV.push_back(pair<double, double>(xMax, y));
                    
                    double lBound = boundsV.back().first;
                    double uBound = boundsV.back().second;
                    boundsV.push_back(pair<double, double>(lBound, uBound));
               }
               
               
               //   Add the data points associated to this type of agent
               //   to the list
               data.push_back(dataV);
               bounds.push_back(boundsV);
          }
          
          
          //   Create the graph
          GnuplotOptions opt(fontSize);
          
               //   Add grid
          opt.setOption("grid", "lc \"#ED000000\"");
          
               //   Set a palette
          opt.setOption("palette",
                         "defined ( 0 \"green\", 1 \"blue\", 2 \"red\","
                         " 3 \"orange\" ) ");
          opt.unsetOption("colorbox");
          
               //   Set 'with'
          opt.setWith("linespoints");
          
               //   Change y range to ensure that legend is not
               //   overlapping the graph
          // set key above width -8 vertical maxrows 1 -- fix nb rows to 1
          unsigned int n = (classStrList.size() + 1);
          double lW = (0.03 * (fontSize / 12.0));
          yMin -= (lW*n * (yMax - yMin))/(1.0 - lW*n);      
          
          stringstream sstr;
          sstr << "[" << yMin << ":" << yMax + 0.05*(yMax-yMin) << "]";          
          opt.setOption("yrange", sstr.str());
          opt.setOption("title", "\"" + expName + "\"");
          opt.setOption("xlabel", "\"" + xLabel + "\"");
          opt.setOption("ylabel", "\"" + yLabel + "\"");

          
               //   Plot
          sstr.clear(); sstr.str(string());
          sstr << "\'" << output << "-" << i << ".eps\'";
          opt.setOption("output", sstr.str());
          plot(opt, data, classStrList, bounds);
     }


     //   ONLINE/SCORE graph --------------------------------------------------
     xField = ONLINE_TIME;
     yField = MEAN;
     lBField = LOWER_BOUND;
     uBField = UPPER_BOUND;
     fontSize = 12;
     xLabel = "Online time bound per trajectory (in s)";
     yLabel = "Mean score";
     output = "data/export/graph-online";
     xCoeff = (1.0 / 1000.0);
     
     //   For each experiment, create a graph
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<AgentData> cAgentDataList;
          cAgentDataList = filter(agentDataList, EXP_NAME, STR_EQUAL, expName);
          if (cAgentDataList.empty()) { continue; }


          //   Retrieve the maximal 'x' value
          double xMax = xCoeff *
               agentDataList[getMax(agentDataList, xField)].getField(xField);


          //   Retrieve the minimal and maximal 'y' values
          double yMin =
               agentDataList[getMin(agentDataList, yField)].getField(yField);

          double yMax =
               agentDataList[getMax(agentDataList, yField)].getField(yField);


          //   For each type of agent, create a curve
          vector<string> classStrList;         
          classStrList = getList(cAgentDataList, CLASS_NAME);
          vector<vector<pair<double, double> > > data, bounds;
          for (unsigned int j = 0; j < classStrList.size(); ++j)
          {
               string className = classStrList[j];


               //   Apply a filter to get the data related to the current
               //   type of agent
               vector<AgentData> ccAgentDataList =
                         filter(cAgentDataList, CLASS_NAME, STR_EQUAL,
                                className);


               //   For each sample, create a point
               vector<pair<double, double> > dataV, boundsV;
               vector<AgentData> sortedList =
                         sort(ccAgentDataList, xField);

               for (unsigned int k = 0; k < sortedList.size(); ++k)
               {
                    double x = sortedList[k].getField(xField) * xCoeff;
                    double y = sortedList[k].getField(yField);
                    dataV.push_back(pair<double, double>(x, y));
               }
               
               
               //   Remove noise
               dataV = removeNoise(dataV, (500 * xCoeff), NFT_MAX);
               
               
               //   Retrieve the bounds of each point
               for (unsigned int p = 0; p < dataV.size(); ++p)
               {
                    unsigned int k = 0;
                    for (; k < sortedList.size(); ++k)
                    {
                         double x = sortedList[k].getField(xField) * xCoeff;
                         double y = sortedList[k].getField(yField);
                         if ((x == dataV[p].first) && (y == dataV[p].second))
                              break;
                    }

                    boundsV.push_back(pair<double, double>(
                              sortedList[k].getField(lBField),
                              sortedList[k].getField(uBField)));
               }
               
               
               //   Add the (0, 0) point
               dataV.push_back(pair<double, double>(0, 0));
               boundsV.push_back(pair<double, double>(0, 0));
               for (unsigned int i = 0; i < dataV.size(); ++i)
               {
                    pair<double, double> tmp = dataV[i];
                    dataV[i] = dataV.back();
                    dataV[dataV.size() - 1] = tmp;
                    
                    tmp = boundsV[i];
                    boundsV[i] = boundsV.back();
                    boundsV[boundsV.size() - 1] = tmp;
               }
               
               
               //   Add a point for the last 'x' value (if necessary)
               if (xMax > dataV.back().first)
               {
                    double y = dataV.back().second;
                    dataV.push_back(pair<double, double>(xMax, y));
                    
                    double lBound = boundsV.back().first;
                    double uBound = boundsV.back().second;
                    boundsV.push_back(pair<double, double>(lBound, uBound));
               }
               
               
               //   Add the data points associated to this type of agent
               //   to the list
               data.push_back(dataV);
               bounds.push_back(boundsV);
          }
          
          
          //   Create the graph
          GnuplotOptions opt(fontSize);
          
               //   Add grid
          opt.setOption("grid", "lc \"#ED000000\"");
          
               //   Set a palette
          opt.setOption("palette",
                         "defined ( 0 \"green\", 1 \"blue\", 2 \"red\","
                         " 3 \"orange\" ) ");
          opt.unsetOption("colorbox");
          
               //   Set 'with'
          opt.setWith("linespoints");
          
               //   Change y range to ensure that legend is not
               //   overlapping the graph
          // set key above width -8 vertical maxrows 1 -- fix nb rows to 1
          unsigned int n = (classStrList.size() + 1);
          double lW = (0.03 * (fontSize / 12.0));
          yMin -= (lW*n * (yMax - yMin))/(1.0 - lW*n);      
          
          stringstream sstr;
          sstr << "[" << yMin << ":" << yMax + 0.05*(yMax-yMin) << "]";          
          opt.setOption("yrange", sstr.str());
          opt.setOption("title", "\"" + expName + "\"");
          opt.setOption("xlabel", "\"" + xLabel + "\"");
          opt.setOption("ylabel", "\"" + yLabel + "\"");

          
               //   Plot
          sstr.clear(); sstr.str(string());
          sstr << "\'" << output << "-" << i << ".eps\'";
          opt.setOption("output", sstr.str());
          plot(opt, data, classStrList, bounds);
     }


     //   ONLINE/OFFLINE graph ------------------------------------------------
     xField = ONLINE_TIME;
     yField = OFFLINE_TIME;
     fontSize = 12;
     xLabel = "Online time bound per trajectory (in s)";
     yLabel = "Offline time bound per trajectory (in m)";
     output = "data/export/graph-online-offline";
     
     double xStepP = 0.02;
     double yStepP = 0.02;
     
     xCoeff = (1.0 / 1000.0);
     double yCoeff = (1.0 / 1000.0 / 60.0);
     
     
     //   For each experiment, create a graph
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<AgentData> cAgentDataList;
          cAgentDataList = filter(agentDataList, EXP_NAME, STR_EQUAL, expName);
          if (cAgentDataList.empty()) { continue; }


          //   Retrieve the minimal and maximal 'x' value
          double xMin = 0.0;
          double xMax =
               agentDataList[getMax(agentDataList, xField)].getField(xField);

          //   Retrieve the minimal and maximal 'y' values
          double yMin = 0.0;
          double yMax =
               agentDataList[getMax(agentDataList, yField)].getField(yField);
          yMax /= 60.0;

          //   Compute 'xStep' and 'yStep'
          double xStep = xStepP*(xMax - xMin);
          double yStep = yStepP*(yMax - yMin);


          //   For each point, determine to which agent it belongs
          vector<vector<pair<double, double> > > data;
          vector<string> titles;
          data.push_back(vector<pair<double, double> >());
          titles.push_back("NONE");
          
          vector<string> classStrList;         
          classStrList = getList(cAgentDataList, CLASS_NAME);
          for (unsigned int x = xMin; x < (xMax - xStep); x += xStep)
          {
               vector<AgentData> ccAgentDataList;
               ccAgentDataList = filter(
                              cAgentDataList, xField, DBL_BELOW_EQUAL, x);

               for (unsigned int y = yMin; y < (yMax - yStep); y += yStep)
               {
                    vector<AgentData> cccAgentDataList;
                    cccAgentDataList = filter(
                              ccAgentDataList, yField, DBL_BELOW_EQUAL, y);

                    //   If no agent remains, mark the point as 'NONE'
                    if (cccAgentDataList.empty())
                    {
                         data[0].push_back(pair<double, double>(
                              x*xCoeff, y*yCoeff));
                         continue;
                    }


                    //   Get the best instance of each type of agent
                    vector<AgentData> bestList;
                    for (unsigned int j = 0; j < classStrList.size(); ++j)
                    {
                         vector<AgentData> tmp;
                         tmp = filter(cccAgentDataList, CLASS_NAME, STR_EQUAL,
                                      classStrList[j]);

                         int s = getMax(tmp, MEAN);
                         if (s != -1) { bestList.push_back(tmp[s]); }
                    }
                    
                    
                    //   Sort them by score by descending order
                    bestList = sort(bestList, MEAN, false);
                    
                    
                    //   Build a list of instances such that the agents
                    //   which are statistically different from the first
                    //   agent are not part of it
                    vector<double> p = bestList[0].getDSRList();
                    
                    vector<AgentData> chosenList;
                    chosenList.push_back(bestList[0]);
                    for (unsigned int j = 1; j < bestList.size(); ++j)
                    {
                         vector<double> q = bestList[j].getDSRList();
                         double z = computePairedZ<double>(p, q);
                         
                         //   Determine if it is significantly different
                         if (z < getBilateralPairedZThreshold95())
                              chosenList.push_back(bestList[j]);
                         
                         else { break; }
                    }


                    //   Check if an entry already exist for this list
                    string chosenStr = chosenList[0].getField(CLASS_NAME);
                    for (unsigned int j = 1; j < chosenList.size(); ++j)
                    {
                         chosenStr +=
                              (", " + chosenList[j].getField(CLASS_NAME));
                    }
                    
                    int s = -1;
                    for (unsigned int j = 0; j < titles.size(); ++j)
                         if (chosenStr == titles[j]) { s = j; break; }

                         //   No entry found
                    if (s == -1)
                    {
                         titles.push_back(chosenStr);
                         data.push_back(vector<pair<double, double> >());
                         s = (titles.size() - 1);
                    }
                    
                         //   Add the point
                    data[s].push_back(pair<double, double>(xCoeff*x, yCoeff*y));
               }
          }
          
          
          //   Create the graph
          GnuplotOptions opt(fontSize);
          
               //   Set a palette
          opt.setOption("palette",
                         "defined ( 0 \"green\", 1 \"blue\", 2 \"red\","
                         " 3 \"orange\" ) ");
          opt.unsetOption("colorbox");
          
               //   Set xrange/yrange
          stringstream sstr;
          sstr << "[" << xMin*xCoeff << ":" << xMax*xCoeff << "]";
          opt.setOption("xrange", sstr.str());
          sstr.clear(); sstr.str(string());
          sstr << "[" << yMin*yCoeff << ":" << yMax*yCoeff << "]";
          opt.setOption("yrange", sstr.str());
          
          
               //   Set 'with' option
          opt.setWith("points");

          opt.setOption("title", "\"" + expName + "\"");
          opt.setOption("xlabel", "\"" + xLabel + "\"");
          opt.setOption("ylabel", "\"" + yLabel + "\"");
          opt.setOption("key", "outside right top");

          
               //   Plot
          sstr.clear(); sstr.str(string());
          sstr << "\'" << output << "-" << i << ".eps\'";
          opt.setOption("output", sstr.str());

          plot(opt, data, titles);
     }


     //   OFFLINE/ONLINE/SCORE table ------------------------------------------
     output = "data/export/table";
     
     //   For each experiment, create a table
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<AgentData> cAgentDataList;
          cAgentDataList = filter(agentDataList, EXP_NAME, STR_EQUAL, expName);
          if (cAgentDataList.empty()) { continue; }

          
          //   Retrieve the best one
          int s = getMax(agentDataList, MEAN);

          
          //   Create the table
          vector<vector<Cell*> > grid;

               //   First Line
          vector<Cell*> firstLine;
          firstLine.push_back(new StrCell("Agent"));
          firstLine.push_back(new StrCell("Offline time"));
          firstLine.push_back(new StrCell("Online time"));
          firstLine.push_back(new StrCell("Score"));
          grid.push_back(firstLine);
          
               //   For each agent, add an entry
          for (unsigned int j = 0; j < cAgentDataList.size(); ++j)
          {
               const AgentData& agentData = cAgentDataList[j];
               
               vector<Cell*> line;
               line.push_back(new StrCell(agentData.getField(NAME)));
               line.push_back(new TimeCell(agentData.getField(OFFLINE_TIME)));
               line.push_back(new TimeCell(agentData.getField(ONLINE_TIME)));
               
               double lowerBound = agentData.getField(LOWER_BOUND);
               double upperBound = agentData.getField(UPPER_BOUND);
               line.push_back(new NumberIntervalCell(lowerBound, upperBound));
               
               if (j == s)
               {
                    for (unsigned int k = 0; k < line.size(); ++k)
                         line[k]->setBold();
               }
               
               grid.push_back(line);
          }
          
               //   'hLines'
          vector<unsigned int> hLines(grid.size() + 1);
          for (unsigned int j = 0; j < hLines.size(); ++j) { hLines[j] = 0; }
          hLines[1] = 1;
          
               //   'vLines'
          vector<unsigned int> vLines(grid[0].size() + 1);
          for (unsigned int j = 0; j < vLines.size(); ++j) { vLines[j] = 0; }
          vLines[1] = 1;
          vLines[2] = 1;
          vLines[3] = 1;
          
               //   Make the table
          stringstream sstr;
          sstr << output << "-" << i << ".tex";
          table(grid, sstr.str(), hLines, vLines);

               //   Free the cells
          for (unsigned int k = 0; k < grid.size(); ++k)
               for (unsigned int l = 0; l < grid[k].size(); ++l)
                    delete grid[k][l];
     }

     
     //   Return
     return 0;
}
