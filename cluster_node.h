#ifndef CLUSTERING_COMMON_CLUSTER_NODE_H
#define CLUSTERING_COMMON_CLUSTER_NODE_H
#include <tr1/memory>
#include <string>
#include <set>
namespace clustering
{
////////////////////////////////////////////////////////////////////////////////
class ClusterNode;
typedef std::tr1::shared_ptr<ClusterNode> ClusterNodePtr;

class ClusterNode
{
public:
    typedef std::set<ClusterNodePtr> ClusterNodeContainerType;

    virtual bool IsSkip() const
    {
        return false;
    }
    virtual bool IsLeaf() const = 0;

    virtual float Similarity(const ClusterNode& other) const = 0;

    virtual std::string ToString() const
    {
        return "";
    }

    virtual ~ClusterNode(){};

    virtual ClusterNodeContainerType* GetContent() 
    {
        return NULL;
    }
};
////////////////////////////////////////////////////////////////////////////////
}
#endif
