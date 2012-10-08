#ifndef GALAXY_ANTISPAM_PAGE_FEATURE_H
#define GALAXY_ANTISPAM_PAGE_FEATURE_H
#include <string>
#include <vector>
#include <algorithm>
namespace cluster
{
enum PageType
{
    NON_TYPE = 0,
    GOV_TYPE,
    EDU_TYPE,
    NAV_TYPE,
    INFO_TYPE,
    COMCN_TYPE,
    OTHER_TYPE
};
struct PageFeature
{
    PageFeature():clusterId(0), similarity(0.0), pageType(0),hostRank(0)
    {
    }
    std::string url; //the url, some feature comes from url,e.g. dynamic page
    std::string host;//the host
    std::string regId;     //the regid
    uint32_t clusterId;
    double similarity;
    uint32_t pageType;  // gov, edu , info
    uint32_t hostRank;   //
};
typedef std::tr1::shared_ptr<PageFeature> PageFeaturePtr;
}
#endif
