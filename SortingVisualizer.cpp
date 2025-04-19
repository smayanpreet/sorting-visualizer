// Sorting Visualizer in C++ using SDL2
// Features: Multiple algorithms, user controls, color highlights
// Requires SDL2 (https://www.libsdl.org/download-2.0.php)

#include <SDL.h>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <string>

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const int BAR_COUNT = 100;

const SDL_Color COLOR_BG = {30, 30, 30, 255};
const SDL_Color COLOR_BAR = {0, 153, 255, 255};
const SDL_Color COLOR_COMPARE = {255, 153, 0, 255};
const SDL_Color COLOR_SWAP = {255, 51, 51, 255};
const SDL_Color COLOR_SORTED = {0, 255, 102, 255};

enum SortType { BUBBLE, SELECTION, INSERTION, MERGE, QUICK, SORT_COUNT };
const char* SORT_NAMES[] = {"Bubble Sort", "Selection Sort", "Insertion Sort", "Merge Sort", "Quick Sort"};

struct Bar {
    int value;
    SDL_Color color;
};

class SortingVisualizer {
public:
    SortingVisualizer();
    ~SortingVisualizer();
    bool init();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<Bar> bars;
    int speed;
    SortType currentSort;
    bool sorting;
    bool paused;
    bool sorted;

    void resetBars();
    void shuffleBars();
    void drawBars();
    void handleEvents();
    void sortStep();

    // Sorting helpers
    int bubble_i, bubble_j;
    int selection_i, selection_j, selection_min;
    int insertion_i, insertion_j;
    int merge_size;
    std::vector<std::pair<int, int>> quick_stack;

    void initSortState();
    void bubbleSortStep();
    void selectionSortStep();
    void insertionSortStep();
    void mergeSortStep();
    void quickSortStep();
};

SortingVisualizer::SortingVisualizer() :
    window(nullptr), renderer(nullptr), speed(15), currentSort(BUBBLE), sorting(false), paused(false), sorted(false) {}

SortingVisualizer::~SortingVisualizer() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool SortingVisualizer::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow("Sorting Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    resetBars();
    return true;
}

void SortingVisualizer::resetBars() {
    bars.clear();
    for (int i = 0; i < BAR_COUNT; ++i) {
        bars.push_back({ (i + 1), COLOR_BAR });
    }
    shuffleBars();
    sorted = false;
    sorting = false;
    paused = false;
    initSortState();
}

void SortingVisualizer::shuffleBars() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(bars.begin(), bars.end(), g);
}

void SortingVisualizer::drawBars() {
    SDL_SetRenderDrawColor(renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(renderer);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int barW = w / BAR_COUNT;
    for (int i = 0; i < BAR_COUNT; ++i) {
        SDL_Rect rect = { i * barW, h - (bars[i].value * (h - 40) / BAR_COUNT), barW - 1, (bars[i].value * (h - 40) / BAR_COUNT) };
        SDL_SetRenderDrawColor(renderer, bars[i].color.r, bars[i].color.g, bars[i].color.b, bars[i].color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_RenderPresent(renderer);
}

void SortingVisualizer::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE: exit(0); break;
                case SDLK_SPACE: sorting = !sorting; break;
                case SDLK_r: resetBars(); break;
                case SDLK_s: shuffleBars(); sorted = false; sorting = false; paused = false; initSortState(); break;
                case SDLK_RIGHT: currentSort = (SortType)((currentSort + 1) % SORT_COUNT); resetBars(); break;
                case SDLK_LEFT: currentSort = (SortType)((currentSort - 1 + SORT_COUNT) % SORT_COUNT); resetBars(); break;
                case SDLK_UP: speed = std::max(1, speed - 5); break;
                case SDLK_DOWN: speed = std::min(100, speed + 5); break;
                case SDLK_p: paused = !paused; break;
            }
        }
    }
}

void SortingVisualizer::initSortState() {
    bubble_i = bubble_j = 0;
    selection_i = selection_j = selection_min = 0;
    insertion_i = 1; insertion_j = 0;
    merge_size = 1;
    quick_stack.clear();
    quick_stack.push_back({0, BAR_COUNT - 1});
}

void SortingVisualizer::sortStep() {
    switch (currentSort) {
        case BUBBLE: bubbleSortStep(); break;
        case SELECTION: selectionSortStep(); break;
        case INSERTION: insertionSortStep(); break;
        case MERGE: mergeSortStep(); break;
        case QUICK: quickSortStep(); break;
        default: break;
    }
}

void SortingVisualizer::bubbleSortStep() {
    if (bubble_i < BAR_COUNT - 1) {
        for (int k = 0; k < BAR_COUNT; ++k) bars[k].color = COLOR_BAR;
        bars[bubble_j].color = COLOR_COMPARE;
        bars[bubble_j + 1].color = COLOR_COMPARE;
        if (bars[bubble_j].value > bars[bubble_j + 1].value) {
            std::swap(bars[bubble_j], bars[bubble_j + 1]);
            bars[bubble_j].color = COLOR_SWAP;
            bars[bubble_j + 1].color = COLOR_SWAP;
        }
        if (++bubble_j >= BAR_COUNT - bubble_i - 1) {
            ++bubble_i;
            bubble_j = 0;
        }
    } else {
        for (auto& bar : bars) bar.color = COLOR_SORTED;
        sorted = true;
        sorting = false;
    }
}

void SortingVisualizer::selectionSortStep() {
    if (selection_i < BAR_COUNT - 1) {
        for (int k = 0; k < BAR_COUNT; ++k) bars[k].color = COLOR_BAR;
        selection_min = selection_i;
        for (int j = selection_i + 1; j < BAR_COUNT; ++j) {
            bars[j].color = COLOR_COMPARE;
            if (bars[j].value < bars[selection_min].value) {
                selection_min = j;
            }
        }
        std::swap(bars[selection_i], bars[selection_min]);
        bars[selection_i].color = COLOR_SWAP;
        ++selection_i;
    } else {
        for (auto& bar : bars) bar.color = COLOR_SORTED;
        sorted = true;
        sorting = false;
    }
}

void SortingVisualizer::insertionSortStep() {
    if (insertion_i < BAR_COUNT) {
        for (int k = 0; k < BAR_COUNT; ++k) bars[k].color = COLOR_BAR;
        int j = insertion_i;
        while (j > 0 && bars[j - 1].value > bars[j].value) {
            std::swap(bars[j], bars[j - 1]);
            bars[j].color = COLOR_SWAP;
            bars[j - 1].color = COLOR_SWAP;
            --j;
        }
        bars[insertion_i].color = COLOR_COMPARE;
        ++insertion_i;
    } else {
        for (auto& bar : bars) bar.color = COLOR_SORTED;
        sorted = true;
        sorting = false;
    }
}

void SortingVisualizer::mergeSortStep() {
    if (merge_size < BAR_COUNT) {
        for (int k = 0; k < BAR_COUNT; ++k) bars[k].color = COLOR_BAR;
        int left = 0;
        while (left < BAR_COUNT) {
            int mid = std::min(left + merge_size - 1, BAR_COUNT - 1);
            int right = std::min(left + 2 * merge_size - 1, BAR_COUNT - 1);
            int n1 = mid - left + 1;
            int n2 = right - mid;
            std::vector<Bar> L(bars.begin() + left, bars.begin() + mid + 1);
            std::vector<Bar> R(bars.begin() + mid + 1, bars.begin() + right + 1);
            int i = 0, j = 0, k = left;
            while (i < n1 && j < n2) {
                bars[k].color = COLOR_COMPARE;
                if (L[i].value <= R[j].value) {
                    bars[k++] = L[i++];
                } else {
                    bars[k++] = R[j++];
                }
            }
            while (i < n1) bars[k++] = L[i++];
            while (j < n2) bars[k++] = R[j++];
            left += 2 * merge_size;
        }
        merge_size *= 2;
    } else {
        for (auto& bar : bars) bar.color = COLOR_SORTED;
        sorted = true;
        sorting = false;
    }
}

void SortingVisualizer::quickSortStep() {
    for (int k = 0; k < BAR_COUNT; ++k) bars[k].color = COLOR_BAR;
    if (!quick_stack.empty()) {
        auto& top = quick_stack.back();
        int l = top.first, r = top.second;
        if (l < r) {
            int pivot = bars[r].value;
            int i = l - 1;
            for (int j = l; j < r; ++j) {
                bars[j].color = COLOR_COMPARE;
                if (bars[j].value < pivot) {
                    ++i;
                    std::swap(bars[i], bars[j]);
                    bars[i].color = COLOR_SWAP;
                    bars[j].color = COLOR_SWAP;
                }
            }
            std::swap(bars[i + 1], bars[r]);
            bars[i + 1].color = COLOR_SWAP;
            quick_stack.pop_back();
            quick_stack.push_back({l, i});
            quick_stack.push_back({i + 2, r});
        } else {
            quick_stack.pop_back();
        }
    } else {
        for (auto& bar : bars) bar.color = COLOR_SORTED;
        sorted = true;
        sorting = false;
    }
}

void SortingVisualizer::run() {
    while (true) {
        handleEvents();
        if (sorting && !paused && !sorted) {
            sortStep();
            drawBars();
            SDL_Delay(speed);
        } else {
            drawBars();
            SDL_Delay(10);
        }
    }
}

int main(int argc, char* argv[]) {
    SortingVisualizer visualizer;
    if (!visualizer.init()) {
        SDL_Log("Failed to initialize SDL or window");
        return 1;
    }
    visualizer.run();
    return 0;
}

// Controls:
// SPACE: Start/Pause sorting
// R: Reset (sorted array)
// S: Shuffle (randomize array)
// LEFT/RIGHT: Previous/Next algorithm
// UP/DOWN: Increase/Decrease speed
// P: Pause/Resume
// ESC: Quit
