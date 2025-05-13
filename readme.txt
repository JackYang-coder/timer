定时器应用：
心跳检测、keepalive、应用层发送心跳包、技能冷却、倒计时、延时处理

定时器=数据结构+驱动方式


数据结构：
//1)按照先后过期的任务进行排序      红黑树（nginx、workflow）  最小堆(libevent、libev、libuv)  跳表（redis）
//2)按照执行序进行组织              时间轮（skynet、netty、kafka）

驱动方式：
1）利用IO多路复用系统调用最后一个参数，来触发定时器
while (!quit) {
    int timeout = get_nearest_timer() - now();
    if (timeout < 0) timeout = -1;  // Assuming -1 is for immediate handling of events
    int nevent = epoll_wait(epfd, ev, nev, timeout);
    for (int i = 0; i < nevent; i++) {
        // Process network events
    }
    // Process timer events
    update_timer();
}

2）利用timerfd,将定时检测作为IO多路复用中的事件进行处理 




最小堆（Min-Heap）：是一种完全二叉树，满足以下性质：

完全二叉树：除了最后一层外，每一层的节点都必须完全填满，且最后一层的节点从左到右依次排列。
堆性质：每个父节点的值都小于或等于其子节点的值


插入：通过将新元素放置在堆的末尾，然后进行上浮操作（heapifyUp）来恢复堆的性质
 void insert(int value) {
    heap.push_back(value);        // 将元素添加到堆的末尾
    heapifyUp(heap.size() - 1);   // 通过上浮操作恢复堆的性质
 }
void heapifyUp(int index) {
  // 如果当前节点不是根节点，并且当前节点小于父节点，就交换
  while (index > 0 && heap[index] < heap[(index - 1) / 2]) {
    // 交换当前节点与父节点的值
    std::swap(heap[index], heap[(index - 1) / 2]);
    // 更新索引，移动到父节点位置，继续检查
    index = (index - 1) / 2;
    }
}

删除：删除堆中的根节点，并将堆中的最后一个元素放到根节点的位置。然后通过下沉操作（heapifyDown）来恢复堆的性质
int extractMin() {
        if (heap.empty()) return -1;  // 如果堆为空，返回 -1
        int min = heap[0];            // 记录当前堆顶元素（最小值）
        heap[0] = heap.back();        // 用堆中的最后一个元素替代根节点
        heap.pop_back();              // 删除堆中的最后一个元素
        heapifyDown(0);               // 通过下沉操作恢复堆的性质
        return min;                   // 返回最小元素
    }
   void heapifyDown(int index) {
        int leftChild = 2 * index + 1;  // 左子节点索引
        int rightChild = 2 * index + 2; // 右子节点索引
        int smallest = index;  // 假设当前节点是最小的

        // 如果左子节点存在且小于当前节点，更新最小节点为左子节点
        if (leftChild < heap.size() && heap[leftChild] < heap[smallest])
            smallest = leftChild;
        
        // 如果右子节点存在且小于当前节点，更新最小节点为右子节点
        if (rightChild < heap.size() && heap[rightChild] < heap[smallest])
            smallest = rightChild;
        
        // 如果最小节点不是当前节点，交换并继续下沉
        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            // 递归调用，继续下沉到新的位置
            heapifyDown(smallest);
        }
    }
你好你好1