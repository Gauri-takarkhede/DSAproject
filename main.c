#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define MAX_USERS 100
#define MAX_HOSPITALS 100

typedef struct
{
    char username[50];
    char password[50];
} User;

typedef struct
{
    char name[50];
    int x, y; // Coordinates of the hospital
} Hospital;

typedef struct
{
    int source;
    int destination;
    int weight;
} Edge;

typedef struct
{
    int vertexCount;
    Hospital hospitals[MAX_HOSPITALS];
    int adjacencyMatrix[MAX_HOSPITALS][MAX_HOSPITALS];
} Graph;

User users[MAX_USERS];
int userCount = 0;
Graph graph;

void saveUserAccounts()
{
    FILE *file = fopen("user_accounts.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    for (int i = 0; i < userCount; i++)
    {
        fprintf(file, "%s %s\n", users[i].username, users[i].password);
    }

    fclose(file);
}

void loadUserAccounts()
{
    FILE *file = fopen("user_accounts.txt", "r");
    if (file == NULL)
    {
        printf("No user accounts found.\n");
        return;
    }

    userCount = 0;
    char username[50], password[50];

    while (fscanf(file, "%s %s", username, password) != EOF)
    {
        strcpy(users[userCount].username, username);
        strcpy(users[userCount].password, password);
        userCount++;
    }

    fclose(file);
}

void addNewUser()
{
    if (userCount == MAX_USERS)
    {
        printf("Maximum user limit reached.\n");
        return;
    }

    User newUser;
    printf("Enter username: ");
    scanf("%s", newUser.username);

    // Check if the username already exists
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, newUser.username) == 0)
        {
            printf("Username already exists. Please choose a different username.\n");
            return;
        }
    }

    printf("Enter password: ");
    scanf("%s", newUser.password);

    users[userCount++] = newUser;
    printf("User created successfully.\n");

    saveUserAccounts();
}

int findUserIndex(const char *username)
{
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool login()
{
    char username[50], password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    int userIndex = findUserIndex(username);
    if (userIndex != -1 && strcmp(users[userIndex].password, password) == 0)
    {
        printf("Login successful.\n");
        return true;
    }
    else
    {
        printf("Invalid username or password.\n");
        return false;
    }
}

void changePassword()
{
    char username[50], oldPassword[50], newPassword[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter old password: ");
    scanf("%s", oldPassword);

    int userIndex = findUserIndex(username);
    if (userIndex != -1 && strcmp(users[userIndex].password, oldPassword) == 0)
    {
        printf("Enter new password: ");
        scanf("%s", newPassword);
        strcpy(users[userIndex].password, newPassword);
        printf("Password changed successfully.\n");

        saveUserAccounts();
    }
    else
    {
        printf("Invalid username or password.\n");
    }
}

void addHospital(const char *name, int x, int y)
{
    if (graph.vertexCount == MAX_HOSPITALS)
    {
        printf("Maximum hospital limit reached.\n");
        return;
    }

    strcpy(graph.hospitals[graph.vertexCount].name, name);
    graph.hospitals[graph.vertexCount].x = x;
    graph.hospitals[graph.vertexCount].y = y;

    // Calculate the distance to other hospitals and update adjacency matrix
    for (int i = 0; i < graph.vertexCount; i++)
    {
        int distance = abs(graph.hospitals[i].x - x) + abs(graph.hospitals[i].y - y);
        graph.adjacencyMatrix[graph.vertexCount][i] = distance;
        graph.adjacencyMatrix[i][graph.vertexCount] = distance;
    }

    graph.vertexCount++;
    printf("Hospital added successfully.\n");
}

int findNearestHospital(int userX, int userY)
{
    int minDistance = INT_MAX;
    int nearestHospital = -1;

    for (int i = 0; i < graph.vertexCount; i++)
    {
        int distance = abs(graph.hospitals[i].x - userX) + abs(graph.hospitals[i].y - userY);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestHospital = i;
        }
    }

    return nearestHospital;
}

void bookAmbulance()
{
    char username[50];
    int userIndex;

    printf("Enter username: ");
    scanf("%s", username);

    userIndex = findUserIndex(username);
    if (userIndex == -1)
    {
        printf("User not found.\n");
        return;
    }

    int userX, userY;
    printf("Enter user's current location (x y): ");
    scanf("%d %d", &userX, &userY);

    int nearestHospital = findNearestHospital(userX, userY);
    if (nearestHospital == -1)
    {
        printf("No hospitals available.\n");
        return;
    }

    printf("Ambulance booked. Nearest hospital: %s\n", graph.hospitals[nearestHospital].name);
}

int main()
{
    int choice;

    graph.vertexCount = 0;

    loadUserAccounts();

    while (1)
    {
        printf("\n=== Menu ===\n");
        printf("1. Sign up\n");
        printf("2. Login\n");
        printf("3. Change password\n");
        printf("4. Add hospital\n");
        printf("5. Book ambulance\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addNewUser();
            break;
        case 2:
            if (login())
            {
                // Proceed to the next menu after successful login
                while (1)
                {
                    printf("\n=== User Menu ===\n");
                    printf("1. Change password\n");
                    printf("2. Book ambulance\n");
                    printf("3. Logout\n");
                    printf("Enter your choice: ");
                    scanf("%d", &choice);

                    switch (choice)
                    {
                    case 1:
                        changePassword();
                        break;
                    case 2:
                        bookAmbulance();
                        break;
                    case 3:
                        printf("Logged out.\n");
                        break;
                    default:
                        printf("Invalid choice.\n");
                        break;
                    }

                    if (choice == 3)
                    {
                        break;
                    }
                }
            }
            break;
        case 3:
            changePassword();
            break;
        case 4:
        {
            char name[50];
            int x, y;
            printf("Enter hospital name: ");
            scanf("%s", name);
            printf("Enter hospital location (x y): ");
            scanf("%d %d", &x, &y);
            addHospital(name, x, y);
            break;
        }
        case 5:
            bookAmbulance();
            break;
        case 6:
            printf("Exiting...\n");
            saveUserAccounts();
            exit(0);
        default:
            printf("Invalid choice.\n");
            break;
        }
    }

    return 0;
}
