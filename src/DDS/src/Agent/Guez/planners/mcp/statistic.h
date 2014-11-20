#ifndef STATISTIC_H
#define STATISTIC_H

#include <math.h>

//----------------------------------------------------------------------------

class STATISTIC
{
public:

    STATISTIC();
    STATISTIC(double val, int count);

    void Add(double val);
    void Clear();
    int GetCount() const;
    void Initialise(double val, int count);
    double GetTotal() const;
    double GetMean() const;
    double GetVariance() const;
    double GetStdDev() const;
    double GetStdErr() const;
    double GetMax() const;
    double GetMin() const;
    void Print(const std::string& name, std::ostream& ostr) const;

private:

    int Count;
    double Mean;
    double Variance;
    double Min, Max;
};

inline STATISTIC::STATISTIC()
{
    Clear();
}

inline STATISTIC::STATISTIC(double val, int count)
{
    Initialise(val, count);
}

inline void STATISTIC::Add(double val)
{
    double meanOld = Mean;
    int countOld = Count;
    ++Count;
    assert(Count > 0); // overflow
    Mean += (val - Mean) / Count;
    Variance = (countOld * (Variance + meanOld * meanOld)
                  + val * val) / Count - Mean * Mean;
    if (val > Max)
        Max = val;
    if (val < Min)
        Min = val;
}

inline void STATISTIC::Clear()
{
    Count = 0;
    Mean = 0;
    Variance = 0;
    Min = +INFINITY;//+Infinity;
    Max = -INFINITY;//-Infinity;
}

inline int STATISTIC::GetCount() const
{
    return Count;
}

inline void STATISTIC::Initialise(double val, int count)
{
    Count = count;
    Mean = val;
}

inline double STATISTIC::GetTotal() const
{
    return Mean * Count;
}

inline double STATISTIC::GetMean() const
{
    return Mean;
}

inline double STATISTIC::GetStdDev() const
{
    return sqrt(Variance);
}

inline double STATISTIC::GetStdErr() const
{
    return sqrt(Variance / Count);
}

inline double STATISTIC::GetMax() const
{
    return Max;
}

inline double STATISTIC::GetMin() const
{
    return Min;
}
    
inline void STATISTIC::Print(const std::string& name, std::ostream& ostr) const
{
    ostr << name << ": " << Mean << " [" << Min << ", " << Max << "]" << std::endl;
}

//----------------------------------------------------------------------------

#endif // STATISTIC
