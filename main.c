#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <windows.h>

extern void imgCvtGrayDoubleToInt(double* input, uint8_t* output, int height, int width);

static void fillRandom(double* data, int height, int width) {
    for (int i = 0; i < height * width; i++) {
        data[i] = (double)rand() / (double)RAND_MAX;
    }
}

static void printIntImage(uint8_t* data, int height, int width) {
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            printf("%3d ", data[r * width + c]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int height, width;
    double* input;
    uint8_t* output;
    int useRandom = 0;

    if (argc > 1 && argv[1][0] == 'r') {
        useRandom = 1;
        height = atoi(argv[2]);
        width  = atoi(argv[3]);
    } else {
        if (scanf("%d %d", &height, &width) != 2) {
            fprintf(stderr, "Failed to read height/width\n");
            return 1;
        }
    }

    input  = (double*) malloc(sizeof(double) * height * width);
    output = (uint8_t*) malloc(sizeof(uint8_t) * height * width);
    if (!input || !output) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    if (useRandom) {
        srand((unsigned int)time(NULL));
        fillRandom(input, height, width);
    } else {
        for (int i = 0; i < height * width; i++) {
            if (scanf("%lf", &input[i]) != 1) {
                fprintf(stderr, "Failed to read pixel %d\n", i);
                return 1;
            }
        }
    }

    const int RUNS = 30;
    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);

    double totalSeconds = 0.0;
    for (int run = 0; run < RUNS; run++) {
        QueryPerformanceCounter(&t0);
        imgCvtGrayDoubleToInt(input, output, height, width);
        QueryPerformanceCounter(&t1);
        totalSeconds += (double)(t1.QuadPart - t0.QuadPart) / (double)freq.QuadPart;
    }
    double avgSeconds = totalSeconds / RUNS;

    if (!useRandom) {
        printf("Output:\n");
        printIntImage(output, height, width);
    }

    fprintf(stderr, "Size: %dx%d | Avg time over %d runs: %.9f sec\n",
            height, width, RUNS, avgSeconds);

    free(input);
    free(output);
    return 0;
}
