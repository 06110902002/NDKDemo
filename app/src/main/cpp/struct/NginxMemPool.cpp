
#include <LogUtils.h>
#include "NginxMemPool.h"

NginxMemPool::NginxMemPool() {

}

NginxMemPool::~NginxMemPool() {
    LOGI("内存池析构");
}

struct mp_pool_s *NginxMemPool::mp_create_pool(size_t size) {
    LOGI("mp_create_pool  创建内存池初始大小 = %ld, "
           "池本身占用大小 = %ld  "
           "小块结点描述信息大小 = %ld  "
           "大块结点描述信息 = %ld  \n",
           size,
           sizeof(struct mp_pool_s),
           sizeof(struct mp_node_s),
           sizeof(struct mp_large_s));

    struct mp_pool_s *pool;
    if (size < PAGE_SIZE || size % PAGE_SIZE != 0) {
        size = PAGE_SIZE;
    }

    int ret = posix_memalign((void **) &pool, MP_ALIGNMENT, size); //4K + mp_pool_s
    if (ret) {
        return nullptr;
    }
    pool->large = nullptr;

    // 申请的PAGE_SIZE 大小 需要给sizeof(struct mp_pool_s) 大小用于 mp_pool_s 本身信息使用
    // 当前小块结点与小块的头结点均指向除了内存池本身大小 处
    pool->current = pool->head = reinterpret_cast<mp_node_s *>((unsigned char *) pool +
                                                               sizeof(struct mp_pool_s));
    // 小块结点的 last 指针 指向 内存池本身+小块结点本身 下一地址
    pool->head->last = (unsigned char *) pool + sizeof(struct mp_pool_s) + sizeof(struct mp_node_s);
    // 小块结点的结尾指针反指向 当前申请到的内存块地址末尾处
    pool->head->end = (unsigned char *) pool + PAGE_SIZE;

    pool->head->failed = 0;

    return pool;
}

void NginxMemPool::mp_destroy_pool(struct mp_pool_s *pool) {
    struct mp_large_s *large;
    for (large = pool->large; large; large = large->next) {
        if (large->alloc) {
            free(large->alloc);
        }
    }

    struct mp_node_s *cur, *next;
    cur = pool->head->next;

    while (cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    free(pool);
}

void *NginxMemPool::mp_malloc_large(struct mp_pool_s *pool, size_t size) {
    unsigned char *big_addr;
    int ret = posix_memalign((void **) &big_addr, MP_ALIGNMENT, size);
    if (ret) {
        return nullptr;
    }

    struct mp_large_s *large;
    //released struct large resume
    int n = 0;
    //将申请到的内存池与前面已经申请好的结点连接好，后插法
    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->size = size;
            large->alloc = big_addr;
            return big_addr;
        }
        if (n++ > 3) {
            break;// 为了避免过多的遍历，限制次数
        }
    }
    //如果当前池子中没有大块内存 直接申请一个大块内存，作为池中第一个大块内存的结点
    large = static_cast<mp_large_s *>(mp_malloc(pool, sizeof(struct mp_large_s)));
    if (large == NULL) {
        free(big_addr);
        return NULL;
    }
    large->size = size;
    large->alloc = big_addr;
    large->next = pool->large;
    pool->large = large;
    return big_addr;
}

void *NginxMemPool::mp_malloc(struct mp_pool_s *pool, size_t size) {
    if (size <= 0) {
        return nullptr;
    }
    if (size > PAGE_SIZE - sizeof(struct mp_node_s)) {
        //large
        return mp_malloc_large(pool, size);
    } else {
        //small
        unsigned char *mem_addr = NULL;
        struct mp_node_s *cur = NULL;
        cur = pool->current;
        while (cur) {
            mem_addr = (unsigned char *)mp_align_ptr(cur->last, MP_ALIGNMENT); //按16位对齐
            LOGI("-------分配第 %d 块内存 "
                   "上一次分配到的地址 0x1%02x "
                   "将一次已经分配到的内存偏移后的值为 = 0x1%02x \n",
                   cur->quote,
                   cur->last,
                   mem_addr);
            if (cur->end - mem_addr >= size) {
                cur->quote++;//引用+1
                cur->last = mem_addr + size;
                return mem_addr;
            } else {
                LOGI("-------已经分配 %d 块  "
                       "大小为 %ld 字节内存 "
                       "已经分配的总大小 = %ld "
                       "剩余的空间  = %ld "
                       "不足以装下 %ld 大小  "
                       "需要申请新的内存块 \r\n",
                       cur->quote,
                       size,
                       (cur->quote * size),
                       (cur->end - mem_addr),
                       size);
                cur = cur->next;
            }
        }
        return mp_malloc_block(pool, size);// open new space 当前块分配完了，重新分配一块内存，并连接到pool->current->next 结点上
    }
}

void *NginxMemPool::mp_calloc(struct mp_pool_s *pool, size_t size) {
    void *mem_addr = mp_malloc(pool, size);
    if (mem_addr) {
        memset(mem_addr, 0, size);
    }
    return mem_addr;
}

void NginxMemPool::mp_free(struct mp_pool_s *pool, void *p) {
    struct mp_large_s *large;
    for (large = pool->large; large; large = large->next) {//大块
        if (p == large->alloc) {
            free(large->alloc);
            large->size = 0;
            large->alloc = NULL;
            return;
        }
    }
    //小块 引用-1
    struct mp_node_s *cur = NULL;
    for (cur = pool->head; cur; cur = cur->next) {
//        printf("cur:%p   p:%p   end:%p\n", (unsigned char *) cur, (unsigned char *) p, (unsigned char *) cur->end);
        if ((unsigned char *) cur <= (unsigned char *) p && (unsigned char *) p <= (unsigned char *) cur->end) {
            cur->quote--;
            if (cur->quote == 0) {
                if (cur == pool->head) {
                    pool->head->last = (unsigned char *) pool + sizeof(struct mp_pool_s) + sizeof(struct mp_node_s);
                } else {
                    cur->last = (unsigned char *) cur + sizeof(struct mp_node_s);
                }
                cur->failed = 0;
                pool->current = pool->head;
            }
            return;
        }
    }
}

void NginxMemPool::mp_reset_pool(struct mp_pool_s *pool) {
    struct mp_node_s *cur;
    struct mp_large_s *large;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc) {
            free(large->alloc);
        }
    }

    pool->large = NULL;
    pool->current = pool->head;
    for (cur = pool->head; cur; cur = cur->next) {
        cur->last = (unsigned char *) cur + sizeof(struct mp_node_s);
        cur->failed = 0;
        cur->quote = 0;
    }
}

void NginxMemPool::monitor_mp_poll(struct mp_pool_s *pool, char *tk) {
    LOGI("\r------start monitor poll------%s\n", tk);
    struct mp_node_s *head = NULL;
    int i = 0;
    for (head = pool->head; head; head = head->next) {
        i++;
        if (pool->current == head) {
            LOGI("current==>第%d块\n", i);
        }
        if (i == 1) {
            LOGI("第%d块small block  "
                   "起始位置 %p  "
                   "已经使用到的位置 %p "
                   "已使用:%4ld "
                   "剩余空间:%4ld  "
                   "引用:%4d  "
                   "failed:%4d\n",
                   i,
                   pool,
                   head->last,
                   (unsigned char *) head->last - (unsigned char *) pool,
                   head->end - head->last,
                   head->quote,
                   head->failed);
        } else {
            LOGI("第%d块small block  "
                   "起始位置 %p  "
                   "已经使用到的位置 %p "
                   "已使用:%4ld  "
                   "剩余空间:%4ld  "
                   "引用:%4d  "
                   "failed:%4d\n",
                   i,
                   head,
                   head->last,
                   (unsigned char *) head->last - (unsigned char *) head,
                   head->end - head->last,
                   head->quote,
                   head->failed);
        }
    }
    struct mp_large_s *large;
    i = 0;
    for (large = pool->large; large; large = large->next) {
        i++;
        if (large->alloc != NULL) {
            LOGI("第%d块large block  size=%d\n", i, large->size);
        }
    }
    LOGI("\r------stop monitor poll------\n");
}

void *NginxMemPool::mp_malloc_block(struct mp_pool_s *pool, size_t size) {
    unsigned char *block;
    // 申请一块新的内存 大小为 PAGE_SIZE
    int ret = posix_memalign((void **) &block, MP_ALIGNMENT, PAGE_SIZE); //4K
    if (ret) {
        return NULL;
    }
    // 申请到的新的内存，取出一块大小用来存放 mp_node_s 描述信息
    struct mp_node_s *new_node = (struct mp_node_s *) block;
    //当前这块的 end 指向本次新申请的内存的末尾处
    new_node->end = block + PAGE_SIZE;
    new_node->next = NULL;

    //对齐内存 按16位对齐
    unsigned char *ret_addr = (unsigned char *)mp_align_ptr(block + sizeof(struct mp_node_s), MP_ALIGNMENT);

    new_node->last = ret_addr + size;
    new_node->quote++;

    struct mp_node_s *current = pool->current;
    struct mp_node_s *cur = NULL;

    for (cur = current; cur->next; cur = cur->next) {
        if (cur->failed++ > 4) {
            current = cur->next;
        }
    }
    //now cur = last node
    cur->next = new_node;  //将新申请的内存连接到 pool->current->next 处
    pool->current = new_node;
    return ret_addr;
}
