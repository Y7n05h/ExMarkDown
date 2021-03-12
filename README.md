## ExMarkDown
![GitHub](https://img.shields.io/github/license/Y7n05h/ExMarkDown?style=flat-square)

本项目希望提供一种工具，简化在 MarkDown 中的交叉引用与参考文献排版时的困难。

### 项目依赖

- [re2](https://github.com/google/re2)

### 项目起因

Y7n05h 在撰写实验报告时苦于使用办公软件进行繁琐的文字编辑及排版工作，有由于 Tex 排版较为复杂，随即决定使用 CSS 渲染 MarkDown 生成 PDF 使其符合规定的样式。但因 MarkDown 无法解决图表题注的自动添加以及交叉引用问题，且人工完成该项工作十分繁琐，于是决定开发 ExMarkDown 扩充 MarkDown 部分语法。

### 项目原理

本项目不涉及任何有关 MarkDown 渲染与解析的相关技术，本项目通过自定语法，在 MarkDown 渲染前通过文字处理，实现图表题注、交叉引用、章节序号、脚注顺序的自动修正。


### LICENSE

本项目一切内容均使用 [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html) 发布。