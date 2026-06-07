OS_Course_Design (操作系统课程设计)
所有模块均在Ubuntu Linux环境下使用C语言开发，涵盖了操作系统中最核心的四大管理机制：处理器调度、内存管理、进程同步以及文件系统模拟。

项目目录结构说明：
```text
OS_Course_Design/
├── 1_Processor_Scheduling/       # 第一部分：处理器调度模块
│   ├── scheduler.c               # 进程调度算法源码 (FCFS/SJF/RR/HRRN/HPF)
│   └── scheduler                 # 编译后的可执行程序
├── 2_Memory_Management/          # 第二部分：内存管理模块
│   ├── memory_system.c           # 动态分区分配算法与页面置换算法源码
│   └── memory_system             # 编译后的可执行程序
├── 3_Process_Synchronization/     # 第三部分：进程同步与互斥模块
│   ├── sync_system.c             # 基于POSIX信号量的经典并发问题模拟源码
│   └── sync_system               # 编译后的可执行程序
└── 4_File_System/                # 第四部分：文件系统与目录管理模块
    ├── file_system.c             # 虚拟文件系统模拟源码 (支持连续分配与位示图)
    └── file_system               # 编译后的可执行程序
```

各模块功能详解

1. 处理器调度模块 (1_Processor_Scheduling)
实现功能：模拟了经典的多任务处理器调度流。
核心算法：
先来先服务算法 (FCFS)
短作业优先算法 (SJF)
时间片轮转算法 (RR)
高响应比优先算法 (HRRN)
最高优先级优先算法 (HPF)
输出指标：实时计算并打印每个进程的完成时间、周转时间以及带权周转时间。

2. 内存管理模块 (2_Memory_Management)
实现功能：模拟操作系统对主存的划分、分配回收机制以及虚拟存储器的页面调度。
核心算法：
动态分区分配：首次适应 (FF)、最佳适应 (BF)、最坏适应 (WF)。
页面置换算法：先进先出 (FIFO)、最近最少使用 (LRU)。

3. 进程同步与互斥模块 (3_Process_Synchronization)
实现功能：解决多线程并发环境下的经典同步/互斥互锁问题。
技术栈：采用 Linux pthread 多线程库，利用 sem_init、sem_wait、sem_post 等 POSIX 信号量机制确保线程安全，完美避免死锁与饥饿。

4. 文件系统与目录管理模块 (4_File_System)
实现功能：在内存中开辟大数组模拟物理磁盘，构建了一个支持多用户交互的简易虚拟文件系统。
核心设计：
空闲空间管理：采用位示图 (Bitmap) 机制，用 ○ 表示空闲盘块，● 表示占用盘块，实时可视化磁盘空间。
文件组织形式：采用连续分配 (Contiguous Allocation) 策略。
基础文件操作：完美支持文件的 创建 (Create)、写入 (Write)、读取 (Read)、删除 (Delete)、列出文件流 (List)。

如何运行本项目
在本地 Ubuntu 环境下，你可以进入任意一个实验子目录进行编译和运行：

Bash
# 以第四部分文件系统为例：
cd ~/OS_Course_Design/4_File_System
# 1. 编译源码
gcc file_system.c -o file_system
# 2. 运行程序
./file_system
(注：第3部分进程同步模块由于使用了多线程，编译时请记得添加 -pthread 参数：gcc sync_system.c -o sync_system -pthread)

👨‍💻 开发环境
OS: Ubuntu Linux (VMware Virtual Platform)
Compiler: gcc (Ubuntu 11.x 或更高版本)
Version Control: Git / GitHub
