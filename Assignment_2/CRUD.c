#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct User
{
    int id;
    char name[50];
    int age;
};

int idExists(int id)
{
    FILE *file;
    struct User user;

    file = fopen("users.txt", "r");

    if (file == NULL)
    {
        return 0;
    }

    while (fscanf(file, "%d %49s %d",
                  &user.id, user.name, &user.age) == 3)
    {
        if (user.id == id)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void createUser()
{
    FILE *file;
    struct User user;
    file = fopen("users.txt", "a");
    if (file == NULL)
    {
        printf("File could not be opened.\n");
        return;
    }

    printf("Enter User ID: ");
    scanf("%d", &user.id);

    if (idExists(user.id))
    {
        printf("User ID already exists.\n");
        fclose(file);
        return;
    }

    printf("Enter User Name: ");
    scanf("%49s", user.name);

    printf("Enter User Age: ");
    scanf("%d", &user.age);

    fprintf(file, "%d %s %d\n", user.id, user.name, user.age);
    fclose(file);

    printf("User added successfully.\n");
}

void readUsers()
{
    FILE *file;
    struct User user;
    file = fopen("users.txt", "r");
    if (file == NULL)
    {
        printf("No users found.\n");
        return;
    }

    printf("\n---- User Records ----\n");
    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3)
    {
        printf("ID: %d | Name: %s | Age: %d\n", user.id, user.name, user.age);
    }
    fclose(file);
}

void updateUser()
{
    FILE *file, *temp;
    struct User user;
    int id, found = 0;

    file = fopen("users.txt", "r");

    if (file == NULL)
    {
        printf("File does not exist.\n");
        return;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL)
    {
        printf("Temporary file could not be created.\n");
        fclose(file);
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3)
    {
        if (user.id == id)
        {
            found = 1;

            printf("Enter new name: ");
            scanf("%49s", user.name);

            printf("Enter new age: ");
            scanf("%d", &user.age);
        }

        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
    {
        printf("User updated successfully.\n");
    }
    else
    {
        printf("User ID not found.\n");
    }
}

void deleteUser()
{
    FILE *file, *temp;
    struct User user;
    int id, found = 0;

    file = fopen("users.txt", "r");

    if (file == NULL)
    {
        printf("File does not exist.\n");
        return;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL)
    {
        printf("Temporary file could not be created.\n");
        fclose(file);
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3)
    {
        if (user.id == id)
        {
            found = 1;
            continue;
        }

        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
    {
        printf("User deleted successfully.\n");
    }
    else
    {
        printf("User ID not found.\n");
    }
}

int main()
{
    int choice;
    do
    {
        printf("\n----User Management----\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createUser();
            break;

        case 2:
            readUsers();
            break;

        case 3:
            updateUser();
            break;

        case 4:
            deleteUser();
            break;

        case 5:
            printf("Program ended.\n");
            break;

        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 5);
    return 0;
}