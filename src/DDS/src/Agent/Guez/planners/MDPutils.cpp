#include "MDPutils.h"

#include "../utils/guez_utils.h"

namespace MDPutils{

	void policyEvaluation(uint S, 
			uint A, 
			bool rsas, 
			double* P,
			double* R,
			double gamma,
			double epsilon,
			const uint* PI,
			double* V){
		
		assert(gamma > 0);
		assert(gamma < 1); // != 1 to guarantee convergence

		uint SA = S*A;
		double sqeps = epsilon*epsilon;
		
		double* V0 = new double[S];
		double* V1 = new double[S];
		for(size_t l=0;l<S;++l)
			V1[l]=0.0;
		size_t z = 1;
		double* Vu;
		double* Vv;
		size_t ll;

		//size_t it = 0;
		do{
			if(z==1){
				Vu = V1;Vv = V0;z=0;
			}
			else{
				Vu = V0;Vv = V1;z=1;
			}
			for(ll=0; ll<S; ++ll){
				if(!rsas)
					Vv[ll] = R[ll*A+PI[ll]] + gamma*guez_utils::inner_prod(P+ll*SA+PI[ll]*S,Vu,S);
				else{
					Vv[ll] = 0;	
					uint aS = PI[ll]*S;
					uint lSA = ll*SA;
					for(uint ss=0;ss<S;++ss)
						Vv[ll] += P[lSA+aS+ss]*(R[lSA+aS+ss] + gamma*Vu[ss]);
				}	
			}
			//std::cout << ++it << " " << utils::sqnorm_2(V0,V1,S) << " " << sqeps << std::endl;
		}
		while(guez_utils::sqnorm_2(V0,V1,S) > sqeps);

		memcpy(V,Vv,S*sizeof(double));
		delete[] V0;
		delete[] V1;

	}

	void valueIteration(uint S,
			uint A,
			bool rsas,
			double* P,
			double* R,
			double gamma,
			double epsilon,
			uint* PI,
			double* V){

		assert(gamma > 0);
		assert(gamma < 1); // != 1 to guarantee convergence

		uint SA = S*A;
		double sqeps = epsilon*epsilon;
		//Initialize value function
		//(Create 2 to avoid copying memory each iteration)
		double* V0 = new double[S];
		double* V1 = new double[S];
		for(size_t ll=0;ll<S;++ll)
			V1[ll]=0.0;
		size_t z = 1;
		double* Vu;
		double* Vv;

		//	TODO - Modified
//		double Q[S][A];
		std::vector<std::vector<double> > Q(S);
		for (unsigned int i = 0; i < Q.size(); ++i) { Q[i].resize(A); }
		//

		size_t ll,aa;
		do{
			if(z==1){
				Vu = V1;Vv = V0;z=0;
			}
			else{
				Vu = V0;Vv = V1;z=1;
			}
			for(ll=0;ll<S;++ll){
				PI[ll] = 0;
				Vv[ll] = -std::numeric_limits<double>::infinity();
				uint lSA = ll*SA;
				for(aa=0;aa<A;++aa){
					if(!rsas){
						Q[ll][aa] = R[ll*A+aa] + gamma*guez_utils::inner_prod(P+lSA+aa*S,Vu,S);
					}
					else{
						Q[ll][aa] = 0;
						uint aS = aa*S;
						for(uint ss=0;ss<S;++ss){
							Q[ll][aa] += P[lSA+aS+ss]*(R[lSA+aS+ss] + gamma*Vu[ss]);
						}
					}
					if(Q[ll][aa] > Vv[ll]){
						PI[ll] = aa;
						Vv[ll] = Q[ll][aa];
					}
				}
			}
		//	std::cout <<  utils::sqnorm_2(V0,V1,S) << " " << std::flush;
		}	
		while(guez_utils::sqnorm_2(V0,V1,S) > sqeps);

		memcpy(V,Vv,S*sizeof(double));
		delete[] V0;
		delete[] V1;

	}


	void valueIterationRmax(uint S,
			uint A,
			bool rsas,
			double* P,
			double* R,
			double gamma,
			double epsilon,
			uint* PI,
			double* V,
			const uint* counts,
			uint B){

		assert(gamma > 0);
		assert(gamma < 1); // != 1 to guarantee convergence

		//TEMP: assumes rmax=1	
		double Vmax = 1/(1-gamma);

		uint SA = S*A;
		double sqeps = epsilon*epsilon;
		//Initialize value function
		//(Create 2 to avoid copying memory each iteration)
		double* V0 = new double[S];
		double* V1 = new double[S];
		for(size_t ll=0;ll<S;++ll)
			V1[ll]=0.0;
		size_t z = 1;
		double* Vu;
		double* Vv;

		//	TODO - Modified
//		double Q[S][A];
		std::vector<std::vector<double> > Q(S);
		for (unsigned int i = 0; i < Q.size(); ++i)
			Q[i].resize(S);
		//
		size_t ll,aa;
		do{
			if(z==1){
				Vu = V1;Vv = V0;z=0;
			}
			else{
				Vu = V0;Vv = V1;z=1;
			}
			for(ll=0;ll<S;++ll){
				PI[ll] = 0;
				Vv[ll] = -std::numeric_limits<double>::infinity();
				uint lSA = ll*SA;
				for(aa=0;aa<A;++aa){
					//TEMP super non-efficient
					uint sum = 0;
					for(uint ss=0;ss<S;++ss)
						sum += counts[lSA+aa*S+ss];
					if(sum < B)
						Q[ll][aa] = R[ll*A+aa] + gamma*Vmax; //Transition to absorbing state with max reward at every step
					else if(!rsas)
						Q[ll][aa] = R[ll*A+aa] + gamma*guez_utils::inner_prod(P+lSA+aa*S,Vu,S);
					else{
						Q[ll][aa] = 0;
						uint aS = aa*S;
						for(uint ss=0;ss<S;++ss){
							Q[ll][aa] += P[lSA+aS+ss]*(R[lSA+aS+ss] + gamma*Vu[ss]);
						}
					}
					if(Q[ll][aa] > Vv[ll]){
						PI[ll] = aa;
						Vv[ll] = Q[ll][aa];
					}
				}
			}
		}	
		while(guez_utils::sqnorm_2(V0,V1,S) > sqeps);

		memcpy(V,Vv,S*sizeof(double));
		delete[] V0;
		delete[] V1;

	}
}
