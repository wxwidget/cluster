/**
 * @file ClusterNode_cluster.h
 * @brief the file designed to cluster the ClusterNodes
 *
 * @author charis.yangc@aliyun-inc.com
 * @version 2.0
 * @date 2010-09-13
 *
 */
#ifndef GALAXY_ANTISPAM_ANCHOR_COMMON_CLUSTER_METHOD_H
#define GALAXY_ANTISPAM_ANCHOR_COMMON_CLUSTER_METHOD_H

#include "cluster_node.h"
#include "cluster_structure.h"

namespace galaxy
{
namespace antispam
{
namespace anchor
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
}
}
#include "cluster_method.hpp"
#endif /* GALAXY_ANTISPAM_ClusterNode_COMMON_CLUSTER_H */
