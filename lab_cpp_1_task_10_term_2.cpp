/*10.На основе интерфейса из задания 5 реализуйте класс косой приоритетной очереди.
Продемонстрируйте работу реализованного функционала*/
#include <iostream>
#include <cstring>
#include <stdexcept>

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
    char* value;
    int key;
    Node* left;
    Node* right;

    Node(const char* val, int k)
    : key(k), left(nullptr), right(nullptr) {
        value = new char[std::strlen(val) + 1];
        std::strcpy(value, val);
    }

    ~Node() {
        delete[] value;
    }
};

class SkewPriorityQueue : public IPriorityQueue {
    Node* root;
    int nodeCount;

    Node* mergeNodes(Node* a, Node* b) {
        if (!a) {
            return b;
        }
        if (!b) {
            return a;
        }

        if (a->key > b->key){
            Node* temp = a;
            a = b;
            b = temp;
        }

        a->right = mergeNodes(a->right, b);

        Node* temp = a->left;
        a->left = a->right;
        a->right = temp;

        return a;
    }

    void clearTree(Node* node) {
        if (!node) {
            return;
        }
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

    Node* copyTree(Node* node) {
        if (!node) {
            return nullptr;
        }
        Node* newNode = new Node(node->value, node->key);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);

        return newNode;
    }
public:
    SkewPriorityQueue() : root(nullptr), nodeCount(0) {}

    ~SkewPriorityQueue() {
        clearTree(root);
    }

    SkewPriorityQueue(const SkewPriorityQueue& other)
    : root(nullptr), nodeCount(other.nodeCount) {
        root = copyTree(other.root);
    }

    SkewPriorityQueue& operator=(const SkewPriorityQueue& other) {
        if (this != &other) {
            clearTree(root);
            root = copyTree(other.root);
            nodeCount = other.nodeCount;
        }
        return *this;
    }

    SkewPriorityQueue(SkewPriorityQueue&& other) noexcept
    : root(other.root), nodeCount(other.nodeCount) {
        other.root = nullptr;
        other.nodeCount = 0;
    }

    SkewPriorityQueue& operator=(SkewPriorityQueue&& other) noexcept {
        if (this != & other) {
            clearTree(root);
            root = other.root;
            nodeCount = other.nodeCount;
            other.root = nullptr;
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
        } catch (const std::bad_alloc&) {
            throw std::runtime_error("Memory allocation failed");
        }

        root = mergeNodes(root, newNode);
        nodeCount++;
    }

    const char* peek() const override {
        if (!root) {
            throw std::runtime_error("Queue is empty");
        }
        return root->value;
    }

    void dequeue() override {
        if (!root) {
            throw std::runtime_error("Queue is empty");
        }
        
        Node* oldRoot = root;
        root = mergeNodes(root->left, root->right);
        delete oldRoot;
        nodeCount--;
    }

    IPriorityQueue& merge(IPriorityQueue&& other) override {
        SkewPriorityQueue* otherQueue = dynamic_cast<SkewPriorityQueue*>(&other);

        if (!otherQueue) {
            throw std::invalid_argument("Cannot merge different queue types");
        }

        root = mergeNodes(root, otherQueue->root);
        nodeCount += otherQueue->nodeCount;

        otherQueue->root = nullptr;
        otherQueue->nodeCount = 0;

        return *this;
    }

    bool isEmpty() const {
        return root == nullptr;
    }
    
    int size() const {
        return nodeCount;
    }
};

int main() {
    try {
        std::cout << "=== Skew Priority Queue ===\n\n";
        
        SkewPriorityQueue queue1;
        
        // Добавление элементов
        std::cout << "Adding elements:\n";
        queue1.enqueue("Task 1", 10);
        queue1.enqueue("Task 2", 5);
        queue1.enqueue("Task 3", 15);
        queue1.enqueue("Task 4", 3);
        queue1.enqueue("Task 5", 7);
        queue1.enqueue("Task 6", 1);
        
        std::cout << "Queue size: " << queue1.size() << "\n";
        std::cout << "Highest priority element: " << queue1.peek() << "\n\n";
        
        // Удаление элементов
        std::cout << "Removing elements by priority:\n";
        while (!queue1.isEmpty()) {
            std::cout << "  Removing: " << queue1.peek() << "\n";
            queue1.dequeue();
        }
        
        // Демонстрация слияния
        std::cout << "\n=== Merge Demonstration ===\n";
        
        SkewPriorityQueue queueA;
        SkewPriorityQueue queueB;
        
        queueA.enqueue("A1", 8);
        queueA.enqueue("A2", 3);
        queueA.enqueue("A3", 6);
        
        queueB.enqueue("B1", 7);
        queueB.enqueue("B2", 2);
        queueB.enqueue("B3", 5);
        queueB.enqueue("B4", 1);
        
        std::cout << "Queue A size: " << queueA.size() << ", highest: " << queueA.peek() << "\n";
        std::cout << "Queue B size: " << queueB.size() << ", highest: " << queueB.peek() << "\n";
        
        queueA.merge(std::move(queueB));
        
        std::cout << "After merge - Queue A size: " << queueA.size() << "\n";
        std::cout << "All elements in priority order:\n";
        
        int order = 1;
        while (!queueA.isEmpty()) {
            std::cout << "  " << order++ << ". " << queueA.peek() << "\n";
            queueA.dequeue();
        }
        
        // Демонстрация копирования
        std::cout << "\n=== Copy Demonstration ===\n";
        SkewPriorityQueue original;
        original.enqueue("Original", 100);
        original.enqueue("Copy test", 50);
        
        SkewPriorityQueue copy(original);
        
        std::cout << "Original: " << original.peek() << "\n";
        std::cout << "Copy: " << copy.peek() << "\n";
        
        original.dequeue();
        std::cout << "After modifying original:\n";
        std::cout << "  Original: " << original.peek() << "\n";
        std::cout << "  Copy: " << copy.peek() << " (unchanged)\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nProgram completed successfully.\n";
    return 0;
}
