#ifndef  CLUSTER_DOCUMENT_CLUSTERING_ALGORITHM
#define  CLUSTER_DOCUMENT_CLUSTERING_ALGORITHM
#include <list>
#include <vector>
#include "document_cluster.h"
#include "page_features.h"
namespace cluster
{
//Fwd declaration
//class DocumentCluster;
class SimilarityMeasure;
typedef std::vector<PageFeaturePtr> IndexDocumentInstance;

class ClusteringAlgorithm
{
public:
    ClusteringAlgorithm(double threshold,
        IndexDocumentInstance &docIsndex,
        SimilarityMeasure &similarity,
        uint32_t nbClustersMax = 2,
        bool affectAllIndividuals = false);

    ~ClusteringAlgorithm();

private:
    /// avoid default constructor
    ClusteringAlgorithm();
    /// avoid copy constructor
    ClusteringAlgorithm(const ClusteringAlgorithm &e);
    /// avoid affectation operator
    ClusteringAlgorithm& operator=(const ClusteringAlgorithm &e);

public:
    /**
     * @brief try to merge two clusters. The first one is the cluster that
     * contains doc1 and the second is the cluster that contains
     * doc2
     */
    double TryMerge(uint32_t doc1, uint32_t doc2);

    /**
     * finalize the clustering algorithm and return the clusters
     */
    std::list<DocumentClusterPtr> Finalize();

    DocumentClusterPtr GetClusterByIndex(uint32_t index);

    /// initialize menbers : create the new id
    std::list<DocumentClusterPtr>& CreateCentriod(uint32_t clusterNb);
    
    std::list<DocumentClusterPtr>& GetCentriod() { return mClusterCentriod;}
    //assign the index document to centriod
    bool Assignment(uint32_t index);

    bool FinalMergeCluster();
private:
    std::list<DocumentClusterPtr> mClusterCentriod;
    // similarity measure 
    SimilarityMeasure			&mSimilarity;
    /// clustering threshold
    double				mThreshold;
    /// documents index : using old index
    IndexDocumentInstance	&mDocsIndex;
    /// index document to cluster. Get the cluster associed to a document
    std::vector<DocumentClusterPtr>		mIndexToCluster;
    /// maximum number of clusters (0 if there is no limit)
    uint32_t				mNbClustersMax;
    /// number of clusters created
    uint32_t				mNbClusters;
    /// when set to true, this force all individuals to be set in
    /// one cluster
    bool				mAffectAllIndividuals;
};
}
#endif 	    /* !CLUSTERINGALGORITHM_H_ */
