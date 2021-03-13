## ExMarkDown
[![GitHub](https://img.shields.io/github/license/Y7n05h/ExMarkDown?style=flat-square)](https://www.gnu.org/licenses/gpl-3.0.html)

本项目希望提供一种工具，简化在 MarkDown 中的交叉引用与参考文献排版时的困难。

### 使用方式
- [ExMarkDown 语法说明](doc/syntax.md)

### 项目依赖

- [re2](https://github.com/google/re2)

### 编译说明
因项目结构过于简单，故暂不提供预编译版本与 `CMakeList.txt` 。
下载 [ExMarkDown.cpp](src/ExMarkDown.cpp)，请使用 `C++` 编译工具进行编译。
编译参数：`-lre2`、`-std=c++14`、`-O2`。

### 项目起因

Y7n05h 在撰写实验报告时苦于使用办公软件进行繁琐的文字编辑及排版工作，又由于 Tex 排版较为复杂，随即决定使用 CSS 渲染 MarkDown 生成 PDF 使其符合规定的样式。但因 MarkDown 无法解决图表题注的自动添加以及交叉引用问题，且人工完成该项工作十分繁琐，于是决定开发 ExMarkDown 扩充 MarkDown 部分语法。

### 项目原理

本项目不涉及任何有关 MarkDown 渲染与解析的相关技术，本项目通过自定语法，在 MarkDown 渲染前通过文字处理，实现图表题注、交叉引用、章节序号、脚注顺序的自动修正。

### 测试环境

```
OS：Arch Linux
Clang++：11.1.0
```

理论上，本程序具有良好的可移植性，但对此未作任何测试。对于在其他平台运行时出现的 Bugs，也欢迎使用者通过 [Issues](https://github.com/Y7n05h/ExMarkDown/issues) 与我们联系。
请谨慎使用，本项目所有贡献者对使用 ExMarkDown 造成的后果均不负责。
### LICENSE

本项目一切内容均使用 [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html) 发布。

欢迎通过 Pull request 与 Issuse 对本项目的代码、设计、功能等方面提出建议。
