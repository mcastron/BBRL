
#include "utils.h"
#include "RandomGen.h"

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
void utils::gnuplot::plot(const vector<vector<pair<double, double> > >& data,
                          const vector<string>& titles,
                          utils::gnuplot::GnuplotOptions opt)
{
     //   1.   Write the data file
     string dataFile = ("data/export/tmp.dat");
     ofstream datOS(dataFile.c_str());
     for (unsigned int i = 0; i < data.size(); ++i)
     {
          for (unsigned int j = 0; j < data[i].size(); ++j)
               datOS << data[i][j].first << "\t" << data[i][j].second << "\n";
          if (!data[i].empty()) { datOS << "\n\n"; }
     }
     datOS.close();
     
     
     //   2.   Write the script file
     string scriptFile = ("data/export/tmp.gp");     
     ofstream scriptOS(scriptFile.c_str());
     scriptOS << opt.getScriptOptions();
     
     unsigned int index = 0;
     for (unsigned int i = 0; i < data.size(); ++i)
     {
          if (!data[i].empty())
          {
               if (index == 0) { scriptOS << "plot \'" << dataFile << "\'"; }
               else            { scriptOS << ", ''";                        }
               
               scriptOS << " index " << index++;
               if (opt.getWith() != "")
                    scriptOS << " with " << opt.getWith();
               scriptOS << " lt palette frac " << (i / (double) data.size());
     
               if (i < titles.size())
                    scriptOS << " title \"" << titles[i] << "\"";
          }
     }
     scriptOS << "\n";
     scriptOS.close();


     //   3.   Generate the 'eps' file     
     string command = "gnuplot -e \"input=\'";
     command += dataFile;
     command += "\'\" ";
     command += scriptFile;
     
     system(command.c_str());
     
     
     //   4.   Delete the temporary files
     command = "rm " + dataFile;
     system(command.c_str());
     
     command = "rm " + scriptFile;
     system(command.c_str());
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
     os << "\\begin{tabular}{";
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
     os << "\\end{tabular}\n";
     os.close();
}
