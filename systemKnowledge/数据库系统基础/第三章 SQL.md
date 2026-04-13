# 连接查询
当一次查询操作涉及两个及以上的表时，称之为连接查询。
# 集合查询
SELECT的查询结果是元组的集合，因此对于SELECT的结果可以进行集合操作。包括：
- 并 UNION
- 交 INTERSECT
- 差 EXCEPT
集合运算前提：列数相同，且对应数据类型一致。
例：查询计算机科学与技术专业里年龄不大于19岁的学生
```SQL
SELECT *
FROM Student
WHERE Smajor = '计算机科学与技术'
UNION
SELECT *
FROM Student
WHERE (extract(year from current_date)-extract(year from Sbirthdate)) <=19
```
# 视图
视图是由一个或多个基本表导出的表。与基本表不同，是一个虚表。数据库只存储视图的定义，而不存放视图中的数据。
视图只是一个窗口，用来安全，直观地查看数据。
## 视图的定义
1. 建立视图
	SQL语言使用CREATE VIEW创建视图，语法如下：
```
	CREATE VIEW <视图名>[(<视图>[<视图>]...)]
	AS <子查询>
	[WITH CHECK OPTION]
```