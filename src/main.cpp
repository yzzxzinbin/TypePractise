#include "type_practise.h"
#include "gui_helper.h" // 添加GUI辅助模块

using namespace GUIHelper; // 使用GUI辅助模块命名空间

// 全局变量的定义
const string CORPUS_DIR = "k:\\test program\\TypePractise\\corpus";
const string USER_DIR = "k:\\test program\\TypePractise\\users";
const string SETTINGS_DIR = "k:\\test program\\TypePractise\\settings"; // 添加设置目录
UserManager *userManager = nullptr;
StatisticsManager *statsManager = nullptr;
CorpusManager *corpusManager = nullptr;
SettingsManager *settingsManager = nullptr; // 添加设置管理器实例

// 难度对应的文字数量
const map<DifficultyLevel, int> difficultyWordCount = {
    {EASY, 10},
    {MEDIUM, 25},
    {HARD, 40},
    {CUSTOM, 15},
    {IMPORTED, 25}, // 导入文本的默认长度
    {CHI, 60}       // 中文拼音练习的默认长度
};

// 练习文本库
const vector<string> practiceTexts = {
    "The quick brown fox jumps over the lazy dog.",
    "Programming is the art of telling another human what one wants the computer to do.",
    "C++ is a general-purpose programming language created as an extension of the C programming language.",
    "Practice makes perfect. After a long time of practicing, our work will become natural, skillful, swift, and steady.",
    "The only way to learn a new programming language is by writing programs in it.",
    "The more you type, the faster you will become. Keep practicing every day.",
    "Typing is an essential skill in the modern world. Everyone should learn to type properly.",
    "This typing tutor will help you improve your speed and accuracy over time."
};

// 窗口对象
WINDOW *headerWin = nullptr;
WINDOW *contentWin = nullptr;
WINDOW *statusWin = nullptr;

// 初始化窗口，替换为调用GUIHelper版本
void initWindows()
{
    GUIHelper::initWindows(&headerWin, &contentWin, &statusWin);
}

// 初始化颜色，替换为调用GUIHelper版本
void initColors()
{
    GUIHelper::initColors();
}

// 更新标题窗口，替换为调用GUIHelper版本
void updateHeaderWindow(const string &title)
{
    GUIHelper::updateHeaderWindow(headerWin, title);
}

// 更新状态窗口，替换为调用GUIHelper版本
void updateStatusWindow(const string &status)
{
    GUIHelper::updateStatusWindow(statusWin, status);
}

// 清除内容窗口，替换为调用GUIHelper版本
void clearContentWindow()
{
    GUIHelper::clearContentWindow(contentWin);
}

// 随机生成练习文本
string generatePracticeText(int wordCount)
{
    wordCount = 15; // 默认值
    static vector<string> words;
    if (words.empty())
    {
        for (const auto &text : practiceTexts)
        {
            istringstream iss(text);
            string word;
            while (iss >> word)
            {
                // 移除标点符号
                if (!word.empty() && ispunct(word.back()))
                    word.pop_back();
                if (!word.empty())
                    words.push_back(word);
            }
        }
    }

    static mt19937 rng(random_device{}());
    shuffle(words.begin(), words.end(), rng);

    string result;
    for (int i = 0; i < min(wordCount, static_cast<int>(words.size())); ++i)
    {
        if (!result.empty())
            result += " ";
        result += words[i];
    }
    return result + " "; // 添加空格以便最后一个词能被正确检查
}

// 更新统计数据
void updateStatistics(double timeTaken, int charsTyped, int correctChars, double wpm, double accuracy)
{
    // 直接使用 StatisticsManager 类的更新方法
    statsManager->updateStatistics(timeTaken, charsTyped, correctChars, wpm, accuracy);
}

// 显示欢迎界面
void showWelcomeScreen()
{
    // 加载统计数据（如果存在）
    if (!statsManager->isStatsLoaded())
    {
        string statsPath = userManager->getCurrentUserStatsPath();
        if (!statsPath.empty())
        {
            statsManager->setStatsPath(statsPath);
            statsManager->loadStatistics();
        }
    }

    updateHeaderWindow("TYPING TUTOR PROGRAM");
    clearContentWindow();

    // 使用颜色和属性的组合来增强导引文字的视觉差异
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 添加粗体属性
    mvwprintw(contentWin, 2, 2, "Welcome to the typing tutor program!");
    mvwprintw(contentWin, 4, 2, "Instructions:");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 同样移除粗体属性
    
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 5, 2, "1. Type the text displayed on screen as fast and accurately as possible.");
    mvwprintw(contentWin, 6, 2, "2. The current character to type is highlighted in yellow.");
    mvwprintw(contentWin, 7, 2, "3. Correct characters will turn green, incorrect ones will turn red.");
    mvwprintw(contentWin, 8, 2, "4. Press ESC anytime to return to the main menu.");
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    updateStatusWindow("Press any key to continue...");
    wrefresh(contentWin);

    getch();
}

// 显示难度选择菜单
DifficultyLevel showDifficultyMenu()
{
    updateHeaderWindow("SELECT DIFFICULTY");
    clearContentWindow();

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 添加粗体属性
    mvwprintw(contentWin, 2, 2, "Select difficulty level:");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 同样移除粗体属性
    
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 4, 2, "1. Easy (10 words)");
    mvwprintw(contentWin, 5, 2, "2. Medium (20 words)");
    mvwprintw(contentWin, 6, 2, "3. Hard (30 words)");
    mvwprintw(contentWin, 7, 2, "4. Chinese Pinyin");
    mvwprintw(contentWin, 8, 2, "5. Custom text");
    mvwprintw(contentWin, 10, 2, "0. Return to main menu");
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    updateStatusWindowWithHelp("Enter your choice (0-5):", "Or press ESC to return");
    wrefresh(contentWin);

    // 等待有效输入
    while (true)
    {
        int ch = getch();

        if (ch == KEY_ESC || ch == '0')
        {
            // 返回CANCEL表示用户想返回
            return CANCEL;
        }
        else if (ch >= '1' && ch <= '5')
        {
            switch (ch)
            {
            case '1':
                return EASY;
            case '2':
                return MEDIUM;
            case '3':
                return HARD;
            case '4':
                return CHI;
            case '5':
                return CUSTOM;
            }
        }
    }
}

// 获取自定义文本
string getCustomText()
{
    string customText;
    char input[256];

    updateHeaderWindow("CUSTOM TEXT");
    clearContentWindow();

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 添加粗体属性
    mvwprintw(contentWin, 2, 2, "Enter your custom text (max 255 characters):");
    mvwprintw(contentWin, 3, 2, "Leave empty and press Enter to cancel");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 同样移除粗体属性

    updateStatusWindowWithHelp("Press Enter when done", "ESC to return without saving");
    wrefresh(contentWin);

    // 禁用全局回显
    noecho();

    // 移动光标到输入位置
    wmove(contentWin, 5, 2);
    wrefresh(contentWin);

    // 清空输入缓冲区
    memset(input, 0, sizeof(input));
    int inputPos = 0;

    // 显示光标，但只在内容窗口
    curs_set(1);

    // 确保输入只在内容窗口
    keypad(contentWin, TRUE);

    bool inputDone = false;
    bool cancelled = false;

    while (!inputDone)
    {
        // 使用wgetch而不是getch，确保从contentWin获取输入
        int ch = wgetch(contentWin);

        if (ch == KEY_ESC)
        {
            cancelled = true;
            inputDone = true;
        }
        else if (ch == KEY_RETURN || ch == '\r' || ch == '\n')
        {
            inputDone = true;
        }
        else if (ch == KEY_BACKSPACE || ch == KEY_BACKSPACE_ALT || ch == 127)
        {
            // 处理退格键
            if (inputPos > 0)
            {
                inputPos--;
                input[inputPos] = '\0';

                // 更新显示 - 使用空格覆盖
                wmove(contentWin, 5, 2 + inputPos);
                waddch(contentWin, ' ');
                wmove(contentWin, 5, 2 + inputPos);
                wrefresh(contentWin);
            }
        }
        else if (isprint(ch) && inputPos < 255)
        {
            // 处理可打印字符
            input[inputPos++] = ch;
            // 只在内容窗口添加字符
            waddch(contentWin, ch);
            wrefresh(contentWin);
        }
    }

    // 隐藏光标
    curs_set(0);

    // 重置窗口键盘模式
    keypad(stdscr, TRUE);

    if (cancelled)
    {
        return ""; // 返回空字符串表示取消
    }

    customText = string(input);
    if (customText.empty())
    {
        return ""; // 用户未输入任何内容，视为取消
    }

    // 确保文本以空格结尾
    if (customText.back() != ' ')
    {
        customText += ' ';
    }

    return customText;
}

// 显示设置菜单
void showSettingsMenu()
{
    settingsManager->showSettingsMenu(headerWin, contentWin, statusWin);
}

// 显示主菜单
int showMainMenu(bool showUserOption)
{
    string username = userManager->getCurrentUser().username;
    updateHeaderWindow("Main Menu - " + username);
    clearContentWindow();
    
    // 在右上角显示版本号
    int maxX = getmaxx(contentWin);
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
    mvwprintw(contentWin, 1, maxX - strlen(PROGRAM_VERSION) - 2, "%s", PROGRAM_VERSION);
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));
    
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "1. Quick Start");  // 修改这里，从 "Start Typing Practice" 改为 "Quick Start"
    mvwprintw(contentWin, 3, 2, "2. Choose from Corpus Library");
    mvwprintw(contentWin, 4, 2, "3. Import New Text File");
    mvwprintw(contentWin, 5, 2, "4. View Statistics");
    mvwprintw(contentWin, 6, 2, "5. Settings"); // 添加设置选项

    if (showUserOption)
    {
        mvwprintw(contentWin, 7, 2, "6. Switch User");
        mvwprintw(contentWin, 8, 2, "7. Exit");
    }
    else
    {
        mvwprintw(contentWin, 7, 2, "6. Exit");
    }
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    // 添加快捷键提示
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
    mvwprintw(contentWin, 10, 2, "F1: Help");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));

    // 显示一些简短的统计信息（如果有）
    UserStats stats = statsManager->getStatistics();
    if (statsManager->isStatsLoaded() && stats.sessionsCompleted > 0)
    {
        wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
        mvwprintw(contentWin, 12, 2, "Sessions completed: %d", stats.sessionsCompleted);
        mvwprintw(contentWin, 13, 2, "Best WPM: %.1f", stats.bestWPM);
        mvwprintw(contentWin, 14, 2, "Best accuracy: %.1f%%", stats.bestAccuracy);
        wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));
    }

    updateStatusWindowWithHelp("Select option (1-" + std::to_string(showUserOption ? 7 : 6) + "):", "F1 for Help");
    wrefresh(contentWin);

    // 处理按键输入前确保键盘输入设置正确
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE); // 确保是阻塞模式

    // 处理按键输入
    while (true)
    {
        int ch = getch();

        // 处理F1帮助键
        if (ch == KEY_F(1))
        {
            string helpText = "1-" + string(showUserOption ? "7" : "6") + ": Select menu option\n";
            helpText += "F1: Show this help\n";
            helpText += "ESC: Exit program from any screen\n";
            helpText += "This program helps you practice typing skills through various texts.";

            showMessageDialog(helpText, DIALOG_INFO, "Help");
            continue;
        }

        // 处理数字选择
        if (ch >= '1' && (showUserOption ? ch <= '7' : ch <= '6'))
        {
            return ch - '0';
        }

        // ESC键直接退出程序，增加确认
        if (ch == KEY_ESC)
        {
            if (showConfirmDialog("Are you sure you want to exit?", "Confirm Exit"))
            {
                return showUserOption ? 7 : 6; // 选择退出选项
            }
        }
    }
}

// 新增：快速开始子菜单
int showQuickStartMenu()
{
    updateHeaderWindow("QUICK START");
    clearContentWindow();

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 2, 2, "Choose practice type:");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 4, 2, "1. Keyboard Practice - Basic key position exercises");
    mvwprintw(contentWin, 5, 2, "2. Smart Text Practice - Auto-selected based on your history");
    mvwprintw(contentWin, 6, 2, "3. Custom Difficulty - Choose your own difficulty level");
    mvwprintw(contentWin, 8, 2, "0. Return to main menu");
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    updateStatusWindowWithHelp(statusWin, "Select option (0-3):", "ESC to return");
    wrefresh(contentWin);

    // 等待有效输入
    while (true)
    {
        int ch = getch();

        if (ch == KEY_ESC || ch == '0')
        {
            return 0; // 返回主菜单
        }
        else if (ch >= '1' && ch <= '3')
        {
            return ch - '0';
        }
    }
}

// 键位练习
void keyboardPractice()
{
    // 基本键位练习内容
    string practiceText = "asdf jkl; asdf jkl; asdf jkl; asdf jkl; asdf jkl; ";
    practiceText += "asdf jkl; asdf jkl; asdf jkl; asdf jkl; asdf jkl; ";
    practiceText += "fdsa ;lkj fdsa ;lkj fdsa ;lkj fdsa ;lkj fdsa ;lkj ";
    practiceText += "fdsa ;lkj fdsa ;lkj fdsa ;lkj fdsa ;lkj fdsa ;lkj ";
    
    // 调用现有的练习会话函数
    practiceSession(practiceText);
}

// 智能文本练习 - 根据用户历史数据自动选择文本
void smartTextPractice()
{
    DifficultyLevel difficulty = MEDIUM; // 默认中等难度
    
    // 获取用户统计信息
    UserStats stats = statsManager->getStatistics();
    
    // 根据用户的打字速度和准确率调整难度
    if (statsManager->isStatsLoaded() && stats.sessionsCompleted > 0)
    {
        // 如果平均WPM < 20，或准确率 < 85%，选择简单难度
        double avgWPM = (stats.totalTypingTime > 0) ? 
            (stats.totalCharsTyped / 5.0) / (stats.totalTypingTime / 60.0) : 0;
        double accuracy = (stats.totalCharsTyped > 0) ? 
            (stats.totalCorrectChars * 100.0 / stats.totalCharsTyped) : 0;
            
        if (avgWPM < 20 || accuracy < 85.0)
        {
            difficulty = EASY;
        }
        // 如果平均WPM > 40 且准确率 > 95%，选择困难难度
        else if (avgWPM > 40 && accuracy > 95.0)
        {
            difficulty = HARD;
        }
    }
    
    // 从语料库中选择相应难度的文本
    vector<Corpus> matchingCorpus;
    for (const auto& corpus : corpusManager->getCorpusLibrary())
    {
        if (corpus.difficulty == difficulty)
        {
            matchingCorpus.push_back(corpus);
        }
    }
    
    // 如果没有找到匹配难度的文本，使用生成的文本
    if (matchingCorpus.empty())
    {
        int wordCount = corpusManager->getWordCountForDifficulty(difficulty);
        string practiceText = generatePracticeText(wordCount);
        practiceSession(practiceText);
        return;
    }
    
    // 随机选择一个文本
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, matchingCorpus.size() - 1);
    int randomIndex = distrib(gen);
    
    // 读取并提取练习文本
    string fullText = corpusManager->readCorpusFile(matchingCorpus[randomIndex].filename);
    int wordCount = corpusManager->getWordCountForDifficulty(difficulty);
    string practiceText = corpusManager->extractPracticeSegment(fullText, wordCount);
    
    // 开始练习
    practiceSession(practiceText);
}

// 处理快速开始菜单选择
void handleQuickStart()
{
    int choice = showQuickStartMenu();
    
    switch (choice)
    {
    case 0: // 返回主菜单
        return;
    case 1: // 键位练习
        keyboardPractice();
        break;
    case 2: // 智能文本练习
        smartTextPractice();
        break;
    case 3: // 自定义难度
        practiceSession(""); // 使用空字符串调用现有函数，让它显示难度选择菜单
        break;
    }
}

// 修改打字练习会话函数
void practiceSession(string practiceText)
{
    // 获取练习文本
    bool userCancelled = false;

    if (practiceText.empty())
    {
        DifficultyLevel difficulty = showDifficultyMenu();

        // 检查用户是否取消了难度选择
        if (difficulty == CANCEL)
        {
            // 用户选择了返回主菜单
            return;
        }
        else if (difficulty == CUSTOM)
        {
            // 获取自定义文本或检查用户是否取消
            practiceText = getCustomText();
            if (practiceText.empty())
            {
                userCancelled = true;
            }
        }
        else
        {
            // 使用难度对应的词数生成练习文本
            int wordCount = corpusManager->getWordCountForDifficulty(difficulty);
            practiceText = generatePracticeText(wordCount);
        }
    }

    // 如果用户取消了，直接返回
    if (userCancelled || practiceText.empty())
    {
        return;
    }

    updateHeaderWindow("TYPING PRACTICE");
    clearContentWindow();

    string userInput;
    int currentPos = 0;
    bool exitRequested = false;
    // 移除paused变量，不再使用暂停功能

    // 记录错误的字符
    map<char, int> sessionErrors;

    // 计时相关变量
    auto startTime = high_resolution_clock::now();
    // 移除暂停时间相关变量

    // 为长文本准备多行显示
    int maxCharsPerLine = getmaxx(contentWin) - 6; // 留出边距
    vector<string> textLines;

    // 分割文本到多行
    string remainingText = practiceText;
    while (!remainingText.empty())
    {
        if (remainingText.length() <= maxCharsPerLine)
        {
            textLines.push_back(remainingText);
            remainingText.clear();
        }
        else
        {
            // 寻找适当的断行点
            int breakPos = maxCharsPerLine;
            while (breakPos > 0 && remainingText[breakPos] != ' ')
            {
                breakPos--;
            }

            if (breakPos == 0)
            {
                // 没找到空格，强制断行
                breakPos = maxCharsPerLine;
            }

            textLines.push_back(remainingText.substr(0, breakPos));
            remainingText = remainingText.substr(breakPos);
        }
    }

    // 显示练习文本
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 导引文字使用粗体属性
    mvwprintw(contentWin, 2, 2, "Type the following text:");
    
    // 获取设置
    bool skipSpace = settingsManager->getBoolSetting(SettingKeys::SKIP_SPACE, true);
    bool ignoreCase = settingsManager->getBoolSetting(SettingKeys::IGNORE_CASE, false);
    
    // 显示相关设置提示
    int line = 3;
    
    if (skipSpace) {
        mvwprintw(contentWin, line++, 2, "(Spaces will be skipped automatically)");
    }
    
    if (ignoreCase) {
        mvwprintw(contentWin, line++, 2, "(Case differences will be ignored)");
    }
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 关闭导引文字属性

    // 显示所有行 - 使用默认颜色显示实际打字内容
    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    for (size_t i = 0; i < textLines.size(); i++)
    {
        mvwprintw(contentWin, 4 + i, 2, "%s", textLines[i].c_str());
    }
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    // 高亮显示第一个字符 - 使用COLOR_CURRENT而不是GUIHelper::COLOR_CURRENT
    int currentLine = 0;
    wmove(contentWin, 4 + currentLine, 2);
    
    // 只需要使用 COLOR_PAIR 而不需要添加其他属性
    wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));  
    waddch(contentWin, practiceText[0]);
    wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));  

    // 显示帮助和进度 - 移除P键提示
    updateStatusWindowWithHelp("Progress: 0%", "ESC: Return to Menu  F1: Help");

    wrefresh(contentWin);

    // 禁用光标
    curs_set(0);

    while (currentPos < practiceText.size() && !exitRequested)
    {
        // 如果启用了Skip Space且当前字符是空格，自动跳过
        if (skipSpace && practiceText[currentPos] == ' ')
        {
            // 计算当前位置对应的行和列
            int posLine = 0;
            int posInLine = currentPos;

            for (size_t i = 0; i < textLines.size(); i++)
            {
                if (posInLine < textLines[i].length())
                {
                    posLine = i;
                    break;
                }
                posInLine -= textLines[i].length();
            }

            // 更新显示 - 将空格标记为正确输入
            wmove(contentWin, 4 + posLine, 2 + posInLine);
            wattron(contentWin, COLOR_PAIR(COLOR_CORRECT));
            waddch(contentWin, ' ');
            wattroff(contentWin, COLOR_PAIR(COLOR_CORRECT));

            // 添加空格到用户输入，并移动到下一个字符
            userInput += ' ';
            currentPos++;

            // 如果已经到达文本末尾则退出循环
            if (currentPos >= practiceText.size()) {
                break;
            }

            // 计算下一个位置
            posLine = 0;
            posInLine = currentPos;
            for (size_t i = 0; i < textLines.size(); i++)
            {
                if (posInLine < textLines[i].length())
                {
                    posLine = i;
                    break;
                }
                posInLine -= textLines[i].length();
            }

            // 高亮显示下一个字符
            wmove(contentWin, 4 + posLine, 2 + posInLine);
            wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));  
            waddch(contentWin, practiceText[currentPos]);
            wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));  

            // 更新进度
            int progress = (currentPos * 100) / practiceText.size();
            updateStatusWindowWithHelp("Progress: " + to_string(progress) + "%", "ESC: Menu");

            wrefresh(contentWin);
            continue;
        }

        int ch = getch();

        // 特殊键处理
        switch (ch)
        {
        case KEY_ESC:
            if (showConfirmDialog("Are you sure you want to exit?", "Confirm Exit"))
            {
                exitRequested = true;
            }
            continue;

            // 移除P键暂停功能相关代码

        case KEY_F(1): // F1键显示帮助
            showMessageDialog(
                "ESC: Return to menu\n"
                "Backspace: Delete last character\n"
                "Type the displayed text as fast and accurately as possible.",
                DIALOG_INFO, "Help");
            continue;
        }

        // 处理退格键
        if (ch == KEY_BACKSPACE || ch == KEY_BACKSPACE_ALT || ch == 127)
        {
            if (currentPos > 0 && !userInput.empty())
            {
                // 计算当前位置对应的行和列
                int posLine = 0;
                int posInLine = currentPos;

                // 找出当前字符所在的行
                for (size_t i = 0; i < textLines.size(); i++)
                {
                    if (posInLine < textLines[i].length())
                    {
                        posLine = i;
                        break;
                    }
                    posInLine -= textLines[i].length();
                }

                // 恢复之前字符的显示
                wmove(contentWin, 4 + posLine, 2 + posInLine);
                wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
                waddch(contentWin, practiceText[currentPos]);
                wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

                // 删除最后一个输入的字符
                userInput.pop_back();
                currentPos--;

                // 重新计算当前位置
                posLine = 0;
                posInLine = currentPos;
                for (size_t i = 0; i < textLines.size(); i++)
                {
                    if (posInLine < textLines[i].length())
                    {
                        posLine = i;
                        break;
                    }
                    posInLine -= textLines[i].length();
                }

                // 高亮显示当前字符
                wmove(contentWin, 4 + posLine, 2 + posInLine);
                wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));  
                waddch(contentWin, practiceText[currentPos]);
                wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));  

                // 更新进度
                int progress = (currentPos * 100) / practiceText.size();
                updateStatusWindowWithHelp("Progress: " + to_string(progress) + "%", "ESC: Menu");

                wrefresh(contentWin);
            }
            continue;
        }

        // 只处理可打印字符
        if (isprint(ch))
        {
            char typedChar = static_cast<char>(ch);
            
            // 当启用Skip Space时，忽略用户输入的空格
            if (skipSpace && typedChar == ' ')
            {
                // 直接忽略，不做任何处理
                continue;
            }
            
            // 保存用户实际输入的字符
            char displayChar = typedChar;
            
            // 当启用忽略大小写时，进行大小写转换
            if (ignoreCase)
            {
                // 如果参考文本是小写但用户输入大写，将显示字符转为小写
                if (islower(practiceText[currentPos]) && isupper(typedChar))
                {
                    displayChar = tolower(typedChar);
                }
                // 如果参考文本是大写但用户输入小写，将显示字符转为大写
                else if (isupper(practiceText[currentPos]) && islower(typedChar))
                {
                    displayChar = toupper(typedChar);
                }
            }
            
            userInput += displayChar;

            // 检查输入是否正确，当忽略大小写时使用不区分大小写的比较
            bool correct = false;
            if (ignoreCase)
            {
                // 使用不区分大小写的比较
                correct = (tolower(typedChar) == tolower(practiceText[currentPos]));
            }
            else
            {
                // 使用区分大小写的比较
                correct = (typedChar == practiceText[currentPos]);
            }

            // 如果不正确，记录错误字符
            if (!correct)
            {
                sessionErrors[practiceText[currentPos]]++;
                statsManager->updateErrorChar(practiceText[currentPos]);
            }

            // 计算当前位置对应的行和列
            int posLine = 0;
            int posInLine = currentPos;

            for (size_t i = 0; i < textLines.size(); i++)
            {
                if (posInLine < textLines[i].length())
                {
                    posLine = i;
                    break;
                }
                posInLine -= textLines[i].length();
            }

            // 更新显示
            wmove(contentWin, 4 + posLine, 2 + posInLine);
            if (correct)
            {
                wattron(contentWin, COLOR_PAIR(COLOR_CORRECT));
            }
            else
            {
                wattron(contentWin, COLOR_PAIR(COLOR_INCORRECT));
            }
            // 显示可能经过大小写转换的字符
            waddch(contentWin, displayChar);

            if (correct)
            {
                wattroff(contentWin, COLOR_PAIR(COLOR_CORRECT));
            }
            else
            {
                wattroff(contentWin, COLOR_PAIR(COLOR_INCORRECT));
            }

            currentPos++;

            // 计算下一个位置
            posLine = 0;
            posInLine = currentPos;
            for (size_t i = 0; i < textLines.size(); i++)
            {
                if (posInLine < textLines[i].length())
                {
                    posLine = i;
                    break;
                }
                posInLine -= textLines[i].length();
            }

            // 高亮显示下一个字符(如果有)
            if (currentPos < practiceText.size())
            {
                wmove(contentWin, 4 + posLine, 2 + posInLine);
                wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));  
                waddch(contentWin, practiceText[currentPos]);
                wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));  
            }

            // 更新进度
            int progress = (currentPos * 100) / practiceText.size();
            updateStatusWindowWithHelp("Progress: " + to_string(progress) + "%", "ESC: Menu");

            wrefresh(contentWin);
        }
    }

    // 恢复光标
    curs_set(1);

    if (exitRequested)
    {
        return;
    }

    auto endTime = high_resolution_clock::now();
    double totalSeconds = duration_cast<milliseconds>(endTime - startTime).count() / 1000.0;
    double timeTaken = totalSeconds;

    // 计算统计信息
    int correctChars = 0;
    for (size_t i = 0; i < min(userInput.size(), practiceText.size()); ++i)
    {
        if (userInput[i] == practiceText[i])
        {
            correctChars++;
        }
    }

    double accuracy = (userInput.empty()) ? 0 : (correctChars * 100.0 / userInput.size());
    double wpm = (timeTaken > 0) ? (userInput.size() / 5.0) / (timeTaken / 60.0) : 0;

    // 更新统计数据
    statsManager->updateStatistics(timeTaken, userInput.size(), correctChars, wpm, accuracy);

    // 显示结果
    updateHeaderWindow("SESSION RESULTS");
    clearContentWindow();

    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "Time taken: %.1f seconds", timeTaken);
    mvwprintw(contentWin, 3, 2, "Characters typed: %zu", userInput.size());
    mvwprintw(contentWin, 4, 2, "Correct characters: %d", correctChars);
    mvwprintw(contentWin, 5, 2, "Accuracy: %.1f%%", accuracy);
    mvwprintw(contentWin, 6, 2, "Typing speed: %.1f WPM", wpm);

    // 显示本次练习常见错误
    if (!sessionErrors.empty())
    {
        mvwprintw(contentWin, 8, 2, "Most common errors:");

        // 按错误次数排序
        vector<pair<char, int>> sortedErrors(sessionErrors.begin(), sessionErrors.end());
        sort(sortedErrors.begin(), sortedErrors.end(),
             [](const pair<char, int> &a, const pair<char, int> &b)
             {
                 return a.second > b.second;
             });

        // 显示前5个最常见错误
        int row = 9;
        for (size_t i = 0; i < min(size_t(5), sortedErrors.size()); ++i)
        {
            mvwprintw(contentWin, row++, 2, "Character '%c': %d times",
                      sortedErrors[i].first, sortedErrors[i].second);
        }
    }

    // 改进结果分析
    if (wpm > 0)
    {
        mvwprintw(contentWin, 15, 2, "Performance Analysis:");

        string speedComment;
        if (wpm < 20)
        {
            speedComment = "Beginner level. Keep practicing!";
        }
        else if (wpm < 40)
        {
            speedComment = "Developing skills. Good progress!";
        }
        else if (wpm < 60)
        {
            speedComment = "Intermediate level. Well done!";
        }
        else if (wpm < 80)
        {
            speedComment = "Advanced level. Impressive!";
        }
        else
        {
            speedComment = "Expert level. Amazing speed!";
        }

        string accuracyComment;
        if (accuracy < 90)
        {
            accuracyComment = "Focus on accuracy over speed.";
        }
        else if (accuracy < 95)
        {
            accuracyComment = "Good accuracy. Keep refining.";
        }
        else if (accuracy < 98)
        {
            accuracyComment = "Excellent accuracy!";
        }
        else
        {
            accuracyComment = "Perfect! Maintain this precision.";
        }

        mvwprintw(contentWin, 16, 4, "%s", speedComment.c_str());
        mvwprintw(contentWin, 17, 4, "%s", accuracyComment.c_str());
    }

    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    // 询问是否要再次尝试相同的文本
    mvwprintw(contentWin, getmaxy(contentWin) - 4, 2, "R: Retry with same text");
    mvwprintw(contentWin, getmaxy(contentWin) - 3, 2, "Enter: Return to main menu");

    updateStatusWindowWithHelp("Session complete", "Press R to retry or Enter to continue");
    wrefresh(contentWin);

    // 等待用户选择
    while (true)
    {
        int ch = getch();
        if (ch == 'r' || ch == 'R')
        {
            practiceSession(practiceText);
            return;
        }
        else if (ch == KEY_RETURN || ch == '\r' || ch == KEY_ESC)
        {
            return;
        }
    }
}

// 显示统计信息
void showStatistics()
{
    updateHeaderWindow("STATISTICS");
    clearContentWindow();

    UserStats stats = statsManager->getStatistics();
    if (!statsManager->isStatsLoaded() || stats.sessionsCompleted == 0)
    {
        wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 添加粗体属性
        mvwprintw(contentWin, 2, 2, "No statistics available yet.");
        mvwprintw(contentWin, 3, 2, "Complete at least one practice session.");
        wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);  // 同样移除粗体属性
    }
    else
    {
        wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
        mvwprintw(contentWin, 2, 2, "Sessions completed: %d", stats.sessionsCompleted);
        mvwprintw(contentWin, 3, 2, "Total typing time: %.1f seconds", stats.totalTypingTime);
        mvwprintw(contentWin, 4, 2, "Average typing speed: %.1f WPM",
                  (stats.totalTypingTime > 0) ? (stats.totalCharsTyped / 5.0) / (stats.totalTypingTime / 60.0) : 0);
        mvwprintw(contentWin, 5, 2, "Overall accuracy: %.1f%%",
                  (stats.totalCharsTyped > 0) ? (stats.totalCorrectChars * 100.0 / stats.totalCharsTyped) : 0);
        mvwprintw(contentWin, 6, 2, "Best WPM: %.1f", stats.bestWPM);
        mvwprintw(contentWin, 7, 2, "Best accuracy: %.1f%%", stats.bestAccuracy);
        wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));

        // 显示常见错误
        if (!stats.errorChars.empty())
        {
            wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
            mvwprintw(contentWin, 9, 2, "Most common errors:");
            wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));

            // 按错误次数排序
            vector<pair<char, int>> sortedErrors(stats.errorChars.begin(), stats.errorChars.end());
            sort(sortedErrors.begin(), sortedErrors.end(),
                 [](const pair<char, int> &a, const pair<char, int> &b)
                 {
                     return a.second > b.second;
                 });

            // 显示前5个最常见错误 - 用默认颜色显示实际错误内容
            wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
            int row = 10;
            for (size_t i = 0; i < min(size_t(5), sortedErrors.size()); ++i)
            {
                mvwprintw(contentWin, row++, 2, "Character '%c': %d times",
                          sortedErrors[i].first, sortedErrors[i].second);
            }
            wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));
        }
    }

    updateStatusWindow("Press any key to return to the main menu...");
    wrefresh(contentWin);
    getch();
}

// 程序结束清理，替换为调用GUIHelper版本
void cleanup()
{
    GUIHelper::cleanup(headerWin, contentWin, statusWin);
}

// 显示确认对话框，替换为调用GUIHelper版本
bool showConfirmDialog(const string &message, const string &title)
{
    return GUIHelper::showConfirmDialog(headerWin, contentWin, statusWin, message, title);
}

// 显示消息对话框，替换为调用GUIHelper版本
void showMessageDialog(const string &message, DialogType type, const string &title)
{
    GUIHelper::showMessageDialog(headerWin, contentWin, statusWin, message, type, title);
}

// 更新状态栏，添加返回提示，替换为调用GUIHelper版本
void updateStatusWindowWithHelp(const string &status, const string &help)
{
    GUIHelper::updateStatusWindowWithHelp(statusWin, status, help);
}

// 程序主入口
int main()
{
// 设置本地化支持中文
#ifdef _WIN32
    // 启用Windows虚拟终端
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");
#endif

    // 初始化ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // 检查彩色支持
    if (!has_colors())
    {
        endwin();
        cout << "您的终端不支持彩色显示.\n";
        return 1;
    }

    // 初始化
    GUIHelper::initColors(); // 使用完全限定名称
    initWindows();

    // 禁用光标显示 - 全局设置
    curs_set(0);

    // 初始化用户管理器和统计管理器
    userManager = new UserManager(USER_DIR);
    statsManager = new StatisticsManager();
    corpusManager = new CorpusManager(CORPUS_DIR); // 初始化语料库管理器
    
    // 创建设置目录如果不存在
    #ifdef _WIN32
    if (!fs::exists(SETTINGS_DIR))
    {
        fs::create_directories(SETTINGS_DIR);
    }
    #endif
    
    // 初始化设置管理器
    settingsManager = new SettingsManager();

    // 显示用户选择界面
    if (!userManager->selectUser(headerWin, contentWin, statusWin))
    {
        // 如果用户取消了选择，退出程序
        cleanup();
        delete userManager;
        delete statsManager;
        delete corpusManager; // 释放语料库管理器
        delete settingsManager; // 释放设置管理器
        return 0;
    }

    // 加载当前用户的统计数据
    string statsPath = userManager->getCurrentUserStatsPath();
    if (!statsPath.empty())
    {
        statsManager->setStatsPath(statsPath);
        statsManager->loadStatistics();
    }
    
    // 加载当前用户的设置
    string settingsPath = SETTINGS_DIR + "\\" + userManager->getCurrentUser().username + ".settings";
    settingsManager->setSettingsPath(settingsPath);
    settingsManager->loadSettings();

    // 添加欢迎界面显示
    showWelcomeScreen();

    bool running = true;
    while (running)
    {
        // 显示主菜单并获取用户选择
        int choice = showMainMenu(true); // 显示用户选项

        switch (choice)
        {
        case 1:
            handleQuickStart(); // 修改此处为调用新的快速开始菜单处理函数
            break;
        case 2:
        {
            // 使用语料库管理器的选择函数
            Corpus selectedCorpus = corpusManager->selectCorpus(headerWin, contentWin, statusWin);
            if (!selectedCorpus.filename.empty())
            {
                // 使用选定的语料进行练习
                string fullText = corpusManager->readCorpusFile(selectedCorpus.filename);
                int wordCount = corpusManager->getWordCountForDifficulty(selectedCorpus.difficulty);
                string practiceSegment = corpusManager->extractPracticeSegment(fullText, wordCount);
                practiceSession(practiceSegment);
            }
            break;
        }
        case 3:
            // 使用语料库管理器的导入函数
            corpusManager->importCorpusFile(headerWin, contentWin, statusWin);
            break;
        case 4:
            showStatistics();
            break;
        case 5:
            // 显示设置菜单
            showSettingsMenu();
            break;
        case 6:
            // 切换用户
            if (userManager->switchUser(headerWin, contentWin, statusWin))
            {
                // 重新加载用户统计数据
                statsPath = userManager->getCurrentUserStatsPath();
                statsManager->resetStatistics();
                if (!statsPath.empty())
                {
                    statsManager->setStatsPath(statsPath);
                    statsManager->loadStatistics();
                }
                
                // 重新加载用户设置
                settingsPath = SETTINGS_DIR + "\\" + userManager->getCurrentUser().username + ".settings";
                settingsManager->setSettingsPath(settingsPath);
                settingsManager->loadSettings();
            }
            break;
        case 7:
            running = false;
            break;
        }
    }

    // 清理资源
    cleanup();
    delete userManager;
    delete statsManager;
    delete corpusManager; // 释放语料库管理器
    delete settingsManager; // 释放设置管理器
    return 0;
}
