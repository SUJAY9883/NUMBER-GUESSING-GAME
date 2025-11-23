#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void play_game(char name[]) {
    // Some fonts to make the game look professional
    printf("\nHEY %s", name);
    printf("\nWELCOME TO THE NUMBER GUESSING GAME!\n");
    printf("A GAME OF LOGIC AND INTUITION!\n\n");
    printf("---GAME INSTRUCTIONS---\n");
    printf("I HAVE PICKED A RANDOM NUMBER BETWEEN 1 AND 100.\n");
    printf("TRY TO GUESS IT IN AS FEW ATTEMPTS AS POSSIBLE.\n");
    printf("I WILL TELL YOU IF THE NUMBER IS HIGHER OR LOWER.\n\n");

    // Generate a random number by computer
    srand(time(0));
    int secret_number = (rand() % 100) + 1;
    int user_guess = 0;
    int attempts = 0;

    // Loop for condition check
    while (1) {
        attempts++;
        printf("ATTEMPT %d: ENTER YOUR GUESS: ", attempts);

        // Input validation
        if (scanf("%d", &user_guess) != 1) {
            printf("\nINVALID INPUT! PLEASE ENTER A NUMBER.\n\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }

        if (user_guess < 1 || user_guess > 100) {
            printf("\nPLEASE CHOOSE A NUMBER BETWEEN 1 AND 100!\n\n");
        } 
        else if (user_guess > secret_number) {
            printf("TOO HIGH! TRY A LOWER NUMBER.\n\n");
        } 
        else if (user_guess < secret_number) {
            printf("TOO LOW! TRY A HIGHER NUMBER.\n\n");
        } 
        else {
            // Correct guess
            printf("\nCONGRATULATION!\n");
            printf("YOU GUESSED THE CORRECT NUMBER: %d\n", secret_number);
            printf("IT TOOK YOU %d ATTEMPTS TO WIN.\n", attempts);
            
            // Give specific praise based on attempts
            if (attempts <= 5) {
                printf("AMAZING! YOU ARE A GENIUS!\n");
            } else if (attempts <= 10) {
                printf("GOOD JOB! SOLID PERFORMANCE!\n");
            } else {
                printf("YOU DID IT, BUT YOU CAN DO BETTER!\n");
            }
            break; // Exit the loop
        }
    }
}

int main() {
    char name[30];
    char play_again;
    do {
        // Take player name
        printf("\nENTER YOUR NAME: ");
        if (fgets(name, sizeof(name), stdin) != NULL) {
            name[strcspn(name, "\n")] = 0;
        }

        // If name is empty
        while (strlen(name) == 0) {
            printf("NAME CANNOT BE EMPTY. ENTER YOUR NAME: ");
            if (fgets(name, sizeof(name), stdin) != NULL) {
                name[strcspn(name, "\n")] = 0;
            }
        }

        // Function calling to play the game
        play_game(name);

        // Play again
        printf("\n\nDo you want to play again?\n");
        printf("Enter \"Y\" for 'YES'\n");
        printf("Enter \"N\" for 'NO'\n");
        printf("Enter your choice: ");

        // Clear buffer before reading char to avoid skipping input
        // Note: Since play_game uses scanf, a newline is usually left in buffer.
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        // Loop to continue or exit the game
        while (1) {
            // Take input from user
            play_again = getchar();

            // Clear buffer after reading the char
            while ((c = getchar()) != '\n' && c != EOF);

            if (play_again == 'Y' || play_again == 'y' || play_again == 'N' || play_again == 'n') {
                break;
            } else {
                printf("Invalid!! Please enter \"Y\" or \"N\": ");
            }
        }

    } while (play_again == 'Y' || play_again == 'y');

    printf("Thanks for playing %s! Goodbye!\n", name);

    return 0;
}