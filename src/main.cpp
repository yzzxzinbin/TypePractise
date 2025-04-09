#include "type_practise.h"

using namespace GUIHelper; // 使用GUI辅助模块命名空间

// 全局变量的定义
const string CORPUS_DIR = "k:\\test program\\TypePractise\\corpus";
const string USER_DIR = "k:\\test program\\TypePractise\\users";
const string SETTINGS_DIR = "k:\\test program\\TypePractise\\settings"; // 添加设置目录
UserManager *userManager = nullptr;
StatisticsManager *statsManager = nullptr;
CorpusManager *corpusManager = nullptr;
SettingsManager *settingsManager = nullptr; // 添加设置管理器实例

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

// 随机生成练习文本，修改为从语料库中获取
string generatePracticeText(int wordCount)
{
    DifficultyLevel difficulty = MEDIUM; // 默认使用中等难度

    if (wordCount <= 0)
    {
        wordCount = corpusManager->getWordCountForDifficulty(difficulty);
    }

    vector<Corpus> matchingCorpus;
    for (const auto &corpus : corpusManager->getCorpusLibrary())
    {
        if (corpus.difficulty == difficulty)
        {
            matchingCorpus.push_back(corpus);
        }
    }

    if (matchingCorpus.empty())
    {
        return "The quick brown fox jumps over the lazy dog. ";
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, matchingCorpus.size() - 1);
    int randomIndex = distrib(gen);

    string fullText = corpusManager->readCorpusFile(matchingCorpus[randomIndex].filename);
    return corpusManager->extractPracticeSegment(fullText, wordCount);
}

// 更新统计数据
void updateStatistics(double timeTaken, int charsTyped, int correctChars, double wpm, double accuracy)
{
    statsManager->updateStatistics(timeTaken, charsTyped, correctChars, wpm, accuracy);
}

// 显示欢迎界面
void showWelcomeScreen()
{
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

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 2, 2, "Welcome to the typing tutor program!");
    mvwprintw(contentWin, 4, 2, "Instructions:");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);

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

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 2, 2, "Select difficulty level:");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);

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

    while (true)
    {
        int ch = getch();

        if (ch == KEY_ESC || ch == '0')
        {
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

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 2, 2, "Enter your custom text (max 255 characters):");
    mvwprintw(contentWin, 3, 2, "Leave empty and press Enter to cancel");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);

    updateStatusWindowWithHelp("Press Enter when done", "ESC to return without saving");
    wrefresh(contentWin);

    noecho();
    wmove(contentWin, 5, 2);
    wrefresh(contentWin);

    memset(input, 0, sizeof(input));
    int inputPos = 0;

    curs_set(1);
    keypad(contentWin, TRUE);

    bool inputDone = false;
    bool cancelled = false;

    while (!inputDone)
    {
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
            if (inputPos > 0)
            {
                inputPos--;
                input[inputPos] = '\0';

                wmove(contentWin, 5, 2 + inputPos);
                waddch(contentWin, ' ');
                wmove(contentWin, 5, 2 + inputPos);
                wrefresh(contentWin);
            }
        }
        else if (isprint(ch) && inputPos < 255)
        {
            input[inputPos++] = ch;
            waddch(contentWin, ch);
            wrefresh(contentWin);
        }
    }

    curs_set(0);
    keypad(stdscr, TRUE);

    if (cancelled)
    {
        return "";
    }

    customText = string(input);
    if (customText.empty())
    {
        return "";
    }

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

    int maxX = getmaxx(contentWin);
    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
    mvwprintw(contentWin, 1, maxX - strlen(PROGRAM_VERSION) - 2, "%s", PROGRAM_VERSION);
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));

    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "1. Quick Start");
    mvwprintw(contentWin, 3, 2, "2. Choose from Corpus Library");
    mvwprintw(contentWin, 4, 2, "3. Import New Text File");
    mvwprintw(contentWin, 5, 2, "4. View Statistics");
    mvwprintw(contentWin, 6, 2, "5. Settings");

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

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
    mvwprintw(contentWin, 10, 2, "F1: Help");
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));

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

    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);

    while (true)
    {
        int ch = getch();

        if (ch == KEY_F(1))
        {
            string helpText = "1-" + string(showUserOption ? "7" : "6") + ": Select menu option\n";
            helpText += "F1: Show this help\n";
            helpText += "ESC: Exit program from any screen\n";
            helpText += "This program helps you practice typing skills through various texts.";

            showMessageDialog(helpText, DIALOG_INFO, "Help");
            continue;
        }

        if (ch >= '1' && (showUserOption ? ch <= '7' : ch <= '6'))
        {
            return ch - '0';
        }

        if (ch == KEY_ESC)
        {
            if (showConfirmDialog("Are you sure you want to exit?", "Confirm Exit"))
            {
                return showUserOption ? 7 : 6;
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

    while (true)
    {
        int ch = getch();

        if (ch == KEY_ESC || ch == '0')
        {
            return 0;
        }
        else if (ch >= '1' && ch <= '3')
        {
            return ch - '0';
        }
    }
}

// 键位练习函数，修改为使用新的可视化键盘实现
void keyboardPractice()
{
    // 调用新的键盘练习实现
    startKeyboardPractice(headerWin, contentWin, statusWin);
}

// 智能文本练习 - 根据用户历史数据自动选择语料库中的文本
void smartTextPractice()
{
    DifficultyLevel difficulty = MEDIUM;

    UserStats stats = statsManager->getStatistics();

    if (statsManager->isStatsLoaded() && stats.sessionsCompleted > 0)
    {
        double avgWPM = (stats.totalTypingTime > 0) ? (stats.totalCharsTyped / 5.0) / (stats.totalTypingTime / 60.0) : 0;
        double accuracy = (stats.totalCharsTyped > 0) ? (stats.totalCorrectChars * 100.0 / stats.totalCharsTyped) : 0;

        if (avgWPM < 20 || accuracy < 85.0)
        {
            difficulty = EASY;
        }
        else if (avgWPM > 40 && accuracy > 95.0)
        {
            difficulty = HARD;
        }
    }

    vector<Corpus> matchingCorpus;
    for (const auto &corpus : corpusManager->getCorpusLibrary())
    {
        if (corpus.difficulty == difficulty)
        {
            matchingCorpus.push_back(corpus);
        }
    }

    if (matchingCorpus.empty())
    {
        for (const auto &corpus : corpusManager->getCorpusLibrary())
        {
            if (corpus.difficulty != CHI && corpus.difficulty != CANCEL && corpus.difficulty != CUSTOM)
            {
                matchingCorpus.push_back(corpus);
            }
        }
    }

    if (matchingCorpus.empty())
    {
        showMessageDialog("No appropriate text found in corpus library. Please import some text files first.",
                          DIALOG_INFO, "Information");
        return;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, matchingCorpus.size() - 1);
    int randomIndex = distrib(gen);

    string fullText = corpusManager->readCorpusFile(matchingCorpus[randomIndex].filename);
    int wordCount = corpusManager->getWordCountForDifficulty(difficulty);

    string practiceText;
    if (matchingCorpus[randomIndex].difficulty == CHI)
    {
        practiceText = corpusManager->extractChinesePinyinSegment(fullText, wordCount);
    }
    else
    {
        practiceText = corpusManager->extractPracticeSegment(fullText, wordCount);
    }

    practiceSession(practiceText);
}

// 处理快速开始菜单选择
void handleQuickStart()
{
    int choice = showQuickStartMenu();

    switch (choice)
    {
    case 0:
        return;
    case 1:
        keyboardPractice();
        break;
    case 2:
        smartTextPractice();
        break;
    case 3:
        practiceSession("");
        break;
    }
}

// 修改打字练习会话函数
void practiceSession(string practiceText)
{
    bool userCancelled = false;

    if (practiceText.empty())
    {
        DifficultyLevel difficulty = showDifficultyMenu();

        if (difficulty == CANCEL)
        {
            return;
        }
        else if (difficulty == CUSTOM)
        {
            practiceText = getCustomText();
            if (practiceText.empty())
            {
                userCancelled = true;
            }
        }
        else if (difficulty == CHI)
        {
            vector<Corpus> chineseCorpus;
            for (const auto &corpus : corpusManager->getCorpusLibrary())
            {
                if (corpus.difficulty == CHI)
                {
                    chineseCorpus.push_back(corpus);
                }
            }

            if (!chineseCorpus.empty())
            {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> distrib(0, chineseCorpus.size() - 1);
                int selectedIndex = distrib(gen);

                string fullText = corpusManager->readCorpusFile(chineseCorpus[selectedIndex].filename);
                int charCount = corpusManager->getWordCountForDifficulty(CHI);
                practiceText = corpusManager->extractChinesePinyinSegment(fullText, charCount);
            }
            else
            {
                showMessageDialog("No Chinese pinyin text found in corpus. Please import some Chinese pinyin files first.",
                                  DIALOG_INFO, "Information");
                return;
            }
        }
        else
        {
            vector<Corpus> matchingCorpus;
            for (const auto &corpus : corpusManager->getCorpusLibrary())
            {
                if (corpus.difficulty == difficulty)
                {
                    matchingCorpus.push_back(corpus);
                }
            }

            if (!matchingCorpus.empty())
            {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> distrib(0, matchingCorpus.size() - 1);
                int selectedIndex = distrib(gen);

                string fullText = corpusManager->readCorpusFile(matchingCorpus[selectedIndex].filename);
                int wordCount = corpusManager->getWordCountForDifficulty(difficulty);
                practiceText = corpusManager->extractPracticeSegment(fullText, wordCount);
            }
            else
            {
                int wordCount = corpusManager->getWordCountForDifficulty(difficulty);
                practiceText = generatePracticeText(wordCount);
            }
        }
    }

    if (userCancelled || practiceText.empty())
    {
        return;
    }

    updateHeaderWindow("TYPING PRACTICE");
    clearContentWindow();

    string userInput;
    int currentPos = 0;
    bool exitRequested = false;

    map<char, int> sessionErrors;

    auto startTime = high_resolution_clock::now();

    int maxCharsPerLine = getmaxx(contentWin) - 6;
    vector<string> textLines;

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
            int breakPos = maxCharsPerLine;
            while (breakPos > 0 && remainingText[breakPos] != ' ')
            {
                breakPos--;
            }

            if (breakPos == 0)
            {
                breakPos = maxCharsPerLine;
            }

            textLines.push_back(remainingText.substr(0, breakPos));
            remainingText = remainingText.substr(breakPos);
        }
    }

    wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
    mvwprintw(contentWin, 2, 2, "Type the following text:");

    bool skipSpace = settingsManager->getBoolSetting(SettingKeys::SKIP_SPACE, true);
    bool ignoreCase = settingsManager->getBoolSetting(SettingKeys::IGNORE_CASE, false);

    int line = 3;

    if (skipSpace)
    {
        mvwprintw(contentWin, line++, 2, "(Spaces will be skipped automatically)");
    }

    if (ignoreCase)
    {
        mvwprintw(contentWin, line++, 2, "(Case differences will be ignored)");
    }
    wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);

    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    for (size_t i = 0; i < textLines.size(); i++)
    {
        mvwprintw(contentWin, 4 + i, 2, "%s", textLines[i].c_str());
    }
    wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

    int currentLine = 0;
    wmove(contentWin, 4 + currentLine, 2);

    wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));
    waddch(contentWin, practiceText[0]);
    wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));

    updateStatusWindowWithHelp("Progress: 0%", "ESC: Return to Menu  F1: Help");

    wrefresh(contentWin);

    curs_set(0);

    while (currentPos < practiceText.size() && !exitRequested)
    {
        if (skipSpace && practiceText[currentPos] == ' ')
        {
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

            wmove(contentWin, 4 + posLine, 2 + posInLine);
            wattron(contentWin, COLOR_PAIR(COLOR_CORRECT));
            waddch(contentWin, ' ');
            wattroff(contentWin, COLOR_PAIR(COLOR_CORRECT));

            userInput += ' ';
            currentPos++;

            if (currentPos >= practiceText.size())
            {
                break;
            }

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

            wmove(contentWin, 4 + posLine, 2 + posInLine);
            wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));
            waddch(contentWin, practiceText[currentPos]);
            wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));

            int progress = (currentPos * 100) / practiceText.size();
            updateStatusWindowWithHelp("Progress: " + to_string(progress) + "%", "ESC: Menu");

            wrefresh(contentWin);
            continue;
        }

        int ch = getch();

        switch (ch)
        {
        case KEY_ESC:
            if (showConfirmDialog("Are you sure you want to exit?", "Confirm Exit"))
            {
                exitRequested = true;
            }
            continue;

        case KEY_F(1):
            showMessageDialog(
                "ESC: Return to menu\n"
                "Backspace: Delete last character\n"
                "Type the displayed text as fast and accurately as possible.",
                DIALOG_INFO, "Help");
            continue;
        }

        if (ch == KEY_BACKSPACE || ch == KEY_BACKSPACE_ALT || ch == 127)
        {
            if (currentPos > 0 && !userInput.empty())
            {
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

                wmove(contentWin, 4 + posLine, 2 + posInLine);
                wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
                waddch(contentWin, practiceText[currentPos]);
                wattroff(contentWin, COLOR_PAIR(COLOR_DEFAULT));

                userInput.pop_back();
                currentPos--;

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

                wmove(contentWin, 4 + posLine, 2 + posInLine);
                wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));
                waddch(contentWin, practiceText[currentPos]);
                wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));

                int progress = (currentPos * 100) / practiceText.size();
                updateStatusWindowWithHelp("Progress: " + to_string(progress) + "%", "ESC: Menu");

                wrefresh(contentWin);
            }
            continue;
        }

        if (isprint(ch))
        {
            char typedChar = static_cast<char>(ch);

            if (skipSpace && typedChar == ' ')
            {
                continue;
            }

            char displayChar = typedChar;

            if (ignoreCase)
            {
                if (islower(practiceText[currentPos]) && isupper(typedChar))
                {
                    displayChar = tolower(typedChar);
                }
                else if (isupper(practiceText[currentPos]) && islower(typedChar))
                {
                    displayChar = toupper(typedChar);
                }
            }

            userInput += displayChar;

            bool correct = false;
            if (ignoreCase)
            {
                correct = (tolower(typedChar) == tolower(practiceText[currentPos]));
            }
            else
            {
                correct = (typedChar == practiceText[currentPos]);
            }

            if (!correct)
            {
                sessionErrors[practiceText[currentPos]]++;
                statsManager->updateErrorChar(practiceText[currentPos]);
            }

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

            wmove(contentWin, 4 + posLine, 2 + posInLine);
            if (correct)
            {
                wattron(contentWin, COLOR_PAIR(COLOR_CORRECT));
            }
            else
            {
                wattron(contentWin, COLOR_PAIR(COLOR_INCORRECT));
            }
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

            if (currentPos < practiceText.size())
            {
                wmove(contentWin, 4 + posLine, 2 + posInLine);
                wattron(contentWin, COLOR_PAIR(COLOR_CURRENT));
                waddch(contentWin, practiceText[currentPos]);
                wattroff(contentWin, COLOR_PAIR(COLOR_CURRENT));
            }

            int progress = (currentPos * 100) / practiceText.size();
            updateStatusWindowWithHelp("Progress: " + to_string(progress) + "%", "ESC: Menu");

            wrefresh(contentWin);
        }
    }

    curs_set(1);

    if (exitRequested)
    {
        return;
    }

    auto endTime = high_resolution_clock::now();
    double totalSeconds = duration_cast<milliseconds>(endTime - startTime).count() / 1000.0;
    double timeTaken = totalSeconds;

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

    statsManager->updateStatistics(timeTaken, userInput.size(), correctChars, wpm, accuracy);

    updateHeaderWindow("SESSION RESULTS");
    clearContentWindow();

    wattron(contentWin, COLOR_PAIR(COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "Time taken: %.1f seconds", timeTaken);
    mvwprintw(contentWin, 3, 2, "Characters typed: %zu", userInput.size());
    mvwprintw(contentWin, 4, 2, "Correct characters: %d", correctChars);
    mvwprintw(contentWin, 5, 2, "Accuracy: %.1f%%", accuracy);
    mvwprintw(contentWin, 6, 2, "Typing speed: %.1f WPM", wpm);

    if (!sessionErrors.empty())
    {
        mvwprintw(contentWin, 8, 2, "Most common errors:");

        vector<pair<char, int>> sortedErrors(sessionErrors.begin(), sessionErrors.end());
        sort(sortedErrors.begin(), sortedErrors.end(),
             [](const pair<char, int> &a, const pair<char, int> &b)
             {
                 return a.second > b.second;
             });

        int row = 9;
        for (size_t i = 0; i < min(size_t(5), sortedErrors.size()); ++i)
        {
            mvwprintw(contentWin, row++, 2, "Character '%c': %d times",
                      sortedErrors[i].first, sortedErrors[i].second);
        }
    }

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

    mvwprintw(contentWin, getmaxy(contentWin) - 4, 2, "R: Retry with same text");
    mvwprintw(contentWin, getmaxy(contentWin) - 3, 2, "Enter: Return to main menu");

    updateStatusWindowWithHelp("Session complete", "Press R to retry or Enter to continue");
    wrefresh(contentWin);

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
        wattron(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
        mvwprintw(contentWin, 2, 2, "No statistics available yet.");
        mvwprintw(contentWin, 3, 2, "Complete at least one practice session.");
        wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE) | A_BOLD);
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

        if (!stats.errorChars.empty())
        {
            wattron(contentWin, COLOR_PAIR(COLOR_GUIDE));
            mvwprintw(contentWin, 9, 2, "Most common errors:");
            wattroff(contentWin, COLOR_PAIR(COLOR_GUIDE));

            vector<pair<char, int>> sortedErrors(stats.errorChars.begin(), stats.errorChars.end());
            sort(sortedErrors.begin(), sortedErrors.end(),
                 [](const pair<char, int> &a, const pair<char, int> &b)
                 {
                     return a.second > b.second;
                 });

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
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "zh_CN.UTF-8");
#endif

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    if (!has_colors())
    {
        endwin();
        cout << "您的终端不支持彩色显示.\n";
        return 1;
    }

    GUIHelper::initColors();
    initWindows();

    curs_set(0);

    userManager = new UserManager(USER_DIR);
    statsManager = new StatisticsManager();
    corpusManager = new CorpusManager(CORPUS_DIR);

#ifdef _WIN32
    if (!fs::exists(SETTINGS_DIR))
    {
        fs::create_directories(SETTINGS_DIR);
    }
#endif

    settingsManager = new SettingsManager();

    if (!userManager->selectUser(headerWin, contentWin, statusWin))
    {
        cleanup();
        delete userManager;
        delete statsManager;
        delete corpusManager;
        delete settingsManager;
        return 0;
    }

    string statsPath = userManager->getCurrentUserStatsPath();
    if (!statsPath.empty())
    {
        statsManager->setStatsPath(statsPath);
        statsManager->loadStatistics();
    }

    string settingsPath = SETTINGS_DIR + "\\" + userManager->getCurrentUser().username + ".settings";
    settingsManager->setSettingsPath(settingsPath);
    settingsManager->loadSettings();

    showWelcomeScreen();

    bool running = true;
    while (running)
    {
        int choice = showMainMenu(true);

        switch (choice)
        {
        case 1:
            handleQuickStart();
            break;
        case 2:
        {
            Corpus selectedCorpus = corpusManager->selectCorpus(headerWin, contentWin, statusWin);
            if (!selectedCorpus.filename.empty())
            {
                string fullText = corpusManager->readCorpusFile(selectedCorpus.filename);
                int wordCount = corpusManager->getWordCountForDifficulty(selectedCorpus.difficulty);
                string practiceSegment = corpusManager->extractPracticeSegment(fullText, wordCount);
                practiceSession(practiceSegment);
            }
            break;
        }
        case 3:
            corpusManager->importCorpusFile(headerWin, contentWin, statusWin);
            break;
        case 4:
            showStatistics();
            break;
        case 5:
            showSettingsMenu();
            break;
        case 6:
            if (userManager->switchUser(headerWin, contentWin, statusWin))
            {
                statsPath = userManager->getCurrentUserStatsPath();
                statsManager->resetStatistics();
                if (!statsPath.empty())
                {
                    statsManager->setStatsPath(statsPath);
                    statsManager->loadStatistics();
                }

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

    cleanup();
    delete userManager;
    delete statsManager;
    delete corpusManager;
    delete settingsManager;
    return 0;
}
