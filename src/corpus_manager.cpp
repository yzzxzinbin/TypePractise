#include "corpus_manager.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>
#include <cstring>

namespace fs = std::filesystem;
using namespace std;
using namespace GUIHelper; // 添加这行以便于使用GUI辅助函数

// 构造函数
CorpusManager::CorpusManager(const string &corpusDirPath) : corpusDir(corpusDirPath)
{
    // 难度对应的文字数量
    difficultyWordCount = {
        {EASY, 10},
        {MEDIUM, 25},
        {HARD, 40},
        {CUSTOM, 15},
        {IMPORTED, 25}, // 导入文本的默认长度
        {CHI, 60}       // 中文拼音练习的默认长度
    };

    // 加载语料库
    loadCorpusLibrary();
}

// 析构函数
CorpusManager::~CorpusManager()
{
    // 无需特殊清理
}

// 获取难度对应的词数
int CorpusManager::getWordCountForDifficulty(DifficultyLevel difficulty) const
{
    auto it = difficultyWordCount.find(difficulty);
    if (it != difficultyWordCount.end())
    {
        return it->second;
    }
    return 15; // 默认值
}

// 加载语料库
void CorpusManager::loadCorpusLibrary()
{
    try
    {
        // 创建语料库目录（如果不存在）
        if (!fs::exists(corpusDir))
        {
            fs::create_directories(corpusDir);
            createDefaultCorpus();
        }

        // 清空现有语料库
        corpusLibrary.clear();

        // 遍历语料库目录并加载语料
        for (const auto &entry : fs::recursive_directory_iterator(corpusDir))
        {
            if (entry.is_regular_file())
            {
                // 只考虑txt文件
                if (entry.path().extension() != ".txt")
                {
                    continue;
                }

                Corpus corpus;
                string path = entry.path().string();
                string filename = entry.path().filename().string();

                // 获取父目录名称
                fs::path parent_path = entry.path().parent_path();
                string parentDir = parent_path.filename().string();

                corpus.filename = path;
                corpus.name = filename.substr(0, filename.find_last_of('.'));

                // 根据父目录确定难度
                if (parentDir == "easy")
                {
                    corpus.difficulty = EASY;
                    corpus.description = "Easy text for beginners";
                }
                else if (parentDir == "medium")
                {
                    corpus.difficulty = MEDIUM;
                    corpus.description = "Medium difficulty text";
                }
                else if (parentDir == "hard")
                {
                    corpus.difficulty = HARD;
                    corpus.description = "Hard text for advanced typists";
                }
                else if (parentDir == "custom")
                {
                    corpus.difficulty = CUSTOM;
                    corpus.isCustom = true;
                    corpus.description = "Custom imported text";
                }
                else if (parentDir == "chinese")
                {
                    corpus.difficulty = CHI;
                    corpus.description = "Chinese pinyin text";
                }
                else
                {
                    corpus.difficulty = IMPORTED;
                    corpus.isCustom = true;
                    corpus.description = "Imported from " + parentDir;
                }

                corpusLibrary.push_back(corpus);
            }
        }

        // 检查是否有语料库
        if (corpusLibrary.empty())
        {
            // 如果加载后仍为空，尝试创建默认语料
            createDefaultCorpus();
            // 重新加载
            loadCorpusLibrary();
        }
    }
    catch (const exception &e)
    {
        cerr << "Error loading corpus library: " << e.what() << endl;
    }
}

// 创建默认语料库
void CorpusManager::createDefaultCorpus()
{
    // 确保目录存在
    fs::create_directories(corpusDir + "\\easy");
    fs::create_directories(corpusDir + "\\medium");
    fs::create_directories(corpusDir + "\\hard");
    fs::create_directories(corpusDir + "\\custom");
    fs::create_directories(corpusDir + "\\chinese");

    // 创建默认语料样例
    ofstream easyFile(corpusDir + "\\easy\\sample.txt");
    if (easyFile.is_open())
    {
        easyFile << "This is a sample easy text for typing practice.\n";
        easyFile << "Short sentences with common words are easier to type.\n";
        easyFile << "Practice these sentences to improve your basic typing skills.";
        easyFile.close();
    }

    ofstream mediumFile(corpusDir + "\\medium\\sample.txt");
    if (mediumFile.is_open())
    {
        mediumFile << "Medium difficulty texts contain longer sentences and some uncommon words.\n";
        mediumFile << "They may include numbers (like 123) and special characters ($, %, &).\n";
        mediumFile << "Practice these to build upon your foundation of typing skills.";
        mediumFile.close();
    }

    ofstream hardFile(corpusDir + "\\hard\\sample.txt");
    if (hardFile.is_open())
    {
        hardFile << "Difficult texts contain complex vocabulary, technical terms, and various symbols.\n";
        hardFile << "They might include programming code or scientific terminology (e.g., C++, quantum physics).\n";
        hardFile << "These exercises are designed to challenge experienced typists and build advanced skills.";
        hardFile.close();
    }

    // 添加中文语料样例
    ofstream chineseFile(corpusDir + "\\chinese\\tech_chin.txt");
    if (chineseFile.is_open())
    {
        chineseFile << "ke ji zai jin shi nian zhong de fa zhan gai bian le ren lei sheng huo de fang shi\n";
        chineseFile << "cong hu lian wang dao ren gong zhi neng cong wu ren ji dao yi liao ke ji\n";
        chineseFile << "mei yi ci ji shu ge ming dou dai lai le geng shen ke de bian hua\n";
        chineseFile << "shou xian wang luo shi jie rang quan qiu bian de geng jia mi qie\n";
        chineseFile << "tong guo dian zi you jian she jiao mei ti ren men ke yi ji shi gou tong\n";
        chineseFile << "sui zhe 5G ji shu de pu ji shu ju chuan shu su du da fu ti sheng\n";
        chineseFile << "wei xu ni xian shi he zeng qiang xian shi ji shu ti gong le geng duo ke neng\n\n";

        chineseFile << "qi ci ren gong zhi neng zheng zai shen ru ge ge hang ye\n";
        chineseFile << "ji qi xue xi suan fa neng cong hai liang shu ju zhong ti qu you xiao xin xi\n";
        chineseFile << "zai yi liao ling yu AI ke yi bang zhu yi sheng zuo chu geng zhun que de zhen duan\n";
        chineseFile << "er zi dong jia shi ji shu ze you wang jiang jiao tong shi gu jiang di 90%\n";
        chineseFile << "tong shi sheng wu ke ji de jin bu ye ling ren xing fen\n";
        chineseFile << "ji yin bian ji ji shu CRISPR wei zhi liao ji bing ti gong le xin xi wang\n";
        chineseFile << "ren zao qi guan he gan xi bao zai sheng ji shu wei yi xue dai lai ge ming\n\n";

        chineseFile << "ling yi fang mian ke ji ye dai lai le tiao zhan\n";
        chineseFile << "shu ju yin si he wang luo an quan cheng wei quan qiu guan zhu de hua ti\n";
        chineseFile << "mei nian yin shu ju xie lou zao cheng de sun shi chao guo shi yi mei yuan\n";
        chineseFile << "xu ni bi quan he qu kuai lian ji shu sui ran dai lai jin rong chuang xin\n";
        chineseFile << "dan ye yin fa le guan yu jin rong wen ding xing de zheng lun\n\n";

        chineseFile << "zai wei lai ke ji jiang ji xu shen ru fa zhan\n";
        chineseFile << "wu lian wang shi dai de dao lai shi she bei jiang shi shi lian jie\n";
        chineseFile << "liu dian chi he ren gong tai yang deng xin neng yuan ji shu jiang jie jue huan jing wen ti\n";
        chineseFile << "hang tian ke ji rang ren lei you wang zai ben shi ji mo shang jian li yue qiu ji di\n\n";

        chineseFile << "ke ji de li liang wu xian da dan xu yao yu she hui lun li xiang jie he\n";
        chineseFile << "zhi you zai bao hu ren lei li yi de qian ti xia\n";
        chineseFile << "ke ji jin bu cai neng zhen zheng fu wu yu ren lei ming yun";
        chineseFile.close();
    }
}

// 读取语料文件内容
string CorpusManager::readCorpusFile(const string &filepath)
{
    ifstream file(filepath);
    string content, line;

    if (file)
    {
        while (getline(file, line))
        {
            // 替换制表符为空格
            replace(line.begin(), line.end(), '\t', ' ');

            // 移除多余空格
            string cleanLine;
            bool lastWasSpace = false;
            for (char c : line)
            {
                if (c == ' ')
                {
                    if (!lastWasSpace)
                    {
                        cleanLine += c;
                    }
                    lastWasSpace = true;
                }
                else
                {
                    cleanLine += c;
                    lastWasSpace = false;
                }
            }

            content += cleanLine + " ";
        }
    }

    // 确保内容不为空
    if (content.empty())
    {
        content = "This is a default text for practice. ";
    }

    return content;
}

// 从文本中选择一段用于练习
string CorpusManager::extractPracticeSegment(const string &fullText, int minWords)
{
    // 将文本分割成单词
    istringstream iss(fullText);
    vector<string> words;
    string word;

    while (iss >> word)
    {
        words.push_back(word);
    }

    if (words.size() <= minWords)
    {
        return fullText + " "; // 如果文本很短，直接返回全文
    }

    // 随机选择起始位置
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, max(0, static_cast<int>(words.size() - minWords)));
    int startPos = distrib(gen);

    // 提取文本片段
    string segment;
    for (int i = startPos; i < min(startPos + minWords, static_cast<int>(words.size())); ++i)
    {
        if (!segment.empty())
            segment += " ";
        segment += words[i];
    }

    return segment + " "; // 添加空格以便最后一个词能被正确检查
}

// 语料库选择界面
Corpus CorpusManager::selectCorpus(WINDOW *headerWin, WINDOW *contentWin, WINDOW *statusWin)
{
    if (corpusLibrary.empty())
    {
        loadCorpusLibrary();
    }

    // 按难度排序
    vector<Corpus> sortedCorpus = corpusLibrary;
    sort(sortedCorpus.begin(), sortedCorpus.end(), [](const Corpus &a, const Corpus &b)
         { return a.difficulty < b.difficulty; });

    if (sortedCorpus.empty())
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "No corpus files found. Please import some text files first.", GUIHelper::DIALOG_INFO, "Corpus Library");

        // 返回一个空语料体表示取消
        return Corpus();
    }

    int page = 0;
    int itemsPerPage = getmaxy(contentWin) - 10; // 留出更多空间
    int totalPages = (sortedCorpus.size() + itemsPerPage - 1) / itemsPerPage;

    int selectedIndex = 0;
    bool selectionMade = false;

    // 更新标题窗口
    GUIHelper::updateHeaderWindow(headerWin, "CORPUS LIBRARY");

    while (!selectionMade)
    {
        // 清除内容窗口
        GUIHelper::clearContentWindow(contentWin);

        wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
        mvwprintw(contentWin, 1, 2, "Select a text to practice (Page %d/%d):", page + 1, totalPages);

        // 显示当前页的语料
        int startIdx = page * itemsPerPage;
        int endIdx = min(startIdx + itemsPerPage, static_cast<int>(sortedCorpus.size()));

        for (int i = startIdx; i < endIdx; i++)
        {
            const auto &corpus = sortedCorpus[i];
            string difficultyText;

            switch (corpus.difficulty)
            {
            case EASY:
                difficultyText = "[Easy]";
                break;
            case MEDIUM:
                difficultyText = "[Medium]";
                break;
            case HARD:
                difficultyText = "[Hard]";
                break;
            case CUSTOM:
                difficultyText = "[Custom]";
                break;
            case IMPORTED:
                difficultyText = "[Imported]";
                break;
            case CHI:
                difficultyText = "[Chinese]";
                break;
            }

            // 高亮显示当前选中项
            if (i == selectedIndex)
            {
                wattron(contentWin, A_REVERSE);
            }

            mvwprintw(contentWin, 3 + (i - startIdx), 2, "%d. %s %s - %s",
                      i + 1, difficultyText.c_str(), corpus.name.c_str(), corpus.description.c_str());

            if (i == selectedIndex)
            {
                wattroff(contentWin, A_REVERSE);
            }
        }

        // 底部导航提示，添加删除和重命名选项
        mvwprintw(contentWin, getmaxy(contentWin) - 5, 2, "UP/DOWN: Move selection   PgUp/PgDn: Change page");
        mvwprintw(contentWin, getmaxy(contentWin) - 4, 2, "Enter: Select   D: Delete   R: Rename");
        mvwprintw(contentWin, getmaxy(contentWin) - 3, 2, "ESC: Return to menu");
        wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));

        GUIHelper::updateStatusWindowWithHelp(statusWin, "Select a corpus", "ESC to cancel");
        wrefresh(contentWin);
        wrefresh(statusWin);

        int ch = wgetch(contentWin);
        switch (ch)
        {
        case KEY_UP:
            if (selectedIndex > 0)
            {
                selectedIndex--;
                // 如果选择移出当前页，切换到上一页
                if (selectedIndex < startIdx)
                {
                    page--;
                }
            }
            break;

        case KEY_DOWN:
            if (selectedIndex < sortedCorpus.size() - 1)
            {
                selectedIndex++;
                // 如果选择移出当前页，切换到下一页
                if (selectedIndex >= endIdx)
                {
                    page++;
                }
            }
            break;

        case KEY_PPAGE: // Page Up
            if (page > 0)
            {
                page--;
                selectedIndex = page * itemsPerPage;
            }
            break;

        case KEY_NPAGE: // Page Down
            if (page < totalPages - 1)
            {
                page++;
                selectedIndex = page * itemsPerPage;
            }
            break;

        case '\r':
        case '\n':
        case KEY_ENTER:
            if (selectedIndex < sortedCorpus.size())
            {
                selectionMade = true;
            }
            break;

        case 'd':
        case 'D':
            if (selectedIndex < sortedCorpus.size())
            {
                // 询问用户确认删除
                Corpus selected = sortedCorpus[selectedIndex];
                deleteCorpus(headerWin, contentWin, statusWin);

                // 重新加载语料库
                loadCorpusLibrary();

                // 更新排序后的列表
                sortedCorpus = corpusLibrary;
                sort(sortedCorpus.begin(), sortedCorpus.end(), [](const Corpus &a, const Corpus &b)
                     { return a.difficulty < b.difficulty; });

                // 更新页面信息
                totalPages = (sortedCorpus.size() + itemsPerPage - 1) / itemsPerPage;
                if (sortedCorpus.empty())
                {
                    return Corpus(); // 如果删除后没有语料，返回空
                }

                // 调整选中索引
                if (selectedIndex >= sortedCorpus.size())
                {
                    selectedIndex = max(0, static_cast<int>(sortedCorpus.size()) - 1);
                }

                page = min(page, totalPages - 1);
            }
            break;

        case 'r':
        case 'R':
            if (selectedIndex < sortedCorpus.size())
            {
                // 调用重命名函数
                renameCorpus(headerWin, contentWin, statusWin, sortedCorpus[selectedIndex]);

                // 重新加载语料库
                loadCorpusLibrary();

                // 刷新排序列表
                sortedCorpus = corpusLibrary;
                sort(sortedCorpus.begin(), sortedCorpus.end(), [](const Corpus &a, const Corpus &b)
                     { return a.difficulty < b.difficulty; });
            }
            break;

        case 27: // ESC
        case 'q':
        case 'Q':
            // 返回一个空语料标记取消
            return Corpus();

        default:
            // 允许通过数字键快速选择
            if (ch >= '1' && ch <= '9')
            {
                int index = ch - '1' + startIdx;
                if (index < endIdx)
                {
                    selectedIndex = index;
                    selectionMade = true;
                }
            }
            break;
        }
    }

    return sortedCorpus[selectedIndex];
}

// 重命名语料
void CorpusManager::renameCorpus(WINDOW *headerWin, WINDOW *contentWin, WINDOW *statusWin, const Corpus &corpus)
{
    // 更新标题窗口
    GUIHelper::updateHeaderWindow(headerWin, "RENAME CORPUS FILE");
    GUIHelper::clearContentWindow(contentWin);

    wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "Current file name: %s", corpus.name.c_str());
    mvwprintw(contentWin, 4, 2, "Enter new name for the corpus file:");
    mvwprintw(contentWin, 5, 2, "(Leave empty and press Enter to cancel)");
    wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));

    // 更新状态窗口
    GUIHelper::updateStatusWindowWithHelp(statusWin, "Enter new name", "ESC to cancel");
    wrefresh(contentWin);
    wrefresh(statusWin);

    // 输入框设置
    char newName[256] = {0};
    wmove(contentWin, 7, 2);
    wrefresh(contentWin);

    // 禁用全局回显
    noecho();

    // 清空输入缓冲区
    memset(newName, 0, sizeof(newName));
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

        if (ch == 27)
        { // ESC键
            cancelled = true;
            inputDone = true;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER)
        {
            inputDone = true;
        }
        else if (ch == KEY_BACKSPACE || ch == 8 || ch == 127)
        {
            // 处理退格键
            if (inputPos > 0)
            {
                inputPos--;
                newName[inputPos] = '\0';

                // 更新显示 - 使用空格覆盖
                wmove(contentWin, 7, 2 + inputPos);
                waddch(contentWin, ' ');
                wmove(contentWin, 7, 2 + inputPos);
                wrefresh(contentWin);
            }
        }
        else if (isprint(ch) && inputPos < 255)
        {
            // 过滤掉文件名中的非法字符
            if (ch != '\\' && ch != '/' && ch != ':' && ch != '*' &&
                ch != '?' && ch != '"' && ch != '<' && ch != '>' && ch != '|')
            {
                // 处理可打印字符
                newName[inputPos++] = ch;
                // 只在内容窗口添加字符
                waddch(contentWin, ch);
                wrefresh(contentWin);
            }
        }
    }

    // 隐藏光标
    curs_set(0);

    // 重置窗口键盘模式
    keypad(stdscr, TRUE);

    if (cancelled || strlen(newName) == 0)
    {
        // 更新状态窗口
        GUIHelper::updateStatusWindow(statusWin, "Rename canceled");

        napms(1000);
        return;
    }

    // 构建新文件路径，保持同一目录
    fs::path oldPath(corpus.filename);
    fs::path directory = oldPath.parent_path();
    fs::path extension = oldPath.extension();
    fs::path newPath = directory / (string(newName) + extension.string());

    // 检查新名称文件是否已存在
    if (fs::exists(newPath))
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "A file with this name already exists. Please choose a different name.", GUIHelper::DIALOG_ERROR, "Error");
        return;
    }

    // 询问是否确认重命名
    string confirmMessage = "Rename '" + corpus.name + "' to '" + string(newName) + "'?";
    if (!GUIHelper::showConfirmDialog(headerWin, contentWin, statusWin, confirmMessage, "Confirm Rename"))
    {
        // 更新状态窗口
        GUIHelper::updateStatusWindow(statusWin, "Rename canceled");

        napms(1000);
        return;
    }

    // 执行文件重命名
    try
    {
        fs::rename(oldPath, newPath);
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "File renamed successfully!", GUIHelper::DIALOG_SUCCESS, "Success");
    }
    catch (const fs::filesystem_error &e)
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     string("Error renaming file: ") + e.what(), GUIHelper::DIALOG_ERROR, "Error");
    }
}

// 导入语料
void CorpusManager::importCorpusFile(WINDOW *headerWin, WINDOW *contentWin, WINDOW *statusWin)
{
    // 更新标题窗口
    GUIHelper::updateHeaderWindow(headerWin, "IMPORT TEXT FILE");
    GUIHelper::clearContentWindow(contentWin);

    wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "Enter the full path to the text file:");
    mvwprintw(contentWin, 3, 2, "(Leave empty and press Enter to cancel)");
    wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));

    // 更新状态窗口
    GUIHelper::updateStatusWindowWithHelp(statusWin, "Enter file path", "ESC to cancel");

    wrefresh(contentWin);
    wrefresh(statusWin);

    // 输入框
    char filepath[256] = {0};
    wmove(contentWin, 5, 2);

    // 捕获ESC键
    nodelay(stdscr, TRUE);
    int ch;
    bool inputDone = false;
    bool cancelled = false;
    int inputPos = 0;

    curs_set(1); // 显示光标

    while (!inputDone)
    {
        ch = getch();

        if (ch == ERR)
        {
            napms(50);
            continue;
        }

        if (ch == 27)
        { // ESC键
            cancelled = true;
            inputDone = true;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER)
        {
            inputDone = true;
        }
        else if (ch == KEY_BACKSPACE || ch == 8 || ch == 127)
        {
            if (inputPos > 0)
            {
                inputPos--;
                filepath[inputPos] = '\0';
                wmove(contentWin, 5, 2 + inputPos);
                waddch(contentWin, ' ');
                wmove(contentWin, 5, 2 + inputPos);
                wrefresh(contentWin);
            }
        }
        else if (isprint(ch) && inputPos < 255)
        {
            filepath[inputPos++] = ch;
            waddch(contentWin, ch);
            wrefresh(contentWin);
        }
    }

    nodelay(stdscr, FALSE);
    curs_set(0);
    noecho();

    if (cancelled || strlen(filepath) == 0)
    {
        // 更新状态窗口
        GUIHelper::updateStatusWindow(statusWin, "Import canceled");

        napms(1000);
        return;
    }

    // 检查文件是否存在
    if (!fs::exists(filepath))
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "File not found: " + string(filepath), GUIHelper::DIALOG_ERROR, "Error");
        return;
    }

    // 获取文件名
    string filename = fs::path(filepath).filename().string();

    // 询问难度级别
    mvwprintw(contentWin, 7, 2, "Select difficulty level for this text:");
    mvwprintw(contentWin, 9, 2, "1. Easy");
    mvwprintw(contentWin, 10, 2, "2. Medium");
    mvwprintw(contentWin, 11, 2, "3. Hard");
    mvwprintw(contentWin, 13, 2, "0. Cancel import");

    // 更新状态窗口
    GUIHelper::updateStatusWindowWithHelp(statusWin, "Enter your choice (0-3):", "ESC to cancel");

    wrefresh(contentWin);
    wrefresh(statusWin);

    // 等待有效选择
    while (true)
    {
        ch = getch();

        if (ch == 27 || ch == '0')
        { // ESC或0键
            // 更新状态窗口
            GUIHelper::updateStatusWindow(statusWin, "Import canceled");

            napms(1000);
            return;
        }

        if (ch >= '1' && ch <= '3')
        {
            break;
        }
    }

    int choice = ch - '0';
    string targetDir;
    switch (choice)
    {
    case 1:
        targetDir = "\\easy\\";
        break;
    case 2:
        targetDir = "\\medium\\";
        break;
    case 3:
        targetDir = "\\hard\\";
        break;
    default:
        return; // 不应该到达这里
    }

    // 询问是否确认导入
    if (!GUIHelper::showConfirmDialog(headerWin, contentWin, statusWin,
                                      "Import file '" + filename + "'?", "Confirm Import"))
    {
        // 更新状态窗口
        GUIHelper::updateStatusWindow(statusWin, "Import canceled");

        napms(1000);
        return;
    }

    // 复制文件到语料库
    string destPath = corpusDir + targetDir + filename;
    try
    {
        fs::copy_file(filepath, destPath, fs::copy_options::overwrite_existing);

        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "File imported successfully!", GUIHelper::DIALOG_SUCCESS, "Success");

        // 重新加载语料库
        loadCorpusLibrary();
    }
    catch (const fs::filesystem_error &e)
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     string("Error importing file: ") + e.what(), GUIHelper::DIALOG_ERROR, "Error");
    }
}

// 删除语料
void CorpusManager::deleteCorpus(WINDOW *headerWin, WINDOW *contentWin, WINDOW *statusWin)
{
    if (corpusLibrary.empty())
    {
        loadCorpusLibrary();
    }

    if (corpusLibrary.empty())
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     "No corpus files found to delete.", GUIHelper::DIALOG_INFO, "Corpus Library");
        return;
    }

    // 更新标题窗口
    GUIHelper::updateHeaderWindow(headerWin, "DELETE CORPUS FILE");
    GUIHelper::clearContentWindow(contentWin);

    wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
    mvwprintw(contentWin, 2, 2, "Select a corpus file to delete:");
    wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
    wrefresh(contentWin);

    // 按难度排序
    vector<Corpus> sortedCorpus = corpusLibrary;
    sort(sortedCorpus.begin(), sortedCorpus.end(), [](const Corpus &a, const Corpus &b)
         { return a.difficulty < b.difficulty; });

    int page = 0;
    int itemsPerPage = getmaxy(contentWin) - 10;
    int totalPages = (sortedCorpus.size() + itemsPerPage - 1) / itemsPerPage;

    int selectedIndex = 0;
    bool selectionMade = false;

    while (!selectionMade)
    {
        // 清除内容窗口
        GUIHelper::clearContentWindow(contentWin);

        wattron(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
        mvwprintw(contentWin, 2, 2, "Select a corpus file to delete (Page %d/%d):", page + 1, totalPages);

        // 显示当前页的语料
        int startIdx = page * itemsPerPage;
        int endIdx = min(startIdx + itemsPerPage, static_cast<int>(sortedCorpus.size()));

        for (int i = startIdx; i < endIdx; i++)
        {
            const auto &corpus = sortedCorpus[i];
            string difficultyText;

            switch (corpus.difficulty)
            {
            case EASY:
                difficultyText = "[Easy]";
                break;
            case MEDIUM:
                difficultyText = "[Medium]";
                break;
            case HARD:
                difficultyText = "[Hard]";
                break;
            case CUSTOM:
                difficultyText = "[Custom]";
                break;
            case IMPORTED:
                difficultyText = "[Imported]";
                break;
            case CHI:
                difficultyText = "[Chinese]";
                break;
            }

            // 高亮显示当前选中项
            if (i == selectedIndex)
            {
                wattron(contentWin, A_REVERSE);
            }

            mvwprintw(contentWin, 4 + (i - startIdx), 2, "%d. %s %s - %s",
                      i + 1, difficultyText.c_str(), corpus.name.c_str(), corpus.description.c_str());

            if (i == selectedIndex)
            {
                wattroff(contentWin, A_REVERSE);
            }
        }

        // 底部导航提示
        mvwprintw(contentWin, getmaxy(contentWin) - 4, 2, "UP/DOWN: Move selection   PgUp/PgDn: Change page");
        mvwprintw(contentWin, getmaxy(contentWin) - 3, 2, "Enter: Select to delete   ESC: Return to menu");
        wattroff(contentWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));

        GUIHelper::updateStatusWindowWithHelp(statusWin, "Select a corpus to delete", "ESC to cancel");
        wrefresh(contentWin);
        wrefresh(statusWin);

        int ch = wgetch(contentWin);
        switch (ch)
        {
        case KEY_UP:
            if (selectedIndex > 0)
            {
                selectedIndex--;
                // 如果选择移出当前页，切换到上一页
                if (selectedIndex < startIdx)
                {
                    page--;
                }
            }
            break;

        case KEY_DOWN:
            if (selectedIndex < sortedCorpus.size() - 1)
            {
                selectedIndex++;
                // 如果选择移出当前页，切换到下一页
                if (selectedIndex >= endIdx)
                {
                    page++;
                }
            }
            break;

        case KEY_PPAGE: // Page Up
            if (page > 0)
            {
                page--;
                selectedIndex = page * itemsPerPage;
            }
            break;

        case KEY_NPAGE: // Page Down
            if (page < totalPages - 1)
            {
                page++;
                selectedIndex = page * itemsPerPage;
            }
            break;

        case '\r':
        case '\n':
        case KEY_ENTER:
            if (selectedIndex < sortedCorpus.size())
            {
                selectionMade = true;
            }
            break;

        case 27: // ESC键
        case 'q':
        case 'Q':
            return; // 取消删除操作

        default:
            // 允许通过数字键快速选择
            if (ch >= '1' && ch <= '9')
            {
                int index = ch - '1' + startIdx;
                if (index < endIdx)
                {
                    selectedIndex = index;
                    selectionMade = true;
                }
            }
            break;
        }
    }

    // 获取选中的语料库
    Corpus selectedCorpus = sortedCorpus[selectedIndex];

    // 询问用户确认删除
    string confirmMessage = "Are you sure you want to delete '" + selectedCorpus.name + "'?";
    if (!GUIHelper::showConfirmDialog(headerWin, contentWin, statusWin, confirmMessage, "Confirm Delete"))
    {
        return;
    }

    // 执行文件删除
    try
    {
        if (fs::remove(selectedCorpus.filename))
        {
            GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                         "Corpus file deleted successfully!", GUIHelper::DIALOG_SUCCESS, "Success");
            // 重新加载语料库以更新列表
            loadCorpusLibrary();
        }
        else
        {
            GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                         "Failed to delete the corpus file.", GUIHelper::DIALOG_ERROR, "Error");
        }
    }
    catch (const fs::filesystem_error &e)
    {
        GUIHelper::showMessageDialog(headerWin, contentWin, statusWin,
                                     string("Error deleting file: ") + e.what(), GUIHelper::DIALOG_ERROR, "Error");
    }
}

// 显示确认对话框
bool showConfirmDialog(WINDOW *headerWin, WINDOW *contentWin, WINDOW *statusWin,
                       const string &message, const string &title)
{
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // 计算对话框大小和位置
    int dialogWidth = min(60, maxX - 10);
    int dialogHeight = 7;
    int startY = (maxY - dialogHeight) / 2;
    int startX = (maxX - dialogWidth) / 2;

    // 创建对话框窗口
    WINDOW *dialogWin = newwin(dialogHeight, dialogWidth, startY, startX);
    GUIHelper::drawBox(dialogWin); // 使用GUIHelper::drawBox

    // 显示标题
    wattron(dialogWin, COLOR_PAIR(GUIHelper::COLOR_HEADER));
    mvwprintw(dialogWin, 1, (dialogWidth - title.length()) / 2, "%s", title.c_str());
    wattroff(dialogWin, COLOR_PAIR(GUIHelper::COLOR_HEADER));

    // 显示消息和按钮
    wattron(dialogWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
    mvwprintw(dialogWin, 3, 2, "%s", message.c_str());
    mvwprintw(dialogWin, 5, dialogWidth / 3 - 4, "[Y] Confirm");
    mvwprintw(dialogWin, 5, 2 * dialogWidth / 3 - 4, "[N] Cancel");
    wattroff(dialogWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));

    wrefresh(dialogWin);

    // 获取用户响应
    bool result = false;
    bool responded = false;

    while (!responded)
    {
        int ch = wgetch(dialogWin);
        switch (tolower(ch))
        {
        case 'y':
            result = true;
            responded = true;
            break;
        case 'n':
        case GUIHelper::KEY_ESC:
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
void showMessageDialog(WINDOW *headerWin, WINDOW *contentWin, WINDOW *statusWin,
                       const string &message, int type, const string &title)
{
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // 计算对话框大小和位置
    int dialogWidth = min(60, maxX - 10);
    int dialogHeight = 10;
    int startY = (maxY - dialogHeight) / 2;
    int startX = (maxX - dialogWidth) / 2;

    // 创建对话框窗口
    WINDOW *dialogWin = newwin(dialogHeight, dialogWidth, startY, startX);
    GUIHelper::drawBox(dialogWin); // 使用GUIHelper::drawBox

    // 根据对话框类型设置颜色
    int colorPair;
    switch (type)
    {
    case 2: // ERROR
        colorPair = GUIHelper::COLOR_INCORRECT;
        break;
    case 1: // WARNING
        colorPair = GUIHelper::COLOR_INCORRECT;
        break;
    default:
        colorPair = GUIHelper::COLOR_HEADER;
    }

    // 显示标题
    wattron(dialogWin, COLOR_PAIR(colorPair));
    mvwprintw(dialogWin, 1, (dialogWidth - title.length()) / 2, "%s", title.c_str());
    wattroff(dialogWin, COLOR_PAIR(colorPair));

    // 显示消息 - 支持多行消息
    wattron(dialogWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));
    size_t pos = 0, lineStart = 0;
    int line = 0;

    while (pos < message.length())
    {
        if (message[pos] == '\n' || pos - lineStart >= dialogWidth - 4)
        {
            mvwprintw(dialogWin, 3 + line, 2, "%s", message.substr(lineStart, pos - lineStart).c_str());
            lineStart = message[pos] == '\n' ? pos + 1 : pos;
            line++;
        }
        pos++;
    }

    // 最后一行
    if (lineStart < message.length())
    {
        mvwprintw(dialogWin, 3 + line, 2, "%s", message.substr(lineStart).c_str());
    }

    // 显示按钮
    mvwprintw(dialogWin, dialogHeight - 2, (dialogWidth - 15) / 2, "Press any key");
    wattroff(dialogWin, COLOR_PAIR(GUIHelper::COLOR_DEFAULT));

    wrefresh(dialogWin);

    // 等待用户按键
    wgetch(dialogWin);

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
