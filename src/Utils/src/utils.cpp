
#include "utils.h"
#include "RandomGen.h"
#include "Algorithm/StoSOO.h"

using namespace std;


// ===========================================================================
//	Namespaces
// ===========================================================================
// ---------------------------------------------------------------------------
//	'statistics' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Details (to ignore)
// ===========================================================================
pair<double, double> utils::statistics::details::computeCI(
		double alpha, double mean, double stDev, unsigned int n)
{
	assert(n > 0);

	pair<double, double> p;
	
	double tmp = (alpha * (stDev / sqrt((double) n)));
	
	p.first = (mean - tmp);
	p.second = (mean + tmp);

	return p;
}


double utils::statistics::details::gamma(double shape, double scale)
{
	if (shape < 1.0)
	{
		return (gamma((shape + 1.0), scale)
				* pow(RandomGen::rand01_open_Uniform(), (1.0 / shape)));
	}
	
	const double d = (shape - (1.0 / 3.0));
	const double c = (1.0 / sqrt(9.0 * d));
	double x, v;
	for (;;)
	{
		do
		{
			x = RandomGen::rand_Gaussian(0.0, 1.0);
			v = (1.0 + (c * x));
		}
		while (v <= 0.0);
		
		v = (v * v * v);
		const double u = RandomGen::rand01_open_Uniform();
		const double x2 = (x * x);
		
		if (u < (1.0 - (0.0331 * x2 * x2))) { return ((d * v) / scale); }	
		if (log(u) < ((0.5 * x2) + (d * (1.0 - v + log(v)))))
			return ((d * v) / scale);
	}
}


// ===========================================================================
//	Functions
// ===========================================================================
void	utils::statistics::sampleDirichlet(
		unsigned int hTheta, const vector<double>& theta,
		unsigned int hP, vector<double>& P,
		unsigned int n)
{
	assert(n > 0);
	assert(hTheta + (n - 1) < theta.size());
	assert(hP + (n - 1) < P.size());
	
	double gammaSum = 0.0;
	double randGamma;
	bool allZeros = true;
	for (unsigned i = 0; i < n; i++)
	{
		randGamma = utils::statistics::details::gamma(theta[hTheta + i], 1);
		if(randGamma > 0) { allZeros = false; }
		
		P[hP + i] = randGamma;
		gammaSum += randGamma;
	}
	
	if (allZeros)
	{
		//	Handle special case, occurs when params are really small
		//	because of numerical approximation around 0
		P[hP + RandomGen::randIntRange_Uniform(0, (n - 1))] = 1.0;
	}
	
	else if (gammaSum < 1e-300)
	{
		for (unsigned i = 0; i < n; i++) { P[hP + i] = 0; }
		P[hP + RandomGen::randIntRange_Uniform(0, (n - 1))] = 1.0;
	}
	
	else
	{
		//Normalize
		for (unsigned int i = 0; i < n; ++i)
			P[hP + i] = (P[hP + i] / gammaSum);
	}
}

void utils::statistics::sampleDirichlet(
		const vector<double>& theta, vector<double>& P)
{
	utils::statistics::sampleDirichlet(0, theta, 0, P, theta.size());
}

vector<double> utils::statistics::sampleDirichlet(
		const vector<double>& theta)
{
	vector<double> P(theta.size());
	utils::statistics::sampleDirichlet(theta, P);
	
	return P;
}


double utils::statistics::getBilateralPairedZThreshold95()  { return 1.96;  }


double utils::statistics::getUnilateralPairedZThreshold95() { return 1.645; }


// ---------------------------------------------------------------------------
//	'compression' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
string utils::compression::compressStr(
	const string& str, int compressionlevel) throw (runtime_error)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionlevel) != Z_OK)
        throw(runtime_error("deflateInit failed while compressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(runtime_error(oss.str()));
    }

    return outstring;
}


string utils::compression::uncompressStr(const string& str)
										throw (runtime_error)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
        throw(runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    string outstring;

    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(runtime_error(oss.str()));
    }

    return outstring;
}


// ---------------------------------------------------------------------------
//	'parsing' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
string utils::parsing::getValue(int argc, char* argv[], string opt)
                                                       throw (ParsingException)
{
     for (int i = 1; i < (argc - 1); ++i)
          if (string(argv[i]) == opt) { return string(argv[i + 1]); }
          
     throw parsing::ParsingException(opt);
}


vector<string> utils::parsing::getValues(int argc, char* argv[],
          string opt, unsigned int n) throw (ParsingException)
{
     vector<string> values;
     for (int i = 1; i < (argc - (int) n); ++i)
     {
          if (string(argv[i]) == opt)
          {
               for (int j = 1; j <= (int) n; ++j)
                    values.push_back(string(argv[i + j]));
               break;
          }
     }
     if (values.size() == n) { return values; }
     
     throw parsing::ParsingException(opt);
}


bool utils::parsing::hasFlag(int argc, char* argv[], string opt)
{
     for (int i = 1; i < argc; ++i)
          if (string(argv[i]) == opt) { return true; }
     
     return false;
}


// ---------------------------------------------------------------------------
//	'gnuplot' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
void utils::gnuplot::plot(utils::gnuplot::GnuplotOptions opt,
                          vector<vector<pair<double, double> > > data,
                          const vector<string>& titles,
                          vector<vector<pair<double, double> > > bounds)
{
     //   'inBox' case
     vector<details::Polygon> polygons;
     if (opt.getInBox())
     {
          //   Retrieve the boxes
          bool logScaleX = false, logScaleY = false;
          
          string options = opt.getScriptOptions();
          
          string tmp;
          size_t f = options.find("logscale ");
          if (f != string::npos)
          {
               tmp = options.substr(f);
               f = tmp.find("\n");
               if (f != string::npos) { tmp = tmp.substr(0, f); }
          }
          
          if      (tmp == "logscale x")  { logScaleX = true; }
          else if (tmp == "logscale y")  { logScaleY = true; }
          else if (tmp == "logscale xy") { logScaleX = true; logScaleY = true; }
          
          polygons  = details::getPolygons(data, logScaleX, logScaleY);
          
          
          //   Modify 'data'
          for (unsigned int f = 0; f < data.size(); ++f) { data[f].clear(); }
          for (unsigned int p = 0; p < polygons.size(); ++p)
          {
               details::Point centerP =
                         polygons[p].getCenter(logScaleX, logScaleY);
               set<unsigned int> list = polygons[p].getList();
               
               if (logScaleX) { centerP.applyLog10X(); }
               if (logScaleY) { centerP.applyLog10Y(); }
               
               set<unsigned int>::iterator it  = list.begin();
               set<unsigned int>::iterator end = list.end();
               for (unsigned int c = 0; it != end; ++it, ++c)
               {
                    double r = (list.size() == 1) ? 0.0 : 0.15;
                    double theta = c * (2.0*M_PI / (double) list.size());
                    
                    double x = centerP.getX() + r * cos(theta);
                    double y = centerP.getY() + r * sin(theta);
                    
                    if (logScaleX) { x = pow(10.0, x); }
                    if (logScaleY) { y = pow(10.0, y); }
                    
                    data[*it].push_back(
                         pair<double, double>(x, y));
               }
          }
     }
     
     
     //   1.   Write the data file
     string dataFile = ("data/export/tmp.dat");
     ofstream datOS(dataFile.c_str());
     for (unsigned int i = 0; i < data.size(); ++i)
     {
          if (data[i].empty())
               data[i].push_back(pair<double, double>(-DBL_MAX, DBL_MAX));

          for (unsigned int j = 0; j < data[i].size(); ++j)
          {
               datOS << data[i][j].first << "\t" << data[i][j].second;
               if ((i < bounds.size()) && (bounds[i].size() == data[i].size()))
               {
                    datOS << "\t" << bounds[i][j].first;
                    datOS << "\t" << bounds[i][j].second;
               }
               datOS << "\n";               
          }
          if (!data[i].empty()) { datOS << "\n\n"; }
     }
     datOS.close();
     
     
     //   2.   Write the script file
     string scriptFile = ("data/export/tmp.gp");     
     ofstream scriptOS(scriptFile.c_str());
     scriptOS << opt.getScriptOptions();
     
          //   2.1  Polygons
     unsigned int id = 1;
     for (unsigned int i = 0; i < polygons.size(); ++i)
     {
          vector<details::Path> loops = polygons[i].getIntLoops();
          loops.push_back(polygons[i].getExtLoop());
          
          for (unsigned int j = 0; j < loops.size(); ++j)
          {
               scriptOS << "set object " << id++ << " poly from ";
               
               details::Point p = loops[j].get(0).getFrom();
               scriptOS << p.getX() << "," << p.getY();
               
               for (unsigned int k = 1; k < loops[j].size(); ++k)
               {
                    details::Point p = loops[j].get(k).getFrom();
                    scriptOS << " to " << p.getX() << "," << p.getY();
               }
               
               p = loops[j].get(0).getFrom();
               scriptOS << " to " << p.getX() << "," << p.getY();
               
               scriptOS << " fs empty border 0\n";
          }
     }
          
          //   2.2  Plot
     unsigned int n = data.size();
     unsigned int index = 0;
     for (unsigned int i = 0; i < data.size(); ++i)
     {
          if (!data[i].empty())
          {               
               if (index == 0) { scriptOS << "plot \'" << dataFile << "\'"; }
               else            { scriptOS << ", ''";                        }
               
               scriptOS << " index " << index;
               if (opt.getWith() != "")
                    scriptOS << " with " << opt.getWith();

               double colorFrac = ((n <= 1) ? 0 : (i / (double) (n - 1)));
               scriptOS << " lt palette frac " << colorFrac;

               scriptOS << " title \"";
               if (i < titles.size()) { scriptOS << titles[i];  }
               else                   { scriptOS << "NO TITLE"; }
               scriptOS << "\"";
                    
               if ((i < bounds.size()) && bounds[i].size() == data[i].size())
               {
                   scriptOS <<  ", \'\' index " << index;
                   scriptOS << " with yerrorbars notitle pt -1";
                   scriptOS << " lt palette frac " << colorFrac;
               }
               
               ++index;
          }
     }
     scriptOS << "\n";
     scriptOS.close();


     //   3.   Generate the 'eps' file     
     string command = "gnuplot -e \"input=\'";
     command += dataFile;
     command += "\'\" ";
     command += scriptFile;
     
     if (system(command.c_str()) == -1)
     {
          cout << "\n\tError: Unable to create gnuplot graph!\n\n";
          return;
     }
     
     
     //   4.   Delete the temporary files
     command = "rm " + dataFile;
     if (system(command.c_str()) == -1)
     {
          cout << "\n\tWarning: Unable temporary files (gnuplot)!\n\n";
          return;
     }
     
     command = "rm " + scriptFile;
     if (system(command.c_str()) == -1)
     {
          cout << "\n\tWarning: Unable temporary files (gnuplot)!\n\n";
          return;
     }
}
     

vector<pair<double, double> >
     utils::gnuplot::removeNoise(
          const vector<pair<double, double> >& dataV,
          double epsilon, utils::gnuplot::NoiseFilterType nft)
{
     vector<pair<double, double> > filteredV, cDataV = dataV;
     vector<pair<double, double> > cPack =
               details::getFirstPack(cDataV, epsilon);
     while (!cPack.empty())
     {
          //   Filter the current pack to produce a single point
          double x, y;
          switch (nft)
          {
               case NFT_MIN:
                    double minX, minY;
                    for (unsigned int i = 0; i < cPack.size(); ++i)
                    {
                         if ((i == 0) || (cPack[i].second < minY))
                         {
                              minX = cPack[i].first;
                              minY = cPack[i].second;
                         }
                    }

                    x = minX;
                    y = minY;
                    break;

              
               case NFT_MAX:
                    double maxX, maxY;
                    for (unsigned int i = 0; i < cPack.size(); ++i)
                    {
                         if ((i == 0) || (cPack[i].second > maxY))
                         {
                              maxX = cPack[i].first;
                              maxY = cPack[i].second;
                         }
                    }

                    x = maxX;
                    y = maxY;
                    break;


               case NFT_MEAN:
                    double sumX = 0.0, sumY = 0.0;
                    for (unsigned int i = 0; i < cPack.size(); ++i)
                    {
                         sumX += cPack[i].first;
                         sumY += cPack[i].second;
                    }
                    
                    x = (sumX / (double) cPack.size());
                    y = (sumY / (double) cPack.size());
                    break;
          }
          
          
          //   Save the generated point
          filteredV.push_back(pair<double, double>(x, y));
          
          
          //   Remove 'cPack' from 'cDataV'
          for (unsigned int i = 0; i < (cDataV.size() - cPack.size()); ++i)
               cDataV[i] = cDataV[i + cPack.size()];
               
          for (unsigned int i = 0; i < cPack.size(); ++i) { cDataV.pop_back(); }
          
          
          //   Retrieve the next pack
          cPack = details::getFirstPack(cDataV, epsilon);
     }
     
     
     //   Return
     return filteredV;
}


// ===========================================================================
//   'details' namespace
// ===========================================================================
//   LabelPositionOptim
class utils::gnuplot::details::LabelPositionOptim : public algorithm::StoSOO
{
     public:
          LabelPositionOptim(const vector<double>& x_,
                             const vector<double>& y_,
                             const vector<pair<double, double> >& bounds) :
                                   StoSOO(2, 3,
                                          getNbEvalPerNode(100),
                                          getMaxDepth(100),
                                          getDelta(100), bounds),
                                   x(x_), y(y_) { precalcValues(x, y); }


          vector<double> run() { return algorithm::StoSOO::run(100); }

     protected:
          double f(const vector<double>& p) const throw (exception)
          {
               if (!pointInPolygon(p)) { return -DBL_MAX; }
               
               double minDist, var;
               getMinVarDistance(p, minDist, var);
               
               return (minDist - var);
          }


     private:
          const vector<double>& x;
          const vector<double>& y;
          vector<double> constant, multiple;
          vector<double> lengths;
          
          void precalcValues(const vector<double>& x,
                             const vector<double>& y)
          {
               int polyCorners = x.size();
               int i, j = polyCorners - 1;
               constant.resize(polyCorners);
               multiple.resize(polyCorners);
               
               const vector<double>& polyX = x;
               const vector<double>& polyY = y;
               for (i = 0; i < polyCorners; i++)
               {
                    if (polyY[j] == polyY[i])
                    {
                         constant[i] = polyX[i];
                         multiple[i] = 0;
                    }

                    else
                    {
                         constant[i] = polyX[i]
                              -(polyY[i]*polyX[j])/(polyY[j]-polyY[i])
                              +(polyY[i]*polyX[i])/(polyY[j]-polyY[i]);

                         multiple[i] = (polyX[j]-polyX[i])/(polyY[j]-polyY[i]);
                    }

                    j = i;
               }
               

               for (unsigned int i = 0; i < x.size(); ++i)
               {
                    double xDist = (x[i] - x[(i + 1) % x.size()]);
                    xDist *= xDist;
                    
                    double yDist = (y[i] - y[(i + 1) % y.size()]);
                    yDist *= yDist;                    
                    
                    lengths.push_back(sqrt(xDist + yDist));
               }
          }
          
          
          bool pointInPolygon(const vector<double>& p) const
          {
               int polyCorners = x.size();
               int i, j = polyCorners - 1;
               bool oddNodes = false;
               
               const vector<double>& polyX = x;
               const vector<double>& polyY = y;
               for (i = 0; i < polyCorners; i++)
               {
                    if ((polyY[i]< p[1] && polyY[j] >= p[1])
                              || (polyY[j] < p[1] && polyY[i] >= p[1]))
                    {
                         oddNodes ^= (p[1]*multiple[i]+constant[i]<p[0]);
                    }
                    
                    j = i;
               }
               
               return oddNodes;
          }


          void getMinVarDistance(const vector<double>& p,
                                 double& minDist, double& var) const
          {
               double c[2];
               c[0] = p[0];
               c[1] = p[1];
           
               vector<double> dist;    
               for (unsigned int i = 0; i < x.size(); ++i)
               {
                    double a[2];
                    a[0] = x[i];
                    a[1] = y[i];  
                
                    double b[2];
                    b[0] = x[(i + 1) % x.size()];
                    b[1] = y[(i + 1) % x.size()];                  
                    
                    dist.push_back(LineToPointDistance2D(a, b, c, true));
               }
               

               double sum = 0.0;
               for (unsigned int i = 0; i < dist.size(); ++i)
               {
                    if (i == 0 || minDist > dist[i]) { minDist = dist[i]; }
                    sum += dist[i];
               }
               
               
               var = 0.0;
               double mean = (sum / (double) dist.size());
               for (unsigned int i = 0; i < dist.size(); ++i)
                    var += (mean - dist[i])*(mean - dist[i]);

               var /= ((double) dist.size());
          }
          
          
          //Compute the dot product AB . AC
          double DotProduct(double* pointA,
                            double* pointB,
                            double* pointC) const
          {
              double AB[2];
              double BC[2];
              AB[0] = pointB[0] - pointA[0];
              AB[1] = pointB[1] - pointA[1];
              BC[0] = pointC[0] - pointB[0];
              BC[1] = pointC[1] - pointB[1];
              double dot = AB[0] * BC[0] + AB[1] * BC[1];
          
              return dot;
          }


          //Compute the cross product AB x AC
          double CrossProduct(double* pointA,
                              double* pointB,
                              double* pointC) const
          {
              double AB[2];
              double AC[2];
              AB[0] = pointB[0] - pointA[0];
              AB[1] = pointB[1] - pointA[1];
              AC[0] = pointC[0] - pointA[0];
              AC[1] = pointC[1] - pointA[1];
              double cross = AB[0] * AC[1] - AB[1] * AC[0];
          
              return cross;
          }


          //Compute the distance from A to B
          double Distance(double* pointA, double* pointB) const
          {
              double d1 = pointA[0] - pointB[0];
              double d2 = pointA[1] - pointB[1];
          
              return sqrt(d1 * d1 + d2 * d2);
          }


          //Compute the distance from AB to C
          //if isSegment is true, AB is a segment, not a line.
          double LineToPointDistance2D(double* pointA,
                                       double* pointB,
                                       double* pointC, 
                                       bool isSegment) const
          {
              double dist = CrossProduct(pointA, pointB, pointC)
                              / Distance(pointA, pointB);
              if (isSegment)
              {
                  double dot1 = DotProduct(pointA, pointB, pointC);
                  if (dot1 > 0) 
                      return Distance(pointB, pointC);
          
                  double dot2 = DotProduct(pointB, pointA, pointC);
                  if (dot2 > 0) 
                      return Distance(pointA, pointC);
              }
              return fabs(dist);
          }
};


//   Path
std::vector<utils::gnuplot::details::Path>
     utils::gnuplot::details::Path::split(std::vector<int> cuts) const
{
     sort(cuts.begin(), cuts.end());
                              
     std::vector<Path> paths;
     Path cPath;
     
     int c = 0;
     for (unsigned int i = 0; i < size(); ++i)
     {
          if (i == cuts[c])
          {
               if (!cPath.empty())
               {
                    paths.push_back(cPath);
                    cPath = Path();
               }
               ++c;
          }
          else { cPath.add(get(i)); }
     }
     
     if (!cPath.empty()) { paths.push_back(cPath); }
     
     
     if (paths.size() >= 2)
     {
          Path& a = paths.back();
          Path& b = paths.front();
          
          Point tail = a.get(a.size() - 1).getTo();
          Point head = b.get(0).getFrom();
          
          if (tail == head)
          {
               for (int j = 0; j < b.size(); ++j) { a.add(b.get(j)); }

               paths[0] = a;
               paths.pop_back();
          }
     }


     return paths;
}


double utils::gnuplot::details::Path::computeSurface()
{
     if (!surfaceHasChanged) { return surface; }
     surfaceHasChanged = false;
     
     if (!isLoop()) { surface = 0.0; return surface; }


     // retrieve the points
     std::vector<double> x, y;
     for (unsigned int i = 0; i < edges.size(); ++i)
     {
          x.push_back(edges[i].getFrom().getX());
          y.push_back(edges[i].getFrom().getY());
     }


     // "close" polygon
     unsigned int N = x.size();
     x.push_back(x[0]); //x[N] = x[0];
     x.push_back(x[1]); //x[N+1] = x[1];
     y.push_back(y[0]); //y[N] = y[0];
     y.push_back(y[1]); //y[N+1] = y[1];


     // compute area
     double area = 0.0;
     for( size_t i = 1; i <= N; ++i )
          area += x[i]*( y[i+1] - y[i-1] );
     area /= 2.0;


     // return
     surface = fabs(area);
     return surface;
}


//   Polygon
bool utils::gnuplot::details::Polygon::mergeWith(const Polygon& poly)
{
     //   List all loops
     vector<Path> loops1;
     loops1.push_back(extLoop);
     for (unsigned int i = 0; i < intLoops.size(); ++i)
          loops1.push_back(intLoops[i]);
     
     vector<Path> loops2;     
     loops2.push_back(poly.getExtLoop());
     vector<Path> intLoops2 = poly.getIntLoops();
     for (unsigned int i = 0; i < intLoops2.size(); ++i)
          loops2.push_back(intLoops2[i]);
     
     
     //   Identify the list of common edges and the two loops colliding
     vector<pair<int, int> > commonEdges;
     int k, l;
     
          //   Check if external loop of polygon #1 touches another loop
     k = 0;
     for (l = 0; l < loops2.size(); ++l)
     {
          commonEdges = loops1[k].getCommonEdges(loops2[l]);
          if (!commonEdges.empty()) { break; }
     }
     
          //   Check if external loop of polygon #2 touches another loop
     if (commonEdges.empty())
     {
          l = 0;
          for (k = 0; k < loops1.size(); ++k)
          {
               commonEdges = loops1[k].getCommonEdges(loops2[l]);
               if (!commonEdges.empty()) { break; }
          }
     }
     
          //   Exit if no common edges
     if (commonEdges.empty()) { return false; }
     
     
     //   Split the colliding loops in several paths
     vector<int> cuts1, cuts2;
     for (unsigned int i = 0; i < commonEdges.size(); ++i)
     {
          cuts1.push_back(commonEdges[i].first);
          cuts2.push_back(commonEdges[i].second);
     }
     
     vector<Path> paths1 = loops1[k].split(cuts1);
     vector<Path> paths2 = loops2[l].split(cuts2);
     
     
     //   Merge the paths in loops
     vector<Path> mLoops;
     while (!paths1.empty())
     {
          //   Loop case
          if (paths1[0].isLoop())
          {
               mLoops.push_back(paths1[0]);
               paths1[0] = paths1.back();
               paths1.pop_back();
               
               continue;
          }
          
          
          //   Path case
          Point head1 = paths1[0].get(0).getFrom();
          Point tail1 = paths1[0].get(paths1[0].size() - 1).getTo();
          
          for (unsigned int i = 0; i < paths2.size(); ++i)
          {
               Point head2 = paths2[i].get(0).getFrom();
               Point tail2 = paths2[i].get(paths2[i].size() - 1).getTo();
               
               if ((head1 == head2) && (tail1 == tail2))
               {
                    if (paths1[0].size() < paths2[i].size())
                    {
                         paths1[0].reverse();
                         head1 = paths1[0].get(0).getFrom();
                         tail1 = paths1[0].get(paths1[0].size() - 1).getTo();
                    }
                    
                    else
                    {
                         paths2[i].reverse();
                         head2 = paths2[i].get(0).getFrom();
                         tail2 = paths2[i].get(paths2[i].size() - 1).getTo();
                    }
               }
               
               if ((tail1 == head2) && (tail2 == head1))
               {
                    //   Merge
                    for (unsigned int j = 0; j < paths2[i].size(); ++j)
                         paths1[0].add(paths2[i].get(j));


                    //   Save the loop
                    mLoops.push_back(paths1[0]);
                    
                    
                    //   Next iteration
                    paths1[0] = paths1.back();
                    paths1.pop_back();
                    
                    paths2[i] = paths2.back();
                    paths2.pop_back();

                    break;
               }
          }
     }
     
     
     //   Identify the external loop
     int max = -1;
     double maxS = 0.0;
     for (unsigned int i = 0; i < mLoops.size(); ++i)
     {
          double curS = mLoops[i].computeSurface();
          if ((max == -1) || (maxS < curS))
          {
               max = i;
               maxS = curS;
          }
     }
     extLoop = mLoops[max];


     //   Gather the internal loops
     intLoops.clear();
     for (unsigned int i = 0; i < loops1.size(); ++i)
     {
          if (i != k) { intLoops.push_back(loops1[i]); }
     }
     
     for (unsigned int i = 0; i < loops2.size(); ++i)
     {
          if (i != l) { intLoops.push_back(loops2[i]); }
     }
     
     for (unsigned int i = 0; i < mLoops.size(); ++i)
     {
          if (i != max) { intLoops.push_back(mLoops[i]); }
     }


     //   Return
     return true;
}


utils::gnuplot::details::Point
     utils::gnuplot::details::Polygon::getCenter(bool logScaleX,
                                                 bool logScaleY) const
{
     //   Simplify the polygon and retrieve the coordinates of each edge
     unsigned int N = extLoop.size();
     vector<double> x, y;
     for (int i = 0; i < N; ++i)
     {
          Point p = extLoop.get((i - 1 + N) % N).getFrom();
          Point q = extLoop.get((i + 1)     % N).getFrom();
          Point r = extLoop.get(i).getFrom();

          bool accept;
          
          if (p.getX() == q.getX())
               accept = fabs(r.getX() - p.getX()) >= 1e-9;

          else
          {
               double a = ((p.getY() - q.getY()) / (p.getX() - q.getX()));
               double b = p.getY() - a*p.getX();
               
               accept = (fabs(r.getY() - (a*r.getX() + b)) >= 1e-9);
          }

          
          if (accept)
          {
               if (logScaleX) { r.applyLog10X(); }
               if (logScaleY) { r.applyLog10Y(); }
               
               x.push_back(r.getX());
               y.push_back(r.getY());
          }
     }
     
     
     //   Uses StoSOO to identify the best spot for the center     
     vector<pair<double, double> > bounds = getBounds();
     bounds[0].first  = log10(bounds[0].first);
     bounds[0].second = log10(bounds[0].second);
     bounds[1].first  = log10(bounds[1].first);
     bounds[1].second = log10(bounds[1].second);

     LabelPositionOptim alg(x, y, bounds);
     vector<double> c = alg.run();

     Point centerP(c[0], c[1], extLoop.get(0).getFrom().getList());
     if (logScaleX) { centerP.apply10PowX(); }
     if (logScaleY) { centerP.apply10PowY(); }
     
     
     //   Return
     return centerP;
}


void utils::gnuplot::details::Polygon::print() const
{
     std::cout << "Ext Loop:\n";
     const Path& l = extLoop;
     for (int i = 0; i < l.size(); ++i)
     {
          Point p = l.get(i).getFrom();
          
          double x = p.getX();
          double y = p.getY();
          
          std::cout << "\t";
          std::cout << x << ", " << y;

          std::cout << " <";
          std::set<unsigned int> list = p.getList();
          std::set<unsigned int>::iterator it, end;
          it  = list.begin();
          end = list.end();
          for (int c = 0; it != end; ++it, ++c)
          {
               std::cout << *it;
               if (c < (list.size() - 1))
                    std::cout << ", ";
          }
          
          
          std::cout << ">\n";
     }
     
     double x = l.get(l.size() - 1).getTo().getX();
     double y = l.get(l.size() - 1).getTo().getY();
     std::cout << "\t";
     std::cout << x << ", " << y << "\n";
     std::cout << "\n";


     std::cout << "\n";
     for (unsigned int j = 0; j < intLoops.size(); ++j)
     {
          std::cout << "Int Loop #" << j << "\n";
          const Path& l = intLoops[j];
          for (int i = 0; i < l.size(); ++i)
          {
               double x = l.get(i).getFrom().getX();
               double y = l.get(i).getFrom().getY();
               
               std::cout << "\t";
               std::cout << x << ", " << y << "\n";
          }
          
          double x = l.get(l.size() - 1).getTo().getX();
          double y = l.get(l.size() - 1).getTo().getY();
          std::cout << "\t";
          std::cout << x << ", " << y << "\n";
          std::cout << "\n";
     }
     
     if (!intLoops.empty()) { std::cout << "\n"; }
}


//   Functions
vector<pair<double, double> >
     utils::gnuplot::details::getFirstPack(
          const vector<pair<double, double> >& dataV, double epsilon)
{
     vector<pair<double, double> > pack;
     for (unsigned int i = 0; i < dataV.size(); ++i)
     {
          if (pack.empty()) { pack.push_back(dataV[i]); }

          else if ((dataV[i].first - pack.back().first) <= epsilon)
               pack.push_back(dataV[i]);

          else { return pack; }          
     }
     return pack;
}


std::vector<utils::gnuplot::details::Polygon>
     utils::gnuplot::details::getPolygons(
          const std::vector<std::vector<std::pair<double, double> > >& data,
          bool logScaleX, bool logScaleY)
{
     //   List all points of the space, and associate to each one:
     //        - Its coordinates.
     //        - The list of functions which are associated to it.
          //   Retrieve all coordinate combinations
     set<double> xSet, ySet;
     for (unsigned int i = 0; i < data.size(); ++i)
     {
          for (unsigned int j = 0; j < data[i].size(); ++j)
          {
               xSet.insert(data[i][j].first);
               ySet.insert(data[i][j].second);
          }
     }
     
     vector<double> xList, yList;
     copy(xSet.begin(), xSet.end(), back_inserter(xList));
     copy(ySet.begin(), ySet.end(), back_inserter(yList));
     
     sort(xList.begin(), xList.end());
     sort(yList.begin(), yList.end());
     
     vector<vector<Point> > grid;
     for (unsigned int i = 0; i < yList.size(); ++i)
     {
          grid.push_back(vector<Point>());
          for (unsigned int j = 0; j < xList.size(); ++j)
          {
               grid[i].push_back(Point(xList[j], yList[i]));
               
               if (logScaleX) { grid[i].back().applyLog10X(); }
               if (logScaleY) { grid[i].back().applyLog10Y(); }
          }
     }
     
          //   Associate the function to each point of the space
     for (unsigned int i = 0; i < data.size(); ++i)
     {
          for (unsigned int j = 0; j < data[i].size(); ++j)
          {
               const unsigned int& f = i;

               double x = data[i][j].first;
               double y = data[i][j].second;
               
               if (logScaleX) { x = log10(x); }
               if (logScaleY) { y = log10(y); }


               for (unsigned int k = 0; k < grid.size(); ++k)
               {
                    for (unsigned int l = 0; l < grid[k].size(); ++l)
                    {
                         if (grid[k][l].getX() == x
                                   && grid[k][l].getY() == y)
                         {                         
                              grid[k][l].addToList(f);
                              break;
                         }
                    }
               }
          }
     }
     
     
     //   Build a polygon around each point
     vector<Polygon> polygons;
     for (unsigned int i = 0; i < grid.size(); ++i)
     {
          for (unsigned int j = 0; j < grid[i].size(); ++j)
          {               
               unsigned int nX = grid[i].size();
               unsigned int nY = grid.size();
               
               unsigned int x0 = ((int) j - 1) <   0 ?  0       : (j - 1);
               unsigned int y0 = ((int) i - 1) <   0 ?  0       : (i - 1);
               unsigned int x1 = ((int) j + 1) >= nX ? (nX - 1) : (j + 1);
               unsigned int y1 = ((int) i + 1) >= nY ? (nY - 1) : (i + 1);
               
               set<unsigned int> list = grid[i][j].getList();
               
               //   Points
               Point topLeft(
                         (grid[y0][x0].getX() + grid[i][j].getX()) / 2.0,
                         (grid[y0][x0].getY() + grid[i][j].getY()) / 2.0,
                         list);            
               
               Point topRight(
                         (grid[y0][x1].getX() + grid[i][j].getX()) / 2.0,
                         (grid[y0][x1].getY() + grid[i][j].getY()) / 2.0,
                         list);
               
               Point bottomRight(
                         (grid[y1][x1].getX() + grid[i][j].getX()) / 2.0,
                         (grid[y1][x1].getY() + grid[i][j].getY()) / 2.0,
                         list);
               
               Point bottomLeft(
                         (grid[y1][x0].getX() + grid[i][j].getX()) / 2.0,
                         (grid[y1][x0].getY() + grid[i][j].getY()) / 2.0,
                         list);
               
               if (logScaleX)
               {
                    topLeft.apply10PowX();
                    topRight.apply10PowX();
                    bottomRight.apply10PowX();
                    bottomLeft.apply10PowX();
               }
               
               if (logScaleY)
               {
                    topLeft.apply10PowY();
                    topRight.apply10PowY();
                    bottomRight.apply10PowY();
                    bottomLeft.apply10PowY();
               }

               
               //   Edges
               Edge topE(topLeft, topRight);
               Edge rightE(topRight, bottomRight);
               Edge bottomE(bottomRight, bottomLeft);
               Edge leftE(bottomLeft, topLeft);
               
               
               //   External loop
               Path extLoop;
               extLoop.add(topE);
               extLoop.add(rightE);
               extLoop.add(bottomE);
               extLoop.add(leftE);
               
               
               //   Polygon
               Polygon poly;
               poly.setExtLoop(extLoop);
               polygons.push_back(poly);
          }
     }
     
     
     //   Merge the polygons
     int count = 0;
     bool merged;
     do
     {
          merged = false;
          for (unsigned int i = 0; i < polygons.size(); ++i)
          {               
               for (unsigned int j = 0; j < polygons.size(); ++j)
               {
                    if (i == j) { continue; }

                    
                    //   Try to merge polygons 'i' and 'j'
                    merged = polygons[i].mergeWith(polygons[j]);
                    if (merged)  
                    {
                         for (int k = (j + 1); k < polygons.size(); ++k)
                              polygons[k - 1] = polygons[k];

                         polygons.pop_back();
                         break;
                    }
               }
               
               if (merged) { break; }
          }
     }
     while (merged);
     
     
     //   Return the remaining polygons
     return polygons;
}



// ---------------------------------------------------------------------------
//	'latex' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//   Details (to ignore)
// ===========================================================================
void utils::latex::details::splitTime(double t,
                                      unsigned int& days,
                                      unsigned int& hours,
                                      unsigned int& minutes, 
                                      unsigned int& seconds,
                                      unsigned int& milliseconds)
{
     days = floor(t / (24*60*60*1000));
     t -= (days * (24*60*60*1000));
     
     hours = floor(t / (60*60*1000));
     t -= (hours * (60*60*1000));
     
     minutes = floor(t / (60*1000));
     t -= (minutes * (60*1000));
     
     seconds = floor(t / 1000);
     t -= (seconds * 1000);
     
     milliseconds = round(t);
}


// ===========================================================================
//   Functions
// ===========================================================================
void utils::latex::table(const vector<vector<Cell*> >& grid,
                         string outputFile,
                         vector<unsigned int> hLines,
                         vector<unsigned int> vLines,
                         vector<double> columnWidth)
{
     ofstream os(outputFile.c_str());
     if (grid.empty()) { os.close(); return; }


     //   Header
     os << "\\begin{longtable}{";
     for (unsigned int i = 0; i < grid[0].size(); ++i)
     {
          if (i < vLines.size())
               for (unsigned int k = 0; k < vLines[i]; ++k) { os << "|"; }


          if (i < columnWidth.size() && columnWidth[i] > 0.0)
               os << "p{" << columnWidth[i] << "cm}";
          else { os << "l"; }
     }
     
     if (grid[0].size() < vLines.size())
     {
               for (unsigned int k = 0; k < vLines[grid[0].size()]; ++k)
                    os << "|";
     }
     
     os << "}\n";
     
     
     //   Content
     for (unsigned int i = 0; i < grid.size(); ++i)
     {
          if (i < hLines.size())
          {
               for (unsigned int k = 0; k < hLines[i]; ++k)
                    os << "\t\\hline\n";
          }
          os << "\t";          
          
          for (unsigned int j = 0; j < grid[i].size(); ++j)
          {
               if (j > 0) { os << " & "; }
               os << grid[i][j]->getStr();
          }
          os << "\\\\\n";
     }
     
     if (grid.size() < hLines.size())
     {
          for (unsigned int k = 0; k < hLines[grid.size()]; ++k)
               os << "\t\\hline\n";
     }


     //   Footer
     os << "\\end{longtable}\n";
     os.close();
}
