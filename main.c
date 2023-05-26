#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <math.h>

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
    bool hasAmbulance;
} Hospital;

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
    FILE *signupLog = fopen("signup_log.txt", "a");
    if (signupLog == NULL)
    {
        printf("Error opening signup log file.\n");
        return;
    }
    fprintf(signupLog, "Username: %s\n", newUser.username);
    fprintf(signupLog, "Password: %s\n", newUser.password);
    fprintf(signupLog, "====================\n");
    fclose(signupLog);
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
        FILE *loginLog = fopen("login_log.txt", "a");
        if (loginLog == NULL)
        {
            printf("Error opening login log file.\n");
            return false;
        }
        fprintf(loginLog, "Username: %s\n", username);
        fprintf(loginLog, "====================\n");
        fclose(loginLog);

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
    char username[50];
    char oldPassword[50];
    char newPassword[50];
    int userIndex;

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter old password: ");
    scanf("%s", oldPassword);

    userIndex = findUserIndex(username);
    if (userIndex != -1 && strcmp(users[userIndex].password, oldPassword) == 0)
    {
        printf("Enter new password: ");
        scanf("%s", newPassword);
        strcpy(users[userIndex].password, newPassword);
        printf("Password changed successfully.\n");

        // Append user password change information to the log file
        FILE *passwordChangeLog = fopen("password_change_log.txt", "a");
        if (passwordChangeLog == NULL)
        {
            printf("Error opening password change log file.\n");
            return;
        }
        fprintf(passwordChangeLog, "Username: %s\n", username);
        fprintf(passwordChangeLog, "====================\n");
        fclose(passwordChangeLog);

        saveUserAccounts();
    }
    else
    {
        printf("Invalid username or old password.\n");
    }
}

void addHospital(const char *name, int x, int y, bool hasAmbulance)
{
    if (graph.vertexCount == MAX_HOSPITALS)
    {
        printf("Maximum hospital limit reached.\n");
        return;
    }

    strcpy(graph.hospitals[graph.vertexCount].name, name);
    graph.hospitals[graph.vertexCount].x = x;
    graph.hospitals[graph.vertexCount].y = y;
    graph.hospitals[graph.vertexCount].hasAmbulance = hasAmbulance;

    // Update the adjacency matrix with distances to other hospitals
    for (int i = 0; i < graph.vertexCount; i++)
    {
        int distance = (int)sqrt(pow(graph.hospitals[i].x - x, 2) + pow(graph.hospitals[i].y - y, 2));
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
        if (graph.hospitals[i].hasAmbulance)
        {
            int distance = (int)sqrt(pow(graph.hospitals[i].x - userX, 2) + pow(graph.hospitals[i].y - userY, 2));
            if (distance < minDistance)
            {
                minDistance = distance;
                nearestHospital = i;
            }
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
        printf("No hospitals with an ambulance available.\n");
        return;
    }

    printf("Ambulance booked. Nearest hospital with an ambulance: %s\n", graph.hospitals[nearestHospital].name);
    FILE *ambulanceBookingLog = fopen("ambulance_booking_log.txt", "a");
    if (ambulanceBookingLog == NULL)
    {
        printf("Error opening ambulance booking log file.\n");
        return;
    }
    fprintf(ambulanceBookingLog, "Username: %s\n", username);
    fprintf(ambulanceBookingLog, "Location: (%d, %d)\n", userX, userY);
    fprintf(ambulanceBookingLog, "Hospital: %s\n", graph.hospitals[nearestHospital].name);
    fprintf(ambulanceBookingLog, "====================\n");
    fclose(ambulanceBookingLog);
}

int main()
{
    int choice;

    graph.vertexCount = 0;

    loadUserAccounts();

    // Adding hospitals to the graph
    addHospital("Hospital A", 10, 5, true);
    addHospital("Hospital B", 8, 12, false);
    addHospital("Hospital C", 50, 33, false);
    addHospital("Hospital D", 67, 42, true);
    addHospital("Hospital E", 23, 2, true);
    addHospital("Hospital F", 85, 72, false);

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
            bool hasAmbulance;
            printf("Enter hospital name: ");
            scanf("%s", name);
            printf("Enter hospital location (x y): ");
            scanf("%d %d", &x, &y);
            printf("Does the hospital have an ambulance? (0 - No, 1 - Yes): ");
            scanf("%d", &hasAmbulance);
            addHospital(name, x, y, hasAmbulance);
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
