---
kind:
  - systemKnowledge
knowledgeKind:
  - computerScience
  - computerSystemBasic
---
`cat`全称concatenate，最常用的就是打印文件内容到终端。
**基本用法**：
```bash
# 打印单个文件
cat fileName
# 打印多个文件
cat fileName1 fileName2
```
**常见组合**：
```bash
# 内容太长时分页查看，按q退出
cat file | less
# 只显示包含xxx的行
cat file | grep xxx
# 显示行号
cat -n file
# 从终端输入内容写入文件，ctrl D结束
cat > file
# 把文件1内容追加到文件2末尾
cat fileName1 >> fileName2
```