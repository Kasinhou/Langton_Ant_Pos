#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct ant_t {
    //na akom policku stoji
    int actualField;
    int position;
    int direction; //0up 1right 2down 3left
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

int createAnt(world_t *world, ant_t *ant, int position, int direction) {
    ant->position = position;
    ant->direction = direction;
    ant->actualField = world->array_world[ant->position];
    if (world->array_world[ant->position] == 2) {
        return 1;
    }
    return 0;
    /*if (world->array_world[ant->position] != 2) {
        world->array_world[ant->position] = 2;
        //world->array_world[ant->position] = 2;//znak mravca
    } else {
        //world->array_world[ant->position] = ant->actualField;
        //ant->position = -1;
        world->ants--;
    }*/
}

void destroyAnt(ant_t *ant, world_t *world) {
    //nejako mravca treba znicit mozno okrem toho ze zmenime poziciu
    //ak sa stretnu dva tak proste zmenime na bielu
    /*world->array_world[ant->position] = 1;
    ant->position = -1;*/
    //world->ants--;
}

void destroyWorld(world_t *world) {
    free(world->array_world);
    world->rows = 0;
    world->columns = 0;
    world->ants = 0;
    world->movement = 0;
}

void generateBlackFields(world_t *world) {
    int count = world->rows * world->columns;
    //pravdepodobnost cierneho policka
    double probability = (double)rand() / RAND_MAX;
    printf("%.2f and %d\n", probability * 100, count);
    for (int i = 0; i < count; ++i) {
        //ak je vygenerovane cislo mensie ako probability tak je cierna (cierna 0 biela 1 mravec 2);
        if ((double)rand() / RAND_MAX < probability) {
            world->array_world[i] = -1;
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
            world->array_world[i * world->columns + j] = (number == 1) ? number : -1;
            //world->array_world[i * world->columns + j] = number;
        }
    }
}

void showWorldState(world_t *world) {
    printf("\n");
    for (int i = 0; i < world->columns; ++i) {
        printf("----");
    }
    printf("-\n");
    for (int i = 0; i < world->rows; ++i) {
        for (int j = 0; j < world->columns; ++j) {
            switch (world->array_world[i * world->columns + j]) {
                case -1:
                    printf("| # ");
                    break;
                case 1:
                    printf("|   ");
                    break;
                case 2:
                    printf("| . ");
                    break;
                default:
                    printf("Something went wrong with world array. Unexpected character!\n");
                    printf("\nThis is not suppose to be here -> %d\n", world->array_world[i * world->columns + j]);
                    return;
            }
            //printf("%d ", world->array_world[i * world->columns + j]);
        }
        printf("|\n");
        for (int j = 0; j < world->columns; ++j) {
            printf("----");
        }
        printf("-\n");
    }
    printf("\n");
}

//direct -> biele 1 otocka vpravo, zmena na cierne 0, type direct/inverse

int antsStep(world_t *world, ant_t *ant, int type) {
    _Bool step = false;
    while (!step) {
        if (ant->actualField == type) {
            ant->direction = (ant->direction + 3) % 4;
        } else {
            ant->direction = (ant->direction + 1) % 4;
        }

        switch (ant->direction) {
            case 0:
                if (ant->position / world->columns != 0) {
                    step = true;
                    //zmenit field na actual field opacnej farby, position mravca, actual field na nove, nove na 2
                    world->array_world[ant->position] = (ant->actualField * -1);
                    //zalezi od smeru vypocet
                    ant->position -= world->columns;
                    ant->actualField = world->array_world[ant->position];
                    //kolizie, ak sa stretnu ten co je tam skor ma prednost a prezil
                    if (world->array_world[ant->position] != 2) {
                        world->array_world[ant->position] = 2;
                    } else {
                        return 1;
                    }
                }
                break;
            case 1:
                if (ant->position % world->columns != world->columns - 1) {
                    step = true;
                    world->array_world[ant->position] = (ant->actualField * -1);
                    ant->position++;
                    ant->actualField = world->array_world[ant->position];
                    if (world->array_world[ant->position] != 2) {
                        world->array_world[ant->position] = 2;
                    } else {
                        return 1;
                    }
                }
                break;
            case 2:
                if (ant->position / world->columns != world->rows - 1) {
                    step = true;
                    world->array_world[ant->position] = (ant->actualField * -1);
                    ant->position += world->columns;
                    ant->actualField = world->array_world[ant->position];
                    if (world->array_world[ant->position] != 2) {
                        world->array_world[ant->position] = 2;
                    } else {
                        return 1;
                    }
                }
                break;
            case 3:
                if (ant->position % world->columns != 0) {
                    step = true;
                    world->array_world[ant->position] = (ant->actualField * -1);
                    ant->position--;
                    ant->actualField = world->array_world[ant->position];
                    if (world->array_world[ant->position] != 2) {
                        world->array_world[ant->position] = 2;
                    } else {
                        return 1;
                    }
                }
                break;
            default:
                printf("Something went wrong with direction. Unexpected error.");
                return -1;
        }
    }
    return 0;
}

void simulation(world_t *world, ant_t *ants, int type) {
    while (true) {
        for (int i = 0; i < world->ants; i++) {
            if (antsStep(world, &ants[i], type) == 1) {
                for (int j = i; j < world->ants - 1; ++j) {
                    ants[j] = ants[j + 1];
                }
                world->ants--;
                i--;
            }
        }
        showWorldState(world);
        //printf("Number of ants = %d\n", world->ants);
        usleep(1000000);
    }
}

int main() {
    srand(time(NULL));
    /*for (int i = 0; i < 100; ++i) {
        printf("%d , ", (int)((double)rand() / RAND_MAX * 4));
    }*/
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
    movement = (movement == 1) ? movement : -1;

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

    ant_t antsArray[numberOfAnts];

    //bud nahodne alebo zo vstupu, chyba vstup
    for (int i = 0; i < numberOfAnts; ++i) {
        //double position = (double)rand() / RAND_MAX;
        if (createAnt(&world, &antsArray[i],
                  (int)((double)rand() / RAND_MAX * (rows * columns)),
                  (int)((double)rand() / RAND_MAX * 4)) == 1) {
            for (int j = 0; j < numberOfAnts; ++j) {
                antsArray[j] = antsArray[j + 1];
            }
            numberOfAnts--;
            world.ants--;
            i--;
        }
    }

    printf("numberOfAnts = %d\n", numberOfAnts);
    printf("world->ants = %d\n", world.ants);
    simulation(&world, antsArray, movement);
    //showWorldState(&world);
    //printf("%d ", -1 % 4);

    //nacitat/ulozit svet z/do lokalneho suboru

    //definovanie koliizi mravcov na poli, za mna odstranenie mravcov asi

    //zapnutie a vypnutie simulacie v ktoromkolvek okamziku, iba prerusenie cyklu pravdepodobne a cakanie

    //pripojenie na server a stiahnutie vzoru na local,
    // staci si prebrat referenciu, mozno pointer na array + stlpce, riadky a mozno pocet mravcom(da sa zistit z arrayu aj) a typ pohybu

    //ulozenie vzoru na server, poslat array, plus parametre, v podstate celu strukturu world_t


    return 0;
}

