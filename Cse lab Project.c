#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FLIGHTS 100
#define MAX_TICKETS 100
#define MAX_USERS 100
#define ADMIN_NAME "mustafiz"
#define ADMIN_PASSWORD "2513681"


#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

typedef struct {
    int id;
    char from[30];
    char to[30];
    char date[15];
    char departure[10];
    float price;
    int seats;
    char type[10];
    char classType[10];
} Flight;

typedef struct {
    int id;
    char username[30];
    char passengerName[50];
    int flightId;
    char classType[10];
} Ticket;

typedef struct {
    char username[30];
    char password[30];
} User;

Flight flights[MAX_FLIGHTS];
int flightCount = 0;
Ticket tickets[MAX_TICKETS];
int ticketCount = 0;
User users[MAX_USERS];
int userCount = 0;


void loadData() {
    FILE *f1 = fopen("flights.dat", "rb");
    if (f1) {
        fread(&flightCount, sizeof(int), 1, f1);
        fread(flights, sizeof(Flight), flightCount, f1);
        fclose(f1);
    }

    FILE *f2 = fopen("tickets.dat", "rb");
    if (f2) {
        fread(&ticketCount, sizeof(int), 1, f2);
        fread(tickets, sizeof(Ticket), ticketCount, f2);
        fclose(f2);
    }

    FILE *f3 = fopen("users.dat", "rb");
    if (f3) {
        fread(&userCount, sizeof(int), 1, f3);
        fread(users, sizeof(User), userCount, f3);
        fclose(f3);
    }
}

void saveData() {
    FILE *f1 = fopen("flights.dat", "wb");
    fwrite(&flightCount, sizeof(int), 1, f1);
    fwrite(flights, sizeof(Flight), flightCount, f1);
    fclose(f1);

    FILE *f2 = fopen("tickets.dat", "wb");
    fwrite(&ticketCount, sizeof(int), 1, f2);
    fwrite(tickets, sizeof(Ticket), ticketCount, f2);
    fclose(f2);

    FILE *f3 = fopen("users.dat", "wb");
    fwrite(&userCount, sizeof(int), 1, f3);
    fwrite(users, sizeof(User), userCount, f3);
    fclose(f3);
}

void colorPrint(const char *text, const char *color) {
    printf("%s%s%s", color, text, RESET);
}

int validateMobile(char *mobile) {
    if (strlen(mobile) != 11) return 0;
    for (int i = 0; i < 11; i++) {
        if (mobile[i] < '0' || mobile[i] > '10') return 0;
    }
    return 1;
}

int validatePassword(char *pass) {
    int len = strlen(pass);
    return len >= 6 && len <= 20;
}

int userLogin(char *username) {
    char pass[30];
    printf("Enter password: ");
    scanf("%s", pass);
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, pass) == 0) return 1;
    }
    return 0;
}

void signUp() {
    char username[30], password[30];
    printf("Enter your 11-digit mobile number: ");
    scanf("%s", username);
    if (!validateMobile(username)) {
        colorPrint("Invalid mobile number.\n", RED);
        return;
    }
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            colorPrint("User already exists.\n", YELLOW);
            return;
        }
    }
    printf("Enter password (6-20 characters): ");
    scanf("%s", password);
    if (!validatePassword(password)) {
        colorPrint("Invalid password length.\n", RED);
        return;
    }
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    userCount++;
    saveData();
    colorPrint("User registered successfully!\n", GREEN);
}

void addFlight() {
    Flight f;
    f.id = flightCount + 1;
    printf("Enter from location: ");
    scanf("%s", f.from);
    printf("Enter to location: ");
    scanf("%s", f.to);
    printf("Enter date (YYYY-MM-DD): ");
    scanf("%s", f.date);
    printf("Enter departure time (HH:MM): ");
    scanf("%s", f.departure);
    printf("Enter ticket price: ");
    scanf("%f", &f.price);
    printf("Enter number of seats: ");
    scanf("%d", &f.seats);
    printf("Trip type (OneWay/Round): ");
    scanf("%s", f.type);
    printf("Class type (Economy/Business): ");
    scanf("%s", f.classType);
    flights[flightCount++] = f;
    saveData();
    colorPrint("Flight added successfully!\n", GREEN);
}

void viewFlights() {
    colorPrint("\nAvailable Flights:\n", CYAN);
    for (int i = 0; i < flightCount; i++) {
        printf("ID: %d | %s -> %s | Date: %s | Time: %s | %s | %s | $%.2f | Seats: %d\n",
            flights[i].id, flights[i].from, flights[i].to, flights[i].date, flights[i].departure,
            flights[i].type, flights[i].classType, flights[i].price, flights[i].seats);
    }
}

void bookTicket(char *username) {
    char name[50];
    int flightId;
    char classChoice[10];

    printf("Enter your name: ");
    scanf("%s", name);
    viewFlights();
    printf("Enter Flight ID to book: ");
    scanf("%d", &flightId);
    printf("Enter class type (Economy/Business): ");
    scanf("%s", classChoice);

    for (int i = 0; i < flightCount; i++) {
        if (flights[i].id == flightId && flights[i].seats > 0 && strcmp(flights[i].classType, classChoice) == 0) {
            Ticket t;
            t.id = ticketCount + 1;
            strcpy(t.username, username);
            strcpy(t.passengerName, name);
            t.flightId = flightId;
            strcpy(t.classType, classChoice);
            tickets[ticketCount++] = t;
            flights[i].seats--;
            saveData();
            printf("Ticket booked successfully! Ticket ID: %d\n", t.id);
            return;
        }
    }
    colorPrint("Invalid flight ID or no available seats.\n", RED);
}

void cancelTicket() {
    int id;
    printf("Enter Ticket ID to cancel: ");
    scanf("%d", &id);
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].id == id) {
            for (int j = 0; j < flightCount; j++) {
                if (flights[j].id == tickets[i].flightId) {
                    flights[j].seats++;
                    break;
                }
            }
            for (int k = i; k < ticketCount - 1; k++) {
                tickets[k] = tickets[k + 1];
            }
            ticketCount--;
            saveData();
            colorPrint("Ticket canceled.\n", YELLOW);
            return;
        }
    }
    colorPrint("Ticket not found.\n", RED);
}

void viewTickets() {
    colorPrint("\nAll Booked Tickets:\n", CYAN);
    if (ticketCount == 0) {
        colorPrint("No Tickets booked yet.\n", YELLOW);
        for (int i = 0; i < flightCount; i++) {
            if (flights[i].seats > 0) {
                printf("Flight ID: %d | From: %s | To: %s | Date: %s | Class: %s | Price: $%.2f | Seats: %d\n",
                       flights[i].id, flights[i].from, flights[i].to, flights[i].date, flights[i].classType,
                       flights[i].price, flights[i].seats);
            }
        }
        return;
    }
    for (int i = 0; i < ticketCount; i++) {
        printf("Ticket ID: %d | Username: %s | Passenger: %s | Flight ID: %d | Class: %s\n",
            tickets[i].id, tickets[i].username, tickets[i].passengerName, tickets[i].flightId, tickets[i].classType);
    }
}

void searchTicket() {
    char keyword[50];
    printf("Enter passenger name, username or ticket ID: ");
    scanf("%s", keyword);
    for (int i = 0; i < ticketCount; i++) {
        if (strstr(tickets[i].passengerName, keyword) || strstr(tickets[i].username, keyword) || atoi(keyword) == tickets[i].id) {
            printf("Found Ticket - ID: %d | Username: %s | Name: %s | Flight ID: %d | Class: %s\n",
                tickets[i].id, tickets[i].username, tickets[i].passengerName, tickets[i].flightId, tickets[i].classType);
            return;
        }
    }
    colorPrint("No matching ticket found.\n", RED);
}

void updateTicket() {
    int id;
    printf("Enter Ticket ID to update: ");
    scanf("%d", &id);
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].id == id) {
            printf("Updating Ticket ID: %d\n", tickets[i].id);
            printf("Current name: %s\nEnter new name: ", tickets[i].passengerName);
            scanf("%s", tickets[i].passengerName);
            printf("Current username: %s\nEnter new username: ", tickets[i].username);
            scanf("%s", tickets[i].username);
            saveData();
            colorPrint("Ticket updated successfully.\n", GREEN);
            return;
        }
    }
    colorPrint("Ticket not found.\n", RED);
}

void adminMenu() {
    int choice;
    do {
        colorPrint("\nAdmin Panel\n", YELLOW);
        printf("1. Add Flight\n2. View Flights\n3. View All Tickets\n4. Logout\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: addFlight(); break;
            case 2: viewFlights(); break;
            case 3: viewTickets(); break;
            case 4: colorPrint("Logging out...\n", CYAN); break;
            default: colorPrint("Invalid choice.\n", RED);
        }
    } while (choice != 4);
}

void passengerMenu(char *username) {
    int choice;
    do {
        colorPrint("\nPassenger Menu\n", CYAN);
        printf("1. Book Ticket\n2. Cancel Ticket\n3. Search Ticket\n4. Update Ticket\n5. View Flights\n6. Logout\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: bookTicket(username); break;
            case 2: cancelTicket(); break;
            case 3: searchTicket(); break;
            case 4: updateTicket(); break;
            case 5: viewFlights(); break;
            case 6: colorPrint("Logging out...\n", YELLOW); break;
            default: colorPrint("Invalid choice.\n", RED);
        }
    } while (choice != 6);
}

int main() {
    loadData();
    int role;
    char name[30], password[30];
    while (1) {
        colorPrint("\n===== Flight Ticket Booking System =====\n", CYAN);
        printf("1. Admin Login\n2. Sign Up\n3. Passenger Login\n4. Exit\nEnter choice: ");
        scanf("%d", &role);
        switch (role) {
            case 1:
                printf("Enter admin name: ");
                scanf("%s", name);
                printf("Enter admin password: ");
                scanf("%s", password);
                if (strcmp(name, ADMIN_NAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0)
                    adminMenu();
                else
                    colorPrint("Incorrect admin credentials.\n", RED);
                break;
            case 2:
                signUp();
                break;
            case 3:
                printf("Enter mobile number: ");
                scanf("%s", name);
                if (userLogin(name)) passengerMenu(name);
                else colorPrint("Login failed.\n", RED);
                break;
            case 4:
                colorPrint("Exiting system. Goodbye!\n", GREEN);
                saveData();
                exit(0);
            default:
                colorPrint("Invalid choice.\n", RED);
        }
    }
    return 0;
}

