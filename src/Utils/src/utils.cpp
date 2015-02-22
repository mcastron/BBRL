

#include "utils.h"
#include "RandomGen.h"


// ===========================================================================
//	Namespaces
// ===========================================================================
// ---------------------------------------------------------------------------
//	'statistics' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Details (to ignore)
// ===========================================================================
std::pair<double, double> utils::statistics::details::computeCI(
		double alpha, double mean, double stDev, unsigned int n)
{
	assert(n > 0);

	std::pair<double, double> p;
	
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
		unsigned int hTheta, const std::vector<double>& theta,
		unsigned int hP, std::vector<double>& P,
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
		const std::vector<double>& theta, std::vector<double>& P)
{
	utils::statistics::sampleDirichlet(0, theta, 0, P, theta.size());
}

std::vector<double> utils::statistics::sampleDirichlet(
		const std::vector<double>& theta)
{
	std::vector<double> P(theta.size());
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
std::string utils::compression::compressStr(
	const std::string& str, int compressionlevel) throw (std::runtime_error)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionlevel) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

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
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}


std::string utils::compression::uncompressStr(const std::string& str)
										throw (std::runtime_error)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

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
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}


// ---------------------------------------------------------------------------
//	'parsing' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
std::string utils::parsing::getValue(int argc, char* argv[], std::string opt)
                                                       throw (ParsingException)
{
     for (int i = 1; i < (argc - 1); ++i)
          if (std::string(argv[i]) == opt) { return std::string(argv[i + 1]); }
          
     throw parsing::ParsingException(opt);
}


std::vector<std::string> utils::parsing::getValues(int argc, char* argv[],
          std::string opt, unsigned int n) throw (ParsingException)
{
     std::vector<std::string> values;
     for (int i = 1; i < (argc - (int) n); ++i)
     {
          if (std::string(argv[i]) == opt)
          {
               for (int j = 1; j <= (int) n; ++j)
                    values.push_back(std::string(argv[i + j]));
               break;
          }
     }
     if (values.size() == n) { return values; }
     
     throw parsing::ParsingException(opt);
}


bool utils::parsing::hasFlag(int argc, char* argv[], std::string opt)
{
     for (int i = 1; i < argc; ++i)
          if (std::string(argv[i]) == opt) { return true; }
     
     return false;
}
