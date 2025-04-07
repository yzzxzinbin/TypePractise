#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>
#include <vector>
#include <curses.h>

// 用户结构体
struct User {
    std::string username;
    std::string statsFile;
    bool isActive = false;
};

// 用户管理类
class UserManager {
private:
    std::vector<User> users;
    User currentUser;
    std::string usersListFile;
    std::string userDir;
    
    // 内部辅助函数
    void loadUsersList();
    void saveUsersList();
    bool createUserDirectory();

public:
    UserManager(const std::string& userDirPath);
    ~UserManager();
    
    // 用户管理函数
    bool selectUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    bool createNewUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    
    // 获取当前用户信息
    User getCurrentUser() const { return currentUser; }
    std::string getCurrentUserStatsPath() const;
    
    // 用户切换功能
    bool switchUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    
    // 新增用户管理功能
    bool renameUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin, const User& user);
    bool deleteUser(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin, const User& user);
};

#endif // USER_MANAGER_H
