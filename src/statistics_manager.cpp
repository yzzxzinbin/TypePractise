#include "statistics_manager.h"
#include <fstream>
#include <algorithm>

using namespace std;

// 构造函数
StatisticsManager::StatisticsManager() : statsLoaded(false)
{
    // 初始化统计数据
    resetStatistics();
}

// 析构函数
StatisticsManager::~StatisticsManager()
{
    // 如果统计已加载，保存统计数据
    if (statsLoaded && !statsPath.empty())
    {
        saveStatistics();
    }
}

// 设置统计文件路径
void StatisticsManager::setStatsPath(const string& path)
{
    statsPath = path;
}

// 重置统计数据
void StatisticsManager::resetStatistics()
{
    userStatistics.sessionsCompleted = 0;
    userStatistics.totalTypingTime = 0.0;
    userStatistics.totalCharsTyped = 0;
    userStatistics.totalCorrectChars = 0;
    userStatistics.bestWPM = 0.0;
    userStatistics.bestAccuracy = 0.0;
    userStatistics.errorChars.clear();
    statsLoaded = false;
}

// 加载统计数据
bool StatisticsManager::loadStatistics()
{
    if (statsPath.empty())
    {
        return false;
    }

    ifstream file(statsPath, ios::binary);
    if (file)
    {
        file.read(reinterpret_cast<char*>(&userStatistics.sessionsCompleted), sizeof(userStatistics.sessionsCompleted));
        file.read(reinterpret_cast<char*>(&userStatistics.totalTypingTime), sizeof(userStatistics.totalTypingTime));
        file.read(reinterpret_cast<char*>(&userStatistics.totalCharsTyped), sizeof(userStatistics.totalCharsTyped));
        file.read(reinterpret_cast<char*>(&userStatistics.totalCorrectChars), sizeof(userStatistics.totalCorrectChars));
        file.read(reinterpret_cast<char*>(&userStatistics.bestWPM), sizeof(userStatistics.bestWPM));
        file.read(reinterpret_cast<char*>(&userStatistics.bestAccuracy), sizeof(userStatistics.bestAccuracy));

        // 加载错误字符统计
        size_t mapSize;
        file.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        for (size_t i = 0; i < mapSize; ++i)
        {
            char key;
            int value;
            file.read(reinterpret_cast<char*>(&key), sizeof(key));
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            userStatistics.errorChars[key] = value;
        }

        statsLoaded = true;
        return true;
    }
    
    return false;
}

// 保存统计数据
bool StatisticsManager::saveStatistics()
{
    if (statsPath.empty())
    {
        return false;
    }

    ofstream file(statsPath, ios::binary);
    if (file)
    {
        file.write(reinterpret_cast<char*>(&userStatistics.sessionsCompleted), sizeof(userStatistics.sessionsCompleted));
        file.write(reinterpret_cast<char*>(&userStatistics.totalTypingTime), sizeof(userStatistics.totalTypingTime));
        file.write(reinterpret_cast<char*>(&userStatistics.totalCharsTyped), sizeof(userStatistics.totalCharsTyped));
        file.write(reinterpret_cast<char*>(&userStatistics.totalCorrectChars), sizeof(userStatistics.totalCorrectChars));
        file.write(reinterpret_cast<char*>(&userStatistics.bestWPM), sizeof(userStatistics.bestWPM));
        file.write(reinterpret_cast<char*>(&userStatistics.bestAccuracy), sizeof(userStatistics.bestAccuracy));

        // 保存错误字符统计
        size_t mapSize = userStatistics.errorChars.size();
        file.write(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        for (const auto& pair : userStatistics.errorChars)
        {
            file.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
            file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }
        
        return true;
    }
    
    return false;
}

// 更新统计数据
void StatisticsManager::updateStatistics(double timeTaken, int charsTyped, int correctChars, double wpm, double accuracy)
{
    userStatistics.sessionsCompleted++;
    userStatistics.totalTypingTime += timeTaken;
    userStatistics.totalCharsTyped += charsTyped;
    userStatistics.totalCorrectChars += correctChars;

    if (wpm > userStatistics.bestWPM)
    {
        userStatistics.bestWPM = wpm;
    }

    if (accuracy > userStatistics.bestAccuracy)
    {
        userStatistics.bestAccuracy = accuracy;
    }

    // 设置统计数据已加载标志
    statsLoaded = true;

    // 保存更新后的统计数据
    saveStatistics();
}

// 更新错误字符
void StatisticsManager::updateErrorChar(char errorChar)
{
    userStatistics.errorChars[errorChar]++;
}

// 获取统计信息
UserStats StatisticsManager::getStatistics() const
{
    return userStatistics;
}

// 设置统计信息
void StatisticsManager::setStatistics(const UserStats& stats)
{
    userStatistics = stats;
    statsLoaded = true;
}
