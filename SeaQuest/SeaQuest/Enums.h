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

enum class SceneID { NONE, MENU, PLAY, HIGHSCR, SETT};

enum class MenuItem { NONE, PLAY, HIGHSCR, SETT, QUIT, SOUND, MUSIC, MENU };

enum class ActionName {
    NONE, PAUSE, QUIT, BACK, LEFT, RIGHT, UP,
    DOWN, FIRE, TOGGLE_TEXTURE, TOGGLE_COLLISION,
    TOGGLE_GRID, ENTER,
    JOYSTICK_FIRE, JOYSTICK_PAUSE, JOYSTICK_MOVE,
    JOYSTICK_QUIT, JOYSTICK_BACK,
    TEST_DIVER_UP, TEST_DIVER_DOWN,
    TEST_SCORE_UP, TEST_SCORE_DOWN
};
enum class ActionType { NONE, START, END };

enum class State { DEAD, SPAWN, REFILL, UNLOAD, SCORE, PLAYING, OXYGEN_BONUS };

enum class EntityType { PLAYER, BACKGROUND, ENEMY_SUB, DIVER, SHARK, NPC, FOREGROUND, NONE, PLAYER_BULLET, ENEMY_BULLET };
