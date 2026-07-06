/*8. На основе интерфейса из задания 5 реализуйте класс фибоначчиевой приоритетной очереди.
Продемонстрируйте работу реализованного функционала.
*/

#include <iostream>
#include <cstring>
#include <climits>

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

struct Node {
    char *value;
    int key;
    int degree;
    bool marked;
    Node* parent;
    Node* child;
    Node* left;
    Node* right;

    Node(const char *val, int k)
    : key(k), degree(0), marked(false), parent(nullptr), child(nullptr) {
        value = new char[strlen(val) + 1];
        strcpy(value, val);
        left = this;
        right = this;
    }

    ~Node() {
        delete[] value;
    }
};

class FibonacciIPriorityQueue : public IPriorityQueue {
    Node* minNode;
    int nodeCount;

    //ВСПОГАТЕЛЬНЫЕ МЕТОДЫ
    void addToRootList(Node* node) {
        if (!minNode) {
            minNode = node;
            node->left = node;
            node->right = node;
        } else {
            node->right = minNode;
            node->left = minNode->left;
            minNode->left->right = node;
            minNode->left = node;
        }
    }

    void removeFromRootList(Node* node) {
        if (node == node->right) {
            minNode = nullptr;
        } else {
            node->left->right = node->right;
            node->right->left = node->left;
            if (minNode == node) {
                minNode = node->right;
            }
        }
        node->left = node;
        node->right = node;
    }

    void consolidate() {
        if (!minNode) {
            return;
        }
        int maxDegree = 45;
        Node** degreeTable = new Node*[maxDegree];

        Node* current = minNode;
        Node* start = minNode;
        bool finished = false;

        while (!finished) {
            Node* next = current->right;
            int deg = current->degree;
            while (degreeTable[deg]) {
                Node* other = degreeTable[deg];
                if (current->key > other->key) {
                    Node* temp = current;
                    current = other;
                    other = temp;
                }
                link(other, current);
                degreeTable[deg] = nullptr;
                deg++;
            }
            degreeTable[deg] = current;
            current = next;
            if (current == start) {
                finished = true;
            }
        }
        delete[] degreeTable;
        findNewMin();
    }

    void link(Node* child, Node* parent) {
        removeFromRootList(child);
        child->parent = parent;

        if (!parent->child) {
            parent->child = child;
            child->left = child;
            child->right = child;
        } else {
            child->right = parent->child;
            child->left = parent->child->left;
            parent->child->left->right = child;
            parent->child->left = child;
        }
        parent->degree++;
        child->marked = false;
    }

    void findNewMin() {
        if (!minNode) {
            return;
        }
        Node* current = minNode;
        Node* start = minNode;
        Node* newMin = minNode;

        do {
            if (current->key < newMin->key) {
                newMin = current;
            }
            current = current->right;
        } while (current != start);

        minNode = newMin;
    }

    void cut(Node* node, Node* parent) {
        if (node == node->right) {
            parent->child = nullptr;
        } else {
            node->left->right = node->right;
            node->right->left = node->left;
            if (parent->child == node) {
                parent->child = node->right;
            }
        }
        parent->degree--;
        addToRootList(node);
        node->parent = nullptr;
        node->marked = false;
    }

    void cascadingCut(Node* node) {
        Node* parent = node->parent;
        if (parent) {
            if (!node->marked) {
                node->marked = true;
            } else {
                cut(node, parent);
                cascadingCut(parent);
            }
        }
    }

    void clearTree(Node* node) {
        if (!node) {
            return;
        }
        Node* current = node;
        Node* start = node;

        do {
            Node* next = current->right;
            if (current->child) {
                clearTree(current->child);
            }
            delete current;
            current = next;
        } while (current != start);
    }

    void copyNode(Node*& dest, Node* src) {
        if (!src) {
            dest = nullptr;
            return;
        }
        
        dest = new Node(src->value, src->key);
        dest->degree = src->degree;
        dest->marked = src->marked;

        if (src->child) {
            copyNode(dest->child, src->child);
            dest->child->parent = dest;
        }
        if (src->right != src) {
            copyNode(dest->right, src->right);
            if (dest->right) {
                dest->right->left = dest;
            }
        }
    }
public:
    FibonacciIPriorityQueue() : minNode(nullptr), nodeCount(0) {}

    ~FibonacciIPriorityQueue() {
        if (minNode) {
            clearTree(minNode);
        }
    }

    FibonacciIPriorityQueue(const FibonacciIPriorityQueue& other)
    : minNode(nullptr), nodeCount(0) {
        if (other.minNode) {
            Node* current = other.minNode;
            Node* start = other.minNode;

            do {
                enqueue(current->value, current->key);
                current = current->right;
            } while (current != start);
        }
    }

    FibonacciIPriorityQueue& operator=(const FibonacciIPriorityQueue& other) {
        if (this != &other) {
            if (minNode) {
                clearTree(minNode);
                minNode = nullptr;
                nodeCount = 0;
            }
            if (other.minNode) {
                Node* current = other.minNode;
                Node* start = other.minNode;

                do {
                    enqueue(current->value, current->key);
                    current = current->right;
                } while (current != start);
            }
        }
        return *this;
    }

    FibonacciIPriorityQueue(FibonacciIPriorityQueue&& other) noexcept
    : minNode(other.minNode), nodeCount(other.nodeCount) {
        other.minNode = nullptr;
        other.nodeCount = 0;
    }

    FibonacciIPriorityQueue& operator=(FibonacciIPriorityQueue&& other) noexcept {
        if (this != & other) {
            if (minNode) {
                clearTree(minNode);
            }
            minNode = other.minNode;
            nodeCount = other.nodeCount;
            other.minNode = nullptr;
            other.nodeCount = 0;
        }
        return *this;
    }

    void enqueue(const char* value, int key) override {
        if (!value) {
            throw std::invalid_argument("Value cannot be null");
        }

        Node* newNode = nullptr;
        try {
            newNode = new Node(value, key);
        } catch (std::bad_alloc&) {
            throw std::runtime_error("Memory allocation failed");
        }

        addToRootList(newNode);

        if (!minNode || newNode->key < minNode->key) {
            minNode = newNode;
        }
        nodeCount++;
    }

    const char* peek() const override {
        if (!minNode) {
            throw std::runtime_error("Queue is empty");
        }
        return minNode->value;
    }

    void dequeue() override {
        if (!minNode) {
            throw std::runtime_error("Queue is empty");
        }
        Node* oldMin = minNode;

        if (oldMin->child) {
            Node* current = oldMin->right;
            Node* start = oldMin->child;

            do {
                Node* next = current->right;
                addToRootList(current);
                current->parent = nullptr;
                current = next;
            } while (current != start);
        }
        removeFromRootList(oldMin);

        if (oldMin == oldMin->right) {
            minNode = nullptr;
        } else {
            minNode = oldMin->right;
            consolidate();
        }

        delete oldMin;
        nodeCount--;
    }

    IPriorityQueue& merge(IPriorityQueue&& other) override {
        FibonacciIPriorityQueue* otherQueue = dynamic_cast<FibonacciIPriorityQueue*>(&other);

        if (!otherQueue) {
            throw std::invalid_argument("Cannot merge different queue types");
        }

        if (otherQueue->minNode) {
            if (!minNode) {
                minNode = otherQueue->minNode;
            } else {
                Node* thisLeft = minNode->left;
                Node* otherLeft = otherQueue->minNode->left;

                thisLeft->right = otherQueue->minNode;
                otherQueue->minNode->left = thisLeft;
                otherLeft->right = minNode;
                minNode->left = otherLeft;

                if (otherQueue->minNode->key < minNode->key) {
                    minNode = otherQueue->minNode;
                }
            }
            nodeCount += otherQueue->nodeCount;

            otherQueue->minNode = nullptr;
            otherQueue->nodeCount = 0;
        }
        return *this;
    }

    bool isEmpty() const {
        return minNode == nullptr;
    }

    int size() const {
        return nodeCount;
    }
};

int main() {
    try {
        std::cout << "=== Fibonacci Priority Queue ===\n\n";
        
        FibonacciIPriorityQueue queue1;
        
        // Добавление элементов
        std::cout << "Adding elements to queue 1:\n";
        queue1.enqueue("Task 1", 5);
        std::cout << "  Added: 'Task 1' with key 5\n";
        queue1.enqueue("Task 2", 3);
        std::cout << "  Added: 'Task 2' with key 3\n";
        queue1.enqueue("Task 3", 7);
        std::cout << "  Added: 'Task 3' with key 7\n";
        queue1.enqueue("Task 4", 1);
        std::cout << "  Added: 'Task 4' with key 1\n";
        queue1.enqueue("Task 5", 4);
        std::cout << "  Added: 'Task 5' with key 4\n";
        
        std::cout << "\nHighest priority element: " << queue1.peek() 
                  << " (key: smallest)\n";
        std::cout << "Queue size: " << queue1.size() << "\n\n";
        
        // Удаление элементов
        std::cout << "Removing elements by priority:\n";
        while (!queue1.isEmpty()) {
            std::cout << "  Removing: " << queue1.peek() << "\n";
            queue1.dequeue();
        }
        
        // Создание двух очередей для демонстрации слияния
        std::cout << "\n=== Queue Merge Demonstration ===\n";
        
        FibonacciIPriorityQueue queueA;
        FibonacciIPriorityQueue queueB;
        
        queueA.enqueue("A: Low priority", 10);
        queueA.enqueue("A: Medium priority", 5);
        queueA.enqueue("A: High priority", 1);
        
        queueB.enqueue("B: Low priority", 8);
        queueB.enqueue("B: Medium priority", 3);
        queueB.enqueue("B: Very high priority", 2);
        
        std::cout << "\nQueue A (size " << queueA.size() << "):\n";
        std::cout << "  Highest priority: " << queueA.peek() << "\n";
        
        std::cout << "Queue B (size " << queueB.size() << "):\n";
        std::cout << "  Highest priority: " << queueB.peek() << "\n";
        
        // Слияние
        queueA.merge(std::move(queueB));
        std::cout << "\nAfter merge, Queue A (size " << queueA.size() << "):\n";
        
        std::cout << "All elements by priority:\n";
        while (!queueA.isEmpty()) {
            std::cout << "  " << queueA.peek() << "\n";
            queueA.dequeue();
        }
        
        // Демонстрация конструктора копирования
        std::cout << "\n=== Copy Constructor Demonstration ===\n";
        FibonacciIPriorityQueue original;
        original.enqueue("Original 1", 5);
        original.enqueue("Original 2", 3);
        
        FibonacciIPriorityQueue copy(original);
        
        std::cout << "Original: " << original.peek() << "\n";
        std::cout << "Copy: " << copy.peek() << "\n";
        
        original.dequeue();
        std::cout << "After removing from original:\n";
        std::cout << "  Original: " << original.peek() << "\n";
        std::cout << "  Copy: " << copy.peek() << " (unchanged)\n";
        
        // Демонстрация перемещения
        std::cout << "\n=== Move Constructor Demonstration ===\n";
        FibonacciIPriorityQueue movedFrom;
        movedFrom.enqueue("Movable 1", 1);
        movedFrom.enqueue("Movable 2", 2);
        
        FibonacciIPriorityQueue movedTo(std::move(movedFrom));
        std::cout << "After moving:\n";
        std::cout << "  movedFrom is empty? " << (movedFrom.isEmpty() ? "Yes" : "No") << "\n";
        std::cout << "  movedTo size: " << movedTo.size() << "\n";
        std::cout << "  movedTo min: " << movedTo.peek() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nProgram completed successfully.\n";
    return 0;
}
