/*7. На основе интерфейса из задания 5 реализуйте класс биномиальной приоритетной очереди.
Продемонстрируйте работу реализованного функционала.
*/

#include <iostream>
#include <cstring>
#include <climits>
#include <cstddef>

//ИСКЛЮЧЕНИЯ
class BadAllocException {
private:
    const char* message;
public:
    explicit BadAllocException(const char* msg) : message(msg) {}
    const char* what() const { return message; }
};

class EmptyQueueException {
public:
    const char* what() const { return "Queue is empty"; }
};

class InvalidMergeException {
public:
    const char* what() const { return "Cannot merge with different queue type"; }
};

class InvalidArgumentException {
public:
    const char* what() const { return "Invalid argument (null or empty string)"; }
};

//ИНТЕРФЕЙС
class IPriorityQueue {
public:
    virtual ~IPriorityQueue() = default;

    // a. Добавление элемента
    virtual void enqueue(const char* value, int key) = 0;

    // b. Поиск значения по наиболее приоритетному ключу
    virtual const char* peek() const = 0;

    // c. Удаление элемента с наиболее приоритетным ключом
    virtual void dequeue() = 0;

    // d. Слияние двух очередей (с поддержкой fluent API)
    virtual IPriorityQueue& merge(IPriorityQueue&& other) = 0;
};

//УЗЕЛ БИНОМИАЛЬНОГО ДЕРЕВА
struct BinomialNode {
    char* value;
    int key;
    int degree;
    BinomialNode* child;
    BinomialNode* sibling;
    BinomialNode* parent;

    BinomialNode(const char* val, int k);
    ~BinomialNode();

private:
    BinomialNode(const BinomialNode&);
    BinomialNode& operator=(const BinomialNode&);
};

BinomialNode::BinomialNode(const char* val, int k)
    : key(k), degree(0), child(nullptr), sibling(nullptr), parent(nullptr) {
    if (!val) {
        throw InvalidArgumentException();
    }
    
    size_t len = std::strlen(val);
    if (len == 0) {
        throw InvalidArgumentException();
    }
    
    value = new (std::nothrow) char[len + 1];
    if (!value) {
        throw BadAllocException("Failed to allocate node value");
    }
    std::strcpy(value, val);
}

BinomialNode::~BinomialNode() {
    delete[] value;
}

//БИНОМИАЛЬНАЯ ОЧЕРЕДЬ
class BinomialPriorityQueue : public IPriorityQueue {
private:
    BinomialNode* head;
    int size;
    mutable BinomialNode* cachedMinNode;
    mutable bool minNodeValid;

    // Вспомогательные статические методы
    static BinomialNode* mergeHeaps(BinomialNode* h1, BinomialNode* h2);
    static BinomialNode* unionHeaps(BinomialNode* h1, BinomialNode* h2);
    static BinomialNode* linkTrees(BinomialNode* y, BinomialNode* z);
    static void reverseSiblings(BinomialNode* node, BinomialNode*& newHead);
    static void clearNodes(BinomialNode* node);
    static BinomialNode* copyNodes(const BinomialNode* node);
    
    void invalidateMinCache();
    BinomialNode* findMinNode() const;
    void removeTree(BinomialNode* prev, BinomialNode* tree);

public:
    BinomialPriorityQueue();
    BinomialPriorityQueue(const BinomialPriorityQueue& other);
    BinomialPriorityQueue(BinomialPriorityQueue&& other) noexcept;
    ~BinomialPriorityQueue();

    BinomialPriorityQueue& operator=(const BinomialPriorityQueue& other);
    BinomialPriorityQueue& operator=(BinomialPriorityQueue&& other) noexcept;

    // Реализация интерфейса IPriorityQueue
    void enqueue(const char* value, int key) override;
    const char* peek() const override;
    void dequeue() override;
    IPriorityQueue& merge(IPriorityQueue&& other) override;

    // Доп.методы
    bool isEmpty() const;
    int getSize() const;
    void print() const;
};

//РЕАЛИЗАЦИЯ ВСПОМОГАТЕЛЬНЫХ МЕТОДОВ

void BinomialPriorityQueue::invalidateMinCache() {
    minNodeValid = false;
    cachedMinNode = nullptr;
}

BinomialNode* BinomialPriorityQueue::mergeHeaps(BinomialNode* h1, BinomialNode* h2) {
    if (!h1) return h2;
    if (!h2) return h1;

    BinomialNode* result = nullptr;
    BinomialNode* tail = nullptr;
    BinomialNode* current = nullptr;

    while (h1 && h2) {
        if (h1->degree <= h2->degree) {
            current = h1;
            h1 = h1->sibling;
        } else {
            current = h2;
            h2 = h2->sibling;
        }

        if (!result) {
            result = current;
            tail = current;
        } else {
            tail->sibling = current;
            tail = current;
        }
    }

    if (h1) {
        if (tail) tail->sibling = h1;
        else result = h1;
    }
    if (h2) {
        if (tail) tail->sibling = h2;
        else result = h2;
    }

    return result;
}

BinomialNode* BinomialPriorityQueue::linkTrees(BinomialNode* y, BinomialNode* z) {
    if (y->key > z->key) {
        BinomialNode* temp = y;
        y = z;
        z = temp;
    }
    z->parent = y;
    z->sibling = y->child;
    y->child = z;
    y->degree++;

    return y;
}

BinomialNode* BinomialPriorityQueue::unionHeaps(BinomialNode* h1, BinomialNode* h2) {
    BinomialNode* heap = mergeHeaps(h1, h2);
    if (!heap) {
        return nullptr;
    }
    BinomialNode* prev = nullptr;
    BinomialNode* curr = heap;
    BinomialNode* next = curr->sibling;

    while (next) {
        if (curr->degree != next->degree || (next->sibling && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        } else {
            if (curr->key <= next->key) {
                curr->sibling = next->sibling;
                linkTrees(curr, next);
            } else {
                if (!prev) {
                    heap = next;
                } else {
                    prev->sibling = next;
                }
                linkTrees(next, curr);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return heap;
}

void BinomialPriorityQueue::reverseSiblings(BinomialNode* node, BinomialNode*& newHead) {
    newHead = nullptr;
    while (node) {
        BinomialNode* next = node->sibling;
        node->sibling = newHead;
        node->parent = nullptr;
        newHead = node;
        node = next;
    }
}

void BinomialPriorityQueue::clearNodes(BinomialNode* node) {
    if (!node) {
        return;
    }
    clearNodes(node->child);
    clearNodes(node->sibling);
    delete node;
}

BinomialNode* BinomialPriorityQueue::copyNodes(const BinomialNode* node) {
    if (!node) {
        return nullptr;
    }
    BinomialNode* newNode = nullptr;
    try {
        newNode = new BinomialNode(node->value, node->key);
        newNode->degree = node->degree;
        newNode->child = copyNodes(node->sibling);
        if (newNode->child) {
            newNode->child->parent = newNode;
        }
    } catch (const BadAllocException&) {
        clearNodes(newNode);
        throw;
    } catch (const InvalidArgumentException&) {
        clearNodes(newNode);
        throw;
    }
    return newNode;
}

BinomialNode* BinomialPriorityQueue::findMinNode() const {
    if (minNodeValid && cachedMinNode) {
        return cachedMinNode;
    }
    if (!head) {
        return nullptr;
    }
    BinomialNode* minNode = head;
    BinomialNode* curr = head->sibling;

    while (curr) {
        if (curr->key < minNode->key) {
            minNode = curr;
        }
        curr = curr->sibling;
    }
    cachedMinNode = minNode;
    minNodeValid = true;
    return minNode;
}

void BinomialPriorityQueue::removeTree(BinomialNode* prev, BinomialNode* tree) {
    if (prev) {
        prev->sibling = tree->sibling;
    } else {
        head = tree->sibling;
    }
}

//конструктор
BinomialPriorityQueue::BinomialPriorityQueue()
: head(nullptr), size(0), cachedMinNode(nullptr), minNodeValid(false) {}

BinomialPriorityQueue::BinomialPriorityQueue(const BinomialPriorityQueue& other)
: head(nullptr), size(0), cachedMinNode(nullptr), minNodeValid(false) {
    if (other.head) {

        head = copyNodes(other.head);
        size = other.size;
    }
}

BinomialPriorityQueue::BinomialPriorityQueue(BinomialPriorityQueue&& other) noexcept
: head(other.head), size(other.size), cachedMinNode(other.cachedMinNode), minNodeValid(other.minNodeValid) {
    other.head = nullptr;
    other.size = 0;
    other.cachedMinNode = nullptr;
    other.minNodeValid = false;
}

BinomialPriorityQueue::~BinomialPriorityQueue() {
    clearNodes(head);
}

BinomialPriorityQueue& BinomialPriorityQueue::operator=(const BinomialPriorityQueue& other) {
   if (this != &other) {
    BinomialPriorityQueue temp(other);
    std::swap(head, temp.head);
    std::swap(size, temp.size);
    std::swap(cachedMinNode, temp.cachedMinNode);
    std::swap(minNodeValid, temp.minNodeValid);
    }
    return *this; 
}

BinomialPriorityQueue& BinomialPriorityQueue::operator=(BinomialPriorityQueue&& other) noexcept {
    if (this != & other) {
        clearNodes(head);
        head = other.head;
        size = other.size;
        cachedMinNode = other.cachedMinNode;
        minNodeValid = other.minNodeValid;
        other.head = nullptr;
        other.size = 0;
        other.cachedMinNode = nullptr;
        other.minNodeValid = false;
    }
    return *this;
}

//РЕАЛИЗАЦИЯ ИНТЕРФЕЙСА

void BinomialPriorityQueue::enqueue(const char* value, int key) {
    if (!value) {
        throw InvalidArgumentException();
    }
    if (std::strlen(value) == 0) {
        throw InvalidArgumentException();
    }
    BinomialNode* newNode = nullptr;
    try {
        newNode = new BinomialNode(value, key);
    } catch (const BadAllocException&) {
        throw;
    } catch (const InvalidArgumentException&) {
        throw;
    }

    BinomialPriorityQueue temp;
    temp.head = newNode;
    temp.size = 1;

    head = unionHeaps(head, temp.head);
    size++;
    invalidateMinCache();

    temp.head = nullptr;
}

const char* BinomialPriorityQueue::peek() const {
    if (!head) {
        throw EmptyQueueException();
    }
    BinomialNode* minNode = findMinNode();
    return minNode->value;
}

void BinomialPriorityQueue::dequeue() {
    if (!head) {
        throw EmptyQueueException();
    }
    //поиск минималного узлаи его предшественника
    BinomialNode* minNode = head;
    BinomialNode* prevMin = nullptr;
    BinomialNode* curr = head;
    BinomialNode* prev = nullptr;

    while (curr) {
        if (curr->key < minNode->key) {
            minNode = curr;
            prevMin = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (prevMin) {
        prevMin->sibling = minNode->sibling;
    } else {
        head = minNode->sibling;
    }

    BinomialNode* childrenHeap = nullptr;
    reverseSiblings(minNode->child, childrenHeap);

    head = unionHeaps(head, childrenHeap);
    delete minNode;
    size--;
    invalidateMinCache();
}

IPriorityQueue& BinomialPriorityQueue::merge(IPriorityQueue&& other) {
    BinomialPriorityQueue* otherQueue = dynamic_cast<BinomialPriorityQueue*>(&other);
    if (!otherQueue) {
        throw InvalidArgumentException();
    }
    head = unionHeaps(head, otherQueue->head);
    size += otherQueue->size;
    invalidateMinCache();

    otherQueue->head = nullptr;
    otherQueue->size = 0;
    otherQueue->invalidateMinCache();

    return *this;
}

//ДОПОЛЬНИТЕЛЬНЫЕ МЕТОДЫ

bool BinomialPriorityQueue::isEmpty() const {
    return head == nullptr;
}

int BinomialPriorityQueue::getSize() const {
    return size;
}

void BinomialPriorityQueue::print() const {
    if (!head) {
        std::cout << "Empty queue\n";
        return;
    }
    std::cout << "Binomial Queue (size=" << size << "):\n";
    BinomialNode* curr = head;
    int treeNum = 1;

    while (curr) {
        std::cout << " Tree " << treeNum++ << " (degree=" << curr->degree << ", root key=" << curr->key << ", value=" << curr ->value << ")\n";
        curr = curr->sibling;
    }
}

int main() {
    std::cout << "=== Binomial Priority Queue Demonstration ===\n\n";

    try {
        // Create first queue
        BinomialPriorityQueue queue1;
        
        std::cout << "1. Adding elements to queue1:\n";
        queue1.enqueue("Task A", 5);
        std::cout << "   + 'Task A' (key=5)\n";
        queue1.enqueue("Task B", 3);
        std::cout << "   + 'Task B' (key=3)\n";
        queue1.enqueue("Task C", 7);
        std::cout << "   + 'Task C' (key=7)\n";
        queue1.enqueue("Task D", 1);
        std::cout << "   + 'Task D' (key=1)\n";
        queue1.enqueue("Task E", 4);
        std::cout << "   + 'Task E' (key=4)\n";
        
        queue1.print();
        
        // peek
        std::cout << "\n2. peek() = " << queue1.peek() << std::endl;
        
        // dequeue
        std::cout << "\n3. dequeue() - removing minimum\n";
        queue1.dequeue();
        std::cout << "   New peek() = " << queue1.peek() << std::endl;
        
        // Add element with high priority
        std::cout << "\n4. Adding 'High Priority' (key=0)\n";
        queue1.enqueue("High Priority", 0);
        std::cout << "   peek() = " << queue1.peek() << std::endl;
        
        // Create second queue
        BinomialPriorityQueue queue2;
        std::cout << "\n5. Creating queue2:\n";
        queue2.enqueue("Urgent A", 2);
        std::cout << "   + 'Urgent A' (key=2)\n";
        queue2.enqueue("Urgent B", 8);
        std::cout << "   + 'Urgent B' (key=8)\n";
        queue2.enqueue("Urgent C", 1);
        std::cout << "   + 'Urgent C' (key=1)\n";
        queue2.print();
        
        // Merge
        std::cout << "\n6. Merging queue2 into queue1:\n";
        queue1.merge(std::move(queue2));
        std::cout << "   After merge:\n";
        queue1.print();
        std::cout << "   queue2 is empty? " << (queue2.isEmpty() ? "yes" : "no") << std::endl;
        
        // Remove all elements
        std::cout << "\n7. Removing all elements from queue1:\n";
        int step = 1;
        while (!queue1.isEmpty()) {
            std::cout << "   Step " << step++ << ": removing '" << queue1.peek()
                      << "' (size: " << queue1.getSize() << ")\n";
            queue1.dequeue();
        }
        std::cout << "   Queue is empty? " << (queue1.isEmpty() ? "yes" : "no") << std::endl;
        
        // Exception handling test
        std::cout << "\n8. Exception handling test:\n";
        try {
            std::cout << "   Trying to call peek() on empty queue...\n";
            queue1.peek();
        } catch (const EmptyQueueException& e) {
            std::cout << "   Exception: " << e.what() << std::endl;
        }
        
    } catch (const BadAllocException& e) {
        std::cerr << "Memory allocation error: " << e.what() << std::endl;
        return 1;
    } catch (const InvalidArgumentException& e) {
        std::cerr << "Validation error: " << e.what() << std::endl;
        return 1;
    } catch (const InvalidMergeException& e) {
        std::cerr << "Merge error: " << e.what() << std::endl;
        return 1;
    } catch (const EmptyQueueException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return 1;
    }
    
    return 0;
}