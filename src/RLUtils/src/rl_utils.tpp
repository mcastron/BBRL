
// ---------------------------------------------------------------------------
//	'rl_utils::simulation' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Classes methods
// ===========================================================================
template<typename XType, typename UType, typename RType>
rl_utils::simulation::SimulationRecord<XType, UType, RType>::
		SimulationRecord(double simGamma_) : simGamma(simGamma_)
{
	assert(simGamma > 0.0);
	assert(simGamma <= 1.0);
}


template<typename XType, typename UType, typename RType>
std::vector<XType>
		rl_utils::simulation::SimulationRecord<XType, UType, RType>::
				getCrossedStateList() const
{
	std::vector<XType> crossedStateList;

	if (!transitionList.empty())
		crossedStateList.push_back(transitionList[0].x);

	for (unsigned int i = 0; i < transitionList.size(); ++i)
		crossedStateList.push_back(transitionList[i].y);
	
	return crossedStateList;
}


template<typename XType, typename UType, typename RType>
std::vector<RType>
		rl_utils::simulation::SimulationRecord<XType, UType, RType>::
				getRewardList() const
{
	std::vector<double> rewardList;
	for (unsigned int i = 0; i < transitionList.size(); ++i)
		rewardList.push_back(transitionList[i].r);
	
	return rewardList;
}


template<typename XType, typename UType, typename RType>
double rl_utils::simulation::SimulationRecord<XType, UType, RType>::
		computeDSR(double gamma) const
{
	
	double tGamma = ((gamma > 0.0) && (gamma <= 1.0)) ? gamma : simGamma;
	return rl_utils::simulation::computeDSR(getRewardList(), tGamma);
}


template<typename XType, typename UType, typename RType>
void rl_utils::simulation::SimulationRecord<XType, UType, RType>::
		serialize(std::ostream& os) const
{
	Serializable::serialize(os);
	
	
	os << getClassName() << "\n";
	os << (2 + 4*transitionList.size()) << "\n";
	
	os << simGamma << "\n";
	
	os << transitionList.size() << "\n";
	for (unsigned int i = 0; i < transitionList.size(); ++i)
	{
		os << (double) transitionList[i].x << "\t";
		os << (double) transitionList[i].u << "\t";
		os << (double) transitionList[i].y << "\t";
		os << (double) transitionList[i].r << "\n";
	}
}


template<typename XType, typename UType, typename RType>
void rl_utils::simulation::SimulationRecord<XType, UType, RType>::
		deserialize(std::istream& is) throw (SerializableException)
{
	Serializable::deserialize(is);
	
	std::string tmp;
	
	
	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	std::string className = tmp;
	if (className != getClassName())
	{
		std::string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	'simGamma'
	if (!getline(is, tmp)) { throwEOFMsg("simGamma"); }
	simGamma = atof(tmp.c_str());
	++i;
	
	
	//	'transitionList'
	if (!getline(is, tmp)) { throwEOFMsg("transitionList"); }
	unsigned int transitionListSize = atoi(tmp.c_str());
	++i;
	
	transitionList.clear();
	for (unsigned int j = 0; j < transitionListSize; ++j)
	{
		if (!getline(is, tmp, '\t')) {
		   std::cout << j << " " << transitionListSize << "\n";
		   throwEOFMsg("transitionList (x)"); }
		XType x = atof(tmp.c_str());
		++i;
		
		if (!getline(is, tmp, '\t')) { throwEOFMsg("transitionList (u)"); }
		UType u = atof(tmp.c_str());
		++i;
		
		if (!getline(is, tmp, '\t')) { throwEOFMsg("transitionList (y)"); }
		XType y = atof(tmp.c_str());
		++i;
		
		if (!getline(is, tmp)) { throwEOFMsg("transitionList (r)"); }
		RType r = atof(tmp.c_str());
		++i;
		
		transitionList.push_back(Transition<XType, UType, RType>(x, u, y, r));
	}
	

	//	Number of parameters check
	if (n != i)
	{
		std::string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
