#ifndef KEY_PRACTICE_H
#define KEY_PRACTICE_H

#include <curses.h>
#include <string>
#include <vector>
#include <map>
#include "gui_helper.h"

// 键位练习类型枚举
enum KeyPracticeType {
    HOME_ROW,      // 主键位行练习 (asdf jkl;)
    TOP_ROW,       // 顶行练习 (qwer...)
    BOTTOM_ROW,    // 底行练习 (zxcv...)
    NUMBER_ROW,    // 数字行练习 (1234...)
    FULL_KEYBOARD, // 全键盘练习        // 自定义练习
};

// 键盘练习功能类
class KeyboardPractice {
private:
    WINDOW* headerWin;
    WINDOW* contentWin;
    WINDOW* statusWin;
    WINDOW* keyboardWin;   // 用于显示键盘布局的窗口
    WINDOW* menuWin;       // 新增：菜单窗口

    // 键盘布局配置
    std::vector<std::string> keyboardLayout;
    std::map<char, std::pair<int, int>> keyPositions;  // 字符到坐标的映射

    // 练习数据
    std::string practiceText;
    std::string userInput;
    int currentPos;
    std::map<char, int> errorCount;  // 错误统计
    
    // 菜单数据
    std::vector<std::string> menuItems;  // 菜单项
    int selectedMenuItem;               // 当前选中的菜单项
    bool menuVisible;                   // 菜单是否可见
    KeyPracticeType currentPracticeType; // 当前练习类型

    // 定义常量
    static const int KEY_ESC = 27;                // ESC键
    static const int KEY_BACKSPACE_ALT = 8;       // 备用退格键
    static const int KEY_TAB = 9;                 // TAB键

    // 初始化键盘布局
    void initKeyboardLayout();
    
    // 初始化菜单
    void initMenu();
    
    // 绘制键盘
    void drawKeyboard();
    
    // 绘制菜单
    void drawMenu();
    
    // 高亮当前键
    void highlightKey(char key, int colorPair);
    
    // 重置键盘显示
    void resetKeyHighlight();
    
    // 处理用户输入
    bool processInput();
    
    // 处理特定按键输入 (用于处理从菜单模式转发的按键)
    bool processInput(int forceKey);
    
    // 处理菜单输入
    bool processMenuInput(int ch);
    
    // 显示结果和分析
    void showResults();
    
    // 切换练习类型
    void changePracticeType(KeyPracticeType type);
    
    // 新增方法：创建菜单窗口
    void createMenuWindow();
    
    // 新增方法：绘制键盘练习界面 (不触发输入循环)
    void drawKeyboardPracticeScreen();

public:
    KeyboardPractice(WINDOW* header, WINDOW* content, WINDOW* status);
    ~KeyboardPractice();

    // 运行键盘练习
    void run();
    
    // 获取不同行的练习文本
    std::string getBasicKeysText() const;    // Home Row
    std::string getTopRowText() const;       // Top Row
    std::string getBottomRowText() const;    // Bottom Row
    std::string getNumberRowText() const;    // Number Row
    std::string getAdvancedKeysText() const; // 全键盘混合练习
};

// 全局函数，可以直接从main.cpp调用
void startKeyboardPractice(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);

#endif // KEY_PRACTICE_H
