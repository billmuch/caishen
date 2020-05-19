#include "mccfr.h"

#include <random>

namespace caishen
{
namespace cfr
{

//TODO: set random seeds outside of this class
int sampleActionIndex(const Eigen::Ref<const Eigen::Array<double, Eigen::Dynamic, 1>> & probs)
{
    double eps = 0.0;

    int size = probs.rows();

    std::mt19937 rng(time(0));

    double roll = std::uniform_real_distribution<double>(0.0, 1.0)(rng);

    double sum = 0.0;
    for (int a = 0; a < size; a++)
    {
        double prob = eps*(1.0 / size) + (1.0-eps)*probs[a];
        if (roll >= sum && roll < sum + prob)
        {
            return a;
        }

        sum += prob;
    }

    assert(false);
    return -1;
}

//TODO: set random seeds outside of this class
int sampleSubActions(std::vector<double> &probs)
{
    double eps = 0.0;

    int size = probs.size();

    std::mt19937 rng(time(0));

    double roll = std::uniform_real_distribution<double>(0.0, 1.0)(rng);

    double sum = 0.0;
    for (int a = 0; a < size; a++)
    {
        double prob = eps*(1.0 / size) + (1.0-eps)*probs[a];
        if (roll >= sum && roll < sum + prob)
        {
            return a;
        }

        sum += prob;
    }

    assert(false);
    return -1;
}

//TODO: adjust at runtime when trainning
int getSubActionsSampleTimes(std::vector<double> &probs)
{
    return (int)(probs.size() * .75);
}


void InformationSet::applyRegretMatching()
{
    currentPolicy = (cumulativeRegrets < 0).select(0, cumulativeRegrets);
    double sum = currentPolicy.sum();
    if (sum > 0)
    {
        currentPolicy /= sum;
    }
    else
    {
        currentPolicy.setConstant(1.0/cumulativeRegrets.rows());
    }
}

void InformationSet::getAverageStrategy(Eigen::Ref<Eigen::Array<double, Eigen::Dynamic, 1>> averageStrategy)
{
    double sum = cumulativePolicy.sum();
    if (sum > 0)
    {
        averageStrategy = cumulativePolicy / sum;
    }
    else
    {
        averageStrategy.setConstant(1.0/cumulativePolicy.rows());
    }
}

int InformationSet::getActionSize()
{
    return cumulativeRegrets.rows();
}

std::ostream &operator<<(std::ostream &strm, const InformationSet &is)
{
    strm << "totalCfr: ";
    for (int i = 0; i < is.cumulativeRegrets.rows(); i++)
    {
        strm << is.cumulativeRegrets[i] << ", ";
    }
    return strm;
}

} // namespace cfr
} // namespace caishen