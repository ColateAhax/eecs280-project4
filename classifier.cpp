#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cmath>
#include "csvstream.hpp"
using namespace std;

// EFFECTS: Return a set of unique whitespace delimited words
set<string> unique_words(const string &str) 
{
  istringstream source(str);
  set<string> words;
  string word;
  while (source >> word) 
  {
    words.insert(word);
  }
  return words;
}

class Classifier
{
    public:
        //  For each word w, the number of posts in the entire 
        //  training set that contain w
        map<string, int> num_posts_with_word;

        //For each label C, the number of posts with that label.
        map<string, int> num_of_labels;

        //For each label C and word w, the number of posts with 
        //label C that contain w
        //   Label       word   count
        map<string, map<string, int>> label_word_count;
        ~Classifier()
        {
            //
        }
        void add_row_to_map(csvstream * file, bool trainOnly);

        int getTotalPosts() {return totalPosts;}
        int getUniqueWords() {return uniqueWords;}

        void print_classes();
        void print_parameters();
        void predict(csvstream * file);

    private:
        int totalPosts = 0;
        int uniqueWords = 0;
        void print_row(string label, string content);
        double log_prior(double num_posts_with_C);
        double log_likelihood(string label, string word);
        double log_probability(string label, set<string> content);
};

void Classifier::add_row_to_map(csvstream * file, bool trainOnly)
{
    if (trainOnly)
    {
        cout << "training data:" << endl;
    }
    std::map<std::string, std::string> curr_row;

    while (*file >> curr_row)
    {
        string label = curr_row["tag"];
        string content = curr_row["content"];
        if (trainOnly) print_row(label, content);

        totalPosts++;
        num_of_labels[label]++;

        //filters out to only include unique words in content
        set<string> words = unique_words(content);

        for (auto it = words.begin(); it != words.end(); it++)
        {
            num_posts_with_word[*it]++;
            label_word_count[label][*it]++;
        }
    }

    uniqueWords = num_posts_with_word.size();
}

void Classifier::print_row(string label, string content)
{
    cout << "  label = " << label << ", content = " << content << endl;
}

void Classifier::print_classes()
{
    cout << "classes:" << endl;
    for (auto &data : num_of_labels)
    {
        string label = data.first;
        int count = data.second;
        cout << "  " << label << ", " << count;
        cout << " examples, log-prior = " << log_prior(count) <<endl;
    }
}

void Classifier::print_parameters()
{
    cout << "classifier parameters:" << endl;
    for (auto &data : label_word_count)
    {
        string label = data.first;
        map<string, int> temp = data.second;
        for (auto &data_2 : temp)
        {
            string word = data_2.first;
            int count = data_2.second;

            cout << "  " << label << ":" << word << ", count = " << count;
            cout << ", log-likelihood = " << log_likelihood(label, word) <<endl;
        }
    }
}

double Classifier::log_prior(double num_posts_with_C)
{
    return log(num_posts_with_C / totalPosts);
}

double Classifier::log_likelihood(string label, string word)
{
    auto it = label_word_count.find(label);
    map temp = it->second;
    auto it_2 = temp.find(word);
    //  W was seen in a post with label C in the training data
    if (it_2 != temp.end())
    {
        double num_posts_with_C_and_W = label_word_count[label][word];
        double num_posts_with_C = num_of_labels[label];

        return log(num_posts_with_C_and_W / num_posts_with_C);
    }

    //  W was NEVER seen in a post with label C in the training data
    else
    {
        //  W does not occur in posts labeled C
        //  but does occur in the training data overall
        if(num_posts_with_word.find(word) != num_posts_with_word.end())
        {
            double num_posts_with_W = num_posts_with_word[word];
            return log(num_posts_with_W / totalPosts);
        }

        //  Use when w does not occur anywhere at all in the training set.
        return log(1 / ( (double)  totalPosts ) );
    }
}

double Classifier::log_probability(string label, set<string> content)
{
    double result = log_prior(num_of_labels[label]);

    for(auto it : content)
    {
        result += log_likelihood(label, it);
    }

    return result;
}

void Classifier::predict(csvstream * file)
{
    int correct = 0, total = 0;
    std::map<std::string, std::string> curr_row;

    while (*file >> curr_row)
    {
        string correct_label = curr_row["tag"];
        string content = curr_row["content"];

        //assumes the first one is the best match
        string best = num_of_labels.begin()->first;

        //filters out to only include unique words in content
        set<string> words = unique_words(content);
        double best_log_prob = log_probability(best, words);

        for (auto it = num_of_labels.begin()++; it != num_of_labels.end(); it++)
        {
            //if its the first item
            string currLabel = it->first;
            double curr_log_prob = log_probability(currLabel, words);

            if (curr_log_prob > best_log_prob)
            {
                best_log_prob = curr_log_prob;
                best = currLabel;
            }
        }

        cout << "  correct = " << correct_label << ", predicted = " << best;
        cout << ", log-probability score = " << best_log_prob <<endl;
        cout << "  content = " << content << "\n" << endl;

        total++;
        if (best == correct_label) correct++;
    }
    cout << "performance: " << correct << " / " << total;
    cout << " posts predicted correctly" << endl;
}

int main(int argc, char* argv[]) 
{
    cout.precision(3);
    
    bool trainOnly;
    std::string trainFile;
    std::string testFile;

    if (argc == 2)
    {
        trainOnly = true;

        //grabs the file names
        trainFile = argv[1];

        csvstream trainCSV(trainFile);
        Classifier clasi;
        clasi.add_row_to_map(&trainCSV, trainOnly);

        cout << "trained on " << clasi.getTotalPosts() << " examples" << endl;
        cout << "vocabulary size = " << clasi.getUniqueWords() << "\n" << endl;
        clasi.print_classes();
        clasi.print_parameters();
    }
    else
    {
        trainOnly = false;

        //grabs the file names
        trainFile = argv[1];
        testFile = argv[2];

        csvstream trainCSV(trainFile);
        csvstream testCVS(testFile);
        Classifier clasi;
        clasi.add_row_to_map(&trainCSV, trainOnly);

        cout << "trained on " << clasi.getTotalPosts() << " examples" << endl;
        cout << "\ntest data:" << endl;
        clasi.predict(&testCVS);
    }

    return 0;
}