#pragma once

template<class COUNT>
class VALUE
{
public:

    void Set(double count, double value)
    {
        Count = count;
        Total = value * count;
    }

    void Add(double totalReward)
    {
        Count += 1.0;
        Total += totalReward;
    }

    void Add(double totalReward, COUNT weight)
    {
        Count += weight;
        Total += totalReward * weight;
    }

    double GetValue() const
    {
        return Count == 0 ? Total : Total / Count;
    }

    COUNT GetCount() const
    {
        return Count;
    }

private:

    COUNT Count;
    double Total;
};


