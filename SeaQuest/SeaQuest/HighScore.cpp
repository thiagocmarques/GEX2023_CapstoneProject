#include "HighScore.h"
#include "json.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

using json = nlohmann::json;

bool HighScoreList::compare_scores(const HScore& a, const HScore& b)
{
	return a.score > b.score;

}

void HighScoreList::addHighScore(std::string name, std::string timestamp, int score)
{
	if (name.size() > 20)
		name = name.substr(0, 20);

	HScore hs = { name, timestamp, score };
	highScores.push_back(hs);

	// sorting
	std::sort(highScores.begin(), highScores.end(), [](HScore a, HScore b) { return a.score > b.score; });
	highScores.resize(10);
}

void HighScoreList::addHighScore(std::string name, int score)
{
	auto now = getCurrentDateTime();
	addHighScore(name, now, score);
}

void HighScoreList::saveToFile(std::string filename)
{
	json j;

	// sorting before saving
	std::sort(highScores.begin(), highScores.end(), [](HScore a, HScore b) { return a.score > b.score; });

	for (auto hs : highScores) {
		json obj = {
			{"name", hs.name},
			{"timestamp", hs.timestamp},
			{"score", hs.score}
		};
		j.push_back(obj);
	}
	std::ofstream file(filename);
	file << std::setw(4) << j;
}

void HighScoreList::saveToFile()
{
	saveToFile(DEFAULT_JSON_FILENAME);
}

void HighScoreList::loadFromFile(std::string filename)
{

	std::ifstream file(filename);
	if (file.good()) {

		highScores.clear();

		json j;
		file >> j;
		for (auto obj : j) {
			HScore hs = {
				obj["name"],
				obj["timestamp"],
				obj["score"]
			};
			highScores.push_back(hs);
		}
	}
	// if file is not good we create a new one with default values
	else {
		highScores.clear();
		addHighScore("Freddy Krugger", 1000);
		addHighScore("Brendan Coderre", 2000);
		addHighScore("Brohen Verhoeven", 3000);
		addHighScore("Bruno Bortoli", 4000);
		addHighScore("Ezequiel Flores", 5000);
		addHighScore("Kay Gabriel", 6000);
		addHighScore("Kyle Guntom", 7000);
		addHighScore("Natalia Castillo", 8000);
		addHighScore("Thiago Marques", 9000);
		addHighScore("David Burchill", 10000);

	}
	// sorting after loading
	std::sort(highScores.begin(), highScores.end(), [](HScore a, HScore b) { return a.score > b.score; });
}

void HighScoreList::loadFromFile()
{
	loadFromFile(DEFAULT_JSON_FILENAME);
}

std::vector<HScore> HighScoreList::getHighScores() const
{
	return highScores;
}

std::string HighScoreList::getCurrentDateTime() const
{

	SYSTEMTIME time;
	GetLocalTime(&time);
	std::stringstream ss;

	if (time.wDay < 10)
		ss << '0';
	ss << time.wDay;
	ss << '/';
	if (time.wMonth < 10)
		ss << '0';
	ss << time.wMonth;
	ss << '/';
	ss << time.wYear;
	ss << ' ';
	if (time.wHour < 10)
		ss << '0';
	ss << time.wHour;
	ss << ':';
	if (time.wMinute < 10)
		ss << '0';
	ss << time.wMinute;
	ss << ':';
	if (time.wSecond < 10)
		ss << '0';
	ss << time.wSecond;

	return ss.str();
}

bool HighScoreList::checkHighScore(int score) const
{
	for (auto s : highScores) {
		if (score > s.score)
			return true;
	}
	return false;
}

std::string HighScoreList::clean_string(const std::string& str)
{
	std::string result;
	std::copy_if(str.begin(), str.end(), std::back_inserter(result), [](char c) {
		return std::isalnum(c) || std::isspace(c);
		});
	return result;

}
