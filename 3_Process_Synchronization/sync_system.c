#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// ==================== 【问题 1：生产者-消费者】 ====================
#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int in = 0, out = 0, product_id = 0;
pthread_mutex_t pc_mutex;
sem_t pc_empty, pc_full;

void* producer_func(void* arg) {
    int id = *(int*)arg;
    for(int i = 0; i < 5; i++) { 
        sleep(1);
        sem_wait(&pc_empty);
        pthread_mutex_lock(&pc_mutex);
        product_id++;
        buffer[in] = product_id;
        printf("【生产者 %d】生产产品 %d -> 缓冲区[%d]\n", id, product_id, in);
        in = (in + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&pc_mutex);
        sem_post(&pc_full);
    }
    return NULL;
}

void* consumer_func(void* arg) {
    int id = *(int*)arg;
    for(int i = 0; i < 5; i++) {
        sleep(2);
        sem_wait(&pc_full);
        pthread_mutex_lock(&pc_mutex);
        int item = buffer[out];
        printf("  〖消费者 %d〗消费产品 %d <- 缓冲区[%d]\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&pc_mutex);
        sem_post(&pc_empty);
    }
    return NULL;
}

void runProducerConsumer() {
    printf("\n--- 1. 生产者-消费者问题演练 ---\n");
    pthread_t p1, p2, c1, c2;
    int id1 = 1, id2 = 2;
    pthread_mutex_init(&pc_mutex, NULL);
    sem_init(&pc_empty, 0, BUFFER_SIZE);
    sem_init(&pc_full, 0, 0);
    in = 0; out = 0; product_id = 0;

    pthread_create(&p1, NULL, producer_func, &id1);
    pthread_create(&p2, NULL, producer_func, &id2);
    pthread_create(&c1, NULL, consumer_func, &id1);
    pthread_create(&c2, NULL, consumer_func, &id2);

    pthread_join(p1, NULL); pthread_join(p2, NULL);
    pthread_join(c1, NULL); pthread_join(c2, NULL);
    
    pthread_mutex_destroy(&pc_mutex);
    sem_destroy(&pc_empty); sem_destroy(&pc_full);
    printf("--------------------------------------\n");
}

// ==================== 【问题 2：读者-写者 (读者优先)】 ====================
int shared_data = 0;
int read_count = 0;
pthread_mutex_t r_mutex; 
sem_t rw_mutex;          

void* reader_func(void* arg) {
    int id = *(int*)arg;
    for(int i = 0; i < 10; i++) { // 保持 10 次长循环，确保数据量丰富
        sleep(1);
        pthread_mutex_lock(&r_mutex);
        read_count++;
        if(read_count == 1) sem_wait(&rw_mutex); 
        pthread_mutex_unlock(&r_mutex);

        printf("[读者 %d] 正在读取共享数据 = %d (当前共有 %d 个读者)\n", id, shared_data, read_count);
        sleep(1); 

        pthread_mutex_lock(&r_mutex);
        read_count--;
        if(read_count == 0) sem_post(&rw_mutex); 
        pthread_mutex_unlock(&r_mutex);
    }
    return NULL;
}

void* writer_func(void* arg) {
    int id = *(int*)arg;
    for(int i = 0; i < 6; i++) { // 保持 6 次长循环
        sleep(2);
        sem_wait(&rw_mutex); 
        shared_data = rand() % 100;
        printf(" 📝【写者 %d】将共享数据修改为 -> %d\n", id, shared_data);
        sleep(1); 
        sem_post(&rw_mutex);
    }
    return NULL;
}

void runReaderWriter() {
    printf("\n--- 2. 读者-写者问题演练 ---\n");
    pthread_t r1, r2, w1;
    int id1 = 1, id2 = 2;
    pthread_mutex_init(&r_mutex, NULL);
    sem_init(&rw_mutex, 0, 1);
    read_count = 0;

    pthread_create(&r1, NULL, reader_func, &id1);
    pthread_create(&r2, NULL, reader_func, &id2);
    pthread_create(&w1, NULL, writer_func, &id1);

    pthread_join(r1, NULL); pthread_join(r2, NULL); pthread_join(w1, NULL);

    pthread_mutex_destroy(&r_mutex);
    sem_destroy(&rw_mutex);
    printf("--------------------------------------\n");
}

// ==================== 【问题 3：哲学家进餐 (状态机制-无死锁完美版)】 ====================
#define PHILO_NUM 5
#define THINKING 0
#define HUNGRY   1
#define EATING   2

int philo_state[PHILO_NUM];       // 记录每个哲学家当前状态
sem_t philo_sem[PHILO_NUM];       // 每个哲学家专用的同步信号量
pthread_mutex_t philo_state_mutex; // 状态改变的全局互斥锁

// 核心测试函数：检查当前哲学家左右两边是否有邻座在吃
void test_neighbors(int id) {
    if (philo_state[id] == HUNGRY && 
        philo_state[(id + 4) % PHILO_NUM] != EATING && 
        philo_state[(id + 1) % PHILO_NUM] != EATING) {
        
        philo_state[id] = EATING;
        // 刚性互斥：在同一块互斥锁内部打印就餐，彻底断绝邻座同时就餐和时序乱序可能
        printf("🍗 哲学家 %d 成功拿到两支筷子，开始就餐！\n", id);
        sem_post(&philo_sem[id]); // 唤醒自己
    }
}

void* philosopher_func(void* arg) {
    int id = *(int*)arg;
    for(int i = 0; i < 2; i++) { 
        printf("🎓 哲学家 %d 正在思考...\n", id);
        sleep(1);

        // 1. 尝试拿起两支筷子
        pthread_mutex_lock(&philo_state_mutex);
        philo_state[id] = HUNGRY;
        test_neighbors(id); // 检查两边是否空闲
        pthread_mutex_unlock(&philo_state_mutex);

        sem_wait(&philo_sem[id]); // 若邻座有人在吃，则在此安全阻塞，且不抱着锁卡死别人

        sleep(1); // 模拟就餐耗时

        // 2. 放下两支筷子
        pthread_mutex_lock(&philo_state_mutex);
        philo_state[id] = THINKING;
        printf("🎓 哲学家 %d 吃饱了，放回筷子。\n", id);
        
        // 关键所在：放下筷子后，主动帮挨饿的左邻右舍测一下，看他们能不能吃，能吃就顺便唤醒他们
        test_neighbors((id + 4) % PHILO_NUM); // 测试左边邻座
        test_neighbors((id + 1) % PHILO_NUM); // 测试右边邻座
        pthread_mutex_unlock(&philo_state_mutex);
    }
    return NULL;
}

void runPhilosopher() {
    printf("\n--- 3. 哲学家进餐问题演练 ---\n");
    pthread_t phils[PHILO_NUM];
    int phil_ids[PHILO_NUM];
    
    pthread_mutex_init(&philo_state_mutex, NULL);
    for(int i = 0; i < PHILO_NUM; i++) {
        sem_init(&philo_sem[i], 0, 0); 
        philo_state[i] = THINKING;
    }

    for(int i = 0; i < PHILO_NUM; i++) {
        phil_ids[i] = i;
        pthread_create(&phils[i], NULL, philosopher_func, &phil_ids[i]);
    }
    for(int i = 0; i < PHILO_NUM; i++) pthread_join(phils[i], NULL);
    for(int i = 0; i < PHILO_NUM; i++) sem_destroy(&philo_sem[i]);
    pthread_mutex_destroy(&philo_state_mutex); 
    printf("--------------------------------------\n");
}

// ==================== 主函数控制核心 ====================
int main() {
    int choice;
    while(1) {
        printf("\n====== 进程同步与并发控制模拟器 ======\n");
        printf("1. 经典生产者-消费者问题\n");
        printf("2. 经典读者-写者问题\n");
        printf("3. 经典哲学家进餐问题\n");
        printf("0. 退出程序\n");
        printf("======================================\n");
        printf("请选择功能模块: ");
        scanf("%d", &choice);

        if(choice == 0) break;
        else if(choice == 1) runProducerConsumer();
        else if(choice == 2) runReaderWriter();
        else if(choice == 3) runPhilosopher();
        else printf("无效选项，请重新输入！\n");
    }
    return 0;
}
