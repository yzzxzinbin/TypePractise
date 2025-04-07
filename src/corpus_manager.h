#ifndef CORPUS_MANAGER_H
#define CORPUS_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <curses.h>
#include "gui_helper.h"

// 难度级别
enum DifficultyLevel
{
    EASY,
    MEDIUM,
    HARD,
    CUSTOM,
    IMPORTED, // 导入的文本
    CHI,      // 中文拼音练习
    CANCEL    // 取消选择
};

// 语料库结构
struct Corpus
{
    std::string name;
    std::string description;
    std::string filename;
    DifficultyLevel difficulty;
    bool isCustom = false;
};

class CorpusManager
{
private:
    std::vector<Corpus> corpusLibrary;
    std::string corpusDir;
    std::map<DifficultyLevel, int> difficultyWordCount;

    // 创建默认语料库
    void createDefaultCorpus();

public:
    CorpusManager(const std::string& corpusDirPath);
    ~CorpusManager();

    // 加载语料库
    void loadCorpusLibrary();
    
    // 读取语料文件内容
    std::string readCorpusFile(const std::string& filepath);
    
    // 从文本中选择一段用于练习
    std::string extractPracticeSegment(const std::string& fullText, int minWords);
    
    // 语料库选择界面
    Corpus selectCorpus(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    
    // 重命名语料
    void renameCorpus(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin, const Corpus& corpus);
    
    // 导入语料
    void importCorpusFile(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    
    // 删除语料
    void deleteCorpus(WINDOW* headerWin, WINDOW* contentWin, WINDOW* statusWin);
    
    // 获取语料库
    const std::vector<Corpus>& getCorpusLibrary() const { return corpusLibrary; }
    
    // 获取难度对应的词数
    int getWordCountForDifficulty(DifficultyLevel difficulty) const;
};

#endif // CORPUS_MANAGER_H
