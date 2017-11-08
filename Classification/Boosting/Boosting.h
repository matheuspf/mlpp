#ifndef CPP_ML_BOOSTING_H
#define CPP_ML_BOOSTING_H

#include "../Classifier.h"

#include "../Stump/Stump.h"

#include "../../ZipIter/ZipIter.h"


namespace impl
{
    template <class Cls = Stump<false, false>, bool EncodeLabels = true, bool Polymorphic = false>
    struct Boosting : public PickClassifierBase<Boosting<Cls, EncodeLabels, Polymorphic>, EncodeLabels, Polymorphic>
    {
        USING_CLASSIFIER(PickClassifierBase<Boosting<Cls, EncodeLabels, Polymorphic>, EncodeLabels, Polymorphic>);


        Boosting (const Cls& classifierModel = Cls()) : BaseClassifier(1, -1, false), classifierModel(classifierModel) {}


        void fit_ (const Mat& X, const Veci& y)
        {
            fit_(X, y, 10);
        }


        void fit_ (const Mat& X, const Veci& y, int numClassifiers)
        {
            classifiers.resize(numClassifiers, classifierModel);
            weights.resize(numClassifiers);
            
            Vec probs = Vec::Constant(M, 1.0);

            Mati predictions(M, numClassifiers);


            for(int k = 0; k < numClassifiers; ++k)
            {
                classifiers[k].fit(X, y, probs / probs.sum());


                double wPos = 0.0, wNeg = 0.0;

                Veci pred = classifiers[k].predict(X);

                for(int i = 0; i < M; ++i)
                {
                    if(pred(i) * y(i) > 0.0)
                        wPos += probs(i);

                    else
                        wNeg += probs(i);
                }

                weights[k] = 0.5 * std::log(wPos / std::max(wNeg, 1e-8));

                predictions.col(k) = pred;


                for(int i = 0; i < M; ++i)
                {
                    probs(i) = 0.0;

                    for(int l = 0; l <= k; ++l)
                        probs(i) += weights[l] * predictions(i, l);

                    probs(i) = std::exp(-y(i) * probs(i));
                }
            }
        }



        int predict_ (const Vec& x)
        {   
            double sum = std::accumulate(ZIP_ALL(classifiers, weights), 0.0, it::unZip([&](double sum, auto& cls, double w)
            {
                return sum + w * cls.predict(x);
            }));



            return sum > 0 ? positiveClass : negativeClass;
        }

        Veci predict_ (const Mat& X)
        {
            Veci pred(X.rows());

            for(int i = 0; i < X.rows(); ++i)
                pred(i) = predict_(Vec(X.row(i)));

            return pred;
        }




        Cls classifierModel;

        std::vector<Cls> classifiers;
        
        std::vector<double> weights;        
    };
}


template <class Cls = Stump<false>, bool EncodeLabels = true>
using Boosting = impl::Boosting<Cls, EncodeLabels, false>;


namespace poly
{
    template <class Cls = Stump<false>, bool EncodeLabels = true>
    using Boosting = impl::Boosting<Cls, EncodeLabels, true>;
}




#endif // CPP_ML_BOOSTING_H