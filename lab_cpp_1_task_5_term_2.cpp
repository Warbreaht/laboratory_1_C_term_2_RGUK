/*5. Опишите интерфейс приоритетной очереди. Интерфейс должен предоставлять следующие
методы:
a. добавление значения типа char * по ключу типа int в приоритетную очередь
b. поиск значения по наиболее приоритетному ключу
c. удаление значения по наиболее приоритетному ключу
d. слияние двух приоритетных очередей в вызывающий объект приоритетной очереди, с
поддержкой fluent API.*/

#include <iostream>
#include <cstring>

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

// Узел очереди
struct Node {
    int key;
    char* value;

    // Конструктор
    Node(int k = 0, const char* v = "") : key(k), value(nullptr) {
        if (v) {
            value = new char[std::strlen(v) + 1];
            std::strcpy(value, v);
        } else {
            value = new char[1];
            value[0] = '\0';
        }
    }

    // Конструктор копирования
    Node(const Node& other) : key(other.key), value(nullptr) {
        if (other.value) {
            value = new char[std::strlen(other.value) + 1];
            std::strcpy(value, other.value);
        } else {
            value = new char[1];
            value[0] = '\0';
        }
    }

    // Оператор присваивания
    Node& operator=(const Node& other) {
        if (this != &other) {
            key = other.key;
            delete[] value;
            if (other.value) {
                value = new char[std::strlen(other.value) + 1];
                std::strcpy(value, other.value);
            } else {
                value = new char[1];
                value[0] = '\0';
            }
        }
        return *this;
    }

    // Конструктор перемещения
    Node(Node&& other) noexcept : key(other.key), value(other.value) {
        other.key = 0;
        other.value = nullptr;
    }

    // Оператор присваивания перемещением
    Node& operator=(Node&& other) noexcept {
        if (this != &other) {
            delete[] value;
            key = other.key;
            value = other.value;
            other.key = 0;
            other.value = nullptr;
        }
        return *this;
    }

    // Деструктор
    ~Node() {
        delete[] value;
    }
};

// Реализация приоритетной очереди на основе двоичной кучи (min-heap)
class PriorityQueue : public IPriorityQueue {
private:
    Node* heap_array_;
    int capacity_;
    int current_size_;

    // Увеличение размера массива
    void resize() {
        capacity_ *= 2;
        Node* new_array = new Node[capacity_];

        for (int i = 0; i < current_size_; ++i) {
            new_array[i] = std::move(heap_array_[i]);  // перемещаем узлы
        }

        delete[] heap_array_;
        heap_array_ = new_array;
    }

    int parent(int i) const { return (i - 1) / 2; }
    int left_child(int i) const { return 2 * i + 1; }
    int right_child(int i) const { return 2 * i + 2; }

    // Восстановление свойств кучи при добавлении (просеивание вверх)
    void heapify_up(int index) {
        while (index > 0 && heap_array_[parent(index)].key > heap_array_[index].key) {
            std::swap(heap_array_[index], heap_array_[parent(index)]);
            index = parent(index);
        }
    }

    // Восстановление свойств кучи при удалении (просеивание вниз)
    void heapify_down(int index) {
        int smallest = index;
        int left = left_child(index);
        int right = right_child(index);

        if (left < current_size_ && heap_array_[left].key < heap_array_[smallest].key) {
            smallest = left;
        }
        if (right < current_size_ && heap_array_[right].key < heap_array_[smallest].key) {
            smallest = right;
        }
        if (smallest != index) {
            std::swap(heap_array_[index], heap_array_[smallest]);
            heapify_down(smallest);
        }
    }

public:
    // Конструктор
    PriorityQueue() : capacity_(4), current_size_(0) {
        heap_array_ = new Node[capacity_];
    }

    // Деструктор
    ~PriorityQueue() {
        delete[] heap_array_;
    }

    PriorityQueue(const PriorityQueue&) = delete;

    PriorityQueue& operator=(const PriorityQueue&) = delete;

    // Конструктор перемещения
    PriorityQueue(PriorityQueue&& other) noexcept
        : heap_array_(other.heap_array_)
        , capacity_(other.capacity_)
        , current_size_(other.current_size_) {
        other.heap_array_ = nullptr;
        other.capacity_ = 0;
        other.current_size_ = 0;
    }

    // Оператор присваивания перемещением
    PriorityQueue& operator=(PriorityQueue&& other) noexcept {
        if (this != &other) {
            delete[] heap_array_;
            heap_array_ = other.heap_array_;
            capacity_ = other.capacity_;
            current_size_ = other.current_size_;
            other.heap_array_ = nullptr;
            other.capacity_ = 0;
            other.current_size_ = 0;
        }
        return *this;
    }

    // a. Добавление элемента
    void enqueue(const char* value, int key) override {
        if (value == nullptr) {
            std::cerr << "Error: value cannot be nullptr" << std::endl;
            return;
        }

        if (current_size_ == capacity_) {
            resize();
        }

        heap_array_[current_size_] = Node(key, value);
        heapify_up(current_size_);
        current_size_++;

        std::cout << "Added: {" << key << ", " << value << "}" << std::endl;
    }

    // b. Поиск значения по наиболее приоритетному ключу (наименьший ключ)
    const char* peek() const override {
        if (current_size_ == 0) {
            std::cerr << "Queue is empty" << std::endl;
            return nullptr;
        }
        return heap_array_[0].value;
    }

    // c. Удаление элемента с наиболее приоритетным ключом
    void dequeue() override {
        if (current_size_ == 0) {
            std::cerr << "Queue is empty, cannot dequeue" << std::endl;
            return;
        }

        std::cout << "Removed: {" << heap_array_[0].key << ", " << heap_array_[0].value << "}" << std::endl;

        // Перемещаем последний элемент в корень
        heap_array_[0] = std::move(heap_array_[current_size_ - 1]);
        current_size_--;

        if (current_size_ > 0) {
            heapify_down(0);
        }
    }

    // d. Слияние двух очередей (fluent API)
    IPriorityQueue& merge(IPriorityQueue&& other) override {
        // Пытаемся привести к типу PriorityQueue
        PriorityQueue* other_queue = dynamic_cast<PriorityQueue*>(&other);
        if (!other_queue) {
            std::cerr << "Error: cannot merge different queue types" << std::endl;
            return *this;
        }

        std::cout << "\n--- Merging queues ---" << std::endl;

        // Временно сохраняем все элементы из other_queue
        int other_size = other_queue->current_size_;
        if (other_size == 0) {
            std::cout << "Other queue is empty, nothing to merge" << std::endl;
            return *this;
        }

        // Создаём копии узлов, так как other_queue будет изменён
        Node* nodes_to_move = new Node[other_size];
        for (int i = 0; i < other_size; ++i) {
            nodes_to_move[i] = other_queue->heap_array_[i];
        }

        // Очищаем other_queue
        other_queue->current_size_ = 0;

        // Добавляем все элементы в текущую очередь
        for (int i = 0; i < other_size; ++i) {
            this->enqueue(nodes_to_move[i].value, nodes_to_move[i].key);
            std::cout << "  Moved: {" << nodes_to_move[i].key << ", " << nodes_to_move[i].value << "}" << std::endl;
        }

        delete[] nodes_to_move;
        std::cout << "--- Merge completed ---\n" << std::endl;

        return *this;
    }

    // Вспомогательный метод для вывода очереди (для демонстрации)
    void print_queue() const {
        if (current_size_ == 0) {
            std::cout << "Queue is empty" << std::endl;
            return;
        }

        std::cout << "Queue contents (key, value): ";
        for (int i = 0; i < current_size_; ++i) {
            std::cout << "{" << heap_array_[i].key << ", " << heap_array_[i].value << "} ";
        }
        std::cout << std::endl;
    }

    // Получение размера очереди
    int size() const { return current_size_; }

    // Проверка на пустоту
    bool empty() const { return current_size_ == 0; }
};

// Функция для демонстрации работы очереди
void demonstrate_queue() {
    std::cout << "=== Demonstration of PriorityQueue ===\n" << std::endl;

    // Создание первой очереди
    PriorityQueue q1;
    std::cout << "--- Queue 1 ---" << std::endl;
    q1.enqueue("Task A", 5);
    q1.enqueue("Task B", 2);
    q1.enqueue("Task C", 8);
    q1.enqueue("Task D", 1);
    q1.enqueue("Task E", 3);

    std::cout << "\nQueue 1 after additions: ";
    q1.print_queue();

    // Демонстрация peek
    std::cout << "\n--- Peek (most priority) ---" << std::endl;
    const char* top = q1.peek();
    if (top) {
        std::cout << "Most priority element value: " << top << std::endl;
    }

    // Демонстрация dequeue
    std::cout << "\n--- Dequeue operations ---" << std::endl;
    q1.dequeue();
    std::cout << "After dequeue: ";
    q1.print_queue();

    q1.dequeue();
    std::cout << "After second dequeue: ";
    q1.print_queue();

    // Создание второй очереди для демонстрации слияния
    PriorityQueue q2;
    std::cout << "\n--- Queue 2 ---" << std::endl;
    q2.enqueue("Task X", 4);
    q2.enqueue("Task Y", 6);
    q2.enqueue("Task Z", 0);  // Самый приоритетный (ключ 0)

    std::cout << "\nQueue 2: ";
    q2.print_queue();

    // Демонстрация слияния (fluent API)
    std::cout << "\n--- Merging q2 into q1 ---" << std::endl;
    q1.merge(std::move(q2));

    std::cout << "Queue 1 after merge: ";
    q1.print_queue();

    std::cout << "\nQueue 2 after merge (should be empty): ";
    q2.print_queue();

    // Демонстрация пустой очереди
    std::cout << "\n--- Empty queue handling ---" << std::endl;
    PriorityQueue empty_q;
    std::cout << "Empty queue peek: ";
    empty_q.peek();
    std::cout << "Empty queue dequeue: ";
    empty_q.dequeue();
}

int main() {
    demonstrate_queue();
    return 0;
}