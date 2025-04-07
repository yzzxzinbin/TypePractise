#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>
#include <map>
#include <curses.h>

// 设置管理器类，负责存储和加载用户设置
class SettingsManager
{
private:
    std::map<std::string, bool> boolSettings;  // 布尔类型设置
    std::map<std::string, int> intSettings;    // 整数类型设置
    std::map<std::string, std::string> strSettings;  // 字符串类型设置
    std::string settingsPath;                  // 设置文件路径
    bool settingsLoaded = false;               // 是否已加载设置

    // 初始化默认设置
    void initDefaultSettings();

public:
    SettingsManager();
    ~SettingsManager();

    // 设置文件路径
    void setSettingsPath(const std::string& path);
    
    // 加载设置
    bool loadSettings();
    
    // 保存设置
    bool saveSettings();
    
    // 获取布尔设置
    bool getBoolSetting(const std::string& key, bool defaultValue = false) const;
    
    // 设置布尔设置
    void setBoolSetting(const std::string& key, bool value);
    
    // 获取整数设置
    int getIntSetting(const std::string& key, int defaultValue = 0) const;
    
    // 设置整数设置
    void setIntSetting(const std::string& key, int value);
    
    // 获取字符串设置
    std::string getStrSetting(const std::string& key, const std::string& defaultValue = "") const;
    
    // 设置字符串设置
    void setStrSetting(const std::string& key, const std::string& value);
    
    // 显示设置界面
    void showSettingsMenu(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    
    // 是否已加载设置
    bool isSettingsLoaded() const { return settingsLoaded; }
};

// 设置键名常量
namespace SettingKeys {
    const std::string SKIP_SPACE = "skip_space";  // 跳过空格设置
    const std::string IGNORE_CASE = "ignore_case"; // 忽略大小写设置
}

#endif // SETTINGS_MANAGER_H
