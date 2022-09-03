# Paper
Papers related to cryptography



本仓库为马昌社老师研究方向的相关论文和代码资源，请各位同门自觉维护仓库。



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

泄漏的简单介绍：Leakage Perturbation is Not Enough: Breaking

Structured Encryption Using Simulated Annealing。

泄漏相关的论文分为两大类：攻击论文和防止泄漏论文



## 相关技术

### Differential privacy

差分隐私是一个解决隐私问题的工具

差分隐私的

经典STE与DP结合的论文，建议精读：Encrypted Databases for Differential Privacy

### ORAM

ORAM完美的解决了access pattern的问题。

入门论文：

- 平方根：Software Protection and Simulation on Oblivious RAMs（1996）

- path-oram：Path ORAM:An Extremely Simple Oblivious RAM Protocol

在文件夹里有oram发展的完整流程。



## 范围查询 

范围查询是一种常用的查询类别，围绕如何提高范围查询效率有非常多的流派下面简单介绍

