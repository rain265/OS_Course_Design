#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_MEMORY 512

// ==================== 1. 动态分区管理结构与函数 ====================
typedef struct MemBlock {
    int id;
    int size;
    int start_addr;
    struct MemBlock *next;
    struct MemBlock *prev;
} MemBlock;

MemBlock *head = NULL;

void initMemory() {
    head = (MemBlock *)malloc(sizeof(MemBlock));
    head->id = -1;
    head->size = TOTAL_MEMORY;
    head->start_addr = 0;
    head->next = NULL;
    head->prev = NULL;
}

void displayMemory() {
    MemBlock *curr = head;
    printf("\n--- 当前内存分区状态 ---\n");
    printf("起始地址\t结束地址\t大小(KB)\t状态\n");
    while (curr != NULL) {
        printf("%d\t\t%d\t\t%d\t\t%s\n", curr->start_addr, curr->start_addr + curr->size, curr->size, (curr->id == -1) ? "[空闲]" : "[已分配]进程");
        curr = curr->next;
    }
}

int allocateFF(int pid, int size) {
    MemBlock *curr = head;
    while (curr != NULL) {
        if (curr->id == -1 && curr->size >= size) {
            if (curr->size > size) {
                MemBlock *new_block = (MemBlock *)malloc(sizeof(MemBlock));
                new_block->id = -1;
                new_block->size = curr->size - size;
                new_block->start_addr = curr->start_addr + size;
                new_block->next = curr->next;
                new_block->prev = curr;
                if (curr->next != NULL) curr->next->prev = new_block;
                curr->next = new_block;
            }
            curr->id = pid;
            curr->size = size;
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int allocateBF(int pid, int size) {
    MemBlock *curr = head;
    MemBlock *best = NULL;
    int min_waste = TOTAL_MEMORY + 1;
    while (curr != NULL) {
        if (curr->id == -1 && curr->size >= size) {
            int waste = curr->size - size;
            if (waste < min_waste) {
                min_waste = waste;
                best = curr;
            }
        }
        curr = curr->next;
    }
    if (best != NULL) {
        if (best->size > size) {
            MemBlock *new_block = (MemBlock *)malloc(sizeof(MemBlock));
            new_block->id = -1;
            new_block->size = best->size - size;
            new_block->start_addr = best->start_addr + size;
            new_block->next = best->next;
            new_block->prev = best;
            if (best->next != NULL) best->next->prev = new_block;
            best->next = new_block;
        }
        best->id = pid;
        best->size = size;
        return 1;
    }
    return 0;
}

int freeMemory(int pid) {
    MemBlock *curr = head;
    int found = 0;
    while (curr != NULL) {
        if (curr->id == pid) {
            curr->id = -1;
            found = 1;
            if (curr->next != NULL && curr->next->id == -1) {
                MemBlock *temp = curr->next;
                curr->size += temp->size;
                curr->next = temp->next;
                if (temp->next != NULL) temp->next->prev = curr;
                free(temp);
            }
            if (curr->prev != NULL && curr->prev->id == -1) {
                MemBlock *temp = curr->prev;
                temp->size += curr->size;
                temp->next = curr->next;
                if (curr->next != NULL) curr->next->prev = temp;
                free(curr);
                curr = temp;
            }
            break;
        }
        curr = curr->next;
    }
    return found;
}

void runPartitionMenu() {
    int choice, algo, pid, size;
    initMemory();
    printf("\n选择分区算法: 1-首次适应(FF), 2-最佳适应(BF): ");
    scanf("%d", &algo);
    while(1) {
        displayMemory();
        printf("\n1-申请内存, 2-释放内存, 0-返回主菜单\n请选择: ");
        scanf("%d", &choice);
        if (choice == 0) {
            MemBlock *curr = head;
            while(curr) { MemBlock *t = curr; curr = curr->next; free(t); }
            head = NULL;
            break;
        }
        if (choice == 1) {
            printf("输入进程ID: "); scanf("%d", &pid);
            printf("输入大小(KB): "); scanf("%d", &size);
            if (algo == 1 ? allocateFF(pid, size) : allocateBF(pid, size))
                printf("【成功】内存分配成功\n");
            else
                printf("【失败】连续空间不足\n");
        } else if (choice == 2) {
            printf("输入要释放的进程ID: "); scanf("%d", &pid);
            if (freeMemory(pid)) printf("【成功】内存已释放并合并碎片\n");
            else printf("【失败】未找到该进程\n");
        }
    }
}

// ==================== 2. 页面置换管理结构与函数 ====================
void runPageReplacementMenu() {
    int frames_count, pages_count, algo;
    printf("\n请输入物理块(Frame)数量: ");
    scanf("%d", &frames_count);
    printf("请输入页面引用序列长度: ");
    scanf("%d", &pages_count);
    
    int *page_seq = (int *)malloc(pages_count * sizeof(int));
    printf("请依次输入页面引用序列(用空格隔开): ");
    for(int i = 0; i < pages_count; i++) {
        scanf("%d", &page_seq[i]);
    }
    
    printf("选择页面置换算法: 1-FIFO, 2-LRU: ");
    scanf("%d", &algo);

    int *frames = (int *)malloc(frames_count * sizeof(int));
    int *time_track = (int *)calloc(frames_count, sizeof(int)); // 用于LRU记录未使用时间
    for(int i = 0; i < frames_count; i++) frames[i] = -1; // -1表示物理块为空

    int page_faults = 0;
    int fifo_index = 0; // 用于FIFO指向最早进入的页面

    printf("\n--- 页面置换演练过程 ---\n");
    for(int i = 0; i < pages_count; i++) {
        int cur_page = page_seq[i];
        int found = 0;

        // 检查页面是否已在物理块中
        for(int j = 0; j < frames_count; j++) {
            if(frames[j] == cur_page) {
                found = 1;
                if(algo == 2) time_track[j] = i; // LRU: 刷新最近被访问的时间戳
                break;
            }
        }

        int is_fault = 0;
        if(!found) { // 发生缺页
            page_faults++;
            is_fault = 1;
            
            // 找有没有空闲的物理块
            int empty_slot = -1;
            for(int j = 0; j < frames_count; j++) {
                if(frames[j] == -1) { empty_slot = j; break; }
            }

            if(empty_slot != -1) {
                frames[empty_slot] = cur_page;
                if(algo == 2) time_track[empty_slot] = i;
            } else {
                // 满了解雇旧页面
                if(algo == 1) { // FIFO
                    frames[fifo_index] = cur_page;
                    fifo_index = (fifo_index + 1) % frames_count;
                } else { // LRU
                    int lru_index = 0, min_time = time_track[0];
                    for(int j = 1; j < frames_count; j++) {
                        if(time_track[j] < min_time) {
                            min_time = time_track[j];
                            lru_index = j;
                        }
                    }
                    frames[lru_index] = cur_page;
                    time_track[lru_index] = i;
                }
            }
        }

        // 打印当前物理块状态
        printf("访问页面 %d -> 物理块状态: [", cur_page);
        for(int j = 0; j < frames_count; j++) {
            if(frames[j] == -1) printf("  ");
            else printf(" %d ", frames[j]);
        }
        printf("] %s\n", is_fault ? "❌ 缺页" : "   命中");
    }

    printf("\n------------------------------------");
    printf("\n总缺页次数: %d 次", page_faults);
    printf("\n总缺页率: %.2f%%\n", ((double)page_faults / pages_count) * 100);
    printf("------------------------------------\n");

    free(page_seq); free(frames); free(time_track);
}

// ==================== 主函数控制核心 ====================
int main() {
    int choice;
    while (1) {
        printf("\n====== 智能内存管理系统模拟器 ======\n");
        printf("1. 动态分区管理 (FF/BF 算法)\n");
        printf("2. 虚拟存储页面置换 (FIFO/LRU 算法)\n");
        printf("0. 退出程序\n");
        printf("====================================\n");
        printf("请选择功能模块: ");
        scanf("%d", &choice);

        if (choice == 0) break;
        if (choice == 1) runPartitionMenu();
        else if (choice == 2) runPageReplacementMenu();
        else printf("无效选项，请重新输入！\n");
    }
    return 0;
}
