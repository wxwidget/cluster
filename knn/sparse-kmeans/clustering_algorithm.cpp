#include <set>
#include <algorithm>
#include "clustering_algorithm.h"
#include "similarity_measure.h"
using namespace cluster;
using namespace std;
ClusteringAlgorithm::ClusteringAlgorithm(double threshold,
                 IndexDocumentInstance& docsIndex,
                 SimilarityMeasure &similarity,
                 unsigned nbClustersMax,
                 bool affectAllIndividuals) :
  mSimilarity(similarity), mThreshold(threshold), mDocsIndex(docsIndex),
  mNbClustersMax(nbClustersMax), mNbClusters(0), 
  mAffectAllIndividuals(affectAllIndividuals)
{
     mIndexToCluster.resize(docsIndex.size());
}

ClusteringAlgorithm::~ClusteringAlgorithm()
{
}

struct _cmpClustersBySize
{
  bool operator()(const DocumentClusterPtr c1, const DocumentClusterPtr c2) const
  {
    if (bool(c1) && bool(c2))
    {
      return c1->Cardinal() > c2->Cardinal();
    }
    return c1;
  }
};

DocumentClusterPtr  ClusteringAlgorithm::GetClusterByIndex(uint32_t index)
{
    if (index < mIndexToCluster.size())
    {
        return mIndexToCluster[index];
    }
    return DocumentClusterPtr();
}

std::list<DocumentClusterPtr>& ClusteringAlgorithm::CreateCentriod(uint32_t clusterNb)
{
    uint32_t docCt = mDocsIndex.size();
    if (docCt < 1)
    {
        return mClusterCentriod;
    }
    srand (time(NULL) );
    set<int> select;
    for (uint32_t i = 0; i < clusterNb; ++i)
    {
        DocumentClusterPtr cluster(new DocumentCluster());
        int rd = 0;
        int mostTry = 100 * clusterNb;
        while(mostTry-- > 0)
        {
            rd = rand() % docCt;
            if (select.find(rd) == select.end())
            {
                select.insert(rd);
                break;
            }
        }
        cluster->AddDocument(rd, mDocsIndex[rd]);
        mClusterCentriod.push_back(cluster);
    }
    mNbClusters = clusterNb;
    return mClusterCentriod;
}

bool  ClusteringAlgorithm::FinalMergeCluster()
{
    bool change = false;
    std::list<DocumentClusterPtr>::iterator it = mClusterCentriod.begin();
    std::list<DocumentClusterPtr>::iterator next = it;
    std::list<DocumentClusterPtr>::iterator ltn = mClusterCentriod.end();
    for (; it != ltn; ++it)
    {
        DocumentClusterPtr& c1 = *it;
        next = it;
        for (++next; next != ltn;)
        {
            DocumentClusterPtr& c2 = *next;
            if (!c1 || !c2) 
            {
                continue;
            }
            double testMerge = mSimilarity.Compute(*c1, *c2);
            if (testMerge >= mThreshold)
            {
                c1->Merge(*c2);
                next = mClusterCentriod.erase(next);
                change = true;
            }
            else
            {
                ++next;
            }
        }
    }
    return change;
}

bool ClusteringAlgorithm::Assignment(uint32_t cnt)
{
    std::list<DocumentClusterPtr>::iterator it = mClusterCentriod.begin(); 
    // Individual cnt is not affected
    DocumentCluster c;
    PageFeaturePtr& page = mDocsIndex[cnt];
    c.AddDocument(cnt, page);
    double maxSimi = -100;
    DocumentClusterPtr maxCluster;
    for (; it != mClusterCentriod.end(); ++it)
    {
        // search the maximum similarity with an existing cluster
        if (!*it) continue;
        double currentSimi = mSimilarity.Compute(**it, c);
        if (currentSimi > maxSimi)
        {
            maxSimi = currentSimi;
            maxCluster = *it;
        }
    }
    DocumentClusterPtr& oldCluster = mIndexToCluster[cnt];
    if (maxCluster && oldCluster.get() != maxCluster.get())
    {
        page->similarity = maxSimi;
        maxCluster->AddDocument(cnt, page);
        if (oldCluster)
        {
            oldCluster->RemoveDocument(cnt, mDocsIndex[cnt]);
        }
        oldCluster = maxCluster;
        return true;
    }
    return false;
}

std::list<DocumentClusterPtr> ClusteringAlgorithm::Finalize()
{
    std::set<DocumentClusterPtr, _cmpClustersBySize> assignedSet;
    std::set<DocumentClusterPtr, _cmpClustersBySize>::const_iterator sit, site;
    std::vector<DocumentClusterPtr>::const_iterator it, ite = mIndexToCluster.end();

    for (it = mIndexToCluster.begin(); it != ite; ++it)
    {
        if (*it && assignedSet.find(*it) == assignedSet.end())
        {
            assignedSet.insert(*it);
        }
    }
    // Try all object that are not affected to test if there is a
    // cluster when it can enter
    unsigned cnt = 0;

    for (it = mIndexToCluster.begin(); it != ite; ++it, ++cnt)
    {
        if (!*it)
        {
            // Individual cnt is not affected
            DocumentCluster c;
            PageFeaturePtr& page = mDocsIndex[cnt];
            c.AddDocument(cnt, page);
            // search the maximum similarity with an existing cluster
            double maxSimi = 0;
            DocumentClusterPtr maxCluster;
            for (sit = assignedSet.begin(), site = assignedSet.end(); sit != site; ++sit)
            {
                double currentSimi = mSimilarity.Compute(**sit, c);
                if (currentSimi > maxSimi)
                {
                    maxSimi = currentSimi;
                    maxCluster = *sit;
                }
            }
            if (maxCluster && (maxSimi > mThreshold || mAffectAllIndividuals))
            { // Add individual in maxCluster
                maxCluster->AddDocument(cnt, page);
                mIndexToCluster[cnt] = maxCluster;
            }
        }
    }
    assignedSet.clear();
    for (it = mIndexToCluster.begin(); it != ite; ++it)
    {
        if (*it  && assignedSet.find(*it) == assignedSet.end())
            assignedSet.insert(*it);
    }
    std::list<DocumentClusterPtr> res;
    res.insert(res.begin(), assignedSet.begin(), assignedSet.end());
    return res;
}

double ClusteringAlgorithm::TryMerge(uint32_t doc1, uint32_t doc2)
{
    if (doc1 >= mIndexToCluster.size() || doc2 >= mIndexToCluster.size())
    {
        return -1; // 
    }
    bool  createC1 = false;
    bool  createC2 = false;
    DocumentClusterPtr c1 = mIndexToCluster[doc1];
    DocumentClusterPtr c2 = mIndexToCluster[doc2];

    if (!c1 && !c2 && mNbClusters > 0 && mNbClusters >= mNbClustersMax)
    {
        return 0; // we can not create new cluster, maximum was already found.
    }
    if (!c1)
    {
        createC1 = true;
        c1.reset(new DocumentCluster());
        ++mNbClusters;
        PageFeaturePtr it = mDocsIndex[doc1];
        c1->AddDocument(doc1, it);
        mIndexToCluster[doc1] = c1;
    }
    if (!c2)
    {
        createC2 = true;
        c2.reset(new DocumentCluster());
        ++mNbClusters;
        PageFeaturePtr it = mDocsIndex[doc1];
        c2->AddDocument(doc2, it);
        mIndexToCluster[doc2] = c2;
    }
    if (c1 == c2)
    {
        return 1;
    }
    double testMerge = mSimilarity.Compute(*c1, *c2);
    // TODO: Test if having three cluster {c1 - doc1}, {doc1, doc2}, {c2 -
    // doc2} is better than {c1}, {c2} or {c1, c2}
    if (testMerge >= mThreshold)
    {
        c1->Merge(*c2);
        std::map<uint32_t, PageFeaturePtr>::const_iterator lit, lite = c2->GetDocuments().end();
        for (lit = c2->GetDocuments().begin(); lit != lite; ++lit)
            mIndexToCluster[lit->first] = c1;
        --mNbClusters;
    }
    else if (mNbClusters > mNbClustersMax)
    {
        // We must not allow cluster of 1 element count for one cluster
        // when number is limited
        if (createC1)
        {
            --mNbClusters;
            mIndexToCluster[doc1].reset();
        }
        if (createC2 && mNbClusters > mNbClustersMax)
        {
            --mNbClusters;
            mIndexToCluster[doc2].reset();
        }
    }
    return testMerge;
}
