#include <array>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <re2/re2.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#define select(x, y)                              \
    do                                            \
    {                                             \
        switch (toupper(x))                       \
        {                                         \
        case 'F':                                 \
        {                                         \
            (y) = &formula;                       \
            break;                                \
        }                                         \
        case 'T':                                 \
        {                                         \
            (y) = &table;                         \
            break;                                \
        }                                         \
        case 'P':                                 \
        {                                         \
            (y) = &picture;                       \
            break;                                \
        }                                         \
        default:                                  \
            throw std::runtime_error("语法错误"); \
        }                                         \
    } while (false)
class Caption
{
    class item
    {
        int title_order;
        int item_order;
        std::string name;

    public:
        item(int title_order, int item_order, std::string &&nm)
            : title_order(title_order), item_order(item_order),
              name(nm) {}
        item(int title_order, int item_order, const char *nm)
            : title_order(title_order), item_order(item_order),
              name(nm) {}
        void quote(std::string *str, const std::string &infix) const
        {
            *str += std::to_string(title_order) + infix + std::to_string(item_order);
        }
        void serial(std::string *str, const std::string &infix) const
        {
            quote(str, infix);
            *str += name;
        }
        bool operator==(const char *str) const
        {
            return name == str;
        }
        bool operator==(const std::string &str) const
        {
            return name == str;
        }
    };
    int title_order = 9;
    int item_order = 0;
    std::vector<item> stack;

    std::string style_prefix = R"(<center style="font-size:14px;color:#000000">)";
    std::string style_infix = ".";
    std::string style_suffix = R"(</center>)";
    std::string types;
    auto search(const std::string &name) const
    {
        auto i = stack.cbegin();
        for (; i != stack.cend(); i++)
        {
            if (*i == name)
                return i;
        }

        throw std::runtime_error("Cannot find the item");
        return i;
    }
    void serial(std::string *str, decltype(stack.cbegin()) s) const
    {
        *str += style_prefix + types;
        s->serial(str, style_infix);
        *str += style_suffix;
    }
    void quote(std::string *str, decltype(stack.cbegin()) s) const
    {
        *str += types;
        s->quote(str, style_infix);
    }

public:
    void set_prefix(const std::string &str)
    {
        style_prefix = str;
    }
    void set_infix(const std::string &str)
    {
        style_infix = str;
    }
    void set_suffix(const std::string &str)
    {
        style_suffix = str;
    }
    void set_prefix(const char *str)
    {
        style_prefix = str;
    }
    void set_infix(const char *str)
    {
        style_infix = str;
    }
    void set_suffix(const char *str)
    {
        style_suffix = str;
    }

    explicit Caption(std::string &&type_name) : types(std::move(type_name))
    {
    }
    explicit Caption(const char *type_name) : types(type_name)
    {
    }
    auto insert(const char *name)
    {
        stack.emplace_back(title_order, ++item_order, name);
    }
    auto insert(std::string &&name)
    {
        stack.emplace_back(title_order, ++item_order, std::move(name));
    }
    auto update()
    {
        ++title_order;
        item_order = 0;
    }
    auto reset()
    {
        title_order = 0;
        item_order = 0;
    }

    auto serial(std::string *str, const std::string &name) const
    {
        auto t = search(name);
        serial(str, t);
    }

    auto quote(std::string *str, const std::string &name) const
    {
        auto t = search(name);
        quote(str, t);
    }
};
class Line
{
    std::ifstream s;
    std::string buf;
    bool codeSegment = false;
    std::unordered_map<int, int> wrongOrder; //通过错误的序号访问正确的序号
    std::vector<int> rightOrder;             //通过正确的序号访问错误的序号

    static const RE2 checkCode;
    static const RE2 discover;
    static const RE2 quote;
    static const RE2 title;
    static const RE2 ref;
    static const RE2 tmpstatus;
    std::vector<int> indexstack;
    std::array<bool, 6> flag;
    int level = 0;
    friend std::ofstream &operator<<(std::ofstream &s, const Line &t);

public:
    Line(const std::string &path, const std::array<bool, 6> t) : s(path), flag(t)
    {
    }
    void getline()
    {
        std::getline(s, buf);
    }

    bool checkCodeSegment()
    {
        bool rc;
        if ((rc = re2::RE2::PartialMatch(buf, checkCode)))
        {
            codeSegment = !codeSegment;
        }
        return rc || codeSegment;
    }
    char findCaption(std::string *str) const
    {
        if (!re2::RE2::Extract(buf, discover, R"(\1)", str))
            return 0;
        char tmp = (*str)[0];
        str->erase(str->begin());
        return tmp;
    }
    auto findtitle() const
    {
        std::string tmp;
        if (!re2::RE2::Extract(buf, title, R"(\1)", &tmp))
            return 0UL;
        return tmp.size();
    }
    char findquote(std::string *str) const
    {
        if (!re2::RE2::Extract(buf, quote, R"(\1)", str))
            return 0;
        char tmp = (*str)[0];
        str->erase(str->begin());
        return tmp;
    }
    auto good() const
    {
        return s.good();
    }
    auto clear()
    {
        return s.clear();
    }
    void redo()
    {
        clear();
        s.seekg(0, std::ifstream::beg);
    }
    void replace(const std::string &str)
    {
        re2::RE2::Replace(&buf, quote, str);
    }
    void addNum(const std::string &numstring)
    {
        std::string regex(R"(\0)");
        regex += numstring;
        re2::RE2::Replace(&buf, title, regex);
    }
    void addNum(int level)
    {
        if (level > indexstack.size())
        {
            do
                indexstack.push_back(0);
            while (indexstack.size() != level);
        }
        else if (level < indexstack.size())
        {
            do
                indexstack.pop_back();
            while (indexstack.size() != level);
        }
        int idx = ++indexstack.back();
        if (flag[level - 1])
        {
            addNum(std::to_string(idx));
        }
    }
    void setflag(const std::array<bool, 6> &t)
    {
        flag = t;
    }
    void findReference()
    {
        std::string copy = buf;
        static int order = 0;
        std::string tmp;
        while (re2::RE2::Extract(copy, ref, R"(\1)", &tmp))
        {
            int idx = atoi(tmp.c_str()); //错误序号，正确序号
            if (wrongOrder.find(idx) == wrongOrder.end())
            {
                wrongOrder[idx] = ++order;
                rightOrder.push_back(idx);
            }
            re2::RE2::Replace(&copy, ref, "");
        }
    }
    void fixReference()
    {
        std::string tmp;
        if (!re2::RE2::Extract(buf, ref, R"(\1)", &tmp))
            return;
        do
        {
            int idx = atoi(tmp.c_str());
            tmp = ">@<" + std::to_string(wrongOrder[idx]) + ">@<";
            re2::RE2::Replace(&buf, ref, tmp);
        } while (re2::RE2::Extract(buf, ref, R"(\1)", &tmp));
        re2::RE2::GlobalReplace(&buf, tmpstatus, R"([\2])");
    }
};
const RE2 Line::checkCode(R"(^```)");
const RE2 Line::discover(R"(^\s*@@([FTP].{0,50}?)@@$)");
const RE2 Line::quote(R"(@@([ftp].{0,50}?)@@)");
const RE2 Line::ref(R"(\[(\d+)\])");
const RE2 Line::title(R"(^(#{1,6})\s)");
const RE2 Line::tmpstatus(R"((>@<(\d+)>@<))");

std::ofstream &operator<<(std::ofstream &s, const Line &t)
{
    s << t.buf;
    return s;
}

void process(const std::string &path, const std::string &newpath, const std::array<bool, 6> &flag, const int target)
{
    Caption picture("图");
    Caption table("表");
    Caption formula("公式");

    Line line(path, flag);

    while (line.good())
    {
        line.getline();
        if (line.checkCodeSegment())
            continue;
        line.findReference();
        int level = line.findtitle();
        if (level != 0)
        {
            if (level == target)
            {
                picture.update();
                table.update();
                formula.update();
            }
            else if (level < target)
            {
                picture.reset();
                table.reset();
                formula.reset();
            }
            continue;
        }
        std::string tmp;
        char rc = line.findCaption(&tmp);
        if (rc == 0)
            continue;
        Caption *pr;
        select(rc, pr);
        pr->insert(std::move(tmp));
    }
    line.redo();

    std::ofstream newfile(newpath, std::ofstream::out);
    while (line.good())
    {
        line.getline();
        line.fixReference();
        if (line.checkCodeSegment())
        {
            newfile << line << std::endl;
            continue;
        }
        int level;
        if ((level = line.findtitle()))
            line.addNum(level);
        std::string tmp;
        char rc = line.findCaption(&tmp);
        if (rc != 0)
        {
            std::string buf;
            Caption *pr;
            select(rc, pr);
            pr->serial(&buf, tmp);
            newfile << buf << std::endl;
            continue;
        }
        while ((rc = line.findquote(&tmp)) != 0)
        {
            std::string buf;
            Caption *pr;
            select(rc, pr);
            pr->quote(&buf, tmp);
            line.replace(buf);
        }
        newfile << line << std::endl;
    }
}

bool filecheck(const std::string &path)
{
    RE2 ExName(R"(\.md$)");
    return re2::RE2::PartialMatch(path, ExName);
}
void newfilepath(const std::string &path, std::string *newpath)
{
    *newpath = path;
    newpath->erase(newpath->end() - 2, newpath->end());
    *newpath += "ex.md";
}
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "本程序希望解决 MarkDown 排版时繁琐的交叉引用、图注添加，参考文献排序问题。-h 查看帮助。";
    }
    std::string path(argv[1]);
    if (!filecheck(path))
    {
        std::cerr << "输入错误！源文档无扩展名或扩展名不为md" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string newpath;
    newfilepath(path, &newpath);
    std::array<bool, 6> flag;
    flag[2] = true;
    flag[3] = true;
    flag[4] = true;
    process(path, newpath, flag, atoi(argv[2]));
}