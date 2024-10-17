#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define HASH_SIZE 64

void draw_board(char board[]) {
    printf("\n");
    printf(" %c | %c | %c\n", board[0], board[1], board[2]);
    printf("-----------\n");
    printf(" %c | %c | %c\n", board[3], board[4], board[5]);
    printf("-----------\n");
    printf(" %c | %c | %c\n", board[6], board[7], board[8]);
    printf("\n");
}

int check_win(char board[], char symbol) {
    return (board[0] == symbol && board[1] == symbol && board[2] == symbol) ||
           (board[3] == symbol && board[4] == symbol && board[5] == symbol) ||
           (board[6] == symbol && board[7] == symbol && board[8] == symbol) ||
           (board[0] == symbol && board[3] == symbol && board[6] == symbol) ||
           (board[1] == symbol && board[4] == symbol && board[7] == symbol) ||
           (board[2] == symbol && board[5] == symbol && board[8] == symbol) ||
           (board[0] == symbol && board[4] == symbol && board[8] == symbol) ||
           (board[2] == symbol && board[4] == symbol && board[6] == symbol);
}

int check_draw(char board[]) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            return 0;
        }
    }
    return 1;
}

char *hash_password(const char *password) {
    char *hashed_password = (char *)malloc((HASH_SIZE + 1) * sizeof(char));
    int i = 0;
    while (password[i] != '\0') {
        hashed_password[i] = password[i] + 1;
        i++;
    }
    hashed_password[i] = '\0';
    return hashed_password;
}

void register_user(FILE *file) {
    char username[50];
    char password[50];

    printf("Enter username: ");
    scanf("%49s", username);
    printf("Enter password: ");
    scanf("%49s", password);

    char *hashed_password = hash_password(password);

    fprintf(file, "%s,%s\n", username, hashed_password);
    fflush(file);
    printf("Registration successful! You may now login.\n");

    free(hashed_password);
}

bool authenticate_user(FILE *file, char username[50], char password[50]) {
    char line[100];
    char *token;

    rewind(file); 
    while (fgets(line, sizeof(line), file)) {
        token = strtok(line, ",");
        if (strcmp(token, username) == 0) {
            token = strtok(NULL, "\n");
            char *stored_hashed_password = token;
            char *hashed_password = hash_password(password);
            bool authenticated = strcmp(stored_hashed_password, hashed_password) == 0;
            free(hashed_password);
            return authenticated;
        }
    }
    return false;
}

int get_computer_move(char board[]) {
    int move;
    do {
        move = rand() % 9;
        printf("move:%d\n" , move);
    } while (board[move] != ' ');
    return move;
}

void play_game(int mode) {
    char board[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    int player = 1;
    int choice;
    bool game_over = false;
    char again;
    int player1_wins = 0;
    int player2_wins = 0;
    int draws = 0;

    do {
        game_over = false;
        memset(board, ' ', 9 * sizeof(char));

        while (!game_over) {
            draw_board(board);
            if ((mode == 1) || (player == 1 && mode == 2)) {
                printf("Player %d (%c), enter your move (1-9): ", player, (player == 1) ? 'X' : 'O');
                while (scanf("%d", &choice) != 1 || choice < 1 || choice > 9 || board[choice - 1] != ' ') {
                    printf("Invalid input. Please enter a number between 1 and 9 corresponding to an empty cell: ");
                    while (getchar() != '\n'); 
                }
                choice--;
            } else {
                printf("Computer (O) is making a move...\n");
                choice = get_computer_move(board);
            }

            if (board[choice] == ' ') {
                board[choice] = (player == 1) ? 'X' : 'O';
                if (check_win(board, (player == 1) ? 'X' : 'O')) {
                    draw_board(board);
                    if (player == 1) {
                        printf("Player 1 (X) wins!\n");
                        player1_wins++;
                    } else {
                        if (mode == 1)
                            printf("Player 2 (O) wins!\n");
                        else
                            printf("Computer (O) wins!\n");
                        player2_wins++;
                    }
                    game_over = true;
                } else if (check_draw(board)) {
                    draw_board(board);
                    printf("It's a draw!\n");
                    draws++;
                    game_over = true;
                } else {
                    player = (player == 1) ? 2 : 1;
                }
            } else {
                printf("That cell is already taken. Please choose another one.\n");
            }
        }

        printf("Scoreboard:\n");
        printf("Player 1 (X) Wins: %d\n", player1_wins);
        if (mode == 1)
            printf("Player 2 (O) Wins: %d\n", player2_wins);
        else
            printf("Computer (O) Wins: %d\n", player2_wins);
        printf("Draws: %d\n", draws);

        printf("Do you want to play again? (y/n): ");
        scanf(" %c", &again);
    } while (again == 'y' || again == 'Y');

    printf("Congratulations!\n");
    if (player1_wins > player2_wins) {
        printf("Player 1 (X) is the overall winner!\n");
    } else if (player2_wins > player1_wins) {
        if (mode == 1)
            printf("Player 2 (O) is the overall winner!\n");
        else
            printf("Computer (O) is the overall winner!\n");
    } else {
        printf("It's a tie! Both players have won the same number of games.\n");
    }
}

int main() {
    FILE *users_file = fopen("users.csv", "a+");
    if (users_file == NULL) {
        printf("Error: Unable to open users file.\n");
        return 1;
    }

    int choice;
    char username[50];
    char password[50];
    bool authenticated = false;

    printf("Welcome to Tic Tac Toe!\n");
    printf("Do you want to register or login?\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("Enter your choice (1 or 2): ");
    while (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        printf("Invalid input. Please enter 1 to register or 2 to login: ");
        while (getchar() != '\n'); 
    }

    if (choice == 1) {
        register_user(users_file);
        fclose(users_file);
        return 0;
    } else if (choice == 2) {
        printf("Enter username: ");
        scanf("%49s", username);
        printf("Enter password: ");
        scanf("%49s", password);

        authenticated = authenticate_user(users_file, username, password);
        if (!authenticated) {
            printf("Invalid username or password.\n");
            fclose(users_file);
            return 0;
        }
    }

    fclose(users_file);

    if (authenticated) {
        int mode;
        printf("Authentication successful. Starting the game...\n");
        srand(time(NULL));
        printf("Select game mode:\n");
        printf("1. Player 1 vs Player 2\n");
        printf("2. Player vs Computer\n");
        printf("Enter your choice (1 or 2): ");
        while (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) {
            printf("Invalid input. Please enter 1 for Player vs Player or 2 for Player vs Computer: ");
            while (getchar() != '\n'); 
        }
        play_game(mode);
    }

    return 0;
}
