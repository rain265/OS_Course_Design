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
