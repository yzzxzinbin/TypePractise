#include "user_manager.h"
#include "gui_helper.h" // 添加GUI辅助模块头文件
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstring>

// 使用标准命名空间和文件系统
using namespace std;
namespace fs = std::filesystem;

// 构造函数 - 初始化用户管理器
UserManager::UserManager(const string& userDirPath) {
    userDir = userDirPath;
    usersListFile = userDir + "\\users.txt";
    
    // 确保用户目录存在
    createUserDirectory();
    
    // 加载用户列表
    loadUsersList();
}

// 析构函数
UserManager::~UserManager() {
    // 保存用户列表
    saveUsersList();
}

// 创建用户目录
bool UserManager::createUserDirectory() {
    try {
        if (!fs::exists(userDir)) {
            fs::create_directories(userDir);
        }
        return true;
    } catch (const fs::filesystem_error& e) {
        cerr << "Error creating user directory: " << e.what() << endl;
        return false;
    }
}

// 加载用户列表
void UserManager::loadUsersList() {
    users.clear();
    
    ifstream file(usersListFile);
    if (file) {
        string username;
        while (getline(file, username)) {
            // 跳过空行
            if (username.empty()) continue;
            
            User user;
            user.username = username;
            user.statsFile = userDir + "\\" + username + ".dat";
            users.push_back(user);
        }
    }
}

// 保存用户列表
void UserManager::saveUsersList() {
    ofstream file(usersListFile);
    if (file) {
        for (const auto& user : users) {
            file << user.username << endl;
        }
    }
}

// 获取当前用户的统计文件路径
string UserManager::getCurrentUserStatsPath() const {
    if (currentUser.username.empty()) {
        return "";
    }
    return currentUser.statsFile;
}

// 用户选择界面
bool UserManager::selectUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin) {
    // 如果没有用户，引导创建新用户
    if (users.empty()) {
        return createNewUser(headerWin, contentWin, statusWin);
    }
    
    GUIHelper::updateHeaderWindow(headerWin, "Select User"); // 使用英文避免乱码
    GUIHelper::clearContentWindow(contentWin);
    
    // 查找当前用户在列表中的位置，默认高亮显示当前用户
    int selectedIndex = 0;
    if (!currentUser.username.empty()) {
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i].username == currentUser.username) {
                selectedIndex = i;
                break;
            }
        }
    }
    
    bool selectionMade = false;
    
    while (!selectionMade) {
        werase(contentWin);
        GUIHelper::drawBox(contentWin);
        
        wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT)); // 修改这里，使用GUIHelper命名空间的常量
        mvwprintw(contentWin, 1, 2, "Select a user or create a new one:"); // 使用英文避免乱码
        
        // 显示现有用户
        for (size_t i = 0; i < users.size(); i++) {
            // 高亮显示当前选择
            if (i == selectedIndex) {
                wattron(contentWin, A_REVERSE);
            }
            
            mvwprintw(contentWin, 3 + i, 2, "%d. %s", (int)(i + 1), users[i].username.c_str());
            
            if (i == selectedIndex) {
                wattroff(contentWin, A_REVERSE);
            }
        }
        
        // 显示"创建新用户"选项
        if (users.size() == selectedIndex) {
            wattron(contentWin, A_REVERSE);
        }
        mvwprintw(contentWin, 3 + users.size(), 2, "N. Create New User"); // 使用英文避免乱码
        if (users.size() == selectedIndex) {
            wattroff(contentWin, A_REVERSE);
        }
        
        // 底部导航提示，添加重命名和删除选项
        mvwprintw(contentWin, getmaxy(contentWin) - 4, 2, "UP/DOWN: Move Selection   ENTER: Confirm");
        mvwprintw(contentWin, getmaxy(contentWin) - 3, 2, "R: Rename User   D: Delete User   ESC: Exit");
        wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT)); // 修改这里
        
        GUIHelper::updateStatusWindowWithHelp(statusWin, "Select User", "or press N to create new");
        wrefresh(contentWin);
        
        // 重点是确保键盘输入捕获
        keypad(contentWin, TRUE); // 确保启用特殊键输入
        
        int ch = wgetch(contentWin); // 从contentWin获取输入
        
        switch (ch) {
            case KEY_UP:
                if (selectedIndex > 0) {
                    selectedIndex--;
                }
                break;
                
            case KEY_DOWN:
                if (selectedIndex < users.size()) {
                    selectedIndex++;
                }
                break;
                
            case '\n':
            case KEY_ENTER:
            case '\r':
                if (selectedIndex < users.size()) {
                    // 选择现有用户
                    currentUser = users[selectedIndex];
                    currentUser.isActive = true;
                    return true;
                } else {
                    // 创建新用户
                    return createNewUser(headerWin, contentWin, statusWin);
                }
                break;
                
            case 'n':
            case 'N':
                return createNewUser(headerWin, contentWin, statusWin);
                
            case 'r':
            case 'R':
                // 重命名用户（需要先选中一个用户）
                if (selectedIndex < users.size()) {
                    renameUser(headerWin, contentWin, statusWin, users[selectedIndex]);
                    // 重新加载用户列表（可能因为重命名而变化）
                    saveUsersList();
                    loadUsersList();
                    // 查找重命名后的用户并高亮
                    for (size_t i = 0; i < users.size(); i++) {
                        if (users[i].username == currentUser.username) {
                            selectedIndex = i;
                            break;
                        }
                    }
                }
                break;
                
            case 'd':
            case 'D':
                // 删除用户（需要先选中一个用户）
                if (selectedIndex < users.size() && users.size() > 1) {
                    if (deleteUser(headerWin, contentWin, statusWin, users[selectedIndex])) {
                        // 更新选中索引（如果删除的是最后一个用户）
                        if (selectedIndex >= users.size()) {
                            selectedIndex = users.size() - 1;
                        }
                    }
                } else if (users.size() <= 1) {
                    // 不允许删除唯一的用户
                    GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "Cannot delete the only user. Create another user first.", 
                                     GUIHelper::DIALOG_ERROR, "Error");
                }
                break;
                
            case 27: // ESC键
                // 如果没有选择用户且没有用户，则创建默认用户
                if (users.empty()) {
                    User defaultUser;
                    defaultUser.username = "Default";
                    defaultUser.statsFile = userDir + "\\Default.dat";
                    defaultUser.isActive = true;
                    users.push_back(defaultUser);
                    currentUser = defaultUser;
                    saveUsersList();
                    return true;
                }
                // 如果有用户但未选择，则默认选第一个
                else if (currentUser.username.empty()) {
                    currentUser = users[0];
                    currentUser.isActive = true;
                    return true;
                }
                return false;
                
            default:
                // 添加数字键支持 - 允许用户通过数字键选择用户
                if (ch >= '1' && ch <= '9') {
                    int index = ch - '1'; // 将数字键转换为索引（0-8）
                    // 检查索引是否在有效范围内
                    if (index < users.size()) {
                        // 选择该用户
                        currentUser = users[index];
                        currentUser.isActive = true;
                        return true;
                    }
                }
                break;
        }
    }
    
    return false;
}

// 创建新用户
bool UserManager::createNewUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin) {
    GUIHelper::updateHeaderWindow(headerWin, "Create New User");
    GUIHelper::clearContentWindow(contentWin);
    
    wattron(contentWin, COLOR_PAIR(1));
    mvwprintw(contentWin, 2, 2, "Enter a new username (max 20 characters):");
    mvwprintw(contentWin, 3, 2, "Username cannot be empty, ESC to cancel");
    wattroff(contentWin, COLOR_PAIR(1));
    
    GUIHelper::updateStatusWindowWithHelp(statusWin, "Enter username", "ESC: Cancel");
    wrefresh(contentWin);
    
    // 输入框
    char username[21] = {0}; // 最多20个字符+结束符
    int inputPos = 0;
    
    // 启用键盘输入但禁用回显
    noecho();
    keypad(contentWin, TRUE);
    
    // 显示光标，但只在内容窗口
    curs_set(1);
    wmove(contentWin, 5, 2);
    wrefresh(contentWin);
    
    bool done = false;
    bool cancelled = false;
    
    while (!done) {
        int ch = wgetch(contentWin);
        
        if (ch == 27) { // ESC键
            cancelled = true;
            done = true;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
            done = true;
        }
        else if (ch == KEY_BACKSPACE || ch == 8 || ch == 127) {
            if (inputPos > 0) {
                inputPos--;
                username[inputPos] = '\0';
                mvwaddch(contentWin, 5, 2 + inputPos, ' ');
                wmove(contentWin, 5, 2 + inputPos);
                wrefresh(contentWin);
            }
        }
        else if (isprint(ch) && inputPos < 20) {
            username[inputPos++] = ch;
            username[inputPos] = '\0';
            waddch(contentWin, ch);
            wrefresh(contentWin);
        }
    }
    
    // 隐藏光标
    curs_set(0);
    
    if (cancelled || strlen(username) == 0) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, "CANCEL", 
                                   GUIHelper::DIALOG_INFO, "TIPS");
        return false;
    }
    
    // 检查用户名是否已存在
    string newUsername = string(username);
    auto it = find_if(users.begin(), users.end(), 
                     [&newUsername](const User& user) { 
                         return user.username == newUsername; 
                     });
    
    if (it != users.end()) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, 
                          "Username '" + newUsername + "' already exists. Please choose another one.", 
                          GUIHelper::DIALOG_ERROR, "Error");
        return createNewUser(headerWin, contentWin, statusWin);
    }
    
    // 创建新用户
    User newUser;
    newUser.username = newUsername;
    newUser.statsFile = userDir + "\\" + newUsername + ".dat";
    newUser.isActive = true;
    
    users.push_back(newUser);
    currentUser = newUser;
    
    saveUsersList();
    
    GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, 
                      "User '" + newUsername + "' created successfully!", 
                      GUIHelper::DIALOG_SUCCESS, "Success");
    
    return true;
}

// 切换用户
bool UserManager::switchUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin) {
    return selectUser(headerWin, contentWin, statusWin);
}

// 重命名用户
bool UserManager::renameUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin, const User& user) {
    GUIHelper::updateHeaderWindow(headerWin, "Rename User");
    GUIHelper::clearContentWindow(contentWin);
    
    wattron(contentWin, COLOR_PAIR(1));
    mvwprintw(contentWin, 2, 2, "Current username: %s", user.username.c_str());
    mvwprintw(contentWin, 4, 2, "Enter new username (max 20 characters):");
    mvwprintw(contentWin, 5, 2, "Username cannot be empty, ESC to cancel");
    wattroff(contentWin, COLOR_PAIR(1));
    
    GUIHelper::updateStatusWindowWithHelp(statusWin, "Enter new username", "ESC: Cancel");
    wrefresh(contentWin);
    
    // 输入框
    char newUsername[21] = {0}; // 最多20个字符+结束符
    int inputPos = 0;
    
    // 禁用回显
    noecho();
    keypad(contentWin, TRUE);
    
    // 显示光标
    curs_set(1);
    wmove(contentWin, 7, 2);
    wrefresh(contentWin);
    
    bool done = false;
    bool cancelled = false;
    
    while (!done) {
        int ch = wgetch(contentWin);
        
        if (ch == 27) { // ESC键
            cancelled = true;
            done = true;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
            done = true;
        }
        else if (ch == KEY_BACKSPACE || ch == 8 || ch == 127) {
            if (inputPos > 0) {
                inputPos--;
                newUsername[inputPos] = '\0';
                mvwaddch(contentWin, 7, 2 + inputPos, ' ');
                wmove(contentWin, 7, 2 + inputPos);
                wrefresh(contentWin);
            }
        }
        else if (isprint(ch) && inputPos < 20) {
            newUsername[inputPos++] = ch;
            newUsername[inputPos] = '\0';
            waddch(contentWin, ch);
            wrefresh(contentWin);
        }
    }
    
    // 隐藏光标
    curs_set(0);
    
    if (cancelled || strlen(newUsername) == 0) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, "Rename cancelled.", 
                                     GUIHelper::DIALOG_INFO, "Information");
        return false;
    }
    
    string newName = string(newUsername);
    
    // 检查新用户名是否与现有用户名相同
    if (newName == user.username) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, 
                                    "New username is the same as the current one.", 
                                    GUIHelper::DIALOG_INFO, "Information");
        return false;
    }
    
    // 检查新用户名是否已存在
    auto it = find_if(users.begin(), users.end(), 
                     [&newName](const User& u) { 
                         return u.username == newName; 
                     });
    
    if (it != users.end()) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, 
                         "Username '" + newName + "' already exists. Please choose another one.", 
                         GUIHelper::DIALOG_ERROR, "Error");
        return false;
    }
    
    // 更改用户名，重命名统计文件
    string oldStatsFile = user.statsFile;
    string newStatsFile = userDir + "\\" + newName + ".dat";
    
    try {
        // 如果统计文件存在，重命名它
        if (fs::exists(oldStatsFile)) {
            fs::rename(oldStatsFile, newStatsFile);
        }
        
        // 更新用户列表
        for (auto& u : users) {
            if (u.username == user.username) {
                u.username = newName;
                u.statsFile = newStatsFile;
                
                // 如果当前用户是被重命名的用户，也更新当前用户信息
                if (currentUser.username == user.username) {
                    currentUser.username = newName;
                    currentUser.statsFile = newStatsFile;
                }
                break;
            }
        }
        
        saveUsersList();
        
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, 
                         "User '" + user.username + "' renamed to '" + newName + "' successfully!", 
                         GUIHelper::DIALOG_SUCCESS, "Success");
        return true;
    }
    catch (const fs::filesystem_error& e) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, 
                         "Error renaming user: " + string(e.what()), 
                         GUIHelper::DIALOG_ERROR, "Error");
        return false;
    }
}

// 删除用户
bool UserManager::deleteUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin, const User& user)
{
    // 更新标题栏
    GUIHelper::updateHeaderWindow(headerWin, "DELETE USER");
    GUIHelper::clearContentWindow(contentWin);

    if (users.empty())
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                    "No users to delete. Please create a user first.",
                                    GUIHelper::DIALOG_ERROR, "No Users Found");
        return false;
    }

    // 保存被删除的用户名称，用于后续的成功提示
    string deletedUsername = user.username;

    // 查找用户在列表中的索引
    int selectedUserIndex = -1;
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].username == user.username) {
            selectedUserIndex = i;
            break;
        }
    }

    if (selectedUserIndex == -1) {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                    "User not found.",
                                    GUIHelper::DIALOG_ERROR, "Error");
        return false;
    }

    // 询问确认
    string confirmMessage = "Are you sure you want to delete user '" + deletedUsername + "'?";
    bool confirmed = GUIHelper::showConfirmDialog(headerWin, contentWin, statusWin, confirmMessage, "Confirm Delete");
    
    if (!confirmed)
    {
        return false;
    }

    // 删除用户文件
    string userPath = userDir + "\\" + deletedUsername;
    string userStatsPath = userDir + "\\" + deletedUsername + ".dat"; // 修正为正确的文件路径

    // 尝试删除统计文件
    if (fs::exists(userStatsPath))
    {
        try {
            fs::remove(userStatsPath);
        }
        catch (const fs::filesystem_error& e) {
            GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                        "Error deleting user statistics: " + string(e.what()),
                                        GUIHelper::DIALOG_ERROR, "Delete Failed");
            return false;
        }
    }
    
    // 删除用户目录（如果存在）
    if (fs::exists(userPath))
    {
        try {
            fs::remove_all(userPath);
        }
        catch (const fs::filesystem_error& e) {
            GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                        "Error deleting user directory: " + string(e.what()),
                                        GUIHelper::DIALOG_ERROR, "Delete Failed");
            return false;
        }
    }

    // 从用户列表中移除
    users.erase(users.begin() + selectedUserIndex);
    
    // 如果删除的是当前用户，则重置当前用户
    if (currentUser.username == deletedUsername)
    {
        currentUser = User(); // 重置当前用户为空
    }

    // 保存更新后的用户列表
    saveUsersList(); // 修正为正确的方法名

    // 使用先前保存的用户名显示成功消息
    GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                               "User '" + deletedUsername + "' deleted successfully!",
                               GUIHelper::DIALOG_SUCCESS, "User Deleted");
    
    return true;
}
