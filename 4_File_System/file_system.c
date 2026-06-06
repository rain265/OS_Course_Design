#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_BLOCKS 100   // 模拟磁盘的总盘块数
#define BLOCK_SIZE 64      // 每个盘块的大小（字节）
#define MAX_FILES 20       // 系统支持的最大文件数

// 1. 定义文件控制块（FCB / 目录项）
typedef struct {
    char filename[20];     // 文件名
    int start_block;       // 起始块号
    int block_count;       // 占用盘块数
    int file_size;         // 文件实际大小（字节）
} FCB;

// 2. 全局模拟磁盘组件
char disk[TOTAL_BLOCKS][BLOCK_SIZE]; // 实际存储数据的物理盘块
int bitmap[TOTAL_BLOCKS];            // 空闲空间管理：位示图 (0:空闲, 1:占用)
FCB directory[MAX_FILES];            // 根目录区
int file_count = 0;                  // 当前文件数量

// 初始化文件系统
void init_file_system() {
    memset(bitmap, 0, sizeof(bitmap));
    file_count = 0;
    // 默认前2个盘块被系统保留（模拟引导块或目录区占位）
    bitmap[0] = 1;
    bitmap[1] = 1;
    printf("💾 虚拟文件系统初始化成功！总盘块: %d，已启用位示图管理。\n", TOTAL_BLOCKS);
}

// 打印当前磁盘位示图（用于直观展示空闲空间管理）
void show_bitmap() {
    printf("\n📊 当前磁盘位示图 (空闲空间管理):\n");
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (bitmap[i] == 0) printf("○ "); // ○ 表示空闲
        else printf("● ");                // ● 表示已被占用
        if ((i + 1) % 10 == 0) printf("\n"); // 每10个盘块换一行
    }
    printf("\n");
}

// 功能1：创建文件
void create_file() {
    char name[20];
    int blocks_needed;
    
    printf("请输入要创建的文件名: ");
    scanf("%s", name);
    
    // 检查重名
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].filename, name) == 0) {
            printf("❌ 错误：文件 '%s' 已存在！\n", name);
            return;
        }
    }
    
    printf("请指定为该文件分配的初始盘块数: ");
    scanf("%d", &blocks_needed);
    
    if (blocks_needed <= 0 || blocks_needed > TOTAL_BLOCKS) {
        printf("❌ 错误：无效的盘块数量！\n");
        return;
    }

    // 寻找连续的空闲盘块（连续分配策略）
    int start = -1;
    int consecutive_count = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (bitmap[i] == 0) {
            if (consecutive_count == 0) start = i;
            consecutive_count++;
            if (consecutive_count == blocks_needed) break;
        } else {
            consecutive_count = 0;
            start = -1;
        }
    }

    // 如果找到了足够的连续空间
    if (consecutive_count == blocks_needed) {
        // 在位示图中标记占用
        for (int i = start; i < start + blocks_needed; i++) {
            bitmap[i] = 1;
        }
        // 登记到根目录中
        strcpy(directory[file_count].filename, name);
        directory[file_count].start_block = start;
        directory[file_count].block_count = blocks_needed;
        directory[file_count].file_size = 0; // 刚创建时内容为空
        
        printf("✨ 成功创建文件 '%s'！起始块号: %d, 占用块数: %d\n", name, start, blocks_needed);
        file_count++;
        show_bitmap();
    } else {
        printf("❌ 错误：磁盘空间不足，无法分配连续的 %d 个盘块！\n", blocks_needed);
    }
}

// 功能2：写入文件
void write_file() {
    char name[20];
    char content[200];
    printf("请输入要写入的文件名: ");
    scanf("%s", name);

    int index = -1;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].filename, name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("❌ 错误：找不到文件 '%s'！\n", name);
        return;
    }

    printf("请输入要写入的文本内容 (不支持空格): ");
    scanf("%s", content);

    int len = strlen(content);
    int max_bytes = directory[index].block_count * BLOCK_SIZE;

    if (len >= max_bytes) {
        printf("❌ 错误：内容大小 (%d 字节) 超过了该文件分配的盘块最大容量 (%d 字节)！\n", len, max_bytes);
        return;
    }

    // 将内容模拟写入对应的物理盘块中
    int start = directory[index].start_block;
    // 清空原有盘块内容
    for (int i = 0; i < directory[index].block_count; i++) {
        memset(disk[start + i], 0, BLOCK_SIZE);
    }
    // 拷贝新内容
    strncpy(disk[start], content, len);
    directory[index].file_size = len;

    printf("📝 成功向文件 '%s' 写入 %d 字节的数据！\n", name, len);
}

// 功能3：读取文件
void read_file() {
    char name[20];
    printf("请输入要读取的文件名: ");
    scanf("%s", name);

    int index = -1;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].filename, name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("❌ 错误：找不到文件 '%s'！\n", name);
        return;
    }

    printf("\n📖 --- 文件 '%s' 的内容 --- \n", name);
    if (directory[index].file_size == 0) {
        printf("[空文件]\n");
    } else {
        int start = directory[index].start_block;
        // 直接从模拟盘块读取字符串
        printf("%s\n", disk[start]);
    }
    printf("-----------------------------\n");
}

// 功能4：删除文件
void delete_file() {
    char name[20];
    printf("请输入要删除的文件名: ");
    scanf("%s", name);

    int index = -1;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].filename, name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("❌ 错误：找不到文件 '%s'！\n", name);
        return;
    }

    // 1. 在位示图中释放盘块空间
    int start = directory[index].start_block;
    int count = directory[index].block_count;
    for (int i = start; i < start + count; i++) {
        bitmap[i] = 0;
        memset(disk[i], 0, BLOCK_SIZE); // 清空物理盘块数据
    }

    // 2. 从目录项数组中移除（用最后一个目录项覆盖当前项）
    directory[index] = directory[file_count - 1];
    file_count--;

    printf("🗑️ 成功删除文件 '%s'，已释放其占用的 %d 个盘块！\n", name, count);
    show_bitmap();
}

// 功能5：列出所有文件（目录流查看）
void list_files() {
    printf("\n📂 --- 根目录文件列表 --- \n");
    if (file_count == 0) {
        printf("当前目录为空。\n");
    } else {
        printf("%-12s\t%-8s\t%-8s\t%-8s\n", "文件名", "起始块号", "总块数", "大小(Bytes)");
        for (int i = 0; i < file_count; i++) {
            printf("%-12s\t%-8d\t%-8d\t%-8d\n", 
                   directory[i].filename, 
                   directory[i].start_block, 
                   directory[i].block_count, 
                   directory[i].file_size);
        }
    }
    printf("---------------------------\n");
}

// 主控菜单
int main() {
    int choice;
    init_file_system();
    
    while (1) {
        printf("\n======= 🖥️ 简易文件系统模拟器 =======\n");
        printf("1. 创建新文件 (Create)\n");
        printf("2. 写入文件内容 (Write)\n");
        printf("3. 读取文件内容 (Read)\n");
        printf("4. 删除已有文件 (Delete)\n");
        printf("5. 列出所有文件 (List)\n");
        printf("6. 显示磁盘位示图 (Bitmap)\n");
        printf("0. 退出系统\n");
        printf("====================================\n");
        printf("请选择操作 (0-6): ");
        scanf("%d", &choice);

        if (choice == 0) {
            printf("感谢使用，退出文件系统。\n");
            break;
        }
        
        switch (choice) {
            case 1: create_file(); break;
            case 2: write_file(); break;
            case 3: read_file(); break;
            case 4: delete_file(); break;
            case 5: list_files(); break;
            case 6: show_bitmap(); break;
            default: printf("无效选项，请重新输入！\n");
        }
    }
    return 0;
}
