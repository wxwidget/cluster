#ifndef CLUSTERING_COMMON_CLUSTER_STRUCTURE_H
#define  CLUSTERING_COMMON_CLUSTER_STRUCTURE_H
#include <vector>
#include <set>
#include <list>
#include <tr1/memory>
#include "cluster_node.h"
namespace clustering
{
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

class Cluster: public ClusterNode
{
public:
    Cluster()
    {
    }
    virtual ~Cluster(){}

    virtual  void Add(const ClusterNodePtr&);
    
    virtual  void Remove(const ClusterNodePtr&);

    void Combine(const  Cluster& otherCluster);

    /*override*/float Similarity(const ClusterNode&) const;

    /*override*/ bool IsLeaf() const
    {
        return false;
    }

    inline void Clear();
   
    /*override*/std::string ToString() const;

    /*overrid*/ ClusterNodeContainerType* GetContent()
    {
        return &mClusterNodes;
    }
private:

    ClusterNodeContainerType mClusterNodes;
    /*
    size_t mClusterNodeNum;
    size_t mUniqClusterNodeNum;
    size_t mTotalWordCnt;
    */
};

inline void Cluster::Clear()
{
    mClusterNodes.clear();
    /*
    mClusterNodeNum = 0;
    mUniqClusterNodeNum = 0;
    mTotalWordCnt = 0;
    */
}

////////////////////////////////////////////////////////////////////////////////

typedef std::tr1::shared_ptr<Cluster> ClusterPtr;
typedef std::vector<ClusterNodePtr>  ClusterNodePtrVector;
typedef std::list<ClusterNodePtr>  ClusterNodePtrList;

}//end of clustering

#endif 
