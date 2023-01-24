//
// 
//  New Brunswick Community College
//  Comprehensive Game Development
//  Instructor: David Burchill
//  Winter 2023
// 
//  Author: Thiago Marques
//  https://github.com/thiagocmarques
//
//  January 2023
// 
//  SeaQuest
// 
//

#include <SFML/System.hpp>
#include "GameEngine.h"

int main()
{
    GameEngine game("../config.txt");
    game.run();

    return 0;
}


