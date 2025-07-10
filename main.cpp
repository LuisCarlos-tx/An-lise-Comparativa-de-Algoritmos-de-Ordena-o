#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

struct Metrics {
    long long comparisons = 0;
    long long swaps = 0;
};

// Funções de ordenação com métricas
void bubbleSort(vector<int>& arr, Metrics& m) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            m.comparisons++;
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                m.swaps++;
            }
        }
    }
}

void insertionSort(vector<int>& arr, Metrics& m) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && ++m.comparisons && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
            m.swaps++;
        }
        arr[j + 1] = key;
    }
}

void selectionSort(vector<int>& arr, Metrics& m) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            m.comparisons++;
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        if (min_idx != i) {
            swap(arr[i], arr[min_idx]);
            m.swaps++;
        }
    }
}

void merge(vector<int>& arr, int l, int m, int r, Metrics& metrics) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<int> L(arr.begin() + l, arr.begin() + m + 1);
    vector<int> R(arr.begin() + m + 1, arr.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        metrics.comparisons++;
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr, int l, int r, Metrics& m) {
    if (l < r) {
        int m_ = l + (r - l) / 2;
        mergeSort(arr, l, m_, m);
        mergeSort(arr, m_ + 1, r, m);
        merge(arr, l, m_, r, m);
    }
}

int partition(vector<int>& arr, int low, int high, Metrics& m) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        m.comparisons++;
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
            m.swaps++;
        }
    }
    swap(arr[i + 1], arr[high]);
    m.swaps++;
    return i + 1;
}

void quickSort(vector<int>& arr, int low, int high, Metrics& m) {
    if (low < high) {
        int pi = partition(arr, low, high, m);
        quickSort(arr, low, pi - 1, m);
        quickSort(arr, pi + 1, high, m);
    }
}

// Gerador de vetores
vector<int> generateArray(int n, const string& type) {
    vector<int> v(n);
    iota(v.begin(), v.end(), 0);
    if (type == "aleatorio") {
        shuffle(v.begin(), v.end(), default_random_engine(42));
    } else if (type == "quase") {
        shuffle(v.begin(), v.begin() + n * 0.1, default_random_engine(42));
    } else if (type == "inverso") {
        reverse(v.begin(), v.end());
    }
    return v;
}

// Teste geral
void run(const string& nome, vector<int> v, void (*sortFunc)(vector<int>&, Metrics&)) {
    Metrics m;
    auto inicio = high_resolution_clock::now();
    sortFunc(v, m);
    auto fim = high_resolution_clock::now();
    auto duracao = duration_cast<milliseconds>(fim - inicio);
    cout << nome << " | Tempo: " << duracao.count() << " ms"
         << " | Comparacoes: " << m.comparisons
         << " | Trocas: " << m.swaps << '\n';
}

int main() {
    vector<string> tipos = {"aleatorio", "quase", "inverso"};
    vector<int> tamanhos = {100, 1000, 10000};
    
    for (auto& tipo : tipos) {
        cout << "\nTipo de vetor: " << tipo << "\n";
        for (auto& tam : tamanhos) {
            cout << "\nTamanho: " << tam << "\n";
            auto base = generateArray(tam, tipo);
            run("Bubble Sort", base, bubbleSort);
            run("Insertion Sort", base, insertionSort);
            run("Selection Sort", base, selectionSort);
            run("Merge Sort", base, [](vector<int>& v, Metrics& m) { mergeSort(v, 0, v.size()-1, m); });
            run("Quick Sort", base, [](vector<int>& v, Metrics& m) { quickSort(v, 0, v.size()-1, m); });
        }
    }
    return 0;
}
