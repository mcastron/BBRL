
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

	\date	2015-05-15
*/
// ===========================================================================
void help();
void generateReport(string folderStr,
                    string prefixStr, string suffixStr, unsigned int nbExp,
                    bool noPdf);


// ---------------------------------------------------------------------------
//   Main function
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
     //   Display help
     if (parsing::hasFlag(argc, argv, "--help"))
     {
          help();
          return 0;
     }
     
     
     //   Initialize TinyBRL
     init();


     //   Retrive the prefix and suffix of the output files
     string prefixStr, suffixStr;
     
     try { prefixStr = parsing::getValue(argc, argv, "--prefix"); }
     catch (parsing::ParsingException& e) {}
     
     try { suffixStr = parsing::getValue(argc, argv, "--suffix"); }
     catch (parsing::ParsingException& e) {}
     
     bool noPdf = parsing::hasFlag(argc, argv, "--no-pdf");
     
     
     //   Create a folder for the generated files
     string folderStr;
     if (!prefixStr.empty()) { folderStr = prefixStr; }
     if (!suffixStr.empty())
     {
          if (!folderStr.empty()) { folderStr += "-"; }
          folderStr += suffixStr;
     }
     if (folderStr.empty()) { folderStr = "files"; }
     folderStr += "/";

     if (system(("mkdir data/export/" + folderStr).c_str()) == -1)
     {
          cout << "\n\tError: Unable to create folder:";
          cout << "\"data/export/\"" << folderStr << "\n\n";
          return -1;
     }

     
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
     
     string output = ("data/export/" + folderStr);
     if (!prefixStr.empty()) { output += (prefixStr + "-"); }
     output += "graph-offline";
     if (!suffixStr.empty()) { output += ("-" + suffixStr); }
     
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
          vector<string> classStrList = getList(c, db, CLASS_NAME);
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
               
               
               //   Add the data points associated to this type of agent
               //   to the list
               data.push_back(dataV);
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
          opt.setWith("points");
          
               //   Change y range to ensure that legend is not
               //   overlapping the graph
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

     output = ("data/export/" + folderStr);
     if (!prefixStr.empty()) { output += (prefixStr + "-"); }
     output += "graph-online";
     if (!suffixStr.empty()) { output += ("-" + suffixStr); }
     
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
          vector<string> classStrList = getList(c, db, CLASS_NAME);
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
               
                              
               //   Add the data points associated to this type of agent
               //   to the list
               data.push_back(dataV);
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
          opt.setWith("points");
          
               //   Change y range to ensure that legend is not
               //   overlapping the graph
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
     
     output = ("data/export/" + folderStr);
     if (!prefixStr.empty()) { output += (prefixStr + "-"); }
     output += "graph-online-offline";
     if (!suffixStr.empty()) { output += ("-" + suffixStr); }
     
     double xStepCoeff = 5.0;
     double yStepCoeff = 5.0;
     
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
          double xMin = 0.01, xMax = 0.01;
          double xLimit = db[getMax(c, db, xField)].getField(xField);

          //   Retrieve the minimal and maximal 'y' values
          double yMin = 0.01, yMax = 0.01;
          double yLimit = db[getMax(c, db, yField)].getField(yField);
          yMax /= 300.0;


          //   For each point, determine to which agent it belongs
          vector<string> classStrList = getList(c, db, CLASS_NAME);
          vector<vector<pair<double, double> > > data(classStrList.size());
          
          unsigned int maxTitleSize = 0;
          for (double x = xMin; x <= xLimit; x *= xStepCoeff)
          {
               xMax = x;

               vector<unsigned int> cc =
                         filter(c, db, xField, DBL_BELOW_EQUAL, x);

               for (double y = yMin; y < yLimit; y *= yStepCoeff)
               {
                    yMax = y;
                    
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


                    //   Add the point for each selected algorithm
                    for (unsigned int j = 0; j < chosenList.size(); ++j)
                    {
                         unsigned int s = chosenList[j];
                         string classStr = db[s].getField(CLASS_NAME);
                         
                         for (unsigned int k = 0; k < classStrList.size(); ++k)
                         {
                              if (classStr == classStrList[k])
                              {
                                   data[k].push_back(
                                             pair<double, double>(xCoeff*x,
                                                                  yCoeff*y));
                                   break;
                              }                        
                         }
                    }
               }
          }
          
          
          //   Create the graph
          GnuplotOptions opt(fontSize);
          
               //   Set 'inBox'
          opt.setInBox(true);
          
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
          sstr.str(string());
          sstr << 0.5437063+(0.2062937*(maxTitleSize / 17.0)) << "," << 0.75;
          opt.setOption("size", sstr.str());
          
          opt.setOption("key", "outside right top");

          
               //   Plot
          sstr.clear(); sstr.str(string());
          sstr << "\'" << output << "-" << i << ".eps\'";
          opt.setOption("output", sstr.str());

          plot(opt, data, classStrList);
     }


     //   OFFLINE/ONLINE/SCORE table ------------------------------------------
     output = ("data/export/" + folderStr);
     if (!prefixStr.empty()) { output += (prefixStr + "-"); }
     output += "table";
     if (!suffixStr.empty()) { output += ("-" + suffixStr); }
     
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
     
     
     //   Generate latex report
     generateReport(folderStr, prefixStr, suffixStr, expStrList.size(), noPdf);

     
     //   Return
     return 0;
}


// ---------------------------------------------------------------------------
//	Extra functions implementation
// ---------------------------------------------------------------------------
void help()
{
     ifstream is("doc/command-line manual (TinyBRL-export).txt");
     for (string tmp; getline(is, tmp);) { cout << tmp << "\n"; }
}


void generateReport(string folderStr,
                    string prefixStr, string suffixStr, unsigned int nbExp,
                    bool noPdf)
{
     string reportStr;
     if (!prefixStr.empty()) { reportStr += (prefixStr + "-"); }
     reportStr += "report";
     if (!suffixStr.empty()) { reportStr += ("-" + suffixStr); }
     reportStr += ".tex";
     
     string output = "data/export/";
     output += reportStr;
     
     ofstream file(output.c_str());
     file << "\\documentclass[10pt,a4paper]{article}\n";
     file << "\\usepackage[utf8]{inputenc}\n";
     file << "\\usepackage[english]{babel}\n";
     file << "\\usepackage{amsmath}\n";
     file << "\\usepackage{amsfonts}\n";
     file << "\\usepackage{amssymb}\n";
     file << "\\usepackage{graphicx}\n";
     file << "\\usepackage{longtable}\n";
     file << "\\usepackage{pdflscape}\n";
     file << "\\usepackage[left=2cm,right=2cm,top=2cm,bottom=2cm]{geometry}\n";
     file << "\n";
     file << "\\begin{document}\n";
     file << "\\section*{Report}\n";
     
     for (unsigned int i = 0; i < nbExp; ++i)
     {
          stringstream offGraph;
          offGraph << folderStr;
          if (!prefixStr.empty()) { offGraph << prefixStr << "-"; }
          offGraph << "graph-offline";
          if (!suffixStr.empty()) { offGraph << "-" << suffixStr; }
          offGraph << "-" << i;

          stringstream onGraph;
          onGraph << folderStr;
          if (!prefixStr.empty()) { onGraph << prefixStr << "-"; }
          onGraph << "graph-online";
          if (!suffixStr.empty()) { onGraph << "-" << suffixStr; }
          onGraph << "-" << i;

          stringstream onOffGraph;
          onOffGraph << folderStr;
          if (!prefixStr.empty()) { onOffGraph << prefixStr << "-"; }
          onOffGraph << "graph-online-offline";
          if (!suffixStr.empty()) { onOffGraph << "-" << suffixStr; }
          onOffGraph << "-" << i;

          stringstream table;
          table << folderStr;
          if (!prefixStr.empty()) { table << prefixStr << "-"; }
          table << "table";
          if (!suffixStr.empty()) { table << "-" << suffixStr; }
          table << "-" << i;
          
          
          file << "\\subsection*{Experiment \\#" << i << "}\n";
          file << "\n";
          file << "\\begin{figure}[!ht]\n";
          file << "\t\\centering\\includegraphics[scale=1.5]{" << offGraph.str() << "}\n";
          file << "\t\\caption{Offline computation cost Vs. Performance}\n";
          file << "\\end{figure}\n";
          file << "\n";
          file << "\\begin{figure}[!ht]\n";
          file << "\t\\centering\\includegraphics[scale=1.5]{" << onGraph.str() << "}\n";
          file << "\t\\caption{Online computation cost Vs. Performance}\n";
          file << "\\end{figure}\n";
          file << "\n";
          file << "\\begin{figure}[!ht]\n";
          file << "\t\\centering\\includegraphics[scale=1.5]{" << onOffGraph.str() << "}\n";
          file << "\t\\caption{Best algorithms w.r.t offline/online time periods}\n";
          file << "\\end{figure}\n";
          file << "\n";
          file << "\\begin{landscape}\n";
          file << "\\input{" << table.str() << ".tex}\n";
          file << "\\end{landscape}\n";
          file << "\n";
     }
     file << "\\end{document}\n";
     file.close();
     
     
     //   Exit if necessary
     if (noPdf) { return; }


     //   Build pdf
     string command;
     command = "cd data/export/";
     command += ("; latexmk -pdf -quiet -silent " + reportStr + " >> tmp.log");
     if (system(command.c_str()) == -1)
     {
          cout << "\n\tError: Unable to build report!\n\n";
          return;
     }

     command = "cd data/export/";
     command += ("; latexmk -c -quiet -silent " + reportStr + " >> tmp.log"); 
     if (system(command.c_str()) == -1)
          cout << "\n\tWarning: Unable to delete temporary files!\n\n";
     
     command = "cd data/export/";
     command += ("; rm " + folderStr + "*eps-converted-to*.pdf; rm tmp.log");
     if (system(command.c_str()) == -1)
          cout << "\n\tWarning: Unable to delete temporary files!\n\n";
}
