#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
    int *data;
    int capacity;
    int size;
} DynamicArray;

typedef struct {
    int maxProfit;    // Největší zisk
    int maxBuyIndex;  // Index nákupu pro max. zisk
    int maxSellIndex; // Index prodeje pro max. zisk
    
    int minProfit;    // Největší ztráta (největší pokles)
    int minBuyIndex;  // Index nákupu pro min. ztrátu
    int minSellIndex; // Index prodeje pro min. ztrátu
} StrValues;

void InnitArray (DynamicArray* arr) {
    arr->capacity = 1;    
    arr->data = (int*)malloc(sizeof(int) * arr->capacity);
    arr->size = 0;
}

void growArray(DynamicArray* arr) {
    arr->capacity *= 2;
    
    // Snažíme se alokovat novou paměť
    int* new_data = (int*)realloc(arr->data, arr->capacity * sizeof(int));
    
    // Pokud realloc selže, vrátí NULL, pak musíme zachovat původní data
    if (new_data == NULL) {
        printf("Nespravna alokace pameti\n");
        free(arr->data);  // Uvolníme původní data
        exit(1);  // Ukončíme program, protože nemůžeme pokračovat
    }
    
    // Pokud realloc uspěje, nastavíme ukazatel na novou paměť
    arr->data = new_data;
}

void AddToArray(DynamicArray* arr, int value) {

    if (arr->capacity == arr->size) 
        growArray(arr);

    arr->data[arr->size] = value;
    arr->size++;
}

void freeArray(DynamicArray* arr) {
    free(arr->data);
}

void AnalyzeDatas(StrValues* Values, DynamicArray* arr, int start, int finish) {
    int CurentMaxValue = 0, MaxIndex = 0, CurentProfit = 0;
    Values->maxProfit = 0;

    for (int i = start; i < finish; i++) {
        for (int j = i + 1; j <=finish; j++) {
            if(CurentMaxValue < arr->data[j]) {
                CurentMaxValue = arr->data[j];
                MaxIndex = j;
            }
        }
        CurentProfit = CurentMaxValue - arr->data[i];
        if ( CurentProfit > Values->maxProfit ) {
            Values->maxBuyIndex = i;
            Values->maxSellIndex = MaxIndex;
            Values->maxProfit = CurentProfit;
        }
        CurentProfit = 0;
        CurentMaxValue = 0;
    }

        if (Values->maxProfit <= 0) {
        printf ("Nejvyssi zisk: N/A\n");
    }
    else {
         printf ("Nejvyssi zisk: %d (%d - %d)\n", Values->maxProfit, Values->maxBuyIndex, Values->maxSellIndex );
    }
}

void Loose(StrValues* Values, DynamicArray* arr, int start, int finish) {
    int CurentMinValue, MinIndex = 0, CurentProfit = 0;
    Values->minProfit = INT_MIN; // Inicializace na nejnižší možnou hodnotu

    for (int i = start; i < finish; i++) {
        CurentMinValue = INT_MAX; // Reset pro každou iteraci vnějšího cyklu

        for (int j = i + 1; j <= finish; j++) {
            if (CurentMinValue > arr->data[j]) {
                CurentMinValue = arr->data[j];
                MinIndex = j;
            }
        }

        CurentProfit = arr->data[i] - CurentMinValue;

        if (CurentProfit > Values->minProfit) { // Porovnání s největší ztrátou
            Values->minBuyIndex = i;
            Values->minSellIndex = MinIndex;
            Values->minProfit = CurentProfit;
        }
    }

    if (Values->minProfit <= 0) {
        printf("Nejvyssi ztrata: N/A\n");
    } else {
        printf("Nejvyssi ztrata: %d (%d - %d)\n", Values->minProfit, Values->minBuyIndex, Values->minSellIndex);
    }
}



int main () {
    DynamicArray arr;
    StrValues Values;
    InnitArray(&arr);

    char input[100];
    int num1, num2, number;
    char extra;

    printf("Ceny, hledani:\n");

    while(fgets(input, 100, stdin)) {
        if(input[0] == '0') break;
        else if (input[0] == '+' && input[1] == ' ' && sscanf(input + 2, "%d %c", &number, &extra) == 1 && number >= 0) {
            AddToArray(&arr, number);
            }
        else if (input[0] == '?' && input[1] == ' ' && sscanf(input + 2, "%d %d %c", &num1, &num2, &extra) == 2 && num1 <= num2 && num1 >= 0 && num2 >= 0 && num1 < arr.size && num2 < arr.size) {
            AnalyzeDatas(&Values, &arr,  num1, num2);
            Loose(&Values, &arr,  num1, num2);
        }
        else {
            printf("Nespravny vstup.\n");
            break;
        }
    }

    freeArray(&arr);
return 0;
}