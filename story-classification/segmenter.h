#ifndef _SEGMENTER_H_
#define _SEGMENTER_H_ value

#include <vector>
#include <string>
#include <algorithm>

#include "definitions.h"
#include "classifier.h"
#include "utility.h"

using namespace utility;

//
// A segment is potentially a story.
//
class Segment
{
public:
    Segment(int num_sentences, const StoryFeature& word_bags)
    {
        num_sentences_ = num_sentences;
        word_bags_ = word_bags;
    }

    void Merge(const Segment& other)
    {
        num_sentences_ += other.GetNumSentences();
        auto np1 = other.GetNP1();
        auto vp = other.GetVP();
        auto np2 = other.GetNP2();

        word_bags_.wordIds_np1.insert(word_bags_.wordIds_np1.end(), np1.begin(), np1.end());
        word_bags_.wordIds_vp.insert(word_bags_.wordIds_vp.end(), vp.begin(), vp.end());
        word_bags_.wordIds_np2.insert(word_bags_.wordIds_np2.end(), np2.begin(), np2.end());
    }

    int GetNumSentences() const { return num_sentences_;}

    PredictResult GetPrediction() const {return prediction_;}
    void SetPrediction(PredictResult pred) {prediction_ = pred;}

    void SetStoryFeature(const StoryFeature& word_bags) {word_bags_ = word_bags;}
    StoryFeature GetStoryFeature() const {return word_bags_;}
    vector<int> GetNP1() const {return word_bags_.wordIds_np1;}
    vector<int> GetVP() const {return word_bags_.wordIds_vp;}
    vector<int> GetNP2() const {return word_bags_.wordIds_np2;}
private:
    PredictResult prediction_;
    int num_sentences_;
    StoryFeature word_bags_;
};

//
// A segmentation is a set of segments.
//
class Segmentation
{
public:
    vector<Segment> segs;
    
    double GetScore() const{ return score_;}
    void SetScore(double score){score_ = score;}

    Segment operator[] (int i) const {return segs[i];}
    int size() const {return segs.size();}
private:
    double score_;
};

//
// Segmenter class.
//
class Segmenter
{
public:   
    Segmenter()
    {}
     
    Segmenter(const string& filename)
    {
        Load(filename);
    }

    // Train / Learn
    void Train(const vector<StoryInfo>& labeled_stories, int num_categories);

    // Inference
    Segmentation DoSegment(const vector<Sentence>& sentences);
    void CalculateTrueScore(const vector<Sentence>& sentences,
        const vector<int>& true_segmentation);

    void Save(const string& filename) const;
    void Load(const string& filename);
private:
    // Train
    Matrix TrainTransitionMatrix(const vector<StoryInfo>& stories);
    vector<double> TrainLengthDistribution(const vector<StoryInfo>& stories);

    // Inference
    Segmentation CreateInitialSegmentation(const vector<Sentence>& sentences);
    Segmentation FindSegmentation(const Segmentation& initial_segmentation);

    // Greedy strategy
    vector<Segmentation> ProposeCandidates(const Segmentation& segmentations);
    Segmentation PickCandidate(const vector<Segmentation>& candidates);
    bool IsCurrentOptimal(const Segmentation& current, const Segmentation& candidate);
    double CalculateScore(Segmentation& segmentation);

private:
    NaiveBayesClassifier classifier_;
    Matrix transitionMatrix_;
    vector<double> length_distrib_;
};

#endif