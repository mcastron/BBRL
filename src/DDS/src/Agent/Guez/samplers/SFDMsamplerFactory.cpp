#include "SFDMsamplerFactory.h"
#include "SFDMTransitionSampler.h"


/*   Model:    S --> V --> P --> N
 *   where S ~ P(S = k)    size of set of outcomes
 *         V ~ Uniform in sets of size S
 *         P ~ Restricted Dirichlet to V, with alpha prior
 *         N ~ Multinomial(P)
 *   
 *   Sample Method 1:
 *
 *   For each s,a:
 *   Given data N, compute integrated P directly in closed-form (See Friedman Singer paper)
 *   and sample from it.
 *	
 *	 Sample Method 2:
 *
 *	 For each s,a:
 *	 Given data N, sample P (exact inference) to sample MDP models.
 *	 First sample P(S | N) then P(V | N, S), then P(P | V, N)
 */
SFDMsamplerFactory::SFDMsamplerFactory(double priorcount,
		bool _expDecay,
		double _beta,
		uint _S, uint _A) : 
	alpha(priorcount),
	expDecay(_expDecay),
	beta(_beta),
	S(_S), A(_A){

		SA=S*A;
		//Compute P(S=k) once
		Pk = new double[S];
		double Pksum = 0;
		if(expDecay){
			for(uint k=0;k<S;++k){
				Pk[k] = pow(beta,k+1);
				Pksum += Pk[k];
			}	
		}else{
			for(uint k=0;k<S;++k){
				Pk[k] = pow(k+1,-beta);
				Pksum += Pk[k];
			}
		}
		for(uint k=0;k<S;++k)
			Pk[k] /= Pksum;

		//Precompute sum of logs and gammalog functions
		// sum_i (log(i)) 
		sumLog = new double[S];
		sumLog[0] = 0; 
		for(uint i=1;i<S;++i)
			sumLog[i] = sumLog[i-1] + log(i+1);
		
		lgam = new double[S*S];
		for(uint i=0;i<S;++i){
			for(uint N=0;N<S;++N){
				lgam[i*S+N] = lgamma((i+1)*alpha+N);
			}
		}

	
		//Observed set of outcomes for all SA
		SIG0 = new std::vector<uint>*[SA];
		OFlag = new bool*[SA]; 
		
		for(uint i=0;i<SA;++i){
			OFlag[i] = new bool[S];
			std::fill(OFlag[i],OFlag[i]+S,0);
			SIG0[i] = new std::vector<uint>();
			SIG0[i]->reserve(20);
		}
		//Maintains dirty flag for the PkD=P(S=k|D) distr.
		//for each (s,a)
		dirtyPkD = new bool[SA];
		std::fill(dirtyPkD,dirtyPkD+SA,0); //Start clean
		PkD = new double*[SA];
		for(uint i=0;i<SA;++i){
			PkD[i] = new double[S];
			memcpy(PkD[i],Pk,S*sizeof(double));
		}
		CSum = new uint[SA];
		std::fill(CSum,CSum+SA,0);
		
		ordering.resize(S);
		for(uint i=0;i<S;++i)
			ordering[i] = i;


		//Precompute CDLpre for K0 and small N
		CDLpre = new double*[S-1];
		double logmk;
		for(uint k0i=0;k0i<(S-1);++k0i){
			CDLpre[k0i] = new double[S];
			uint K0 = k0i + 1;

			for(uint N=0;N<S;++N){
				if(N<K0)
					CDLpre[k0i][N] = 0;
				else{
					double sumMk = 0;
					for(uint i=K0;i<=S;++i){
						logmk = log(Pk[i-1])+sumLog[i-1]+lgam[(i-1)*S]-lgam[(i-1)*S+N];
						if(i!=K0)
							logmk -= sumLog[i-K0-1];
						double mk = exp(logmk);
						CDLpre[k0i][N] += (K0*alpha+N)*mk/(i*alpha+N);
						sumMk += mk;	
					}
					if(sumMk > 0)
						CDLpre[k0i][N] = CDLpre[k0i][N] / sumMk;				
					else  //In case of numerical errors
						CDLpre[k0i][N] = 1;
				}
			}
		}
		
		
}

void SFDMsamplerFactory::reset(){

		for(uint i=0;i<SA;++i){
			std::fill(OFlag[i],OFlag[i]+S,0);
			SIG0[i]->clear();
		}

		std::fill(dirtyPkD,dirtyPkD+SA,0); //Start clean
		for(uint i=0;i<SA;++i){
			memcpy(PkD[i],Pk,S*sizeof(double));
		}
		std::fill(CSum,CSum+SA,0);
}


SFDMsamplerFactory::~SFDMsamplerFactory(){
	delete[] Pk;
	delete[] sumLog;
	delete[] lgam;
	
	delete[] dirtyPkD;
	for(uint i=0;i<SA;++i){
		delete SIG0[i];
		delete[] OFlag[i];
		delete[] PkD[i];
	}
	delete[] SIG0;
	delete[] OFlag;
	delete[] PkD;
	delete[] CSum;

}	

void SFDMsamplerFactory::updateCounts(uint s, uint a, uint obs){
	uint ind = s*A+a;
	if(OFlag[ind][obs] == 0){
		OFlag[ind][obs] = 1;
		SIG0[ind]->push_back(obs);
	}
	CSum[ind]++;
	dirtyPkD[ind] = true;
}



//Sampler* SFDMsamplerFactory::getTransitionSampler(const uint* counts,uint s, uint a, uint S){
Sampler* SFDMsamplerFactory::getTransitionSampler(const uint* counts, uint, uint, uint S){

	double* P = new double[S];
	
	uint K0 = 0;
	uint N = 0;
	for(uint ll=0;ll<S;++ll){
		if(counts[ll] > 0){
			K0++;
			N += counts[ll];
		}
	}
	//Sample Method 1
	uint K0M = std::max(1,(int)K0);
	//Compute C(D,L)
	double CDL;
	if(N == 0)
		CDL = 0;
	else if(N < S)
		CDL = CDLpre[K0-1][N];
	else{
		CDL = 0;	
		double sumMk = 0;
		for(uint i=K0;i<=S;++i){
			double logmk;
			logmk = log(Pk[i-1])+sumLog[i-1]+lgam[(i-1)*S];
			if(N<S)
				logmk -= lgam[(i-1)*S+N];
			else
				logmk -= lgamma(i*alpha+N);
			if(i!=K0)
				logmk -= sumLog[i-K0-1];
			double mk = exp(logmk);
			CDL += (K0M*alpha+N)*mk/(i*alpha+N);
			sumMk += mk;	
		}
		if(sumMk > 0)
			CDL = CDL / sumMk;
		else
			CDL = 1;
	}
	double leftover = (1-CDL)/(S-K0);
	double CDLK0alpN = CDL/(K0M*alpha+N);
	double sum = 0;
	for(uint ll=0;ll<S;++ll){
		if(counts[ll] == 0)
			P[ll] = leftover;
		else
			P[ll] = CDLK0alpN*(alpha+counts[ll]); 
		sum += P[ll];

	}
	SFDMTransitionSampler* sampler = new SFDMTransitionSampler(P,S);
	return (Sampler*) sampler;
}

//Note: this could be optimized further by having a special case for large CSum[ind]
void SFDMsamplerFactory::checkAndUpdate(uint ind){
	if(dirtyPkD[ind]){
		dirtyPkD[ind] = 0;
		uint K0 = SIG0[ind]->size();
		uint K0M = std::max(1,(int)K0);
		//Update P(k | D)
		std::fill(PkD[ind],PkD[ind]+S,0); 
		double sumMk = 0;
		double mk, logmk;
		for(uint i=K0M;i<=S;++i){
			logmk = log(Pk[i-1])+sumLog[i-1]+lgam[(i-1)*S];
			if(CSum[ind]<S)
				logmk -= lgam[(i-1)*S+CSum[ind]];
			else
				logmk -= lgamma(i*alpha+CSum[ind]);
			if(i!=K0)
				logmk -= sumLog[i-K0-1];
			mk = exp(logmk);
			PkD[ind][i-1] = mk;
			sumMk += mk;	

		}
		//Instead of wasting time computing this, bypass
		//computation for large N (large confidence)
		if(sumMk == 0){
			PkD[ind][K0M-1] = 1;
		}else{
			//Normalize
			for(uint i=(K0M-1);i<S;++i){
				PkD[ind][i] /= sumMk;
			}
		}
	}
}

Sampler* SFDMsamplerFactory::getMDPSampler(const uint* counts, uint S, uint A,
		double* R, bool rsas, double gamma){

	for(uint i =0; i<SA; ++i){
		checkAndUpdate(i);
	}

	SFDMMDPSampler* sampler = new SFDMMDPSampler(counts, alpha, S, A, R, rsas, gamma,this);

	return (Sampler*) sampler;
}


//This function sets up a sampler for transition parameters (using precomputations)
Sampler* SFDMsamplerFactory::getTransitionParamSampler(const uint* counts, uint s, uint a, uint S){
	uint ind = s*A+a;
	checkAndUpdate(ind);

	SFDMTransitionParamSampler* sampler = new SFDMTransitionParamSampler(counts,alpha,S,this,ind);
	return (Sampler*) sampler;
}


