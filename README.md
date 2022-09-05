# Paper
Papers related to cryptography



本仓库为马昌社老师研究方向的相关论文和代码资源，请各位同门自觉维护仓库。

建议每个大类的论文都按时间顺序学习。

## SSE

学习SSE的入门流程：

- 2006年karama 首次提出静态SSE
- 2016年bost 提出满足前向安全的动态SSE
- 2017年bost 提出满足前后向安全的动态SSE

以上三篇论文是SSE的主要框架来源，其他相关方案可自行在文件夹里面选择阅读。



## 泄漏

- Access pattern
- Search pattern
- Volume hiding
- Co-occurence pattern

泄漏的简单介绍：Leakage Perturbation is Not Enough: Breaking Structured Encryption Using Simulated Annealing。



## 相关技术

### Differential privacy

差分隐私是一个解决隐私问题的工具

差分隐私的书可以只看前几章节，了解几个DP机制就可以。

经典STE与DP结合的论文，建议精读：Encrypted Databases for Differential Privacy

### ORAM

ORAM完美的解决了access pattern的问题。

入门论文：

- 平方根：Software Protection and Simulation on Oblivious RAMs（1996）

- path-oram：Path ORAM:An Extremely Simple Oblivious RAM Protocol

在文件夹里有oram发展的完整流程。



## 范围查询 

范围查询是一种常用的查询类别，围绕如何提高范围查询效率有非常多的流派。下面简单介绍：

- ORE 
- BRC
- SSE+RangeQuery
  -  DEMERTZIS是首次提出SSE和范围查询结合的，所以可以先看Practical Private Range Search in Depth。
  - 后续由DEMERTZIS为基础再做研究的，都简称为D派。
- others
- DP解决范围查询泄漏

论文会按照上面几个方向分类好，剩下的就是关于范围查询的攻击论文。建议攻击论文多看一些，这样比较好找到想要解决的问题。



## Boolean query

在SSE的场景下，布尔查询又可以理解为多关键字查询，其中包含联合查询(conjunctive query)和非联合查询(disjunctive query)，下面按照时间脉络和重要程度，列出一些具有里程碑意义的论文。

- OXT(2013) ，适合做conjunctive query
- BIEX(2017)，适合做disjunctive query
- CNFFilter(2021)，结合以上两篇进行改进

现有的Boolean Query方案大多都是改进于OXT框架，如：HXT解决了OXT关键字对泄露的问题；PHXT在没有牺牲存储的情况下解决了OXT关键字对泄露的问题；SDSSE-OXT进一步增加token开销的前提下将OXT做成了动态方案；包括CNFFIlter都离不开OXT框架，大家了解Boolean Query可以以OXT为出发点，阅读该方案的扩展论文，当然也有很多没有使用OXT的论文，如VBTree，扩展位图索引等，这一方向也不失为一种思路，更多论文欢迎大家补充。



# 提交流程

1. 流程
   1. 先拉取最新分支：`git pull origin main`
   2. 更新和提交到本地仓库
      1. 将修改的文件添加到暂存区：`git add "文件名"`
         1. 使用命令`git status`查看工作区修改但是未添加到暂存区的文件，如果工作区中修改过的文件**都需要**添加到暂存区中，可以使用命令`git add .`将当前工作区中修改过的文件全部添加到暂存区中。
      2. 将暂存区的修改提交到本地仓库：`git commit -m "备注信息"`
   3. 推送到远程仓库：git push origin main
2. 提交时的备注信息
   1. 新增文件：add 文件名
   2. 修改文件：update 文件名+修改的操作
      1. 例如，update README新增提交实例
   3. 删除文件：delete 文件名
3. 提交实例 

```bash
# 1. 先拉取最新的远程分支
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git pull origin main
From https://github.com/SCNU-CS511/CS511-interview
 * branch            main       -> FETCH_HEAD
Already up to date.
# 2. 查看工作区中已修改的文件（这里只修改了README）
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git status
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   README.md

no changes added to commit (use "git add" and/or "git commit -a")
# 3. 将工作区中的修改添加到暂存区中
# 这里也可以使用"git add ."命令进行提交
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git add README.md
# 4. 将暂存区中的修改提交到本地仓库中
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git commit -m "update README新增提交实例"
[main 4b6a395] update README
 1 file changed, 18 insertions(+)
# 5. 查看是否在本地仓库提交成功
# 发现已经新增了一条提交记录
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git log
commit 5a30dcba4e7662436d0b3c2a6ed3cae94120891d (HEAD -> main)
Author: 2017libin <1963482624@qq.com>
Date:   Mon Aug 22 23:17:04 2022 +0800

    update README新增提交实例

commit 4b6a395a084ed7a988509ff70447531ab8a2c313 (origin/main, origin/HEAD)
Author: 2017libin <1963482624@qq.com>
Date:   Mon Aug 22 23:07:16 2022 +0800

    update README

commit c1b387bb507f50ca79d153e8d544722811f89037
Author: zhuyongshi <993015403@qq.com>
Date:   Mon Aug 22 22:33:46 2022 +0800

    update 计网面试题
# 6. 将本地仓库推送到远程
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git push origin main
Counting objects: 3, done.
Delta compression using up to 16 threads.
Compressing objects: 100% (3/3), done.
Writing objects: 100% (3/3), 1.37 KiB | 1.37 MiB/s, done.
Total 3 (delta 0), reused 0 (delta 0)
To https://github.com/SCNU-CS511/CS511-interview.git
   4b6a395..5a30dcb  main -> main
# 7. 查看是否远程分支是否更新
# (origin/main, origin/HEAD)都已经指向了最新的提交记录，说明了成功将本地的修改提交到远程了
chase@DESKTOP-D3FEH7R:~/code/CS511-interview$ git log
commit 5a30dcba4e7662436d0b3c2a6ed3cae94120891d (HEAD -> main, origin/main, origin/HEAD)
Author: 2017libin <1963482624@qq.com>
Date:   Mon Aug 22 23:17:04 2022 +0800

    update README新增提交实例

commit 4b6a395a084ed7a988509ff70447531ab8a2c313
Author: 2017libin <1963482624@qq.com>
Date:   Mon Aug 22 23:07:16 2022 +0800

    update README

commit c1b387bb507f50ca79d153e8d544722811f89037
Author: zhuyongshi <993015403@qq.com>
Date:   Mon Aug 22 22:33:46 2022 +0800

    update 计网面试题
```

