#include "key_practice.h"
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;
using namespace GUIHelper;

// 构造函数
KeyboardPractice::KeyboardPractice(WINDOW* header, WINDOW* content, WINDOW* status)
    : headerWin(header), contentWin(content), statusWin(status), currentPos(0),
      selectedMenuItem(0), menuVisible(false), currentPracticeType(HOME_ROW)
{
    // 初始化键盘布局
    initKeyboardLayout();
    
    // 初始化菜单
    initMenu();
    
    // 窗口指针初始化为nullptr，实际窗口在run()方法中创建
    keyboardWin = nullptr;
    menuWin = nullptr;
    
    // 设置默认练习文本
    practiceText = getBasicKeysText();
}

// 析构函数
KeyboardPractice::~KeyboardPractice()
{
    // 删除键盘子窗口
    if (keyboardWin != nullptr) {
        delwin(keyboardWin);
        keyboardWin = nullptr;
    }
    
    // 删除菜单子窗口
    if (menuWin != nullptr) {
        delwin(menuWin);
        menuWin = nullptr;
    }
}

// 初始化键盘布局
void KeyboardPractice::initKeyboardLayout()
{
    // QWERTY键盘布局 - 更新按键位置以匹配实际键盘布局
    keyboardLayout = {
        "┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐",
        "│ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │",
        "├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┐",
        "│ q │ w │ e │ r │ t │ y │ u │ i │ o │ p │ [ │ ] │ \\ │",
        "├──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┘",
        "│  │ a │ s │ d │ f │ g │ h │ j │ k │ l │ ; │ ' │   │", 
        "├─┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┴──┬┘",
        "│ │   │ z │ x │ c │ v │ b │ n │ m │ , │ . │ / │   │",
        "└─┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘"
    };
    
    // 设置键位映射 - 调整后的坐标
    // 行1: 1 2 3 4 5 6 7 8 9 0 - = (数字行左移了四格)
    for (int i = 0; i < 12; i++) {
        if (i == 0) keyPositions['1'] = make_pair(1, i*4+2);
        else if (i == 1) keyPositions['2'] = make_pair(1, i*4+2);
        else if (i == 2) keyPositions['3'] = make_pair(1, i*4+2);
        else if (i == 3) keyPositions['4'] = make_pair(1, i*4+2);
        else if (i == 4) keyPositions['5'] = make_pair(1, i*4+2);
        else if (i == 5) keyPositions['6'] = make_pair(1, i*4+2);
        else if (i == 6) keyPositions['7'] = make_pair(1, i*4+2);
        else if (i == 7) keyPositions['8'] = make_pair(1, i*4+2);
        else if (i == 8) keyPositions['9'] = make_pair(1, i*4+2);
        else if (i == 9) keyPositions['0'] = make_pair(1, i*4+2);
        else if (i == 10) keyPositions['-'] = make_pair(1, i*4+2);
        else if (i == 11) keyPositions['='] = make_pair(1, i*4+2);
    }

    // 行2: q w e r t y u i o p [ ] (qwer行已左移一个字符)
    for (int i = 0; i < 12; i++) {
        if (i == 0) keyPositions['q'] = make_pair(3, i*4+2);
        else if (i == 1) keyPositions['w'] = make_pair(3, i*4+2);
        else if (i == 2) keyPositions['e'] = make_pair(3, i*4+2);
        else if (i == 3) keyPositions['r'] = make_pair(3, i*4+2);
        else if (i == 4) keyPositions['t'] = make_pair(3, i*4+2);
        else if (i == 5) keyPositions['y'] = make_pair(3, i*4+2);
        else if (i == 6) keyPositions['u'] = make_pair(3, i*4+2);
        else if (i == 7) keyPositions['i'] = make_pair(3, i*4+2);
        else if (i == 8) keyPositions['o'] = make_pair(3, i*4+2);
        else if (i == 9) keyPositions['p'] = make_pair(3, i*4+2);
        else if (i == 10) keyPositions['['] = make_pair(3, i*4+2);
        else if (i == 11) keyPositions[']'] = make_pair(3, i*4+2);
    }

    // 行3: a s d f g h j k l ; ' (asdf行左移了1个字符)
    for (int i = 0; i < 11; i++) {
        if (i == 0) keyPositions['a'] = make_pair(5, (i+1)*4+1);
        else if (i == 1) keyPositions['s'] = make_pair(5, (i+1)*4+1);
        else if (i == 2) keyPositions['d'] = make_pair(5, (i+1)*4+1);
        else if (i == 3) keyPositions['f'] = make_pair(5, (i+1)*4+1);
        else if (i == 4) keyPositions['g'] = make_pair(5, (i+1)*4+1);
        else if (i == 5) keyPositions['h'] = make_pair(5, (i+1)*4+1);
        else if (i == 6) keyPositions['j'] = make_pair(5, (i+1)*4+1);
        else if (i == 7) keyPositions['k'] = make_pair(5, (i+1)*4+1);
        else if (i == 8) keyPositions['l'] = make_pair(5, (i+1)*4+1);
        else if (i == 9) keyPositions[';'] = make_pair(5, (i+1)*4+1);
        else if (i == 10) keyPositions['\''] = make_pair(5, (i+1)*4+1);
    }

    // 行4: z x c v b n m , . / (zxcv行左移了2格)
    for (int i = 0; i < 10; i++) {
        if (i == 0) keyPositions['z'] = make_pair(7, (i+2)*4+0);
        else if (i == 1) keyPositions['x'] = make_pair(7, (i+2)*4+0);
        else if (i == 2) keyPositions['c'] = make_pair(7, (i+2)*4+0);
        else if (i == 3) keyPositions['v'] = make_pair(7, (i+2)*4+0);
        else if (i == 4) keyPositions['b'] = make_pair(7, (i+2)*4+0);
        else if (i == 5) keyPositions['n'] = make_pair(7, (i+2)*4+0);
        else if (i == 6) keyPositions['m'] = make_pair(7, (i+2)*4+0);
        else if (i == 7) keyPositions[','] = make_pair(7, (i+2)*4+0);
        else if (i == 8) keyPositions['.'] = make_pair(7, (i+2)*4+0);
        else if (i == 9) keyPositions['/'] = make_pair(7, (i+2)*4+0);
    }
    
    // 空格键特殊处理
    keyPositions[' '] = make_pair(8, 28); // 空格键在底部中间位置
}

// 初始化菜单
void KeyboardPractice::initMenu()
{
    menuItems = {
        "Home Row (asdf jkl;)",
        "Top Row (qwerty...)",
        "Bottom Row (zxcv...)",
        "Number Row (1234...)",
        "Full Keyboard",
        "Return to Main Menu"
    };
}

// 绘制键盘
void KeyboardPractice::drawKeyboard()
{
    wclear(keyboardWin);
    
    // 绘制键盘框架
    for (size_t i = 0; i < keyboardLayout.size(); i++) {
        mvwprintw(keyboardWin, i, 0, "%s", keyboardLayout[i].c_str());
    }
    
    // 在底部添加空格键
    mvwprintw(keyboardWin, 8, 10, "┌───┴───┴───┴───┴───┴───┴───┤");
    mvwprintw(keyboardWin, 9, 10, "|           SPACE           |");
    mvwprintw(keyboardWin, 10, 10,"└───────────────────────────┘");
    
    wrefresh(keyboardWin);
}

// 绘制菜单
void KeyboardPractice::drawMenu()
{
    if (!menuVisible || menuWin == nullptr) {
        return;
    }
    
    wclear(menuWin);
    box(menuWin, 0, 0);
    
    // 获取菜单窗口宽度
    int menuWidth = getmaxx(menuWin) - 2; // 减去边框
    
    // 绘制菜单标题 - 居中显示
    wattron(menuWin, COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    
    string title = " SELECT PRACTICE TYPE  ";
    int titlePos = (menuWidth - title.length()) / 2;
    mvwprintw(menuWin, 1, titlePos > 0 ? titlePos : 1, "%s", title.c_str());
    
    string separator = "=======================";
    int separatorPos = (menuWidth - separator.length()) / 2;
    mvwprintw(menuWin, 2, separatorPos > 0 ? separatorPos : 1, "%s", separator.c_str());
    
    wattroff(menuWin, COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    
    // 绘制菜单项
    for (size_t i = 0; i < menuItems.size(); i++) {
        // 高亮显示当前选中项
        if (i == static_cast<size_t>(selectedMenuItem)) {
            wattron(menuWin, A_REVERSE);
        }
        
        mvwprintw(menuWin, i + 4, 2, "%s", menuItems[i].c_str());
        
        if (i == static_cast<size_t>(selectedMenuItem)) {
            wattroff(menuWin, A_REVERSE);
        }
    }
    
    // 绘制操作提示
    wattron(menuWin, COLOR_PAIR(COLOR_GUIDE));
    mvwprintw(menuWin, menuItems.size() + 6, 2, "UP/DOWN: Move");
    mvwprintw(menuWin, menuItems.size() + 7, 2, "Enter: Select");
    mvwprintw(menuWin, menuItems.size() + 8, 2, "TAB: Hide menu");
    wattroff(menuWin, COLOR_PAIR(COLOR_GUIDE));
    
    wrefresh(menuWin);
}

// 高亮显示某个键
void KeyboardPractice::highlightKey(char key, int colorPair)
{
    // 将键转为小写以匹配映射
    char lowerKey = tolower(key);
    
    if (keyPositions.find(lowerKey) != keyPositions.end()) {
        auto pos = keyPositions[lowerKey];
        int y = pos.first;
        int x = pos.second;
        
        // 特殊处理空格键
        if (lowerKey == ' ') {
            wattron(keyboardWin, COLOR_PAIR(colorPair) | A_BOLD);
            mvwprintw(keyboardWin, 9, 11, "           SPACE           ");
            wattroff(keyboardWin, COLOR_PAIR(colorPair) | A_BOLD);
        } else {
            // 高亮单个键
            wattron(keyboardWin, COLOR_PAIR(colorPair) );
            mvwaddch(keyboardWin, y, x, lowerKey);
            wattroff(keyboardWin, COLOR_PAIR(colorPair) );
        }
        
        wrefresh(keyboardWin);
    }
}

// 重绘键盘，清除所有高亮
void KeyboardPractice::resetKeyHighlight()
{
    drawKeyboard();
}

// 处理菜单输入
bool KeyboardPractice::processMenuInput(int ch)
{
    if (!menuVisible) {
        // 只有当菜单可见时才处理菜单输入
        return false;
    }
    
    switch (ch) {
        case KEY_UP:
            if (selectedMenuItem > 0) {
                selectedMenuItem--;
                drawMenu();
            }
            return true;
        
        case KEY_DOWN:
            if (selectedMenuItem < static_cast<int>(menuItems.size()) - 1) {
                selectedMenuItem++;
                drawMenu();
            }
            return true;
        
        case '\r': 
        case '\n': 
        case KEY_ENTER:
            // 处理选择
            if (selectedMenuItem == static_cast<int>(menuItems.size()) - 1) {
                // 选择了"返回主菜单"选项
                return false; // 返回false让外层processInput处理退出逻辑
            } else {
                // 切换练习类型
                changePracticeType(static_cast<KeyPracticeType>(selectedMenuItem));
                menuVisible = false;  // 隐藏菜单
                
                // 清空内容窗口
                wclear(contentWin);
                
                // 这里不要再调用run()，而是设置重绘标志并返回
                wrefresh(contentWin);
                return false; // 返回false使外层重新绘制界面
            }
        
        case '\t':  // TAB键：隐藏菜单
            menuVisible = false;
            // 不要调用run()，让外层逻辑处理界面重绘
            return false;
            
        case GUIHelper::KEY_ESC:  // ESC键：退出练习
            menuVisible = false;  // 隐藏菜单，退出时不要显示菜单
            return false;         // 返回false让外层processInput处理退出逻辑
    }
    
    return true; // 默认情况下捕获所有按键
}

// 切换练习类型
void KeyboardPractice::changePracticeType(KeyPracticeType type)
{
    currentPracticeType = type;
    
    // 根据练习类型设置练习文本
    switch (type) {
        case HOME_ROW:
            practiceText = getBasicKeysText();
            break;
        case TOP_ROW:
            practiceText = getTopRowText();
            break;
        case BOTTOM_ROW:
            practiceText = getBottomRowText();
            break;
        case NUMBER_ROW:
            practiceText = getNumberRowText();
            break;
        case FULL_KEYBOARD:
            practiceText = getAdvancedKeysText();
            break;
        default:
            practiceText = getBasicKeysText();
            break;
    }
    
    // 重置练习状态
    currentPos = 0;
    userInput = "";
    errorCount.clear();
}

// 处理用户输入
bool KeyboardPractice::processInput()
{
    int ch = getch();
    
    // 如果菜单可见，优先处理菜单输入
    if (menuVisible) {
        bool handled = processMenuInput(ch);
        if (!handled) {
            // 菜单处理返回false，表示需要重绘界面或退出
            if (ch == KEY_ESC) {
                // 直接处理ESC退出，而不是再次进入输入循环
                if (showConfirmDialog(headerWin, contentWin, statusWin,
                                    "Are you sure you want to exit this practice?", "Confirm Exit")) {
                    return false; // 确认退出
                } else {
                    // 用户取消退出，重绘界面
                    menuVisible = false; // 确保菜单隐藏
                    wclear(contentWin);
                    
                    // 重绘键盘练习界面，不进入递归
                    updateHeaderWindow(headerWin, "KEYBOARD PRACTICE");
                    drawKeyboardPracticeScreen();
                    return true;
                }
            } else if (ch == '\t') {
                // TAB键隐藏菜单后重绘界面
                menuVisible = false;
                wclear(contentWin);
                drawKeyboardPracticeScreen();
                return true;
            } else {
                // 其他情况(如切换练习类型)，重绘界面
                wclear(contentWin);
                drawKeyboardPracticeScreen();
                return true;
            }
        }
        return true;
    }
    
    // 菜单切换键 (TAB键)
    if (ch == '\t') {
        menuVisible = !menuVisible;
        
        if (menuVisible) {
            // 显示菜单，确保重新创建菜单窗口
            createMenuWindow();
            drawMenu();
        } else {
            // 如果菜单被隐藏，重绘主界面
            if (menuWin != nullptr) {
                delwin(menuWin);
                menuWin = nullptr;
            }
            wclear(contentWin);
            drawKeyboardPracticeScreen();
        }
        return true;
    }
    
    // 退出练习
    if (ch == KEY_ESC) {
        if (showConfirmDialog(headerWin, contentWin, statusWin,
                             "Are you sure you want to exit this practice?", "Confirm Exit")) {
            return false;
        }
        return true;
    }
    
    // 处理退格键
    if (ch == KEY_BACKSPACE || ch == KEY_BACKSPACE_ALT || ch == 127) {
        if (currentPos > 0 && !userInput.empty()) {
            userInput.pop_back();
            currentPos--;
            
            // 重绘文本区域
            mvwprintw(contentWin, 3, 2, "Current character: %c", practiceText[currentPos]);
            
            // 更新进度
            int progress = (currentPos * 100) / practiceText.length();
            updateStatusWindowWithHelp(statusWin, "Progress: " + to_string(progress) + "%", "ESC: Exit  Backspace: Correct");
            
            // 高亮当前键
            resetKeyHighlight();
            highlightKey(practiceText[currentPos], COLOR_CURRENT);
        }
        return true;
    }
    
    // 处理可打印字符
    if (isprint(ch) && currentPos < practiceText.length()) {
        char typedChar = static_cast<char>(ch);
        userInput += typedChar;
        
        // 检查是否正确
        bool correct = (typedChar == practiceText[currentPos]);
        
        // 如果错误，记录错误
        if (!correct) {
            errorCount[practiceText[currentPos]]++;
            
            // 高亮错误键
            highlightKey(typedChar, COLOR_INCORRECT);
            highlightKey(practiceText[currentPos], COLOR_GUIDE);
            
            // 显示提示
            mvwprintw(contentWin, 4, 2, "You pressed: %c  Expected: %c", typedChar, practiceText[currentPos]);
            wrefresh(contentWin);
            
            // 暂停一秒让用户看清错误
            napms(1000);
        } else {
            // 高亮正确键
            highlightKey(typedChar, COLOR_CORRECT);
        }
        
        // 前进到下一个字符
        currentPos++;
        
        // 更新进度
        int progress = (currentPos * 100) / practiceText.length();
        updateStatusWindowWithHelp(statusWin, "Progress: " + to_string(progress) + "%", "ESC: Exit  Backspace: Correct");
        
        // 如果还有下一个字符，高亮它
        if (currentPos < practiceText.length()) {
            napms(500); // 短暂暂停，然后重置键盘高亮
            resetKeyHighlight();
            mvwprintw(contentWin, 3, 2, "Current character: %c", practiceText[currentPos]);
            highlightKey(practiceText[currentPos], COLOR_CURRENT);
        } else {
            // 练习结束，显示结果
            showResults();
            return false;
        }
    }
    
    return true;
}

// 新增方法：创建菜单窗口
void KeyboardPractice::createMenuWindow()
{
    // 获取内容窗口大小
    int maxY, maxX;
    getmaxyx(contentWin, maxY, maxX);
    
    // 分割区域: 左侧用于键盘练习，右侧保留给菜单
    int menuWidth = 25;  // 菜单宽度
    int practiceWidth = maxX - menuWidth - 1;  // 练习区域宽度
    
    // 如果之前有创建过窗口，先删除
    if (menuWin != nullptr) {
        delwin(menuWin);
    }
    
    // 创建新的菜单窗口
    menuWin = derwin(contentWin, maxY - 4, menuWidth, 
                   2, practiceWidth);
}

// 新增方法：绘制键盘练习界面 (不触发输入循环)
void KeyboardPractice::drawKeyboardPracticeScreen()
{
    // 清空内容窗口
    wclear(contentWin);
    
    // 获取内容窗口大小
    int maxY, maxX;
    getmaxyx(contentWin, maxY, maxX);
    
    // 分割区域: 左侧用于键盘练习，右侧保留给菜单
    int menuWidth = 25;  // 菜单宽度
    int practiceWidth = maxX - menuWidth - 1;  // 练习区域宽度
    
    // 显示说明
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 1, 2, "Keyboard Position Practice");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    
    // 显示菜单使用提示
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
    mvwprintw(contentWin, 1, maxX - 26, "[Press TAB to show menu]");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));
    
    // 显示当前练习类型
    wattron(contentWin, COLOR_PAIR(COLOR_HEADER));
    string currentTypeText;
    switch (currentPracticeType) {
        case HOME_ROW: currentTypeText = "Home Row (asdf jkl;)"; break;
        case TOP_ROW: currentTypeText = "Top Row (qwerty...)"; break;
        case BOTTOM_ROW: currentTypeText = "Bottom Row (zxcv...)"; break;
        case NUMBER_ROW: currentTypeText = "Number Row (1234...)"; break;
        case FULL_KEYBOARD: currentTypeText = "Full Keyboard"; break;
        default: currentTypeText = "Home Row"; break;
    }
    mvwprintw(contentWin, 2, 2, "Current Practice: %s", currentTypeText.c_str());
    wattroff(contentWin, COLOR_PAIR(COLOR_HEADER));
    
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 3, 2, "Current character: %c", practiceText[currentPos]);
    mvwprintw(contentWin, 4, 2, "Type the highlighted keys. Focus on accuracy, not speed.");
    mvwprintw(contentWin, 5, 2, "The keyboard below will show you which key to press next.");
    mvwprintw(contentWin, 6, 2, "Press TAB to show/hide practice type menu");
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    
    // 计算键盘窗口的位置和大小
    int keyboardLine = 8; // 从第8行开始显示键盘
    int keyboardHeight = 11; // 键盘布局高度
    
    // 如果之前有创建过窗口，先删除
    if (keyboardWin != nullptr) {
        delwin(keyboardWin);
    }
    
    // 创建新的键盘窗口，紧跟在提示文本下方，限制在左侧区域
    keyboardWin = subwin(contentWin, keyboardHeight, practiceWidth, 
                       getbegy(contentWin) + keyboardLine, getbegx(contentWin) + 2);
    
    // 绘制键盘
    drawKeyboard();
    
    // 如果菜单可见，创建并绘制菜单
    if (menuVisible) {
        createMenuWindow();
        drawMenu();
    }
    
    // 高亮当前字符
    highlightKey(practiceText[currentPos], COLOR_CURRENT);
    
    // 更新状态栏
    updateStatusWindowWithHelp(statusWin, "Progress: " + to_string(currentPos * 100 / practiceText.length()) + "%", 
                            "ESC: Exit  TAB: Menu  Backspace: Correct");
    
    wrefresh(contentWin);
}

// 显示练习结果
void KeyboardPractice::showResults()
{
    // 清空内容窗口
    wclear(contentWin);
    
    // 计算准确率
    int totalChars = userInput.length();
    int correctChars = 0;
    
    for (size_t i = 0; i < min(userInput.length(), practiceText.length()); i++) {
        if (userInput[i] == practiceText[i]) {
            correctChars++;
        }
    }
    
    double accuracy = totalChars > 0 ? (correctChars * 100.0 / totalChars) : 0;
    
    // 显示结果
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 2, 2, "Keyboard Practice Results");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 4, 2, "Total characters: %d", totalChars);
    mvwprintw(contentWin, 5, 2, "Correct characters: %d", correctChars);
    mvwprintw(contentWin, 6, 2, "Accuracy: %.1f%%", accuracy);
    
    // 显示常见错误
    if (!errorCount.empty()) {
        mvwprintw(contentWin, 8, 2, "Most common errors:");
        
        // 排序错误计数
        vector<pair<char, int>> sortedErrors(errorCount.begin(), errorCount.end());
        sort(sortedErrors.begin(), sortedErrors.end(),
             [](const pair<char, int>& a, const pair<char, int>& b) {
                 return a.second > b.second;
             });
        
        // 显示前5个最常见错误
        int row = 9;
        for (size_t i = 0; i < min(size_t(5), sortedErrors.size()); i++) {
            mvwprintw(contentWin, row++, 4, "Character '%c': %d times", 
                      sortedErrors[i].first, sortedErrors[i].second);
        }
    }
    
    // 显示练习建议
    mvwprintw(contentWin, 15, 2, "Practice Suggestions:");
    
    if (accuracy < 90) {
        mvwprintw(contentWin, 16, 4, "Focus on accuracy rather than speed.");
        mvwprintw(contentWin, 17, 4, "Try again with the same keys.");
    } else if (accuracy < 95) {
        mvwprintw(contentWin, 16, 4, "Good accuracy! Keep practicing to improve speed.");
    } else {
        mvwprintw(contentWin, 16, 4, "Excellent! Try a more advanced practice.");
    }
    
    mvwprintw(contentWin, 19, 2, "Press any key to continue...");
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    
    wrefresh(contentWin);
    
    // 等待用户按键
    getch();
}

// 运行键盘练习
void KeyboardPractice::run()
{
    // 设置标题
    updateHeaderWindow(headerWin, "KEYBOARD PRACTICE");
    
    // 绘制键盘练习界面
    drawKeyboardPracticeScreen();
    
    // 输入循环
    bool continue_practice = true;
    while (continue_practice) {
        continue_practice = processInput();
    }
}

// 获取顶行练习文本
string KeyboardPractice::getTopRowText() const
{
    return "qwe qwe qwe qwe qwe rty rty rty rty rty "
           "uio uio uio uio uio pop pop pop pop pop "
           "qwerty qwerty qwerty qwerty qwerty "
           "poiuy poiuy poiuy poiuy poiuy "
           "qwertyuiop qwertyuiop qwertyuiop ";
}

// 获取底行练习文本
string KeyboardPractice::getBottomRowText() const
{
    return "zxc zxc zxc zxc zxc vbn vbn vbn vbn vbn "
           "zxcv zxcv zxcv zxcv bnm bnm bnm bnm "
           "zxcvbnm zxcvbnm zxcvbnm zxcvbnm "
           "mnbvcxz mnbvcxz mnbvcxz mnbvcxz ";
}

// 获取数字行练习文本
string KeyboardPractice::getNumberRowText() const
{
    return "1234 1234 1234 1234 5678 5678 5678 5678 "
           "90 90 90 90 12345 12345 12345 67890 67890 67890 "
           "1234567890 1234567890 1234567890 "
           "0987654321 0987654321 0987654321 ";
}

// 获取基础键位练习文本
string KeyboardPractice::getBasicKeysText() const
{
    return "asdf jkl; asdf jkl; asdf jkl; asdf jkl; asdf jkl; "
           "fdsa ;lkj fdsa ;lkj fdsa ;lkj fdsa ;lkj fdsa ;lkj "
           "ff jj dd kk ss ll aa ;; ff jj dd kk ss ll aa ;; "
           "fj dk sl a; fj dk sl a; fj dk sl a; fj dk sl a; ";
}

// 获取进阶键位练习文本
string KeyboardPractice::getAdvancedKeysText() const
{
    // 生成随机字符序列而不是使用固定文本
    random_device rd;
    mt19937 gen(rd());
    
    // 定义所有可能的字符集，包括字母、数字和常用符号
    string alphaChars = "abcdefghijklmnopqrstuvwxyz";
    string numChars = "1234567890";
    string symbolChars = "`-=[]\\;',./";
    
    // 所有可能的字符集合
    string allChars = alphaChars + numChars + symbolChars;
    
    // 定义字符分布：字母出现概率高，数字其次，符号最低
    // 这样能确保练习主要集中在最常用的按键上
    vector<pair<string, double>> charSets = {
        {alphaChars, 0.65},   // 字母出现概率65%
        {numChars, 0.20},     // 数字出现概率20%
        {symbolChars, 0.15}   // 符号出现概率15%
    };
    
    // 生成随机文本的长度（约150-200个字符）
    uniform_int_distribution<> lenDist(150, 200);
    int textLength = lenDist(gen);
    
    string randomText;
    
    for (int i = 0; i < textLength; i++) {
        // 随机决定使用哪个字符集
        double setSelector = uniform_real_distribution<>(0.0, 1.0)(gen);
        double accumProb = 0.0;
        string selectedSet;
        
        for (const auto& set : charSets) {
            accumProb += set.second;
            if (setSelector <= accumProb) {
                selectedSet = set.first;
                break;
            }
        }
        
        // 从选定的字符集中随机选择一个字符
        uniform_int_distribution<> charDist(0, selectedSet.length() - 1);
        randomText += selectedSet[charDist(gen)];
        
        // 每4-7个字符添加一个空格
        uniform_int_distribution<> spaceDist(4, 7);
        if ((i + 1) % spaceDist(gen) == 0) {
            randomText += ' ';
        }
    }
    
    // 确保以空格结尾，以便最后一个字符能被正确检查
    if (randomText.back() != ' ') {
        randomText += ' ';
    }
    
    return randomText;
}

// 全局函数，启动键盘练习
void startKeyboardPractice(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin)
{
    KeyboardPractice practice(headerWin, contentWin, statusWin);
    practice.run();
}