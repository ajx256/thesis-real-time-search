template <typename T>
class KHeap {
public:
    KHeap(unsigned int capacity, std::function<int(const T&, const T&)> comparator)
            : capacity{capacity}, comparator{comparator}, queue{capacity}, size{0} {}

    void push(T item) {
        if (size == capacity) {
			// Exchange worst item for new item if new item is better than worst
		}

        if (size == 0) {
            queue[0] = item;
        } else {
            siftUp(size, item);
        }

        ++size;
    }

    void pop() {
        --size;
        auto x = queue[size];

        if (size != 0) {
            siftDown(0, x);
        }
	}

    T top() const {
        return queue[0];
    }

    void clear() { size = 0; }

    /*void insertOrUpdate(T& item) {
        if (item.index == std::numeric_limits<unsigned int>::max()) {
            // Item is not in the queue yet
            push(item);
        } else {
            // Already in the queue
            update(item);
        }
    }*/

	/*void update(T& item) {
        auto index = item.index;

        assert(index != std::numeric_limits<unsigned int>::max() && "Cannot update a node that is not in the queue!");

        siftUp(item.index, item);

        if (item.index == index) {
            siftDown(item.index, item);
        }
    }*/

    void reorder(std::function<int(const T&, const T&)> comparator) {
        this->comparator = comparator;
        for (unsigned int i = 0; i < size; i++) {
            T item = queue[i];
            siftDown(i, item);
        }
    }

    void forEach(std::function<void(T)> action) {
        for (unsigned int i = 0; i < size; i++) {
            action(queue[i]);
        }
    }

    unsigned int getCapacity() const { return capacity; }

    unsigned int getSize() const { return size; }

    bool isEmpty() const { return size == 0; }

    bool isNotEmpty() const { return size != 0; }

private:
    void siftUp(const unsigned int index, T item) {
        unsigned int currentIndex = index;
        while (currentIndex > 0) {
            const auto parentIndex = (currentIndex - 1) >> 1;
            const auto parentItem = queue[parentIndex];

            if (comparator(item, parentItem) >= 0) {
                break;
            }

            // Move parent down and update its index
            queue[currentIndex] = parentItem;
            currentIndex = parentIndex;
        }

        queue[currentIndex] = item;
    }

    void siftDown(const unsigned int index, T item) {
        auto currentIndex = index;
        const unsigned int half = size >> 1;

        while (currentIndex < half) {
            auto childIndex = (currentIndex << 1) + 1;
            auto childItem = queue[childIndex];
            auto rightIndex = childIndex + 1;

            if (rightIndex < size && comparator(childItem, queue[rightIndex]) > 0) {
                childIndex = rightIndex;
                childItem = queue[rightIndex];
            }

            if (comparator(item, childItem) <= 0) {
                break;
            }

            queue[currentIndex] = childItem;
            currentIndex = childIndex;
        }

        queue[currentIndex] = item;
    }

    const unsigned int capacity;
    std::function<int(const T&, const T&)> comparator;
    std::vector<T> queue;
    unsigned int size;
};