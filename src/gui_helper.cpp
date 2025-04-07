#include "gui_helper.h"
#include <algorithm>

using namespace std;

namespace GUIHelper {

// 初始化窗口
void initWindows(WINDOW** headerWin, WINDOW** contentWin, WINDOW** statusWin) {
    // 获取终端尺寸
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // 创建三个窗口：标题、内容、状态栏
    *headerWin = newwin(3, maxX, 0, 0);
    *contentWin = newwin(maxY - 6, maxX, 3, 0);
    *statusWin = newwin(3, maxX, maxY - 3, 0);

    // 启用滚动
    scrollok(*contentWin, TRUE);

    // 使用ASCII字符绘制边框
    drawBox(*headerWin);
    drawBox(*contentWin);
    drawBox(*statusWin);

    // 刷新所有窗口
    wrefresh(*headerWin);
    wrefresh(*contentWin);
    wrefresh(*statusWin);
}

// 初始化颜色
void initColors()
{
    // 启动颜色支持
    start_color();
    
    // 定义颜色对
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);        // 默认白字黑底
    init_pair(COLOR_CORRECT, COLOR_GREEN, COLOR_BLACK);        // 正确为绿字
    init_pair(COLOR_INCORRECT, COLOR_RED, COLOR_BLACK);        // 错误为红字
    
    // 修正这一行 - 当前字符应该是黑字黄底，而不是黄字黑底
    init_pair(COLOR_CURRENT, COLOR_BLACK, COLOR_YELLOW);       // 当前字符为黑字黄底
    
    init_pair(COLOR_STATS, COLOR_CYAN, COLOR_BLACK);           // 统计信息为青字
    init_pair(COLOR_HEADER, COLOR_BLACK, COLOR_WHITE);         // 标题栏为黑字白底
    init_pair(COLOR_GUIDE, COLOR_CYAN, COLOR_BLACK);           // 导引文字为青色黑底
}

// 绘制边框
void drawBox(WINDOW* win) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);

    // 绘制四个角
    mvwaddch(win, 0, 0, '+');
    mvwaddch(win, 0, maxx - 1, '+');
    mvwaddch(win, maxy - 1, 0, '+');
    mvwaddch(win, maxy - 1, maxx - 1, '+');

    // 绘制水平线
    for (int i = 1; i < maxx - 1; i++) {
        mvwaddch(win, 0, i, '-');
        mvwaddch(win, maxy - 1, i, '-');
    }

    // 绘制垂直线
    for (int i = 1; i < maxy - 1; i++) {
        mvwaddch(win, i, 0, '|');
        mvwaddch(win, i, maxx - 1, '|');
    }
}

// 更新标题窗口
void updateHeaderWindow(WINDOW* win, const string& title) {
    werase(win);
    drawBox(win);
    wattron(win, COLOR_PAIR(COLOR_HEADER));
    mvwprintw(win, 1, (getmaxx(win) - title.length()) / 2, "%s", title.c_str());
    wattroff(win, COLOR_PAIR(COLOR_HEADER));
    wrefresh(win);
}

// 更新状态窗口
void updateStatusWindow(WINDOW* win, const string& status) {
    werase(win);
    drawBox(win);
    wattron(win, COLOR_PAIR(COLOR_STATS));
    mvwprintw(win, 1, 2, "%s", status.c_str());
    wattroff(win, COLOR_PAIR(COLOR_STATS));
    wrefresh(win);
}

// 更新状态栏，添加返回提示
void updateStatusWindowWithHelp(WINDOW* win, const string& status, const string& help) {
    werase(win);
    drawBox(win);
    wattron(win, COLOR_PAIR(COLOR_STATS));

    // 根据状态栏长度调整布局
    int maxX = getmaxx(win);
    if (status.length() + help.length() + 4 < maxX - 4) {
        // 状态和帮助可以在同一行
        mvwprintw(win, 1, 2, "%s", status.c_str());
        mvwprintw(win, 1, maxX - help.length() - 3, "%s", help.c_str());
    } else {
        // 状态和帮助分行显示
        mvwprintw(win, 1, 2, "%s", status.c_str());
    }

    wattroff(win, COLOR_PAIR(COLOR_STATS));
    wrefresh(win);
}

// 清除内容窗口
void clearContentWindow(WINDOW* win) {
    werase(win);
    drawBox(win);
    wrefresh(win);
}

// 显示确认对话框
bool showConfirmDialog(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin,
                      const string& message, const string& title) {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // 计算对话框大小和位置
    int dialogWidth = min(60, maxX - 10);
    int dialogHeight = 7;
    int startY = (maxY - dialogHeight) / 2;
    int startX = (maxX - dialogWidth) / 2;

    // 创建对话框窗口
    WINDOW* dialogWin = newwin(dialogHeight, dialogWidth, startY, startX);
    drawBox(dialogWin);

    // 显示标题
    wattron(dialogWin, COLOR_PAIR(COLOR_HEADER));
    mvwprintw(dialogWin, 1, (dialogWidth - title.length()) / 2, "%s", title.c_str());
    wattroff(dialogWin, COLOR_PAIR(COLOR_HEADER));

    // 显示消息
    wattron(dialogWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(dialogWin, 3, 2, "%s", message.c_str());

    // 显示按钮
    mvwprintw(dialogWin, 5, dialogWidth / 3 - 4, "[Y] Confirm");
    mvwprintw(dialogWin, 5, 2 * dialogWidth / 3 - 4, "[N] Cancel");
    wattroff(dialogWin, COLOR_PAIR(COLOR_DEFAULT));

    wrefresh(dialogWin);

    // 获取用户响应
    int ch;
    bool result = false;
    bool responded = false;

    while (!responded) {
        ch = getch();
        switch (tolower(ch)) {
        case 'y':
            result = true;
            responded = true;
            break;
        case 'n':
        case KEY_ESC:
            result = false;
            responded = true;
            break;
        }
    }

    // 删除对话框窗口
    delwin(dialogWin);

    // 重绘主窗口
    touchwin(headerWin);
    touchwin(contentWin);
    touchwin(statusWin);
    wrefresh(headerWin);
    wrefresh(contentWin);
    wrefresh(statusWin);

    return result;
}

// 显示消息对话框
void showMessageDialog(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin,
                      const string& message, DialogType type, const string& title) {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // 计算对话框大小和位置
    int dialogWidth = min(60, maxX - 10);
    int dialogHeight = 10;
    int startY = (maxY - dialogHeight) / 2;
    int startX = (maxX - dialogWidth) / 2;

    // 创建对话框窗口
    WINDOW* dialogWin = newwin(dialogHeight, dialogWidth, startY, startX);
    drawBox(dialogWin);

    // 根据对话框类型设置颜色
    int colorPair;
    switch (type) {
    case DIALOG_WARNING:
        colorPair = COLOR_INCORRECT;
        break;
    case DIALOG_ERROR:
        colorPair = COLOR_INCORRECT;
        break;
    default:
        colorPair = COLOR_HEADER;
    }

    // 显示标题
    wattron(dialogWin, COLOR_PAIR(colorPair));
    mvwprintw(dialogWin, 1, (dialogWidth - title.length()) / 2, "%s", title.c_str());
    wattroff(dialogWin, COLOR_PAIR(colorPair));

    // 显示消息 - 支持多行消息
    wattron(dialogWin, COLOR_PAIR(COLOR_DEFAULT));
    size_t pos = 0, lineStart = 0;
    int line = 0;

    while (pos < message.length()) {
        if (message[pos] == '\n' || pos - lineStart >= dialogWidth - 4) {
            mvwprintw(dialogWin, 3 + line, 2, "%s", message.substr(lineStart, pos - lineStart).c_str());
            lineStart = message[pos] == '\n' ? pos + 1 : pos;
            line++;
        }
        pos++;
    }

    // 最后一行
    if (lineStart < message.length()) {
        mvwprintw(dialogWin, 3 + line, 2, "%s", message.substr(lineStart).c_str());
    }

    // 显示按钮
    mvwprintw(dialogWin, dialogHeight - 2, (dialogWidth - 15) / 2, "Press any key");
    wattroff(dialogWin, COLOR_PAIR(COLOR_DEFAULT));

    wrefresh(dialogWin);

    // 等待用户按键
    getch();

    // 删除对话框窗口
    delwin(dialogWin);

    // 重绘主窗口
    touchwin(headerWin);
    touchwin(contentWin);
    touchwin(statusWin);
    wrefresh(headerWin);
    wrefresh(contentWin);
    wrefresh(statusWin);
}

// 程序结束清理
void cleanup(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin) {
    // 删除窗口
    if (headerWin)
        delwin(headerWin);
    if (contentWin)
        delwin(contentWin);
    if (statusWin)
        delwin(statusWin);

    // 结束curses模式
    endwin();
}

} // namespace GUIHelper
