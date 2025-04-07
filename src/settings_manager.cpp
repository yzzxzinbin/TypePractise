#include "settings_manager.h"
#include "gui_helper.h"
#include <fstream>
#include <curses.h>

using namespace std;
using namespace GUIHelper;

// 构造函数
SettingsManager::SettingsManager() : settingsLoaded(false)
{
    // 初始化默认设置
    initDefaultSettings();
}

// 析构函数
SettingsManager::~SettingsManager()
{
    // 如果设置已加载，保存设置
    if (settingsLoaded && !settingsPath.empty())
    {
        saveSettings();
    }
}

// 初始化默认设置
void SettingsManager::initDefaultSettings()
{
    // 添加 Skip Space 设置，默认为启用
    boolSettings[SettingKeys::SKIP_SPACE] = true;
    
    // 添加 Ignore Case 设置，默认为禁用
    boolSettings[SettingKeys::IGNORE_CASE] = false;
    
    // 可以在这里添加更多默认设置
}

// 设置文件路径
void SettingsManager::setSettingsPath(const string& path)
{
    settingsPath = path;
}

// 加载设置
bool SettingsManager::loadSettings()
{
    if (settingsPath.empty())
    {
        return false;
    }

    ifstream file(settingsPath, ios::binary);
    if (file)
    {
        // 加载布尔设置
        size_t boolCount;
        file.read(reinterpret_cast<char*>(&boolCount), sizeof(boolCount));
        
        for (size_t i = 0; i < boolCount; ++i)
        {
            size_t keyLength;
            file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
            
            string key(keyLength, '\0');
            file.read(&key[0], keyLength);
            
            bool value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            
            boolSettings[key] = value;
        }
        
        // 加载整数设置
        size_t intCount;
        file.read(reinterpret_cast<char*>(&intCount), sizeof(intCount));
        
        for (size_t i = 0; i < intCount; ++i)
        {
            size_t keyLength;
            file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
            
            string key(keyLength, '\0');
            file.read(&key[0], keyLength);
            
            int value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            
            intSettings[key] = value;
        }
        
        // 加载字符串设置
        size_t strCount;
        file.read(reinterpret_cast<char*>(&strCount), sizeof(strCount));
        
        for (size_t i = 0; i < strCount; ++i)
        {
            size_t keyLength;
            file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
            
            string key(keyLength, '\0');
            file.read(&key[0], keyLength);
            
            size_t valueLength;
            file.read(reinterpret_cast<char*>(&valueLength), sizeof(valueLength));
            
            string value(valueLength, '\0');
            file.read(&value[0], valueLength);
            
            strSettings[key] = value;
        }
        
        settingsLoaded = true;
        return true;
    }
    
    // 如果文件不存在，使用默认设置
    settingsLoaded = true;
    return false;
}

// 保存设置
bool SettingsManager::saveSettings()
{
    if (settingsPath.empty())
    {
        return false;
    }

    ofstream file(settingsPath, ios::binary);
    if (file)
    {
        // 保存布尔设置
        size_t boolCount = boolSettings.size();
        file.write(reinterpret_cast<const char*>(&boolCount), sizeof(boolCount));
        
        for (const auto& pair : boolSettings)
        {
            size_t keyLength = pair.first.length();
            file.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
            file.write(pair.first.c_str(), keyLength);
            
            file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }
        
        // 保存整数设置
        size_t intCount = intSettings.size();
        file.write(reinterpret_cast<const char*>(&intCount), sizeof(intCount));
        
        for (const auto& pair : intSettings)
        {
            size_t keyLength = pair.first.length();
            file.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
            file.write(pair.first.c_str(), keyLength);
            
            file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }
        
        // 保存字符串设置
        size_t strCount = strSettings.size();
        file.write(reinterpret_cast<const char*>(&strCount), sizeof(strCount));
        
        for (const auto& pair : strSettings)
        {
            size_t keyLength = pair.first.length();
            file.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
            file.write(pair.first.c_str(), keyLength);
            
            size_t valueLength = pair.second.length();
            file.write(reinterpret_cast<const char*>(&valueLength), sizeof(valueLength));
            file.write(pair.second.c_str(), valueLength);
        }
        
        return true;
    }
    
    return false;
}

// 获取布尔设置
bool SettingsManager::getBoolSetting(const string& key, bool defaultValue) const
{
    auto it = boolSettings.find(key);
    if (it != boolSettings.end())
    {
        return it->second;
    }
    return defaultValue;
}

// 设置布尔设置
void SettingsManager::setBoolSetting(const string& key, bool value)
{
    boolSettings[key] = value;
    if (settingsLoaded)
    {
        saveSettings();
    }
}

// 获取整数设置
int SettingsManager::getIntSetting(const string& key, int defaultValue) const
{
    auto it = intSettings.find(key);
    if (it != intSettings.end())
    {
        return it->second;
    }
    return defaultValue;
}

// 设置整数设置
void SettingsManager::setIntSetting(const string& key, int value)
{
    intSettings[key] = value;
    if (settingsLoaded)
    {
        saveSettings();
    }
}

// 获取字符串设置
string SettingsManager::getStrSetting(const string& key, const string& defaultValue) const
{
    auto it = strSettings.find(key);
    if (it != strSettings.end())
    {
        return it->second;
    }
    return defaultValue;
}

// 设置字符串设置
void SettingsManager::setStrSetting(const string& key, const string& value)
{
    strSettings[key] = value;
    if (settingsLoaded)
    {
        saveSettings();
    }
}

// 显示设置界面
void SettingsManager::showSettingsMenu(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin)
{
    GUIHelper::updateHeaderWindow(headerWin, "SETTINGS");
    GUIHelper::clearContentWindow(contentWin);

    bool exitMenu = false;
    int selectedIndex = 0;
    const int totalOptions = 2; // 现在有两个选项："Skip Space"和"Ignore Case"
    
    while (!exitMenu)
    {
        GUIHelper::clearContentWindow(contentWin);
        
        wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
        mvwprintw(contentWin, 2, 2, "Program Settings:");
        
        // "Skip Space" 选项
        bool skipSpace = getBoolSetting(SettingKeys::SKIP_SPACE, true);
        
        // "Ignore Case" 选项
        bool ignoreCase = getBoolSetting(SettingKeys::IGNORE_CASE, false);
        
        // 绘制选择指示符和选项
        if (selectedIndex == 0)
        {
            wattron(contentWin, A_REVERSE);
        }
        
        mvwprintw(contentWin, 4, 2, "1. Skip Space: %s", skipSpace ? "Enabled" : "Disabled");
        
        if (selectedIndex == 0)
        {
            wattroff(contentWin, A_REVERSE);
        }
        
        if (selectedIndex == 1)
        {
            wattron(contentWin, A_REVERSE);
        }
        
        mvwprintw(contentWin, 5, 2, "2. Ignore Case: %s", ignoreCase ? "Enabled" : "Disabled");
        
        if (selectedIndex == 1)
        {
            wattroff(contentWin, A_REVERSE);
        }
        
        // 添加帮助信息
        if (selectedIndex == 0) {
            mvwprintw(contentWin, 7, 2, "Current setting: %s", skipSpace ? "Enabled" : "Disabled");
            mvwprintw(contentWin, 8, 2, "When enabled, you don't need to type spaces.");
        } else if (selectedIndex == 1) {
            mvwprintw(contentWin, 7, 2, "Current setting: %s", ignoreCase ? "Enabled" : "Disabled");
            mvwprintw(contentWin, 8, 2, "When enabled, uppercase and lowercase are treated the same.");
        }
        
        // 底部按键提示
        mvwprintw(contentWin, getmaxy(contentWin) - 3, 2, "ENTER: Toggle setting  ESC: Return to menu");
        wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
        
        GUIHelper::updateStatusWindowWithHelp(statusWin, "Use UP/DOWN to navigate", "ENTER to toggle, ESC to exit");
        wrefresh(contentWin);
        
        int ch = wgetch(contentWin);
        switch (ch)
        {
            case KEY_UP:
                selectedIndex = (selectedIndex - 1 + totalOptions) % totalOptions;
                break;
            
            case KEY_DOWN:
                selectedIndex = (selectedIndex + 1) % totalOptions;
                break;
            
            case '\n':
            case '\r':
            case KEY_ENTER:
                // 切换当前选中的设置
                if (selectedIndex == 0) // Skip Space
                {
                    bool newValue = !skipSpace;
                    setBoolSetting(SettingKeys::SKIP_SPACE, newValue);
                    
                    // 显示确认消息
                    string message = "Skip Space setting " + string(newValue ? "enabled" : "disabled");
                    GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, message, 
                                                GUIHelper::DIALOG_INFO, "Setting Changed");
                }
                else if (selectedIndex == 1) // Ignore Case
                {
                    bool newValue = !ignoreCase;
                    setBoolSetting(SettingKeys::IGNORE_CASE, newValue);
                    
                    // 显示确认消息
                    string message = "Ignore Case setting " + string(newValue ? "enabled" : "disabled");
                    GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, message, 
                                                GUIHelper::DIALOG_INFO, "Setting Changed");
                }
                break;
            
            case KEY_ESC:
            case 'q':
            case 'Q':
                exitMenu = true;
                break;
        }
    }
}
