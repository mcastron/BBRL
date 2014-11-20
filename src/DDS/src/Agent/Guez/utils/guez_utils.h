/*
	TODO:
		-	Rename 'utils' to 'guez_utils'
		-	When boost is not found, ublas::vector becomes std::vector
		-	'append()', 'dumpXXX()', 'snvRevision()', 'StringToFloat()' and
			'StringToInt()' functions have been removed.
*/
//	TODO
#include <assert.h>	//	for 'assert()'
#include <cstring>	//	for 'memcpy()'
#include <limits>	//	for 'numeric_limits'
#include <string>


#ifdef BOOST_FOUND
#include <boost/numeric/ublas/vector.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#else
#include <vector>

#endif
//	TODO



#pragma once

#include "rng.h"

#include <string>

#include <sstream>
#include <iostream>


typedef unsigned int uint;
namespace guez_utils{

extern RNG rng;

void printA(double*v , size_t s);

void normalize(double* v, size_t s);

double** new2DArray(size_t s1, size_t s2);
void free2DArray(double **a, size_t s1);
void free3DArray(double ***a, size_t s1, size_t s2);
double*** new3DArray(size_t s1, size_t s2, size_t s3);

double inner_prod(double* v1, double* v2, size_t s);

#ifdef BOOST_FOUND
double inner_prod(boost::numeric::ublas::vector<double>& v1,double* v2);

#else
double inner_prod(std::vector<double>& v1,double* v2);

#endif

double sqnorm_2(double* v1, double* v2, size_t s);

double TV(double* v1, double* v2, size_t s);

#ifdef BOOST_FOUND
double TV(const boost::numeric::ublas::vector<double>& p,
		const boost::numeric::ublas::vector<double>& q);
double KLdiv(const boost::numeric::ublas::vector<double>& p,
		const boost::numeric::ublas::vector<double>& q);

#else
double TV(const std::vector<double>& p, const std::vector<double>& q);
double KLdiv(const std::vector<double>& p, const std::vector<double>& q);

#endif

void setSeed(ulong _x);

void sampleDirichlet(double* probs, double* params, uint nump);
void sampleDirichlet(double* probs, const uint* params, uint nump, double prior);
//   TODO - ADDED
void sampleDirichlet(double* probs, const double* params, uint nump, double prior);
//   TODO - ADDED

#ifdef BOOST_FOUND
void sampleDirichlet(double* probs,
		const boost::numeric::ublas::vector<double> &params);
boost::numeric::ublas::vector<double> sampleDirichlet(
		const boost::numeric::ublas::vector<double> &params);

#else
void sampleDirichlet(double* probs, const std::vector<double> &params);
std::vector<double> sampleDirichlet(const std::vector<double> &params);

#endif


//	TODO
//template <class T>
//void append(T x, std::string& filename){
//	fs::path p(filename);
//	fs::ofstream ofs(p, std::ios::app);
//	ofs << x << std::endl;
//	ofs.close();
//}
//
////Overwrite file with elements of vector (print as column)
//template <class T>
//void dump(const vector<T> &v, std::string& filename){
//	fs::path p(filename);
//	fs::ofstream ofs(p, std::ios::out);
//	if(ofs.is_open()){
//		BOOST_FOREACH(T d, v){
//			ofs << d << std::endl;
//		}
//		ofs.close();
//	}
//	else
//		std::cout << "Error: couldn't open file for writing: " << filename << std::endl;
//}
//
////print each vector on a *column*
//template <class T>
//void dumpc(const vector<vector<T> > &vv, std::string& filename, uint N){
//  
//	fs::path p(filename);
//	fs::ofstream ofs(p, std::ios::out);
//	if(ofs.is_open())
//	{
//		for(size_t row = 0; row < N; ++row){
//			for(size_t col = 0; col < vv.size(); ++col){
//				ofs << vv[col][row] << " ";
//			}
//			ofs << std::endl;
//		}
//		ofs.close();
//	}
//	else{
//		std::cout << "Error: couldn't open file for writing: " << filename << std::endl;
//	}
//}
//
//void dump(const vector<double> &v, std::string& filename);
//void dumpc(const vector<vector<double> > &vv, std::string& filename);
//void dump(const vector<vector<double> > &vv, std::string& filename);
//void dump(const vector<vector<double> > &vv, std::string& filename);
//
//float StringToFloat(const string String);
//int StringToInt(const string String);
//
//int getSvnRevision();
//	TODO

}
