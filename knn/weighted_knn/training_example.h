/*-------------------------------------------------*/
#include <inttypes.h>
#include <tr1/memory>
#include "vector_ref.h"
#ifndef LINE_MAX
#define LINE_MAX 10000
#endif
enum MODE
{
    TRAINING = 0,
    TESTING,
    VALIDATING
};
class TrainingExample
{
public:
    //Unique Index
    uint32_t index;
    //Values of all Attributes for a instance
    VectorRef<double> innerValue;
    //Euclidean distance
    double distance;
    //Instance weight
    double weight;
    //Is the instance near to anyone
    bool isNearest2AtleastSome;
    int numAttr;
    double* value() {return innerValue.Data();}
    TrainingExample(int num):
        index(0), innerValue(num), distance(0.0), weight(0.0),
        isNearest2AtleastSome(false), numAttr(num)
    {
    }
    TrainingExample(double *a, int num):
        index(0), innerValue(num), distance(0.0), weight(0.0),
        isNearest2AtleastSome(false), numAttr(num)
    {
        SetVal(a, num);
    }
    ~TrainingExample()
    {
        distance = 0.0;
        weight = 0.0;
        index = 0;
    }
    void SetVal(double *a, int num)
    {
        for (int i = 0; i < num; ++i)
        {
            innerValue[i] = a[i];
        }
    }
    void GetVal(double *a)
    {
        double* b = innerValue.Data();
        for (int i = 0; i < numAttr; ++i)
        {
            a[i] = b[i];
        }
    }
    //Not using this normalization anymore
    //Using Standard Deviation instead
    void NormalizeVals()
    {
        for(int i = 0; i < numAttr - 1; i++)
        {
            innerValue[i] = innerValue[i] / (1.0 + innerValue[i]);
        }
    }
};
