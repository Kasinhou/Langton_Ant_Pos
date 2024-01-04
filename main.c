#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

typedef struct ant_t {
    int position;
    enum Direction direction;
} ant_t;

typedef struct world_t {
    int rows;
    int columns;
    int *array_world;
    int ants;
    // 0 = priama, 1 = inverzna
    int movement;
} world_t;

void createWorld(world_t *world, int rows, int columns, int ants, int movement) {
    world->rows = rows;
    world->columns = columns;
    world->array_world = malloc(sizeof(int) * (world->rows * world->columns));
    world->ants = ants;
    world->movement = movement;
    //printf("%d %d %d %d", world->rows, world->columns, world->ants, world->movement);
}

void destroyWorld(world_t *world) {
    free(world->array_world);
    world->rows = 0;
    world->columns = 0;
    world->ants = 0;
    world->movement = 0;
}

//void createAnt() destroyAnt() ???

void generateBlackFields(world_t *world) {
    int count = world->rows * world->columns;
    //pravdepodobnost cierneho policka
    double probability = (double)rand() / RAND_MAX;
    printf("%.2f and %d", probability * 100, count);
    for (int i = 0; i < count; ++i) {
        //ak je vygenerovane cislo mensie ako probability tak je cierna (cierna 0 biela 1 mravec 2);
        if ((double)rand() / RAND_MAX < probability) {
            world->array_world[i] = 0;
        }
        else {
            world->array_world[i] = 1;
        }
    }
}

void defineBlackFieldsByHand(world_t *world) {
    int number;
    printf("Define the world by yourself\n0 -> black\n1 -> white\nNumbers must be separated by a space:\n");
    for (int i = 0; i < world->rows; ++i) {
        for (int j = 0; j < world->columns; ++j) {
            scanf("%d", &number);
            if (number != 0 && number != 1) {
                number = 1;
            }
            world->array_world[i * world->columns + j] = number;
        }
    }
}

void showWorldState(world_t *world) {
    for (int i = 0; i < world->rows; ++i) {
        for (int j = 0; j < world->columns; ++j) {
            printf("%d ", world->array_world[i * world->columns + j]);
        }
        printf("\n");
    }
}


int main() {
    srand(time(NULL));
    //na server sa moze pripojit viac klientov

    //vytvorenie sveta so zadanymi rozmermi (definovat biele cierne mravcov - #O.)
    int rows, columns;
    printf("\n--------------------------------------Langton's ants----------------------------------------\n");
    _Bool wrongInput = true;
    while (wrongInput) {
        wrongInput = false;
        printf("Enter number which will represent number of rows in the world: ");
        if (scanf("%d", &rows) != 1) {
            printf("Invalid input, try again!\n");
            wrongInput = true;
            while (getchar() != '\n');
            continue;
        }
        printf("Enter number which will represent number of columns in the world: ");
        if (scanf("%d", &columns) != 1) {
            printf("Invalid input, try again!\n");
            wrongInput = true;
            while (getchar() != '\n');
        }
    }
    printf("Rows X Columns = %d X %d\n", rows, columns);
//---------------------------------------------------------------------------

    //definovat pocet mravcov vo svete + ich logika(priama/inverzna)
    //priama 0 - biele pole otocka vpravo, zmena na cierne
    //cierne pole otocka vlavo, zmena na biele
    //inverzna 1 - biele pole otocka vlavo, zmena na cierne
    //                //cierne pole otocka vpravo, zmena na biele
    int numberOfAnts;
    int movement;
    wrongInput = true;

    while (wrongInput) {
        wrongInput = false;
        printf("How many ants do you want in the world: ");
        if (scanf("%d", &numberOfAnts) != 1) {
            printf("Invalid input, try again!\n");
            wrongInput = true;
            while (getchar() != '\n');
            continue;
        }

        printf("Movement of ants:\n0 -> Direct\n1 -> Inverse\n"
               "Choose what movement do you want: ");
        scanf("%d", &movement);
        if (movement != 0 && movement != 1) {
            printf("Invalid input, try again!\n");
            wrongInput = true;
            while (getchar() != '\n');
        }
    }
    printf("You choose a %d movement with %d ants.\n", movement, numberOfAnts);

//---------------------------------------------------------------------------
    //generovanie nahodnych ciernych poli
    //rucne definovat cierne polia
    int definingBlackFields;
    wrongInput = true;

    while (wrongInput) {
        wrongInput = false;
        printf("Defining black fields:\n1 -> Random probability and location\n2 -> Define by yourself\n"
               "Choose what option do you prefer: ");
        scanf("%d", &definingBlackFields);
        if (definingBlackFields != 1 && definingBlackFields != 2) {
            printf("Invalid input, try again!\n");
            wrongInput = true;
            while (getchar() != '\n');
        }
    }
    printf("You choose number %d\n", definingBlackFields);

    //-----------------vytvorenie sveta s parametrami zadanymi od pouzivatela
    world_t world;
    createWorld(&world, rows, columns, numberOfAnts, movement);
    if (definingBlackFields == 1) {
        generateBlackFields(&world);
    } else {
        defineBlackFieldsByHand(&world);
    }
    showWorldState(&world);

    //nacitat/ulozit svet z/do lokalneho suboru

    //definovanie koliizi mravcov na poli

    //zapnutie a vypnutie simulacie v ktoromkolvek okamziku

    //pripojenie na server a stiahnutie vzoru na local

    //ulozenie vzoru na server


    return 0;
}

