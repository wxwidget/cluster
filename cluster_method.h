#ifndef CLUSTERING_COMMON_CLUSTER_METHOD_H
#define CLUSTERING_COMMON_CLUSTER_METHOD_H

#include "cluster_node.h"
#include "cluster_structure.h"

namespace clustering
{
////////////////////////////////////////////////////////////////////////////////
template
<
    typename ChildT
>
class ClusterMethodBase
{
public:
    ClusterMethodBase(float threshold = 0.5): mThreshold(threshold)
    {
    }
    virtual ~ClusterMethodBase(){}

    inline void SetThreshold(float thr)
    {
        mThreshold = thr;
    }
    inline float GetThreshold() const
    {
        return mThreshold;
    }
    template<typename NodeT, typename ClusterT>
    void ClusterAnalyst(const NodeT& nodes, 
                 ClusterT& clusters) const
     {
          static_cast<const ChildT&>(*this).DoCluster(nodes, clusters);
     }
protected:
    float mThreshold;
};

///////////////////////////////////////////////////////////////////////////////

class ClusterMethod: public ClusterMethodBase<ClusterMethod>
{
public:
    ClusterMethod(float threshold):ClusterMethodBase<ClusterMethod>(threshold)
    {
    }

    template 
   <
     typename NodeT,
     typename ClusterT 
    >   
    void DoCluster(const NodeT& clusterNodeContiner, 
        ClusterT& resultCluster) const;
};
////////////////////////////////////////////////////////////////////////////////

class ClusterNodeAggregateCluster: public ClusterMethodBase<ClusterNodeAggregateCluster>
{
public:
    ClusterNodeAggregateCluster(float threshold): 
        ClusterMethodBase<ClusterNodeAggregateCluster>(threshold)
    {
    }
    template
    <
        typename NodeT,
        typename ClusterT
    >
    void DoCluster(const NodeT& clusterNodeContiner, 
        ClusterT& resultCluster) const;
};

////////////////////////////////////////////////////////////////////////////////
}
#include "cluster_method.hpp"
#endif 
