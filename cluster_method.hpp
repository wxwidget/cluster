#ifndef CLUSTERING_CLUSTER_METHOD_HPP
#define  CLUSTERING_CLUSTER_METHOD_HPP
#include <iostream>
namespace clustering
{

/////////////////////////////////////////////////////////////////////////////////
template
<
    typename NodeT,
    typename ClusterT
>
void ClusterMethod::DoCluster(const NodeT& clusterNodeVector,
                              ClusterT& outClusterList) const
{
   
    outClusterList.clear();
    size_t  sz = clusterNodeVector.size();

    typename NodeT::const_iterator nodeIt = clusterNodeVector.begin();

    for (; nodeIt !=  clusterNodeVector.end(); ++nodeIt)
    {
        
        ClusterPtr clusterPtr(new Cluster);
        const typename NodeT::value_type& tClusterPtr = *(nodeIt);
        if (!tClusterPtr || tClusterPtr->IsSkip())
        {
            continue;
        }


        clusterPtr->Add(tClusterPtr);


        typename ClusterT::iterator it = outClusterList.begin();

        while(it != outClusterList.end())
        {
            float sim = clusterPtr->Similarity(*(*it));
            /*
            using namespace std;
            cout << clusterPtr->ToString() << "##"<< (*it)->ToString() << " == " << sim << endl;
            */
            if (sim > this->mThreshold)
            {
                clusterPtr->Add(*it);
                it = outClusterList.erase(it);
            }
            else
            {
                ++it;
            }
        }
        outClusterList.push_back(clusterPtr); 
    }

    
}

////////////////////////////////////////////////////////////////////////////////
template
<
    typename NodeT ,
    typename ClusterT 
>
void ClusterNodeAggregateCluster::DoCluster(const NodeT& clusterNodeVector,
        ClusterT& outClusterList) const
{
    std::vector<size_t> parent(clusterNodeVector.size(), 0);
    uint32_t groups = 0;
    typename NodeT::const_iterator it = clusterNodeVector.begin();
    for (int i = 0; it != clusterNodeVector.end(); ++it, ++i)
    {
        bool found = false;
        typename NodeT::const_iterator ne = it;
        ++ne;
        for (int j = i + 1; ne != clusterNodeVector.end(); ++ne, ++j)
        {
            if (parent[i] && parent[j])
            {
                continue;
            }
            float sim = (*it)->Similarity(*(*ne));
            if (sim > this->mThreshold)
            {
                found = true;
                if (0 == parent[i])
                {
                    groups++;
                    parent[i] = groups;
                    parent[j] = groups;
                }
                else
                {
                    parent[j] = parent[i];
                }
            }
        }
        if (!found && 0 == parent[i])
        {
            groups++;
            parent[i] = 1;
        }
    }
    //outClusterContainer.BuildCluster(parent);
}
}//end of namespace
#endif
