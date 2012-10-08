#include <vector>
#include <string>
#include "cluster_structure.h"

using namespace std;
using namespace clustering;


void Cluster::Add(const ClusterNodePtr& ptr)
{
    if (!ptr)
    {
        return;
    }
    ClusterNodeContainerType* children  = ptr->GetContent();
    if (children)
    {
        ClusterNodeContainerType::const_iterator it = children->begin();
        for (; it != children->end(); ++it)
        {
            Add(*it);
        }
    }
    else
    {
        GetContent()->insert(ptr);
    }
}

void Cluster::Remove(const ClusterNodePtr& ptr)
{
    mClusterNodes.erase(ptr);
}

string Cluster::ToString() const
{
    string  result;
    ClusterNodeContainerType::const_iterator it = mClusterNodes.begin(); 
    for(;it != mClusterNodes.end(); ++it)
    {
        result.append("[");
        result.append((*it)->ToString());
        result.append("]");
    }
    return result;
}

float Cluster::Similarity(const ClusterNode& cluster) const
{
    float maxSim = -1;
    ClusterNodeContainerType::const_iterator it = mClusterNodes.begin(); 
    for(;it != mClusterNodes.end(); ++it)
    {
        float v = -2;
        if (cluster.IsLeaf())
        {
            v = (*it)->Similarity(cluster);
        }
        else
        {
            v  = cluster.Similarity(*(*it));
        }
        if (v > maxSim)
        {
            maxSim = v;
        }
    }
    return maxSim;
}

void Cluster::Combine(const  Cluster& otherCluster)
{
    mClusterNodes.insert(otherCluster.mClusterNodes.begin(), 
        otherCluster.mClusterNodes.end());
}
