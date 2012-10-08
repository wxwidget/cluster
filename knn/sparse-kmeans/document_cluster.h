#ifndef CLUSTER_DOCUMENT_CLUSTER
#define CLUSTER_DOCUMENT_CLUSTER
#include <tr1/memory>
#include <list>
#include <vector>
#include <map>
#include <inttypes.h>
namespace cluster
{
class ClusteringAlgorithm;
class PageFeature;
struct Feature
{
    uint32_t id;
    uint32_t freq;
    float  weight; 
    Feature(const Feature& a): id(a.id), freq(a.freq), weight(a.weight)
    {
    }
    Feature(uint32_t i = 0, uint32_t f = 1, float w = 1):
        id(i), freq(f), weight(w)
    {
    }
    class LessThan
    {
    public:
        bool operator()(const Feature& a, const Feature& b) const
        {
            return a.id < b.id;
        }
    };
    bool operator < (const Feature& b) const
    {
        return id < b.id;
    }
    bool operator == (const Feature& b)
    {
        return id == b.id;
    }
};

class DocumentCluster
{
public:
    DocumentCluster():mCardinal(0) {}
    ~DocumentCluster() {}
    typedef std::tr1::shared_ptr<PageFeature> PageFeaturePtr;
private:
    /// avoid copy constructor
    DocumentCluster(const DocumentCluster &e);
    /// avoid affectation operator
    DocumentCluster& operator=(const DocumentCluster &e);

public:
    void AddDocument(uint32_t id, PageFeaturePtr& page);
    void RemoveDocument(uint32_t id, PageFeaturePtr& page);
    /// add the content of other cluster in this one (union)
    void Merge(const DocumentCluster &other);
    /// get cardinal of cluster
    unsigned Cardinal() const { return mCardinal;}
    ///get list of document
    const std::map<uint32_t, PageFeaturePtr>& GetDocuments() const { return mDocuments;}
    /// get list of words that this cluster contains
    const std::list<Feature>& GetFeatures() const { return mFeatures;}
    /// display content of cluster on ostream
    void Display(std::ostream &os) const;

protected:
    friend class ClusteringAlgorithm;

private:
    /// list of documents in this cluster
    std::map<uint32_t, PageFeaturePtr> mDocuments;
    /// list of words in this cluster
    std::list<Feature> mFeatures;
    /// number of documents in this cluster
    uint32_t mCardinal;
};
typedef std::tr1::shared_ptr<DocumentCluster> DocumentClusterPtr;
}
#endif
