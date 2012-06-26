#include <iostream>
#include <sstream>
#include <cmath>
#include "cluster_method.h"
using namespace std;
using namespace galaxy::antispam::anchor;

struct Anchor: public ClusterNode
{
    int m;
    Anchor(int i): m(i)
    {
    }
    /*virtual*/bool IsLeaf() const
    {
        return true;
    };
    /*virtual*/
    float Similarity(const ClusterNode& other) const
    {
        if (other.IsLeaf())
        {
            const Anchor& a = dynamic_cast<const Anchor& >(other);
            return CalSimilarity(a);
        }
        return other.Similarity(*this);
    }

    float CalSimilarity(const Anchor& other) const
    {
        return abs(m - other.m) < 2 ? 1 : 0;
    }
    virtual std::string ToString() const
    {
        stringstream ss;
        ss << m;
        return ss.str();
    }
};

int main(int args, char** argv)
{

    /////
    ClusterNodePtrVector input;

    for (int i = 0; i < 10; ++i)
    {
        input.push_back(ClusterNodePtr(new Anchor(i)));
    }

    for (int i = 0; i < 10; ++i)
    {
        input.push_back(ClusterNodePtr(new Anchor(i + 20)));
    }

    srand(time(0));
    for (int i = 0; i < 10; ++i)
    {
        input.push_back(ClusterNodePtr(new Anchor(random()/100000000)));
    }

    ClusterNodePtrList output;
    /////

    ClusterMethod  myMethod(0.5);
    myMethod.ClusterAnalyst(input, output);
    ClusterNodePtrList::iterator it = output.begin();
    for (it; it != output.end(); ++it)
    {
        cout << "{ " << (*it)->ToString() << " }\n";
    }
    cout << "-------------------------------\n";

    {
        input.clear();
        //output.clear();
        myMethod.ClusterAnalyst(output, input);
        ClusterNodePtrVector::iterator it = input.begin();
        for (it; it != input.end(); ++it)
        {
            cout << "{ " << (*it)->ToString() << " }\n";
        }
    }
    // ClusterNodeAggregateCluster  myMethod2(0.4);
    // myMethod2.ClusterAnalyst(output, input);

    ////
    return 0;
}
