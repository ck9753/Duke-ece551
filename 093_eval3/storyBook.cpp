#include "storyBook.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

// from readStory class, store pages vector to pages in storyBook class
void storyBook::runStoryBook(const char * dir) {
  readStory file;
  file.readStoryFile(dir);

  pages = file.storeParsedDataToPage(dir);

  checkValidity();
}

// check three conditions for validity
void storyBook::checkValidity() {
  std::vector<size_t> possiblePages;
  std::vector<size_t> possibleChoicesDest;

  // checker to check if pages have at least a WIN and a LOSE page
  size_t checkWin = 0;
  size_t checkLose = 0;

  for (std::vector<std::pair<size_t, Page> >::iterator i = pages.begin();
       i != pages.end();
       ++i) {
    possiblePages.push_back((*i).first);
    for (std::vector<std::pair<size_t, std::string> >::iterator j =
             (*i).second.choices.begin();
         j != (*i).second.choices.end();
         ++j) {
      possibleChoicesDest.push_back((*j).first);
    }

    // 3c. At least one page must be a WIN page and at least one page must be a LOSE page
    if ((*i).second.pageType == "W") {
      checkWin = 1;
    }

    else if ((*i).second.pageType == "L") {
      checkLose = 1;
    }
  }

  if (checkWin == 0 || checkLose == 0) {
    std::cerr << "At least one page must be a WIN page and at least one page must be a "
                 "LOSE page."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // 3a. Every page that is referenced by a choice is valid
  size_t checkChoices = 0;  // checker to check if destNum exist in pages
  for (std::vector<size_t>::iterator destIter = possibleChoicesDest.begin();
       destIter != possibleChoicesDest.end();
       ++destIter) {
    for (std::vector<size_t>::iterator pageIter = possiblePages.begin();
         pageIter != possiblePages.end();
         ++pageIter) {
      if (*destIter == *pageIter) {
        checkChoices = 1;
      }
    }
  }
  if (checkChoices != 1) {
    std::cerr << "Every page that is referenced by a choice should be valid" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 3b. Every page (except page 0) is referenced by at least one *other* page's choices.
  size_t checkPages = 0;
  for (std::vector<size_t>::iterator pageIter = possiblePages.begin();
       pageIter != possiblePages.end();
       ++pageIter) {
    if (*pageIter != 0) {
      for (std::vector<size_t>::iterator destIter = possibleChoicesDest.begin();
           destIter != possibleChoicesDest.end();
           ++destIter) {
        if (*pageIter == *destIter) {
          checkPages = 1;
        }
      }
    }
  }
  if (checkPages != 1) {
    std::cerr << "Every page (except page 0) should be referenced by at least one "
                 "other page's choices."
              << std::endl;
    exit(EXIT_FAILURE);
  }
}

bool storyBook::checkUserChoice(size_t userChoice, size_t presentPageNum) {
  // check if user input is smaller than 1 (userChoice = 0 if input is not string)
  if (userChoice < 1) {
    return false;
  }

  // check if user input is greater than the number of choices
  if (userChoice > pages[presentPageNum].second.choices.size()) {
    return false;
  }
  return true;
}

void storyBook::processPages() {
  // print the Page 0
  pages[0].second.printPage();

  // index for present page number
  size_t presentPageNum = 0;
  // index for checking gameOver
  bool gameOver = false;

  while (!gameOver) {
    // declare input from user
    std::string input;

    // read input
    std::getline(std::cin, input);

    size_t userChoice = atoi(input.c_str());

    if (checkUserChoice(userChoice, presentPageNum) == false) {
      std::cout << "That is not a valid choice, please try again" << std::endl;
    }

    else {
      std::pair<size_t, std::string> userChoicePair =
          pages[presentPageNum].second.choices[userChoice - 1];

      presentPageNum = userChoicePair.first;

      pages[presentPageNum].second.printPage();

      // if pageType is W or L, the game is over and the while loop is stopped
      if (pages[presentPageNum].second.pageType == "W" ||
          pages[presentPageNum].second.pageType == "L") {
        gameOver = true;
      }
    }
  }
}

// A function that does Depth-First Search: Recursively
std::vector<Page> storyBook::dfs(Page start, Page end) {
  std::stack<std::vector<Page> > pathStack;
  std::vector<Page> visited;
  std::vector<Page> invalid;

  // create vector of start Page for stack
  std::vector<Page> startVector;
  startVector.push_back(start);

  // push startVector to pathStack
  pathStack.push(startVector);

  while (pathStack.empty() != 1) {
    // pop the first in path to currentPath
    std::vector<Page> currentPath = pathStack.top();
    pathStack.pop();

    // get the last node as the currentNode
    Page currentNode = currentPath.back();

    // if the currentNode matches with the end node, return to currentPath
    if (currentNode.pageNum == end.pageNum) {
      return currentPath;
    }

    // check if currentNode is in visited
    size_t visitedFlag = 0;
    for (std::vector<Page>::iterator it = visited.begin(); it != visited.end(); it++) {
      if ((*it).pageNum == currentNode.pageNum) {
        visitedFlag = 1;
      }
    }

    // in case that currentNode is never visited
    if (visitedFlag != 1) {
      // push the currentNode to visited vector
      visited.push_back(currentNode);

      for (std::vector<std::pair<size_t, std::string> >::iterator it =
               currentNode.choices.begin();
           it != currentNode.choices.end();
           it++) {
        // add each adjacent node to currentNode
        currentPath.push_back(pages[(*it).first].second);
        // push new path to pathStack
        pathStack.push(currentPath);
        // update currentPath by removing a newly added node for next iteration
        currentPath.pop_back();
      }
    }
  }
  // return empty vector if there is no valid path
  return invalid;
}

// A function that prints currentPath
void storyBook::printCurrentPath(std::vector<Page> currentPath) {
  // if the currentPath from dfs function is empty, it is invalid(unwinnable)
  if (currentPath.empty()) {
    std::cerr << "This story is unwinnable!" << std::endl;
    exit(EXIT_FAILURE);
  }

  // if the story is winnable
  else {
    size_t currentPathLen = currentPath.size();
    size_t currentPathCtr = 0;  // counter for currentNode index
    for (std::vector<Page>::iterator i = currentPath.begin(); i != currentPath.end();
         i++) {
      currentPathCtr++;
      size_t curr_pageNum = (*i).pageNum;
      size_t next_pageNum;
      // get next_pageNum if the node is not the last one
      if (currentPathCtr < currentPathLen) {
        next_pageNum = (*(i + 1)).pageNum;
      }

      size_t userChoiceNum;
      size_t choicesCtr = 0;  // counter for choices vector to find index
      for (std::vector<std::pair<size_t, std::string> >::iterator j =
               (*i).choices.begin();
           j != (*i).choices.end();
           j++) {
        // if choices' destNum matches with next_pageNum
        if ((*j).first == next_pageNum) {
          userChoiceNum = choicesCtr + 1;
        }
        choicesCtr++;
      }

      if (currentPathCtr < currentPathLen) {
        std::cout << curr_pageNum << "(" << userChoiceNum << "),";
      }
      else {
        std::cout << curr_pageNum << "(win)";
      }
    }
    std::cout << "\n";
  }
}

// A function that finds cycle-free ways to win
void storyBook::findWaysToWin() {
  for (std::vector<std::pair<size_t, Page> >::iterator it = pages.begin();
       it != pages.end();
       it++) {
    // find a page that has "W" pageType
    if ((*it).second.pageType == "W") {
      std::vector<Page> currentPath;

      currentPath =
          dfs(pages[0].second,
              (*it).second);  // dfs with page0 for start and page with win for end

      // print currentPath
      printCurrentPath(currentPath);
    }
  }
}

// from readStory class, store pages vector to pages in storyBook class (for step 4)
void storyBook_s4::runStoryBook(const char * dir) {
  readStory_s4 file;
  file.readStoryFile(dir);

  pages = file.storeParsedDataToPage(dir);

  checkValidity();
}

// check three conditions for validity
void storyBook_s4::checkValidity() {
  std::vector<size_t> possiblePages;
  std::vector<size_t> possibleChoicesDest;

  // checker to check if pages have at least a WIN and a LOSE page
  size_t checkWin = 0;
  size_t checkLose = 0;

  for (std::vector<std::pair<size_t, Page_s4> >::iterator i = pages.begin();
       i != pages.end();
       ++i) {
    possiblePages.push_back((*i).first);
    for (std::vector<Page_s4::choices_s>::iterator j = (*i).second.choices.begin();
         j != (*i).second.choices.end();
         ++j) {
      possibleChoicesDest.push_back((*j).destNum);
    }

    // 3c. At least one page must be a WIN page and at least one page must be a LOSE page
    if ((*i).second.pageType == "W") {
      checkWin = 1;
    }

    else if ((*i).second.pageType == "L") {
      checkLose = 1;
    }
  }

  if (checkWin == 0 || checkLose == 0) {
    std::cerr << "At least one page must be a WIN page and at least one page must be a "
                 "LOSE page."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // 3a. Every page that is referenced by a choice is valid
  size_t checkChoices = 0;  // checker to check if destNum exist in pages
  for (std::vector<size_t>::iterator destIter = possibleChoicesDest.begin();
       destIter != possibleChoicesDest.end();
       ++destIter) {
    for (std::vector<size_t>::iterator pageIter = possiblePages.begin();
         pageIter != possiblePages.end();
         ++pageIter) {
      if (*destIter == *pageIter) {
        checkChoices = 1;
      }
    }
  }
  if (checkChoices != 1) {
    std::cerr << "Every page that is referenced by a choice should be valid" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 3b. Every page (except page 0) is referenced by at least one *other* page's choices.
  size_t checkPages = 0;
  for (std::vector<size_t>::iterator pageIter = possiblePages.begin();
       pageIter != possiblePages.end();
       ++pageIter) {
    if (*pageIter != 0) {
      for (std::vector<size_t>::iterator destIter = possibleChoicesDest.begin();
           destIter != possibleChoicesDest.end();
           ++destIter) {
        if (*pageIter == *destIter) {
          checkPages = 1;
        }
      }
    }
  }
  if (checkPages != 1) {
    std::cerr << "Every page (except page 0) should be referenced by at least one "
                 "other page's choices."
              << std::endl;
    exit(EXIT_FAILURE);
  }
}

// A function that checks userChoice is valid
bool storyBook_s4::checkUserChoice(size_t userChoice, size_t presentPageNum) {
  // check if user input is smaller than 1 (userChoice = 0 if input is not string)
  if (userChoice < 1) {
    return false;
  }

  // check if user input is greater than the number of choices
  if (userChoice > pages[presentPageNum].second.choices.size()) {
    return false;
  }

  return true;
}

// A function that prints Choice options separately for step 4
std::vector<size_t> storyBook_s4::printChoiceOptions(Page_s4 inputPage) {
  // declare unavailable choice options
  std::vector<size_t> unavail_opts;

  // check if the pageType is Win or Lose
  if (inputPage.pageType == "W") {
    std::cout << "Congratulations! You have won. Hooray!" << std::endl;
  }

  else if (inputPage.pageType == "L") {
    std::cout << "Sorry, you have lost. Better luck next time!" << std::endl;
  }

  // for pageType N
  else {
    std::cout << "What would you like to do?"
              << "\n"
              << std::endl;

    // a counter for choice index
    int k = 1;

    // print all choices
    for (std::vector<Page_s4::choices_s>::iterator j = inputPage.choices.begin();
         j != inputPage.choices.end();
         ++j) {
      // print normal choices
      if ((*j).key == "empty") {
        std::cout << " " << k << ". " << (*j).text << std::endl;
        k++;
      }

      else {
        // print unavailable option
        if ((*j).keyValue[(*j).key] != combinedMap[(*j).key]) {
          std::cout << " " << k << ". "
                    << "<UNAVAILABLE>" << std::endl;
          // add unavailable choice options to unavail_opts vector
          unavail_opts.push_back(k);
          k++;
        }
        // print available option that has matched variable
        else {
          std::cout << " " << k << ". " << (*j).text << std::endl;
          k++;
        }
      }
    }
  }
  return unavail_opts;
}

// A function that checks a variable has already been set
void storyBook_s4::checkVariables(Page_s4 currPage) {
  for (std::vector<Page_s4::choices_s>::iterator i = currPage.choices.begin();
       i != currPage.choices.end();
       i++) {
    // if there is a variable that has not been set yet
    if (combinedMap.find((*i).key) == combinedMap.end()) {
      // insert key with value as 0
      combinedMap.insert(std::pair<std::string, long int>((*i).key, 0));
    }
  }
}

// A function that process each pages and print accordingly
void storyBook_s4::processPages() {
  std::vector<size_t> unavail_opts;

  // print the Page 0
  pages[0].second.printPage();  // only page text
  // store variable in page 0 to combinedMap
  storeToCombinedMap(pages[0].second);
  // print choice options and store unavailable choice options
  unavail_opts = printChoiceOptions(pages[0].second);

  // index for present page number
  size_t presentPageNum = 0;
  // index for checking gameOver
  bool gameOver = false;

  while (!gameOver) {
    // declare input from user
    std::string input;

    // read input
    std::getline(std::cin, input);

    size_t userChoice = atoi(input.c_str());

    // if the userChoice is out of range
    if (checkUserChoice(userChoice, presentPageNum) == false) {
      std::cout << "That is not a valid choice, please try again" << std::endl;
    }

    else {
      bool outFlag = false;  // flag to terminate  while loop
      bool check = true;     // flag to check userChoice is unavailable
      while (!outFlag) {
        for (std::vector<size_t>::iterator i = unavail_opts.begin();
             i != unavail_opts.end();
             i++) {
          if (userChoice == *i) {
            check = false;
            std::cout << "That choice is not available at this time, please try again"
                      << std::endl;
          }
        }

        // in case userChoice is unavailable, re-type the input
        if (check == false) {
          std::getline(std::cin, input);
          userChoice = atoi(input.c_str());
          check = true;
        }

        // in case userChoice is not unavailable, terminate while loop
        else if (check == true) {
          outFlag = true;
        }
      }

      Page_s4 ::choices_s userChoiceStruct =
          pages[presentPageNum].second.choices[userChoice - 1];

      presentPageNum = userChoiceStruct.destNum;
      // print current page
      pages[presentPageNum].second.printPage();

      // store variable map of the called page to combinedMap
      storeToCombinedMap(pages[presentPageNum].second);

      // check if variables has been already encountered
      checkVariables(pages[presentPageNum].second);

      // print choices and store unavailable options at this time
      unavail_opts = printChoiceOptions(pages[presentPageNum].second);
      // if pageType is W or L, the game is over and the while loop is stopped
      if (pages[presentPageNum].second.pageType == "W" ||
          pages[presentPageNum].second.pageType == "L") {
        gameOver = true;
      }
    }
  }
}

// A function that combines variables from all pages
void storyBook_s4::storeToCombinedMap(Page_s4 userChoicePage) {
  storyBook_s4::combinedMap.insert(userChoicePage.variables.begin(),
                                   userChoicePage.variables.end());
}
