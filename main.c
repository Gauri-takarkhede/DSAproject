#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>

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
    int x;
    int y;
    bool hasAmbulance;
} Hospital;

typedef struct
{
    Hospital hospitals[MAX_HOSPITALS];
    int adjacencyMatrix[MAX_HOSPITALS][MAX_HOSPITALS];
    int vertexCount;
} Graph;

User users[MAX_USERS];
Graph graph;
int loggedInUserIndex = -1;
int userCount = 0;

void saveUserAccounts()
{
    FILE *file = fopen("user_accounts.txt", "w");
    if (file == NULL)
    {
        printf("Error saving user accounts.\n");
        return;
    }

    for (int i = 0; i < MAX_USERS; i++)
    {
        if (strlen(users[i].username) > 0)
        {
            fprintf(file, "%s %s\n", users[i].username, users[i].password);
        }
    }

    fclose(file);
    printf("User accounts saved successfully.\n");
}

void loadUserAccounts()
{
    FILE *file = fopen("user_accounts.txt", "r");
    if (file == NULL)
    {
        printf("No user accounts found. Creating a new file.\n");
        return;
    }

    int count = 0;
    while (fscanf(file, "%s %s", users[count].username, users[count].password) != EOF)
    {
        count++;
    }

    printf("Loaded %d user accounts.\n", count);
    fclose(file);
}

int findUserIndex(const char *username)
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            return i;
        }
    }
    return -1;
}

void addNewUser()
{
    User newUser;
    printf("Enter new username: ");
    scanf("%s", newUser.username);

    int existingIndex = findUserIndex(newUser.username);
    if (existingIndex != -1)
    {
        printf("Username already exists.\n");
        return;
    }

    printf("Enter new password: ");
    scanf("%s", newUser.password);

    for (int i = 0; i < MAX_USERS; i++)
    {
        if (strlen(users[i].username) == 0)
        {
            users[i] = newUser;
            printf("User account created successfully.\n");

            // Append user signup information to the log file
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

            saveUserAccounts();
            return;
        }
    }

    printf("Maximum user account limit reached.\n");
}

bool login()
{
    char username[50];
    char password[50];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    loggedInUserIndex = findUserIndex(username);
    if (loggedInUserIndex != -1 && strcmp(users[loggedInUserIndex].password, password) == 0)
    {
        printf("Logged in successfully as %s.\n", username);

        // Append user login information to the log file
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
        loggedInUserIndex = -1; // Reset the index if login fails
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

bool isValidMobileNumber(const char *mobileNumber)
{
    // Check the length of the mobile number
    size_t length = strlen(mobileNumber);
    if (length != 10)
    {
        return false;
    }

    // Check that all characters are digits
    for (size_t i = 0; i < length; i++)
    {
        if (!isdigit(mobileNumber[i]))
        {
            return false;
        }
    }

    return true;
}

void bookAmbulance()
{
    if (loggedInUserIndex != -1)
    {
        int userX, userY;
        printf("Enter your location (x y): ");
        scanf("%d %d", &userX, &userY);

        int nearestHospital = findNearestHospital(userX, userY);
        if (nearestHospital != -1)
        {
            printf("Ambulance booked successfully!\n");
            printf("Nearest hospital: %s\n", graph.hospitals[nearestHospital].name);

            // Collect user information
            char patientName[50];
            int patientAge;
            char mobileNumber[15];
            char fullAddress[100];

            printf("\nEnter patient name: ");
            scanf("%s", patientName);
            printf("Enter patient age: ");
            scanf("%d", &patientAge);
            bool validMobileNumber = false;
            while (!validMobileNumber)
            {
                printf("Enter mobile number: ");
                scanf("%s", mobileNumber);

                if (isValidMobileNumber(mobileNumber))
                {
                    validMobileNumber = true;
                }
                else
                {
                    printf("Invalid mobile number. Please enter a 10-digit number.\n");
                }
            }

            printf("Enter full address: ");
            scanf(" %[^\n]s", fullAddress);

            // Generate and display receipt
            printf("\n--- Receipt ---\n");
            printf("Patient Name: %s\n", patientName);
            printf("Patient Age: %d\n", patientAge);
            printf("Mobile Number: %s\n", mobileNumber);
            printf("Full Address: %s\n", fullAddress);
            printf("Hospital: %s\n", graph.hospitals[nearestHospital].name);
            printf("---------------\n");

            // Append user ambulance booking information to the log file
            FILE *ambulanceBookingLog = fopen("ambulance_booking_log.txt", "a");
            if (ambulanceBookingLog == NULL)
            {
                printf("Error opening ambulance booking log file.\n");
                return;
            }
            fprintf(ambulanceBookingLog, "Username: %s\n", users[loggedInUserIndex].username);
            fprintf(ambulanceBookingLog, "Location: (%d, %d)\n", userX, userY);
            fprintf(ambulanceBookingLog, "Hospital: %s\n", graph.hospitals[nearestHospital].name);
            fprintf(ambulanceBookingLog, "Patient Name: %s\n", patientName);
            fprintf(ambulanceBookingLog, "Patient Age: %d\n", patientAge);
            fprintf(ambulanceBookingLog, "Mobile Number: %s\n", mobileNumber);
            fprintf(ambulanceBookingLog, "Full Address: %s\n", fullAddress);
            fprintf(ambulanceBookingLog, "====================\n");
            fclose(ambulanceBookingLog);
        }
        else
        {
            printf("No hospitals with available ambulances found.\n");
        }
    }
    else
    {
        printf("You must be logged in to book an ambulance.\n");
    }
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
