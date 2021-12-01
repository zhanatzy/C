#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE     26
#define ALPHABET_SORTED   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define ALPHABET_REVERSED "ZYXWVUTSRQPONMLKJIHGFEDCBA"

typedef struct rotor_t {
    int period;
    char static_part[ALPHABET_SIZE];
    char dynamic_part[ALPHABET_SIZE];
} rotor_t;

typedef struct reflector_t {
    char input_part[ALPHABET_SIZE];
    char output_part[ALPHABET_SIZE];
} reflector_t;

typedef struct enigma_t {
    int counter;
    int rotors_num;
    rotor_t *rotors;
    reflector_t reflector;
} enigma_t;

extern enigma_t *enigma_new(reflector_t reflector, rotor_t rotors[], int rotors_num);
extern void enigma_free(enigma_t *enigma);
extern char enigma_encrypt(enigma_t *enigma, char ch);

static void shift_right(char array[], int size);
static int find_index(char array[], int size, char ch);

int main(int argc, char *argv[]) {
    int len;

    reflector_t reflector = {
        .input_part = ALPHABET_SORTED,
        .output_part = ALPHABET_REVERSED
    };

    rotor_t rotors[] = {
        {
            .period = 1,
            .static_part = ALPHABET_SORTED,
            .dynamic_part = ALPHABET_SORTED
        },
        {
            .period = 2,
            .static_part = ALPHABET_SORTED,
            .dynamic_part = ALPHABET_SORTED
        },
        {
            .period = 3,
            .static_part = ALPHABET_SORTED,
            .dynamic_part = ALPHABET_SORTED
        }
    };

    enigma_t *enigma = enigma_new(reflector, rotors, sizeof(rotors)/sizeof(rotors[0]));

    for (int i = 1; i < argc; ++i) {
        len = strlen(argv[i]);
        for (int j = 0; j < len; ++j) {
            putchar(enigma_encrypt(enigma, argv[i][j]));
        }
    }
    putchar('\n');
    
    enigma_free(enigma);

    return 0;
}

extern enigma_t *enigma_new(reflector_t reflector, rotor_t rotors[], int rotors_num) {
    enigma_t *enigma = (enigma_t*)malloc(sizeof(enigma_t));
    
    enigma->counter = 0;
    enigma->reflector = reflector;
    enigma->rotors_num = rotors_num;

    enigma->rotors = (rotor_t*)malloc(sizeof(rotor_t)*rotors_num);
    for (int i = 0; i < rotors_num; ++i) {
        enigma->rotors[i] = rotors[i];
    }

    return enigma;
}

extern void enigma_free(enigma_t *enigma) {
    free(enigma->rotors);
    free(enigma);
}

extern char enigma_encrypt(enigma_t *enigma, char ch) {
    int index;

    // rotors -> reflector
    for (int i = 0; i < enigma->rotors_num; ++i) {
        index = find_index(enigma->rotors[i].static_part, ALPHABET_SIZE, ch);
        if (index == -1) {
            continue;
        }
        ch = enigma->rotors[i].dynamic_part[index];
    }

    // reflector
    index = find_index(enigma->reflector.input_part, ALPHABET_SIZE, ch);
    if (index != -1) {
        ch = enigma->reflector.output_part[index];  
    }

    // reflector -> rotors
    for (int i = enigma->rotors_num-1; i >= 0; --i) {
        index = find_index(enigma->rotors[i].dynamic_part, ALPHABET_SIZE, ch);
        if (index == -1) {
            continue;
        }
        ch = enigma->rotors[i].static_part[index];
    }

    // + new character
    enigma->counter += 1;

    // rotate shift right rotors
    for (int i = 0; i < enigma->rotors_num; ++i) {
        if (enigma->counter % enigma->rotors[i].period == 0) {
            shift_right(enigma->rotors[i].dynamic_part, ALPHABET_SIZE);
        }
    }

    return ch;
}

static void shift_right(char array[], int size) {
    char temp = array[size-1];
    for (int i = size-1; i > 0; --i) {
        array[i] = array[i-1];
    }
    array[0] = temp;
}

static int find_index(char array[], int size, char ch) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == ch) {
            return i;
        }
    }
    return -1;
}
