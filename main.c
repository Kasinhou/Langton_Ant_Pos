#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <strings.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include "pos_sockets/char_buffer.h"
#include "pos_sockets/active_socket.h"
#include "pos_sockets/passive_socket.h"


typedef struct thread_data {
    pthread_mutex_t mutex;
    pthread_cond_t is_full;
    pthread_cond_t is_empty;

    short port;
    ACTIVE_SOCKET* my_socket;
} thread_data_t;


void thread_data_init(thread_data_t* data, short port, ACTIVE_SOCKET* my_socket) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->is_full, NULL);
    pthread_cond_init(&data->is_empty, NULL);

    data->port = port;
    data->my_socket = my_socket;
}

void thread_data_destroy(thread_data_t* data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->is_full);
    pthread_cond_destroy(&data->is_empty);

    data->port = 0;
    data->my_socket = NULL;
}

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

_Bool world_try_deserialize(world_t *world, struct char_buffer* buf) {
    if (sscanf(buf->data, "%d;%d;%d;%d;", &world->rows, &world->columns,
               &world->ants, &world->movement) == 4) {
        printf("\nuspesne deserializovane\n");
        return true;
    }
    printf("\nneuspesne bohuzial\n");
    return false;
}

_Bool try_get_client_data(struct active_socket* my_sock, world_t* client_input_data) {
    CHAR_BUFFER buf;
    char_buffer_init(&buf);
    _Bool result = false;
    if (active_socket_try_get_read_data(my_sock, &buf)) {
        if (!world_try_deserialize(client_input_data, &buf)) { //do objektu chcem ulozit buffer
            if (active_socket_is_end_message(my_sock, &buf)) {
                active_socket_stop_reading(my_sock);
            }
        }
        else {
            result = true;
            printf("\nPrecitali sa data pomocou aktivneho socketu\n");
        }
    }
    else {
        printf("active socket try read data sa nepodarilo\n");
    }
    char_buffer_destroy(&buf);
    return result;
}

/*void* main_consument(void* thread_data) {
    return NULL;
}*/

int createAnt(world_t *world, ant_t *ant, int position, int direction) {
    ant->position = position;
    ant->direction = direction;
    ant->actualField = world->array_world[ant->position];
    if (world->array_world[ant->position] == 2) {
        return 1;
    }
    return 0;
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

void transform_to_buffer(world_t *world) {
    char buffer[1024];
    for (int i = 0; i < world->rows; ++i) {
        for (int j = 0; j < world->columns; ++j) {
            buffer[i * world->columns + j] = (char)world->array_world[i * world->columns + j];
        }
    }
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    //char buffer[256];

    server = gethostbyname("localhost");
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi("11112"));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return;
    }

    /*printf("Please enter a message: ");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);*/

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
        return;
    }

    //bzero(buffer,256);
    n = read(sockfd, buffer, sizeof(buffer));
    if (n < 0)
    {
        perror("Error reading from socket");
        return;
    }

    printf("%s\n",buffer);
    close(sockfd);

}

void sendMessageToClient(world_t *world) {
    char buffer[1024];
    for (int i = 0; i < world->rows; ++i) {
        for (int j = 0; j < world->columns; ++j) {
            buffer[i * world->columns + j] = (char)world->array_world[i * world->columns + j];
        }
    }

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    //adresa
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(11112);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    //5mozu cakat, respektive
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", 11112);

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Error accepting connection");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    //poslanie buffra s datami klientovi
    if (send(clientSocket, buffer, strlen(buffer), 0) == -1) {
        perror("Error sending data");
    }

    //neviem ci treba
    close(clientSocket);
    close(serverSocket);
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
        //sendMessageToClient(world);
        //transform_to_buffer(world);
        //showWorldState(world);
        usleep(1000000);
    }
}

//konkretne vlakno pre klienta
void* handle_client_data(void* thread_data) {
    struct thread_data* data = (struct thread_data*)thread_data;
    printf("\n---->Skusanie ziskavania info od klienta\n");
    world_t world;
    active_socket_start_reading(data->my_socket);
    if (data->my_socket != NULL) {//transfomrovat aelbo deserializovat socket data  do world
        try_get_client_data(data->my_socket, &world);
        printf("NIE JE NULL!!!");
        createWorld(&world, world.rows, world.columns, world.ants, world.movement);
        generateBlackFields(&world);

        ant_t antsArray[world.ants];

        //bud nahodne alebo zo vstupu, chyba vstup
        for (int i = 0; i < world.ants; ++i) {
            //double position = (double)rand() / RAND_MAX;
            if (createAnt(&world, &antsArray[i],
                          (int)((double)rand() / RAND_MAX * (world.rows * world.columns)),
                          (int)((double)rand() / RAND_MAX * 4)) == 1) {
                for (int j = 0; j < world.ants; ++j) {
                    antsArray[j] = antsArray[j + 1];
                }
                world.ants--;
                i--;
            }
        }

        struct char_buffer clientMess;
        char znak = 'A';
        char_buffer_init(&clientMess);

        char_buffer_append(&clientMess, &znak, sizeof(char));
        active_socket_write_data(data->my_socket, &clientMess);
        active_socket_write_end_message(data->my_socket);
        printf("Znak = %c \n", clientMess.data[0]);

        simulation(&world, antsArray, world.movement);
        //free(data);
    }
    else {
        printf("JE NULL!!!");
    }
    printf("Rows: %d\nColumns: %d\nAnts: %d\nMovement: %d\n", world.rows, world.columns, world.ants, world.movement);
    //active_socket_stop_reading(data->my_socket);
    return NULL;
}

void* process_client_data(void* thread_data) {
    struct thread_data* data = (struct thread_data*)thread_data;
    PASSIVE_SOCKET sock;
    passive_socket_init(&sock);
    passive_socket_start_listening(&sock, data->port);
    //tu vytvorit nove vlakno kde bude activestartreading
    passive_socket_wait_for_client(&sock, data->my_socket);
    passive_socket_is_listening(&sock);
    if (passive_socket_is_listening(&sock)) {
        handle_client_data(data);
    }
    /*while (true) {
        pthread_t client_thread;
        passive_socket_wait_for_client(&sock, data->my_socket);
        printf("wait for client\n");
        if (passive_socket_is_listening(&sock)) {
            struct thread_data* data_for_client = (struct thread_data*)malloc(sizeof(struct thread_data));
            if (data_for_client == NULL) {
                printf("Data pre klienta su zle");
                break;
            }

            if (pthread_create(&client_thread, NULL, handle_client_data, &data_for_client) != 0) {
                printf("Client thread sa z nejakho dovodu nevytvoril.\n");
                free(data_for_client);
            } else {
                pthread_detach(client_thread);
                printf("detach");
            }
            //handle_client_data(data);
        }
    }*/
    passive_socket_stop_listening(&sock);//while pas sock is listening-. wait for client
    passive_socket_destroy(&sock);

    return NULL;
}

/*void* consume(void* thread_data) {
    struct thread_data* data = (struct thread_data*)thread_data;

    if (data->my_socket != NULL) {//transfomrovat aelbo deserializovat socket data  do client pi est
        try_get_client_pi_estimation(data->my_socket, &client_pi_estimaton);
        printf("%ld: ", i);
        printf("Odhad pi s vyuzitim dat od klienta: %lf\n",
               4 * (double)(pi_estimaton.inside_count + client_pi_estimaton.inside_count) /
               (double)(pi_estimaton.total_count + client_pi_estimaton.total_count));
    }
    }
    if (data->my_socket != NULL) {
        while (active_socket_is_reading(data->my_socket)) {
            if (try_get_client_pi_estimation(data->my_socket, &client_pi_estimaton)) {
                printf("Odhad pi s vyuzitim dat od klienta: %lf\n",
                       4 * (double)(pi_estimaton.inside_count + client_pi_estimaton.inside_count) /
                       (double)(pi_estimaton.total_count + client_pi_estimaton.total_count));
            }
        }
    }

    return NULL;
}*/

int main(int argc, char* argv[]) {
    srand(time(NULL));

    //pthread_t th_produce;
    pthread_t th_receive;
    struct thread_data data;
    struct active_socket my_socket;

    active_socket_init(&my_socket);
    thread_data_init(&data, 11112, &my_socket);

    //pthread_create(&th_produce, NULL, produce, &data);
    pthread_create(&th_receive, NULL, process_client_data, &data);

    //main_consument(&data);

    //pthread_join(th_produce, NULL);
    pthread_join(th_receive, NULL);


    thread_data_destroy(&data);
    active_socket_destroy(&my_socket);

    //vytvorenie sveta so zadanymi rozmermi (definovat biele cierne mravcov - #O.)
    /*int rows, columns;
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
    simulation(&world, antsArray, movement);*/

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

