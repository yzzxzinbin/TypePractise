#ifndef TYPE_PRACTISE_H
#define TYPE_PRACTISE_H

// 定义程序版本号
#define PROGRAM_VERSION "v1.0"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <curses.h>
#include <map>
#ifdef _WIN32
#include <windows.h>
#include <filesystem>
#endif
#include "user_manager.h"
#include "statistics_manager.h"
#include "corpus_manager.h"
#include "settings_manager.h"  // 添加新的设置管理器
#include "gui_helper.h"
#include "key_practice.h"      // 添加新的键盘练习头文件


using namespace std;
using namespace std::chrono;
#ifdef _WIN32
namespace fs = std::filesystem;
#endif

// 使用GUI模块中定义的常量和枚举
typedef GUIHelper::DialogType DialogType;

// 全局管理器
extern UserManager *userManager;
extern StatisticsManager *statsManager;
extern CorpusManager *corpusManager;
extern SettingsManager *settingsManager;  // 添加设置管理器

// 全局常量
extern const string CORPUS_DIR;
extern const string USER_DIR;

// 窗口对象
extern WINDOW *headerWin;
extern WINDOW *contentWin;
extern WINDOW *statusWin;

// --- 函数声明 ---
void initWindows();
void initColors();
void updateHeaderWindow(const string &title);
void updateStatusWindow(const string &status);
void updateStatusWindowWithHelp(const string &status, const string &help);
void clearContentWindow();
bool showConfirmDialog(const string &message, const string &title);
void showMessageDialog(const string &message, DialogType type, const string &title);
void showWelcomeScreen();
int showMainMenu(bool showUserOption = true);
DifficultyLevel showDifficultyMenu();
void practiceSession(string practiceText = "");
string getCustomText();
string generatePracticeText(int wordCount);
void showStatistics();
void updateStatistics(double timeTaken, int charsTyped, int correctChars, double wpm, double accuracy);
void showSettingsMenu();  // 添加显示设置菜单的函数
void cleanup();

#endif // TYPE_PRACTISE_H
