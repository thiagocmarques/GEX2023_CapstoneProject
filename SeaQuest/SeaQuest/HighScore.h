// New Brunswick Community College
// Gaming Experience Development
// -------------------------------
// Capstone Project - SeaQuest
// -------------------------------
// Instructor: David Burchill
// Student: Thiago Marques
// 
// April 2023
// 


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
    std::vector<HScore>     highScores;

    const std::string       DEFAULT_JSON_FILENAME{ "../highscores.json" };

    bool                    compare_scores(const HScore& a, const HScore& b);
public:

    HighScoreList() {}

    void                    addHighScore(std::string name, std::string timestamp, int score);
    void                    addHighScore(std::string name, int score);
    
    void                    saveToFile(std::string filename);
    void                    saveToFile();

    void                    loadFromFile(std::string filename);
    void                    loadFromFile();

    std::vector<HScore>     getHighScores() const;

    std::string             getCurrentDateTime() const; 

    bool                    checkHighScore(int score) const;

    std::string             clean_string(const std::string& str);


    
};


