#include <list>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "training_example.h"
/* Using a list to store the training and testing examples. */

typedef std::list<TrainingExample, std::allocator<TrainingExample> > TrainingDataList;

class KNNClassifier
{
private:
    int mNumOfAttr;
    int mNumOfClasses;
    int mK;
    float mLearningRate;
    bool* mIsBEAttIncluded;
    double* mAttWeights;
    KNNClassifier(KNNClassifier&){};
public:
    KNNClassifier(int num_att, int num_class, int k = 3, float learningR = 0.005);
    ~KNNClassifier();
    void SetK(int k){ mK = k;}
    void SetLearningRate(float l){ mLearningRate = l;}
    /*-----------------------------------------------------------------*/
    /* filename - File from which the training/testing data is read    */
    /* rlist - The data structure that holds the training/test data    */
    /* rlistExamples - # of training/test examples                     */
    /*-----------------------------------------------------------------*/
    float SimpleKNN(TrainingDataList *trainList,
                    int trainExamples,
                    TrainingDataList *testList,
                    int testExamples);

    /* Attribute Weighted K Nearest Neighbor Algorithm */
    /*-----------------------------------------------------------------*/
    /* trainList - List of training examples                           */
    /* trainExamples - # of training examples                          */
    /* testList - List of testing examples                             */
    /* testExamples - # of testing examples                            */
    /*-----------------------------------------------------------------*/
    float AttributeWKNN(TrainingDataList *trainList,
                        int trainExamples,
                        TrainingDataList *testList,
                        int testExamples);

    /* Instance Weighted K Nearest Neighbor Algorithm */
    /*-----------------------------------------------------------------*/
    /* trainList - List of training examples                           */
    /* trainExamples - # of training examples                          */
    /* testList - List of testing examples                             */
    /* testExamples - # of testing examples                            */
    /*-----------------------------------------------------------------*/
    float InstanceWKNN(TrainingDataList *trainList,
                       int trainExamples,
                       TrainingDataList *testList,
                       int testExamples);

    /* K Nearest Neighbor Algorithm with Backward Elimination */
    /*-----------------------------------------------------------------*/
    /* trainList - List of training examples                           */
    /* trainExamples - # of training examples                          */
    /* testList - List of testing examples                             */
    /* testExamples - # of testing examples                            */
    /*-----------------------------------------------------------------*/
    float BackwardElimination(TrainingDataList *trainList,
                              int trainExamples,
                              TrainingDataList *testList,
                              int testExamples);

    /* Adjust weights by using Gradient Descent */
    /*---------------------------------------------------------------------*/
    /* qvalue - Value of a particular attribute                            */
    /* tlist  - List of training examples                                  */
    /* error  - Error in prediction of class for an instance               */
    /* index  - indices corresponding to the K nearest neighbors(instances)*/
    /* isAttWeightedKNN = true (Adjust attribute weights)                  */
    /*                  = false (Adjust instance weights)                  */
    /*---------------------------------------------------------------------*/
    void AdjustWeightsByGradientDescent(double *qvalue,
                                        TrainingDataList *tlist,
                                        double error, uint32_t *index,
                                        bool isAttWeightedKNN);

    /* Learn weights by cross validation */
    /*----------------------------------------------------------------------*/
    /* 3 Fold Cross Validation                                              */
    /* data - training data                                                 */
    /* iterations - learn weights for number of iterations on a given cross */
    /* fold                                                                 */
    /* numExamples - # of examples in the training set                      */
    /* isAttWKNN = true (Learn attribute weights)                           */
    /*           = false (Learn instance weights)                           */
    /*----------------------------------------------------------------------*/
    float CrossValidate(TrainingDataList *data, int iterations,
                        int numExamples, bool isAttWKNN);


    /*-----------------------------------------------------------------*/
    /* Reads the training and testing data into the list.              */
    /*-----------------------------------------------------------------*/
    /* filename - File from which the training/testing data is read    */
    /* rlist - The data structure that holds the training/test data    */
    /* rlistExamples - # of training/test examples                     */
    /*-----------------------------------------------------------------*/
    bool readData4File(char *filename,
                       TrainingDataList *rlist,
                       int *rlistExamples);


    /* Normalizes data values using standard deviation. */
    void NormalizeByStandardDeviation(TrainingDataList *trainList,
                                      int trainExamples);

    void InitAttWeights();


    /*------------------------------------------------------------------*/
    /* Test Simple KNN, Instance WeightedKNN, Attribute WeightedKNN and */
    /* KNN by Backward Elimination)                                     */
    /* tlist - training list                                            */
    /* data - testing list                                              */
    /* isInstanceWeighted - Instance WKNN                               */
    /* mode - Training/Testing                                          */
    /* isBackwardElimination - KNN by Backward Elimination              */
    /* isAttWKNN - Attribute WKNN                                       */
    /*------------------------------------------------------------------*/
    int TestKNN(TrainingDataList *tlist, TrainingDataList data,
                bool isInstanceWeighted, MODE mode,
                bool isBackwardElimination, bool isAttWKNN);


    /*------------------------------------------------*/
    /* Initialize the array to include all attributes */
    /*------------------------------------------------*/
    void BackwardEliminationInit();

///////////////////////////////////////////////////////////////////////////////
    /* Finds K nearest neighbors and predicts class according to algorithm used. */
    /*---------------------------------------------------------------------------*/
    /* Predict Class by KNN (Simple KNN, Instance WeightedKNN,                   */
    /*                       Attribute WeightedKNN, KNN by Backward Elimination) */
    /* tlist - training list                                                     */
    /* query - query instance to be classified                                   */
    /* isWeightedKNN - Instance Weighted KNN                                     */
    /* index - Indices of the K nearest neighbors will be stored in index array  */
    /* mode - Specifies whether we are training or testing                       */
    /* isBE - Backward Elimination                                               */
    /* isAttWeightedKNN - Attribute WeightedKNN                                  */
    /*---------------------------------------------------------------------------*/
    int PredictByKNN(TrainingDataList *tlist, double *query,
                     bool isWeightedKNN, uint32_t *index, MODE mode,
                     bool isBE, bool isAttWeightedKNN);


//////////////////////////////////////////////////////////////////////////
////////Learning Functions for Instance and Attribute Weighted KNN////////
    /* Learning weights by running KNN on training data. */
    /*---------------------------------------------------------------------------*/
    /* 3 Fold Cross Validation                                                   */

    /* data - training data                                                      */
    /* iterations - learn weights for number of iterations on a given cross fold */
    /* numExamples - # of examples in the training set                           */
    /* isAttWKNN = true (Learn attribute weights)                                */
    /*           = false (Learn instance weights)                                */
    /*---------------------------------------------------------------------------*/
    float LearnWeights(TrainingDataList *tlist,  //Training Data
                       TrainingDataList data,   //Testing Data
                       //Train data = Test data
                       int iterations,				   //Learn for # of iterations
                       int numExamples,			   //# of examples
                       MODE mode,					   // mode = TRAINING
                       int desiredAccuracy,		   //Desired accuracy in %
                       bool isAttWeightedKNN);	   //Attribute or Instance Weighted



    /*-------------------------------------------------------------------------*/
    /* Comparison function used by the sorting function for list objects.      */
    /*-------------------------------------------------------------------------*/
    static inline bool compare(const TrainingExample t1, const TrainingExample t2)
    {
        return (t1.distance < t2.distance);
    }

    /*------------------------------------------------------------------------*/
    /* line - to read file fp line                                            */
    /* max - maximum line length to read                                      */
    /* fp - file to read from                                                 */
    /* Return Parameter: 0 if end of file, line length otherwise.             */
    /* Copies a file contents to another file.                                */
    /*------------------------------------------------------------------------*/
    inline int GetLine(char *line, int max, FILE *fp)
    {
        if(fgets(line, max, fp) == NULL)
            return 0;
        else
            return strlen(line);
    }
};
