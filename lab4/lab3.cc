#include <algorithm>
#include <iostream>
#include <random>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>

using namespace std;

// Структура узла дерева
struct Node {
    int key;
    int height;
    int index;
    Node *left;
    Node *right;

    Node(int k, int index)
        : key(k), height(1), index(index), left(nullptr), right(nullptr) {}
};

// Функция вычисления высоты дерева
int height(Node *node) {
    if (node == nullptr)
        return 0;
    return node->height;
}

// Функция для обновления высоты узла
void updateHeight(Node *node) {
    if (node == nullptr)
        return;
    node->height = max(height(node->left), height(node->right)) + 1;
}

// Вращение узла вправо
Node *rotateRight(Node *p) {
    Node *q = p->left;
    p->left = q->right;
    q->right = p;
    updateHeight(p);
    updateHeight(q);
    return q;
}

// Вращение узла влево
Node *rotateLeft(Node *q) {
    Node *p = q->right;
    q->right = p->left;
    p->left = q;
    updateHeight(q);
    updateHeight(p);
    return p;
}

// Балансировка узла
Node *balance(Node *p) {
    updateHeight(p);
    if (height(p->left) - height(p->right) == 2) {
        if (height(p->left->right) > height(p->left->left))
            p->left = rotateLeft(p->left);
        return rotateRight(p);
    }
    if (height(p->right) - height(p->left) == 2) {
        if (height(p->right->left) > height(p->right->right))
            p->right = rotateRight(p->right);
        return rotateLeft(p);
    }
    return p;
}

// Вставка ключа в дерево
Node *insert(Node *p, int k, int index) {
    if (!p)
        return new Node(k, index);
    if (k < p->key)
        p->left = insert(p->left, k, index);
    else
        p->right = insert(p->right, k, index);
    return balance(p);
}

// Создание множества случайных уникальных чисел
unordered_set<int> generateSet(int size, int minVal, int maxVal) {
    unordered_set<int> s;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minVal, maxVal);

    while (s.size() < size) {
        s.insert(dis(gen));
    }

    return s;
}

// Построение дерева из множества чисел
Node *buildSetTree(const unordered_set<int> &s) {
    Node *root = nullptr;
    for (int num : s) {
        root = insert(root, num, 0);
    }
    return root;
}

Node *buildSequenceTree(const vector<tuple<int, int>> &s) {
    Node *root = nullptr;
    for (tuple<int, int> nums : s) {
        root = insert(root, get<0>(nums), get<1>(nums));
    }
    return root;
}

// Печать элементов дерева в порядке возрастания
void inOrderTraversal(Node *p) {
    if (p) {
        inOrderTraversal(p->left);
//        cout << p->key << " ";
        inOrderTraversal(p->right);
    }
}

// Построение множества из элементов дерева
void iterate(Node *p, vector<tuple<int, int>> &s) {
    if (p) {
        s.push_back({p->key, p->index});
        iterate(p->left, s);
        iterate(p->right, s);
    }
}

void inOrderTraversalSequence(Node *p) {
    vector<tuple<int, int>> s;
    iterate(p, s);
    for (int i = 0; i < s.size(); i++) {
        for (int j = i + 1; j < s.size(); j++) {
            if (get<1>(s[i]) > get<1>(s[j])) {
                swap(s[i], s[j]);
            }
        }
    }

    for (tuple<int, int> nums : s) {
//        cout << get<0>(nums) << " ";
    }
}

// Поиск узла с минимальным ключом
Node *findMin(Node *p) { return p->left ? findMin(p->left) : p; }

// Удаление узла с минимальным ключом
Node *removeMin(Node *p) {
    if (!p->left)
        return p->right;
    p->left = removeMin(p->left);
    return balance(p);
}

Node *remove(Node *p, int k, int index) {
    if (!p)
        return nullptr;
    if (k < p->key)
        p->left = remove(p->left, k, index);
    else if (k > p->key)
        p->right = remove(p->right, k, index);
    else {
        Node *q = p->left;
        Node *r = p->right;
        delete p;
        if (!r)
            return q;
        Node *min = findMin(r);
        min->right = removeMin(r);
        min->left = q;
        return balance(min);
    }
    return balance(p);
}

Node *concatenate(Node *A, Node *B) {
    Node *result = nullptr;
    vector<tuple<int, int>> s;
    iterate(A, s);

    vector<tuple<int, int>> s1;
    iterate(B, s1);

    int maxIndex = 0;
    for (tuple<int, int> nums : s) {
        result = insert(result, get<0>(nums), get<1>(nums));
        maxIndex++;
    }
    for (tuple<int, int> nums : s1) {
        result = insert(result, get<0>(nums), maxIndex + get<1>(nums));
    }
    return result;
}

Node *merge(Node *p1, Node *p2) {
    Node *concatenated = nullptr;
    concatenated = concatenate(p1, p2);
    vector<tuple<int, int>> s;
    iterate(concatenated, s);

    unordered_map<int, int> freq;
    for (tuple<int, int> nums : s) {
        freq[get<0>(nums)] = get<1>(nums);
    }
    Node *result = nullptr;
    for (auto it = freq.begin(); it != freq.end(); it++) {
        result = insert(result, it->first, it->second);
    }
    return result;
}

Node *subst(Node *A, Node *B) {
    Node *result = nullptr;
    vector<tuple<int, int>> s;
    iterate(A, s);

    vector<tuple<int, int>> s1;
    iterate(B, s1);

    unordered_map<int, int> freq;
    for (tuple<int, int> nums : s) {
        freq[get<0>(nums)] = get<1>(nums);
    }

    for (tuple<int, int> nums : s1) {
        if (freq.find(get<0>(nums)) != freq.end()) {
            A = remove(A, get<0>(nums), get<1>(nums));
        }
    }

    return A;
}

void count(Node *root, unordered_map<int, int> &freq) {
    if (!root)
        return;

    // Увеличиваем счетчик для текущего значения
    freq[root->key]++;

    // Рекурсивно обрабатываем левое и правое поддеревья
    count(root->left, freq);
    count(root->right, freq);
}

Node *intersection(Node *p1, Node *p2) {
    if (!p1 || !p2)
        return nullptr;

    if (p1->key == p2->key) {
        Node *intersected = new Node(p1->key, p1->index);
        intersected->left = intersection(p1->left, p2->left);
        intersected->right = intersection(p1->right, p2->right);
        intersected->height =
            max(height(intersected->left), height(intersected->right)) + 1;
        return intersected;
    } else if (p1->key < p2->key) {
        return intersection(p1->right, p2->left);
    } else {
        return intersection(p1->left, p2->right);
    }
}

Node *x_or(Node *A, Node *B) {

    Node *result = nullptr;
    Node *concatenated = concatenate(A, B);
    unordered_map<int, int> freq;
    count(concatenated, freq);

    for (auto it = freq.begin(); it != freq.end(); it++) {
        if (it->second == 1) {
            result = insert(result, it->first, 0);
        }
    }
    return result;
}

// Вычисление операции A ∩ B ⊕ C ∩ D ∩ E
Node *operation(Node *A, Node *B, Node *C, Node *D, Node *E) {
    Node *result = nullptr;
    Node *AB = intersection(A, B);
//    cout << "Intersection of A B: ";
    inOrderTraversal(AB);
//    cout << endl;
    Node *CD = intersection(C, D);
//    cout << "Intersection of C D: ";
    inOrderTraversal(CD);
//    cout << endl;
    Node *CDE = intersection(CD, E);
//    cout << "Intersection of C D E: ";
    inOrderTraversal(CDE);
//    cout << endl;
    result = x_or(AB, CDE);
    return result;
}

void sequenceOperation(Node *A, Node *B) {
    Node *AB = concatenate(A, B);
//    cout << "Concatenation of A B: ";
    inOrderTraversalSequence(AB);
//    cout << endl;
    Node *AB2 = merge(A, B);
//    cout << "Merge of A B: ";
    inOrderTraversalSequence(AB2);
//    cout << endl;
    Node *AB3 = subst(A, B);
//    cout << "Subst of A B: ";
    inOrderTraversalSequence(AB3);
//    cout << endl;
}

int main() {
    for (int i = 10; i < 400; ++i) {
        unordered_set<int> a = generateSet(10, 1, 50);
        unordered_set<int> b = generateSet(10, 1, 50);
        unordered_set<int> c = generateSet(10, 1, 50);
        unordered_set<int> d = generateSet(10, 1, 50);
        unordered_set<int> e = generateSet(10, 1, 50);


        Node *A = buildSetTree(a);
        Node *B = buildSetTree(b);
        Node *C = buildSetTree(c);
        Node *D = buildSetTree(d);
        Node *E = buildSetTree(e);


        auto t1 = chrono::high_resolution_clock::now();
        Node *result = operation(A, B, C, D, E);
        auto t2 = chrono::high_resolution_clock::now();
        cout << i << ";" << chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() << endl;



    }
    // Генерация множеств A, B, C, D, E


//    vector<tuple<int, int>> s = {{1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 4}};
//    cout << "Sequence1: ";
//    for (tuple<int, int> nums : s) {
//        cout << get<0>(nums) << " ";
//    }
//    cout << endl;
//    Node *root = buildSequenceTree(s);
//    vector<tuple<int, int>> s1 = {{4, 0}, {5, 1}, {6, 2}, {7, 3}, {8, 4}};
//    cout << "Sequence2: ";
//    for (tuple<int, int> nums : s1) {
//        cout << get<0>(nums) << " ";
//    }
//    cout << endl;
//    Node *root1 = buildSequenceTree(s1);
//    sequenceOperation(root, root1);

    // Очистка памяти
//    delete result;

    return 0;
}
