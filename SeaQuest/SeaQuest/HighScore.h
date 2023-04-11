#pragma once

#include <string>
#include <vector>


struct HScore
{
    std::string name;
    std::string timestamp;
    int score;
};

class HighScoreList {
private:
    
    
    std::vector<HScore> highScores;

    bool compare_scores(const HScore& a, const HScore& b);
public:

    HighScoreList() {}

    void addHighScore(std::string name, std::string timestamp, int score);
    void addHighScore(std::string name, int score);
    
    void saveToFile(std::string filename);

    void loadFromFile(std::string filename);

    std::vector<HScore> getHighScores() const;

    std::string getCurrentDateTime() const; 

    bool checkHighScore(int score) const;

    const std::string   FILENAME{ "../highscores.json" };
};


