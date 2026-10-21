#include <stdio.h>
#include <stdlib.h>

struct Employee
{
    int emp_id;
    char emp_name[50];
    char designation[50];
    struct Employee *reports[50];
    int report_count;
    struct Employee *reporting_manager;
};

struct Employee *employees[100];
int emp_count = 0;

struct Employee *find_emp(int id)
{
    for (int i = 0; i < emp_count; i++)
    {
        if (employees[i]->emp_id == id)
        {
            return employees[i];
        }
    }
    return NULL;
}

int count_under(struct Employee *emp)
{
    int total = 0;
    for (int i = 0; i < emp->report_count; i++)
    {
        total = total + 1 + count_under(emp->reports[i]);
    }
    return total;
}

void print_tree(struct Employee *emp, int level)
{
    for (int i = 0; i < level; i++)
    {
        printf("    ");
    }
    printf("%d - %s - %s\n", emp->emp_id, emp->emp_name, emp->designation);
    for (int i = 0; i < emp->report_count; i++)
    {
        print_tree(emp->reports[i], level + 1);
    }
}

void add_emp()
{
    struct Employee *employee;
    struct Employee *manager = NULL;
    int id;
    char name[50];
    char desig[50];
    int manager_id;

    printf("Enter employee id: ");
    scanf("%d", &id);
    if (id <= 0)
    {
        printf("Employee id must be a positive number.\n");
        return;
    }
    if (find_emp(id) != NULL)
    {
        printf("emloyee id alrady exists\n");
        return;
    }
    printf("Enter manager id(0 if employee is CEO): ");
    scanf("%d", &manager_id);
    if (manager_id == 0)
    {
        for (int i = 0; i < emp_count; i++)
        {
            if (employees[i]->reporting_manager == NULL)
            {
                printf("A CEO already exists.\n");
                return;
            }
        }
    }
    printf("Enter employee name: ");
    scanf(" %49[^\n]", name);
    printf("Enter employee designation: ");
    scanf(" %49[^\n]", desig);

    if (manager_id == id)
    {
        printf("An employee cannot report to themselves.\n");
        return;
    }
    if (manager_id != 0)
    {
        manager = find_emp(manager_id);
        if (manager == NULL)
        {
            printf("Reporting manager does not exist.\n");
            return;
        }
    }

    employee = malloc(sizeof(struct Employee));
    if (employee == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }
    employee->emp_id = id;
    snprintf(employee->emp_name, sizeof(employee->emp_name), "%s", name);
    snprintf(employee->designation, sizeof(employee->designation), "%s", desig);
    employee->report_count = 0;
    employee->reporting_manager = manager;
    if (manager != NULL)
    {
        manager->reports[manager->report_count] = employee;
        manager->report_count++;
    }

    employees[emp_count] = employee;
    emp_count++;
}

void remove_emp()
{
    struct Employee *emp;
    struct Employee *manager;
    struct Employee *receiver = NULL;
    struct Employee *sibling;
    struct Employee *child;
    int id;
    int position = -1;
    int new_count;

    printf("Enter employee id to remove: ");
    scanf("%d", &id);
    emp = find_emp(id);
    if (emp == NULL)
    {
        printf("Employee not found.\n");
        return;
    }
    if (emp->reporting_manager == NULL)
    {
        printf("The CEO (top-level employee) cannot be removed.\n");
        return;
    }

    manager = emp->reporting_manager;
    if (emp->report_count > 0)
    {
        for (int i = 0; i < manager->report_count; i++)
        {
            sibling = manager->reports[i];

            if (sibling != emp)
            {
                if (receiver == NULL || sibling->report_count < receiver->report_count)
                {
                    receiver = sibling;
                }
            }
        }
        if (receiver == NULL)
        {
            receiver = manager;
        }
        if (receiver == manager)
        {
            new_count = manager->report_count - 1 + emp->report_count;
        }
        else
        {
            new_count = receiver->report_count + emp->report_count;
        }
        if (new_count > 50)
        {
            printf("The employee taking over cannot handle that many reports.\n");
            return;
        }
    }
    for (int i = 0; i < manager->report_count; i++)
    {
        if (manager->reports[i] == emp)
        {
            position = i;
        }
    }
    for (int i = position; i < manager->report_count - 1; i++)
    {
        manager->reports[i] = manager->reports[i + 1];
    }
    manager->report_count--;
    for (int i = 0; i < emp->report_count; i++)
    {
        child = emp->reports[i];
        child->reporting_manager = receiver;
        receiver->reports[receiver->report_count] = child;
        receiver->report_count++;
    }
    for (int i = 0; i < emp_count; i++)
    {
        if (employees[i] == emp)
        {
            position = i;
        }
    }
    for (int i = position; i < emp_count - 1; i++)
    {
        employees[i] = employees[i + 1];
    }
    emp_count--;

    if (emp->report_count > 0)
    {
        printf("Employee removed. Their reports now report to %s (%d).\n", receiver->emp_name, receiver->emp_id);
    }
    else
    {
        printf("Employee removed successfully.\n");
    }
    free(emp);
}

void display_emp_hier()
{
    int id;
    struct Employee *emp;
    printf("Enter employee id: ");
    scanf("%d", &id);
    emp = find_emp(id);
    if (emp == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    printf("\n===== HIERARCHY OF EMPLOYEE %d =====\n", id);
    print_tree(emp, 0);
    printf("Total Employees Under %d: %d\n", id, count_under(emp));
}

void display_comp_hier()
{
    int total = 0;
    if (emp_count == 0)
    {
        printf("No employees in the organization yet.\n");
        return;
    }

    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    for (int i = 0; i < emp_count; i++)
    {
        if (employees[i]->reporting_manager == NULL)
        {
            print_tree(employees[i], 0);
            total = total + 1 + count_under(employees[i]);
        }
    }
    printf("Total Employees: %d\n", total);
}

int main()
{
    int choice;
    do
    {
        printf("==== EMPLOYEE HIERARCHY MANAGEMENT ====\n\n1. Add Employee\n2. Remove Employee\n3. Display Employee Hierarchy\n4. Display Complete Hierarchy\n5. Exit\n\nEnter your choice:");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            add_emp();
            break;
        case 2:
            remove_emp();
            break;
        case 3:
            display_emp_hier();
            break;
        case 4:
            display_comp_hier();
            break;
        case 5:
            printf("Program ended.\n");
            break;
        default:
            printf("Invalid Choice.\n");
            break;
        }
    } while (choice != 5);
    for (int i = 0; i < emp_count; i++)
    {
        free(employees[i]);
    }
    return 0;
}