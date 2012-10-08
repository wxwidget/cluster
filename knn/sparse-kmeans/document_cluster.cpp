#include "document_cluster.h"
#include "page_features.h"
#include <algorithm>
#include <tr1/unordered_map>
#include <tr1/functional>
using namespace cluster;
using namespace std;
namespace 
{
   //push vector<EntityFeature> to vector<Feature> with "type"
}
void DocumentCluster::AddDocument(uint32_t id, PageFeaturePtr& page)
{
    if (!page)
    {
        return;
    }
    if (mDocuments.find(id) != mDocuments.end())
    {
        return ; 
    }
    list<Feature> v;
    //L0: tranform page feature to cluster feature
    v.push_back(Feature(tr1::hash<string>()(page->regId),1));     //the regid
    v.sort();
    list<Feature>::iterator it , ite = mFeatures.end();
    list<Feature>::const_iterator vit , vite = v.end();
    for (it = mFeatures.begin(), vit = v.begin(); vit != vite; ++vit)
    {
        while (it != ite && *it < *vit)
        {
            ++it;
        }
        if (it != ite && *it == *vit)
        {
            ++it->freq;
        }
        else
        {
            mFeatures.insert(it, *vit);
        }
    }
    mDocuments[id] = page;
    ++mCardinal;
}
void DocumentCluster::RemoveDocument(uint32_t id, PageFeaturePtr& page)
{
    list<Feature> v;
    //L0: tranform page feature to cluster feature
    v.push_back(Feature(tr1::hash<string>()(page->regId),1));     //the regid
    v.sort();
    list<Feature>::iterator it , ite = mFeatures.end();
    list<Feature>::const_iterator vit , vite = v.end();
    for (it = mFeatures.begin(), vit = v.begin(); vit != vite; ++vit)
    {
        while (it != ite && *it < *vit)
        {
            ++it;
        }
        if (it != ite && *it == *vit)
        {
            --it->freq;
            if (it->freq < 0)
            {
                it = mFeatures.erase(it);
            }
        }
    }
    mDocuments[id] = PageFeaturePtr();
    --mCardinal;
}
// add the content of other cluster in this one (union)
void DocumentCluster::Merge(const DocumentCluster &other)
{
   // Compute the first part : dot product between two clusters
  std::list<Feature>::iterator wc1, wc1e = mFeatures.end();
  std::list<Feature>::const_iterator wc2, wc2e = other.mFeatures.end();
  for (wc1 = mFeatures.begin(), wc2 = other.mFeatures.begin(); wc2 != wc2e;)
  {
      if (wc1 == mFeatures.end())
      {
          mFeatures.push_back(*wc2);
          ++wc2;
      }
      else if (*wc1 < *wc2)
      {
          ++wc1;
      }
      else if (*wc2 < *wc1)
      {
          mFeatures.insert(wc1, *wc2);
          ++wc2;
      }
      else // equal
      {
          wc1->freq += wc2->freq;
          ++wc1;
          ++wc2;
      }
  }
  mDocuments.insert(other.mDocuments.begin(), other.mDocuments.end());
}
