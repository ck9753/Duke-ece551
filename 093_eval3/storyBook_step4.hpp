#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "page_step4.hpp"

class storyBook {
 public:
  std::vector<std::pair<size_t, Page> > pages;
  std::map<std::string, long int> combinedMap;

 public:
  // default constructor
  storyBook() : pages() {}

  // copy constructor
  storyBook(const storyBook & rhs) : pages(rhs.pages) {}
  // An assignment operator
  storyBook & operator=(const storyBook & rhs) {
    if (this != &rhs) {
      pages = rhs.pages;
    }
    return *this;
  }

  // destructor
  ~storyBook() {}

  void runStoryBook(const char * dir);
  void checkValidity();
  bool checkUserChoice(size_t userChoice, size_t presentPageNum);
  void printChoiceOptions(Page inputPage);
  void processPages();
  std::vector<Page> dfs(Page start, Page end);
  void printCurrentPath(std::vector<Page> currentPath);
  void findWaysToWin();
  void storeToCombinedMap(Page userChoicePage);
};
