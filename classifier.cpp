#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
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
        map<string, int> * num_posts_with_word = new map<string, int>;

        //For each label C, the number of posts with that label.
        map<string, int> * num_of_labels = new map<string, int>;

        //For each label C and word w, the number of posts with 
        //label C that contain w
        //   Label       word   count
        map<string, map<string, int>> * label_word_count 
                        = new map<string, map<string, int>>;

        ~Classifier()
        {
            delete num_posts_with_word;
            delete num_of_labels;
            delete label_word_count;
        }
        void add_row_to_map(csvstream * file);

        int getTotalPosts() {return totalPosts;}
        int getUniqueWords() {return uniqueWords;}

    private:
        int totalPosts = 0;
        int uniqueWords = 0;
};

void Classifier::add_row_to_map(csvstream * file)
{
    std::map<std::string, std::string> curr_row;

    while (*file >> curr_row)
    {
        //
    }
}


int main(int argc, char* argv[]) 
{
    //grabs the file names
    std::string trainFile = argv[1];
    //std::string testFile = argv[2];

    //opens the CSV files in the heap via csvstream
    csvstream * trainCSV = new csvstream(trainFile);
    //csvstream * testCSV = new csvstream(testFile);
    
    Classifier clasi;
    clasi.add_row_to_map(trainCSV);


    delete trainCSV;
    return 0;
}