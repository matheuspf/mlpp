#ifndef CPP_ML_LOGISTIC_REGRESSION_BASE_H
#define CPP_ML_LOGISTIC_REGRESSION_BASE_H

#include "../../Modelo.h"

#include "../Classifier.h"


#define USING_LOGISTIC_REGRESSION(...) using BaseLogisticRegression = __VA_ARGS__;              \
                                       using BaseLogisticRegression::BaseLogisticRegression,    \
                                             BaseLogisticRegression::alpha,                     \
                                             BaseLogisticRegression::regularizer,               \
                                             BaseLogisticRegression::optimizer,                 \
                                             BaseLogisticRegression::sigmoid,                   \
                                             BaseLogisticRegression::logSoftmax;


template <class Regularizer, class Optimizer>
struct LogisticRegressionBase
{
    LogisticRegressionBase (double alpha = 1e-8, const Optimizer& optimizer = Optimizer()) :
                            alpha(alpha), optimizer(optimizer) {}

    LogisticRegressionBase (const Optimizer& optimizer, double alpha = 1e-8) :
                            alpha(alpha), optimizer(optimizer) {}
    

    template <class T>
    static auto sigmoid (const T& x)
    {
        using std::exp;
        using Eigen::exp;

        return 1.0 / (1.0 + exp(-x));
    }


    double logSoftmax (const Vec& vx, int k)
    {
        return vx(k) - std::log(Eigen::exp(vx.array()).sum());
    }

    Vec logSoftmax (const Vec& vx)
    {
        return vx.array() - std::log(Eigen::exp(vx.array()).sum());
    }


    
    double alpha;

    Regularizer regularizer;

    Optimizer optimizer;
};


#endif // CPP_ML_LOGISTIC_REGRESSION_BASE_H