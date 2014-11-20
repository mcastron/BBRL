

// ---------------------------------------------------------------------------
//	'statistics' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
template<typename T>
double utils::statistics::computeMean(const std::vector<T>& data)
{
	if (data.empty()) { return 0.0; }
	
	double sum;
	for (unsigned int i = 0; i < data.size(); ++i) { sum += data[i]; }
	
	return (sum / (double) data.size());	
}


template<typename T>
double utils::statistics::computeVariance(
		double mean, const std::vector<T>& data)
{
	if (data.size() <= 1) { return 0.0; }

	double sumDiff2;
	for (unsigned int i = 0; i < data.size(); ++i)
	{
		double tmp = (mean - data[i]);
		sumDiff2 += (tmp * tmp);
	}
	
	return (sumDiff2 / (double) (data.size() - 1));	
}


template<typename T>
std::pair<double, double> utils::statistics::computeCI68(
		double mean, double stDev, const std::vector<T>& data)
{
	return utils::statistics::details::computeCI(
			1.0, mean, stDev, data.size());
}


template<typename T>
std::pair<double, double> utils::statistics::computeCI95(
		double mean, double stDev, const std::vector<T>& data)
{
	return utils::statistics::details::computeCI(
			1.96, mean, stDev, data.size());
}


template<typename T>
std::pair<double, double> utils::statistics::computeCI99(
		double mean, double stDev, const std::vector<T>& data)
{
	return utils::statistics::details::computeCI(
			3.0, mean, stDev, data.size());
}


// ---------------------------------------------------------------------------
//	'search' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
template<typename C>
std::vector<typename C::const_iterator>
		utils::search::min(
				const typename C::const_iterator begin,
				const typename C::const_iterator end)
{
	std::vector<typename C::const_iterator> minList;	
	for (typename C::const_iterator it = begin; it != end; ++it)
		if (minList.empty() || (*it <= *(minList.front())))
		{
			if (!minList.empty() && (*it < *(minList.front())))
				minList.clear();
			
			minList.push_back(it);
		}
	
	return minList;
}


template<typename C>
std::vector<typename C::const_iterator>
		utils::search::max(
				const typename C::const_iterator begin,
				const typename C::const_iterator end)
{
	std::vector<typename C::const_iterator> maxList;	
	for (typename C::const_iterator it = begin; it != end; ++it)
		if (maxList.empty() || (*it >= *(maxList.front())))
		{
			if (!maxList.empty() && (*it > *(maxList.front())))
				maxList.clear();
			
			maxList.push_back(it);
		}
	
	return maxList;
}
