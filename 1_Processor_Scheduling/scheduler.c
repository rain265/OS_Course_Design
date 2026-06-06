#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[20];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;       // 新增/启用：优先级
    int start_time;
    int finish_time;
    int turnaround_time;
    double weighted_turnaround_time;
    int is_completed;
} PCB;

// 按到达时间排序
void sortByArrival(PCB p[], int n) {
    PCB temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrival_time > p[j+1].arrival_time) {
                temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }
}

// 1. FCFS 算法
void calculateFCFS(PCB p[], int n) {
    p[0].start_time = p[0].arrival_time;
    p[0].finish_time = p[0].start_time + p[0].burst_time;
    p[0].turnaround_time = p[0].finish_time - p[0].arrival_time;
    p[0].weighted_turnaround_time = (double)p[0].turnaround_time / p[0].burst_time;

    for (int i = 1; i < n; i++) {
        if (p[i].arrival_time > p[i-1].finish_time) {
            p[i].start_time = p[i].arrival_time;
        } else {
            p[i].start_time = p[i-1].finish_time;
        }
        p[i].finish_time = p[i].start_time + p[i].burst_time;
        p[i].turnaround_time = p[i].finish_time - p[i].arrival_time;
        p[i].weighted_turnaround_time = (double)p[i].turnaround_time / p[i].burst_time;
    }
}

// 2. SJF 算法
void calculateSJF(PCB p[], int n) {
    int current_time = 0, completed = 0;
    for(int i = 0; i < n; i++) p[i].is_completed = 0;

    while (completed < n) {
        int idx = -1, min_burst = 1e9;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].is_completed == 0) {
                if (p[i].burst_time < min_burst) {
                    min_burst = p[i].burst_time;
                    idx = i;
                }
            }
        }
        if (idx != -1) {
            p[idx].start_time = current_time;
            p[idx].finish_time = p[idx].start_time + p[idx].burst_time;
            p[idx].turnaround_time = p[idx].finish_time - p[idx].arrival_time;
            p[idx].weighted_turnaround_time = (double)p[idx].turnaround_time / p[idx].burst_time;
            p[idx].is_completed = 1;
            completed++;
            current_time = p[idx].finish_time;
        } else {
            current_time++;
        }
    }
}

// 3. RR (时间片轮转) 算法
void calculateRR(PCB p[], int n, int quantum) {
    int current_time = 0, completed = 0;
    int *queue = (int *)malloc(100 * sizeof(int));
    int head = 0, tail = 0;
    int *in_queue = (int *)calloc(n, sizeof(int));

    for (int i = 0; i < n; i++) {
        p[i].remaining_time = p[i].burst_time;
        p[i].is_completed = 0;
        p[i].start_time = -1;
    }

    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time <= current_time) {
            queue[tail++] = i;
            in_queue[i] = 1;
        }
    }

    while (completed < n) {
        if (head != tail) {
            int idx = queue[head++];
            if (p[idx].start_time == -1) p[idx].start_time = current_time;

            int run_time = (p[idx].remaining_time > quantum) ? quantum : p[idx].remaining_time;
            current_time += run_time;
            p[idx].remaining_time -= run_time;

            for (int i = 0; i < n; i++) {
                if (p[i].arrival_time <= current_time && in_queue[i] == 0 && p[i].is_completed == 0) {
                    queue[tail++] = i;
                    in_queue[i] = 1;
                }
            }

            if (p[idx].remaining_time > 0) {
                queue[tail++] = idx;
            } else {
                p[idx].finish_time = current_time;
                p[idx].turnaround_time = p[idx].finish_time - p[idx].arrival_time;
                p[idx].weighted_turnaround_time = (double)p[idx].turnaround_time / p[idx].burst_time;
                p[idx].is_completed = 1;
                completed++;
            }
        } else {
            current_time++;
            for (int i = 0; i < n; i++) {
                if (p[i].arrival_time <= current_time && in_queue[i] == 0) {
                    queue[tail++] = i;
                    in_queue[i] = 1;
                }
            }
        }
    }
    free(queue); free(in_queue);
}

// 4. 非抢占式优先级调度算法 (新增)
void calculatePriority(PCB p[], int n) {
    int current_time = 0, completed = 0;
    for(int i = 0; i < n; i++) p[i].is_completed = 0;

    while (completed < n) {
        int idx = -1;
        int min_priority = 1e9; // 数字越小，优先级越高

        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].is_completed == 0) {
                if (p[i].priority < min_priority) {
                    min_priority = p[i].priority;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            p[idx].start_time = current_time;
            p[idx].finish_time = p[idx].start_time + p[idx].burst_time;
            p[idx].turnaround_time = p[idx].finish_time - p[idx].arrival_time;
            p[idx].weighted_turnaround_time = (double)p[idx].turnaround_time / p[idx].burst_time;
            p[idx].is_completed = 1;
            completed++;
            current_time = p[idx].finish_time;
        } else {
            current_time++;
        }
    }
}

// 打印结果
void displayResults(PCB p[], int n) {
    PCB temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].finish_time > p[j+1].finish_time) {
                temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }

    double total_turnaround = 0, total_weighted_turnaround = 0;
    printf("\n----------------------------------------------------------------------------------\n");
    printf("进程名\t到达时间\t服务时间\t优先级\t开始时间\t完成时间\t周转时间\t带权周转时间\n");
    printf("----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < n; i++) {
        printf("%s\t%d\t\t%d\t\t%d\t%d\t\t%d\t\t%d\t\t%.2f\n", 
               p[i].name, p[i].arrival_time, p[i].burst_time, p[i].priority,
               p[i].start_time, p[i].finish_time, p[i].turnaround_time, p[i].weighted_turnaround_time);
        total_turnaround += p[i].turnaround_time;
        total_weighted_turnaround += p[i].weighted_turnaround_time;
    }
    printf("----------------------------------------------------------------------------------\n");
    printf("平均周转时间: %.2f\n", total_turnaround / n);
    printf("平均带权周转时间: %.2f\n", total_weighted_turnaround / n);
    
    printf("\n进程运行顺序: ");
    for(int i = 0; i < n; i++) {
        printf("%s", p[i].name);
        if(i < n - 1) printf(" -> ");
    }
    printf("\n\n");
}

int main() {
    int n, choice, quantum = 2;
    printf("=== 处理机调度模拟系统 ===\n");
    printf("1. 先来先服务 (FCFS)\n");
    printf("2. 短作业优先 (SJF)\n");
    printf("3. 时间片轮转 (RR)\n");
    printf("4. 优先级调度 (Priority)\n");
    printf("请选择算法 (1-4): ");
    scanf("%d", &choice);

    if (choice == 3) {
        printf("请输入时间片大小 (Quantum): ");
        scanf("%d", &quantum);
    }

    printf("请输入进程总数: ");
    scanf("%d", &n);

    PCB *processes = (PCB *)malloc(n * sizeof(PCB));

    for (int i = 0; i < n; i++) {
        printf("\n--- 请输入第 %d 个进程的信息 ---\n", i + 1);
        printf("进程名称: ");
        scanf("%s", processes[i].name);
        printf("到达时间: ");
        scanf("%d", &processes[i].arrival_time);
        printf("服务时间: ");
        scanf("%d", &processes[i].burst_time);
        if (choice == 4) {
            printf("优先级 (数字越小优先级越高): ");
            scanf("%d", &processes[i].priority);
        } else {
            processes[i].priority = 0; // 其他算法默认0
        }
    }

    sortByArrival(processes, n);

    if (choice == 1) calculateFCFS(processes, n);
    else if (choice == 2) calculateSJF(processes, n);
    else if (choice == 3) calculateRR(processes, n, quantum);
    else if (choice == 4) calculatePriority(processes, n);
    else {
        printf("无效选择！\n");
        free(processes);
        return 1;
    }

    displayResults(processes, n);
    free(processes);
    return 0;
}
