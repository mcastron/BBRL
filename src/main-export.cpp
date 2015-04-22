
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

	\date	2015-04-14
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Main function
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
     init();

     
     //   Retrieve the data
     const vector<AgentData> db         = getAgentDataList(argc, argv);
     const vector<string>    expStrList = getList(db, EXP_NAME);


     //   OFFLINE/SCORE graph -------------------------------------------------
     DblField xField = OFFLINE_TIME;
     DblField yField = MEAN;
     DblField lBField = LOWER_BOUND;
     DblField uBField = UPPER_BOUND;
     unsigned int fontSize = 12;
     string xLabel = "Offline computation cost (in m)";
     string yLabel = "Mean score";
     string output = "data/export/graph-offline";
     double xCoeff = (1.0 / 1000.0 / 60.0);
     
     //   For each experiment, create a graph
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<unsigned int> c = filter(db, EXP_NAME, STR_EQUAL, expName);
          if (c.empty()) { continue; }


          //   Retrieve the maximal 'x' value
          double xMax = xCoeff * db[getMax(c, db, xField)].getField(xField);


          //   Retrieve the minimal and maximal 'y' values
          double yMin = db[getMin(c, db, yField)].getField(yField);
          double yMax = db[getMax(c, db, yField)].getField(yField);


          //   For each type of agent, create a curve
          vector<string> classStrList;         
          classStrList = getList(c, db, CLASS_NAME);
          vector<vector<pair<double, double> > > data, bounds;
          for (unsigned int j = 0; j < classStrList.size(); ++j)
          {
               string className = classStrList[j];


               //   Apply a filter to get the data related to the current
               //   type of agent
               vector<unsigned int> cc = 
                         filter(c, db, CLASS_NAME, STR_EQUAL, className);


               //   For each sample, create a point
               vector<pair<double, double> > dataV, boundsV;               
               vector<unsigned int> sortedList = sort(cc, db, xField);

               for (unsigned int k = 0; k < sortedList.size(); ++k)
               {
                    double x = db[sortedList[k]].getField(xField) * xCoeff;
                    double y = db[sortedList[k]].getField(yField);
                    dataV.push_back(pair<double, double>(x, y));
               }
               
               
               //   Remove noise
//               dataV = removeNoise(dataV, (500 * xCoeff), NFT_MAX);


               //   Retrieve the bounds of each point
//               for (unsigned int p = 0; p < dataV.size(); ++p)
//               {
//                    unsigned int k = 0;
//                    for (; k < sortedList.size(); ++k)
//                    {
//                         double x = sortedList[k].getField(xField) * xCoeff;
//                         double y = sortedList[k].getField(yField);
//                         if ((x == dataV[p].first) && (y == dataV[p].second))
//                              break;
//                    }
//
//                    boundsV.push_back(pair<double, double>(
//                              sortedList[k].getField(lBField),
//                              sortedList[k].getField(uBField)));
//               }
               
               
               //   Add the (0, 0) point
//               dataV.push_back(pair<double, double>(0, 0));
//               boundsV.push_back(pair<double, double>(0, 0));
//               for (unsigned int k = 0; k < dataV.size(); ++k)
//               {
//                    pair<double, double> tmp = dataV[k];
//                    dataV[k] = dataV.back();
//                    dataV[dataV.size() - 1] = tmp;
//
//                    tmp = boundsV[k];
//                    boundsV[k] = boundsV.back();
//                    boundsV[boundsV.size() - 1] = tmp;
//               }
               
               
               //   Add a point for the last 'x' value (if necessary)
//               if (xMax > dataV.back().first)
//               {
//                    double y = dataV.back().second;
//                    dataV.push_back(pair<double, double>(xMax, y));
//                    
//                    double lBound = boundsV.back().first;
//                    double uBound = boundsV.back().second;
//                    boundsV.push_back(pair<double, double>(lBound, uBound));
//               }
               
               
               //   Add the data points associated to this type of agent
               //   to the list
               data.push_back(dataV);
//               bounds.push_back(boundsV);
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
//          opt.setWith("linespoints");
          opt.setWith("points");
          
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
          opt.setOption("logscale", "x");
          
          sstr.str(string());
          double x0 = pow(10, log(xMax) - pow(10, 8));
          double x1 = pow(10, log(xMax) + 1);
          sstr << x0 << ",100," << x1;
          opt.setOption("xtics",  sstr.str());
          opt.setOption("format", "x \"%.e\"");
          opt.setOption("size", "0.75,0.75");

          
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
     xLabel = "Online computation cost (in ms)";
     yLabel = "Mean score";
     output = "data/export/graph-online";
     xCoeff = (1.0);
     
     //   For each experiment, create a graph
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          double T = 250.0;
          xCoeff /= T;
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<unsigned int> c = filter(db, EXP_NAME, STR_EQUAL, expName);
          if (c.empty()) { continue; }


          //   Retrieve the maximal 'x' value
          double xMax = xCoeff * db[getMax(c, db, xField)].getField(xField);


          //   Retrieve the minimal and maximal 'y' values
          double yMin = db[getMin(c, db, yField)].getField(yField);
          double yMax = db[getMax(c, db, yField)].getField(yField);


          //   For each type of agent, create a curve
          vector<string> classStrList;         
          classStrList = getList(c, db, CLASS_NAME);
          vector<vector<pair<double, double> > > data, bounds;
          for (unsigned int j = 0; j < classStrList.size(); ++j)
          {
               string className = classStrList[j];


               //   Apply a filter to get the data related to the current
               //   type of agent
               vector<unsigned int> cc =
                         filter(c, db, CLASS_NAME, STR_EQUAL, className);


               //   For each sample, create a point
               vector<pair<double, double> > dataV, boundsV;
               vector<unsigned int> sortedList = sort(cc, db, xField);

               for (unsigned int k = 0; k < sortedList.size(); ++k)
               {
                    double x = db[sortedList[k]].getField(xField) * xCoeff;
                    double y = db[sortedList[k]].getField(yField);
                    dataV.push_back(pair<double, double>(x, y));
               }
               
               
               //   Remove noise
//               dataV = removeNoise(dataV, (500 * xCoeff), NFT_MAX);
               
               
               //   Retrieve the bounds of each point
//               for (unsigned int p = 0; p < dataV.size(); ++p)
//               {
//                    unsigned int k = 0;
//                    for (; k < sortedList.size(); ++k)
//                    {
//                         double x = sortedList[k].getField(xField) * xCoeff;
//                         double y = sortedList[k].getField(yField);
//                         if ((x == dataV[p].first) && (y == dataV[p].second))
//                              break;
//                    }
//
//                    boundsV.push_back(pair<double, double>(
//                              sortedList[k].getField(lBField),
//                              sortedList[k].getField(uBField)));
//               }
               
               
               //   Add the (0, 0) point
//               dataV.push_back(pair<double, double>(0, 0));
//               boundsV.push_back(pair<double, double>(0, 0));
//               for (unsigned int i = 0; i < dataV.size(); ++i)
//               {
//                    pair<double, double> tmp = dataV[i];
//                    dataV[i] = dataV.back();
//                    dataV[dataV.size() - 1] = tmp;
//                    
//                    tmp = boundsV[i];
//                    boundsV[i] = boundsV.back();
//                    boundsV[boundsV.size() - 1] = tmp;
//               }
               
               
               //   Add a point for the last 'x' value (if necessary)
//               if (xMax > dataV.back().first)
//               {
//                    double y = dataV.back().second;
//                    dataV.push_back(pair<double, double>(xMax, y));
//                    
//                    double lBound = boundsV.back().first;
//                    double uBound = boundsV.back().second;
//                    boundsV.push_back(pair<double, double>(lBound, uBound));
//               }
               
               
               //   Add the data points associated to this type of agent
               //   to the list
               data.push_back(dataV);
//               bounds.push_back(boundsV);
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
//          opt.setWith("linespoints");
          opt.setWith("points");
          
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
          opt.setOption("logscale", "x");
          
          sstr.str(string());
          double x0 = pow(10, log(xMax) - pow(10, 8));
          double x1 = pow(10, log(xMax) + 1);
          sstr << x0 << ",100," << x1;
          opt.setOption("xtics",  sstr.str());
          opt.setOption("format", "x \"%.e\"");
          opt.setOption("size", "0.75,0.75");

          
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
     xLabel = "Online time bound (in ms)";
     yLabel = "Offline time bound (in m)";
     output = "data/export/graph-online-offline";
     
     double xStepP = 0.02;
     double yStepP = 0.02;
     
     xCoeff = (1.0);
     double yCoeff = (1.0 / 1000.0 / 60.0);
     
     
     //   For each experiment, create a graph
     for (unsigned int i = 0; i < expStrList.size(); ++i)
     {
          double T = 250.0;
          xCoeff /= T;
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<unsigned int> c = filter(db, EXP_NAME, STR_EQUAL, expName);
          if (c.empty()) { continue; }


          //   Retrieve the minimal and maximal 'x' value
//          double xMin = 0.0;
          double xMin = 0.01;
          double xMax = db[getMax(c, db, xField)].getField(xField);

          //   Retrieve the minimal and maximal 'y' values
          double yMin = 0.01;
          double yMax = db[getMax(c, db, yField)].getField(yField);
          yMax /= 300.0;

          //   Compute 'xStep' and 'yStep'
          double xStep = xStepP*(xMax - xMin);
          double yStep = yStepP*(yMax - yMin);


          //   For each point, determine to which agent it belongs
          vector<vector<pair<double, double> > > data;
          vector<string> titles;
          data.push_back(vector<pair<double, double> >());
          titles.push_back("NONE");
          
          unsigned int maxTitleSize = 0;
          vector<string> classStrList;         
          classStrList = getList(c, db, CLASS_NAME);
          for (double x = xMin; x <= xMax; x *= 1.8)
          {
               vector<unsigned int> cc =
                         filter(c, db, xField, DBL_BELOW_EQUAL, x);

               for (double y = yMin; y < yMax; y *= 1.8)
               {
                    vector<unsigned int> ccc
                              = filter(cc, db, yField, DBL_BELOW_EQUAL, y);

                    //   If no agent remains, mark the point as 'NONE'
                    if (ccc.empty())
                    {
                         data[0].push_back(pair<double, double>(
                              x*xCoeff, y*yCoeff));
                         continue;
                    }


                    //   Get the best instance of each type of agent
                    vector<unsigned int> bestList;
                    for (unsigned int j = 0; j < classStrList.size(); ++j)
                    {
                         vector<unsigned int> tmp =
                                   filter(ccc, db, CLASS_NAME, STR_EQUAL,
                                          classStrList[j]);

                         int s = getMax(tmp, db, MEAN);
                         if (s != -1) { bestList.push_back(s); }
                    }
                    
                    
                    //   Sort them by score by descending order
                    bestList = sort(bestList, db, MEAN, false);
                    
                    
                    //   Build a list of instances such that the agents
                    //   which are statistically different from the first
                    //   agent are not part of it
                    vector<double> p = db[bestList[0]].getDSRList();
                    
                    vector<unsigned int> chosenList;
                    chosenList.push_back(bestList[0]);
                    for (unsigned int j = 1; j < bestList.size(); ++j)
                    {
                         vector<double> q = db[bestList[j]].getDSRList();
                         double z = computePairedZ<double>(p, q);
                         
                         //   Determine if it is significantly different
                         if (z < getBilateralPairedZThreshold95())
                              chosenList.push_back(bestList[j]);
                         
                         else { break; }
                    }


                    //   Check if an entry already exist for this list
                    for (int j = 0; j < (int) chosenList.size(); ++j)
                    {
                         for (int k = (j + 1); k < (int) chosenList.size(); ++k)
                         {
                              string strJ =
                                        db[chosenList[j]].getField(CLASS_NAME);
                                        
                              string strK =
                                        db[chosenList[k]].getField(CLASS_NAME);

                              if (strK < strJ)
                              {                                   
                                   unsigned int tmp = chosenList[j];
                                   chosenList[j]    = chosenList[k];
                                   chosenList[k]    = tmp;
                              }
                         }
                    }
                    
                    string chosenStr = db[chosenList[0]].getField(CLASS_NAME);
                    for (int j = 1; j < (int) chosenList.size(); ++j)
                    {
                         chosenStr +=
                              (", " + db[chosenList[j]].getField(CLASS_NAME));
                    }
                    
                    int s = -1;
                    for (unsigned int j = 0; j < titles.size(); ++j)
                         if (chosenStr == titles[j]) { s = j; break; }

                         //   No entry found
                    if (s == -1)
                    {
                         titles.push_back(chosenStr);
                         if (maxTitleSize < chosenStr.size())
                              maxTitleSize = chosenStr.size();

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

          opt.setOption("title",  "\"" + expName + "\"");
          opt.setOption("xlabel", "\"" + xLabel  + "\"");
          opt.setOption("ylabel", "\"" + yLabel  + "\"");
          opt.setOption("logscale", "xy");
          
          sstr.str(string());
          double x0 = pow(10, log(xMax) - pow(10, 8));
          double x1 = pow(10, log(xMax) + 1);
          sstr << x0 << ",100," << x1;
          opt.setOption("xtics",  sstr.str());
          
          opt.setOption("format", "xy \"%.e\"");
//          opt.setOption("format", "y \"%.e\"");
          sstr.str(string());
          sstr << 0.5437063+(0.2062937*(maxTitleSize / 17.0)) << "," << 0.75;
          opt.setOption("size", sstr.str());
          
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
          double T = 250.0;
          string expName = expStrList[i];


          //   Apply a filter to get the data related to the current
          //   experiment
          vector<unsigned int> c = filter(db, EXP_NAME, STR_EQUAL, expName);
          if (c.empty()) { continue; }

          
          //   Retrieve the best agent for each class
          vector<string> bestNameList;
          vector<string> classNameList = getList(c, db, CLASS_NAME);
          for (unsigned int j = 0; j < classNameList.size(); ++j)
          {
               string className = classNameList[j];
               
               vector<unsigned int> cc =
                         filter(c, db, CLASS_NAME, STR_EQUAL, className);
                         
               int s = getMax(cc, db, MEAN);
               if (s != -1)
                    bestNameList.push_back(db[s].getField(NAME));
          }

          
          //   Create the table
          vector<vector<Cell*> > grid;

               //   First Line
          vector<Cell*> firstLine;
          firstLine.push_back(new StrCell("Agent"));
          firstLine.push_back(new StrCell("Offline time"));
          firstLine.push_back(new StrCell("Mean online time (per decision)"));
          firstLine.push_back(new StrCell("Score"));
          grid.push_back(firstLine);
          
               //   For each agent, add an entry
          for (unsigned int j = 0; j < c.size(); ++j)
          {
               const AgentData& agentData = db[c[j]];
               
               vector<Cell*> line;
               line.push_back(new StrCell(agentData.getField(NAME)));
               line.push_back(new TimeCell(agentData.getField(OFFLINE_TIME)));
               line.push_back(
                         new TimeCell(agentData.getField(ONLINE_TIME) / T));
               
               double lowerBound = agentData.getField(LOWER_BOUND);
               double upperBound = agentData.getField(UPPER_BOUND);
               line.push_back(new NumberIntervalCell(lowerBound, upperBound));
               
               for (unsigned int k = 0; k < bestNameList.size(); ++k)
               {
                    string cName = agentData.getField(NAME);
                    if (cName == bestNameList[k])
                    {
                         for (unsigned int l = 0; l < line.size(); ++l)
                              line[l]->setBold();
                         
                         break;
                    }
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
