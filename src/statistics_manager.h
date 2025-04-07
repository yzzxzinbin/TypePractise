#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include <string>
#include <map>

// 用户统计信息结构
struct UserStats
{
    int sessionsCompleted = 0;
    double totalTypingTime = 0.0;
    int totalCharsTyped = 0;
    int totalCorrectChars = 0;
    double bestWPM = 0.0;
    double bestAccuracy = 0.0;
    std::map<char, int> errorChars; // 用于跟踪常见错误字符
};

class StatisticsManager
{
private:
    UserStats userStatistics;
    bool statsLoaded = false;
    std::string statsPath;

public:
    StatisticsManager();
    ~StatisticsManager();

    // 设置统计文件路径
    void setStatsPath(const std::string& path);
    
    // 加载统计数据
    bool loadStatistics();
    
    // 保存统计数据
    bool saveStatistics();
    
    // 更新统计数据
    void updateStatistics(double timeTaken, int charsTyped, int correctChars, double wpm, double accuracy);
    
    // 更新错误字符
    void updateErrorChar(char errorChar);
    
    // 重置统计数据
    void resetStatistics();
    
    // 获取统计信息
    UserStats getStatistics() const;
    
    // 设置统计信息
    void setStatistics(const UserStats& stats);
    
    // 检查是否已加载统计
    bool isStatsLoaded() const { return statsLoaded; }
};

#endif // STATISTICS_MANAGER_H
