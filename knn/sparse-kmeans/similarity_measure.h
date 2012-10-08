#ifndef CLUSTER_SIMILARITY_MEASURE_H
#define CLUSTER_SIMILARITY_MEASURE_H

#include <vector>
#include "document_cluster.h"
namespace cluster
{
/**
 * @brief interface that represent a binary similarity measure used
 * for clustering algorithm (it can be Jaccard, Dice, Russel and Rao, ...)
 */
class SimilarityMeasure
{
public:
    SimilarityMeasure() {}
    virtual ~SimilarityMeasure() {}

private:
    /// avoid copy constructor
    SimilarityMeasure(const SimilarityMeasure &e);
    /// avoid affectation operator
    SimilarityMeasure& operator=(const SimilarityMeasure &e);

public:
    /// compute similarity between two clusters
    virtual double Compute(const DocumentCluster& c1, const DocumentCluster& c2) const = 0;

};
}
#endif 
