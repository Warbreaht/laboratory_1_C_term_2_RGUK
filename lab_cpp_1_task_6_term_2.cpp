/*6. На основе интерфейса из задания 5 реализуйте класс двоичной приоритетной очереди.
Продемонстрируйте работу реализованного функционала.*/

#include <iostream>
#include <cstring>
#include <stdexcept>

// Интерфейс приоритетной очереди
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

class BinaryPriorityQueue : public IPriorityQueue {
    struct Node {
        char* value;
        int key;

        //Контруктор
        Node(const char* val, int k) : key(k) {
            value = new char[strlen(val) + 1];
            strcpy(value, val);
        }
        //Деструктор
        ~Node() {
            delete[] value;
        }
        //запрет копирования
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
    };

    Node** heap;
    int size;
    int capacity;

    //функция увеличения размера массива heap(лениво)
    void resize() {
        int newCapacity = capacity * 2;
        Node** newHeap = new Node*[newCapacity];

        for (int i = 0; i < size; i++) {
            newHeap[i] = heap[i];
        }
        delete[] heap;
        heap = newHeap;
        capacity = newCapacity;
    }
    //Смена мест узлов между собой по индексу
    void swapNodes(int i, int j) {
        Node* temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    //Поднятие узла вверх по дереву
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index]->key < heap[parent]->key) {
                swapNodes(index, parent);
            } else {
                break;
            }
        }
    }
    //спуск узла по дереву
    void heapfyDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < size && heap[left]->key < heap[smallest]->key) {
                smallest = left;
            }
            if (right < size && heap[right]->key < heap[smallest]->key) {
                smallest = right;
            }
            if (smallest != index) {
                swapNodes(index, smallest);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    //конструктор(по умолчанию)
    BinaryPriorityQueue(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        heap = new Node*[capacity];
    }
    //деструктор
    ~BinaryPriorityQueue() override {
        // override переопределение виртульной функции
        for (int i = 0; i < size; i++) {
            delete heap[i];
        }
        delete[] heap;
    }
    //запрет копирования(избежания поверхстного копирования)
    BinaryPriorityQueue(const BinaryPriorityQueue&) = delete;
    BinaryPriorityQueue& operator=(const BinaryPriorityQueue&) = delete;
    //контрусктор премещения?
    BinaryPriorityQueue(BinaryPriorityQueue&& other) noexcept : heap(other.heap), capacity(other.capacity), size(other.size) {
        other.heap = nullptr;
        other.capacity = 0;
        other.size = 0;
    }
    //добалвние элемента
    void enqueue(const char* value, int key) override {
        if (size >= capacity) {
            resize();
        }
        heap[size] = new Node(value, key);
        heapifyUp(size);
        size++;
    }
    //родительский элемент
    const char* peek() const override {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return heap[0]->value;
    }
    //Удаление родительского элемента
    void dequeue() override {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        delete heap[0];
        heap[0] = heap[size - 1];
        size--;
        if (size > 0) {
            heapfyDown(0);
        }
    }
    //слияние двух очередей
    IPriorityQueue& merge(IPriorityQueue&& other) override {
        BinaryPriorityQueue* otherQueue = dynamic_cast<BinaryPriorityQueue*>(&other);

        if (!otherQueue) {
            throw std::runtime_error("Cannot merge with different queue type");
        }
        for (int i = 0; i < otherQueue->size; i++) {
            enqueue(otherQueue->heap[i]->value, otherQueue->heap[i]->key);
        }
        for (int i = 0; i < otherQueue->size; i++) {
            delete otherQueue->heap[i];
        }
        otherQueue->size = 0;
        return *this;
    }

    //---ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ---

    //проверека на пустоту
    bool isEmpty() const {
        return size == 0;
    }
    //получить размер массива
    int getSize() const {
        return size;
    }
    //вывод кучи
    void print() const {
        std::cout << "Priority Queue (size=" << size << "): ";
        for (int i = 0; i < size; i++) {
            std::cout << "[" << heap[i]->value << ":" << heap[i]->key << "]";
        }
        std::cout << std::endl;
    }
};

// Демонстрация работы
int main() {
    try {
        std::cout << "=== Demonstration of a binary priority queue ===\n\n";
        
        // Создаем первую очередь
        BinaryPriorityQueue queue1;
        std::cout << "1. Add the elements first:\n";
        queue1.enqueue("Task1", 5);
        queue1.enqueue("Task2", 2);
        queue1.enqueue("Task3", 8);
        queue1.enqueue("Task4", 1);
        queue1.enqueue("Task5", 3);
        queue1.print();
        
        // Демонстрация peek
        std::cout << "\n2. The most important element: " << queue1.peek() << std::endl;
        
        // Демонстрация dequeue
        std::cout << "\n3. Remove the most important elements:\n";
        while (!queue1.isEmpty()) {
            std::cout << "   Deleting it: " << queue1.peek() << std::endl;
            queue1.dequeue();
            std::cout << "   Queue after deletion: ";
            queue1.print();
        }
        
        // Добавляем новые элементы для демонстрации слияния
        std::cout << "\n4. Add items to merge:4. Add items to merge:\n";
        queue1.enqueue("A", 10);
        queue1.enqueue("B", 20);
        queue1.enqueue("C", 5);
        queue1.print();
        
        // Создаем вторую очередь
        BinaryPriorityQueue queue2;
        queue2.enqueue("X", 3);
        queue2.enqueue("Y", 7);
        queue2.enqueue("Z", 1);
        std::cout << "5. The second stage:\n";
        queue2.print();
        
        // Слияние очередей
        std::cout << "\n6. Merge the second queue into the first queue:\n";
        queue1.merge(std::move(queue2));
        std::cout << "   The result of the merger: ";
        queue1.print();
        
        // Показываем, что вторая очередь теперь пуста
        std::cout << "   The second stage after the merger: ";
        queue2.print();
        
        // Извлекаем все элементы в порядке приоритета
        std::cout << "\n7. Extract all elements in order of priority:\n";
        while (!queue1.isEmpty()) {
            std::cout << "   " << queue1.peek() << " (priority: ";
            // Для демонстрации пришлось бы хранить ключи, но в нашем интерфейсе нет метода для получения ключа
            std::cout << "?)" << std::endl;
            queue1.dequeue();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Mistake: " << e.what() << std::endl;
    }
    
    return 0;
}