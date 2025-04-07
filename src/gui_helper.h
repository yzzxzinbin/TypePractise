#ifndef GUI_HELPER_H
#define GUI_HELPER_H

#include <string>
#include <curses.h>

namespace GUIHelper {
    // 对话框类型
    enum DialogType
    {
        DIALOG_INFO,
        DIALOG_WARNING,
        DIALOG_ERROR,
        DIALOG_CONFIRM,
        DIALOG_SUCCESS  // 添加SUCCESS类型
    };

    // 颜色定义（改为常量而非宏定义）
    const int COLOR_DEFAULT = 1;
    const int COLOR_CORRECT = 2;
    const int COLOR_INCORRECT = 3;
    const int COLOR_CURRENT = 4;
    const int COLOR_STATS = 5;
    const int COLOR_HEADER = 6;
    const int COLOR_GUIDE = 7;  // 用于导引文字的颜色

    // 按键定义（改为常量而非宏定义）
    const int KEY_ESC = 27;
    const int KEY_RETURN = '\n';
    const int KEY_BACKSPACE_ALT = 8;

    // 初始化窗口
    void initWindows(WINDOW** headerWin, WINDOW** contentWin, WINDOW** statusWin);
    
    // 初始化颜色
    void initColors();
    
    // 绘制边框
    void drawBox(WINDOW* win);
    
    // 更新标题窗口
    void updateHeaderWindow(WINDOW* win, const std::string& title);
    
    // 更新状态窗口
    void updateStatusWindow(WINDOW* win, const std::string& status);
    
    // 更新状态窗口，带帮助提示
    void updateStatusWindowWithHelp(WINDOW* win, const std::string& status, const std::string& help);
    
    // 清除内容窗口
    void clearContentWindow(WINDOW* win);
    
    // 显示确认对话框
    bool showConfirmDialog(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin,
                          const std::string& message, const std::string& title);
    
    // 显示消息对话框
    void showMessageDialog(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin,
                          const std::string& message, DialogType type, const std::string& title);
    
    // 程序结束清理
    void cleanup(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
}

#endif // GUI_HELPER_H
