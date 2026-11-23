#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define TEAM_COUNT 10
#define MAX_NAME 50
#define MAX_PLAYERS_PER_TEAM 50
#define MAX_PLAYER_ID 1000

enum { BATSMAN = 1, BOWLER = 2, ALL_ROUNDER = 3 };

typedef struct PlayerNode {
    int playerId;
    int teamId;
    int role;
    int totalRuns;
    int wickets;
    float battingAverage;
    float strikeRate;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
    char name[MAX_NAME + 1];
} PlayerNode;

typedef struct {
    int teamId;
    int totalPlayers;
    int battingPlayerCount;
    float averageStrikeRate;
    double battingStrikeRateSum;
    PlayerNode *roleHeads[3];
    char name[MAX_NAME + 1];
} Team;

typedef struct {
    PlayerNode *player;
} HeapItem;

static Team teamList[TEAM_COUNT];
static unsigned char usedPlayerIds[MAX_PLAYER_ID + 1];

static const char *roleNames[] = {"", "Batsman", "Bowler", "All-rounder"};

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

int getTeamIdByName(const char *name) {
    if (strcmp(name, "India") == 0) return 1;
    if (strcmp(name, "Australia") == 0) return 2;
    if (strcmp(name, "England") == 0) return 3;
    if (strcmp(name, "Pakistan") == 0) return 4;
    if (strcmp(name, "New Zealand") == 0) return 5;
    if (strcmp(name, "South Africa") == 0) return 6;
    if (strcmp(name, "Bangladesh") == 0) return 7;
    if (strcmp(name, "Sri Lanka") == 0) return 8;
    if (strcmp(name, "Afghanistan") == 0) return 9;
    if (strcmp(name, "West Indies") == 0) return 10;
    return -1;
}

int getRoleFromText(const char *role) {
    if (strcmp(role, "Batsman") == 0) return BATSMAN;
    if (strcmp(role, "Bowler") == 0) return BOWLER;
    if (strcmp(role, "All-rounder") == 0) return ALL_ROUNDER;
    return 0;
}

float calculatePerformanceIndex(const PlayerNode *player) {
    float battingScore = (player->battingAverage * player->strikeRate) / 100.0f;

    if (player->role == BATSMAN)
        return battingScore;
    if (player->role == BOWLER)
        return (player->wickets * 2.0f) + (100.0f - player->economyRate);
    return battingScore + (player->wickets * 2.0f);
}

void initializeTeams(void) {
    int i;
    memset(usedPlayerIds, 0, sizeof(usedPlayerIds));

    for (i = 0; i < TEAM_COUNT; i++) {
        teamList[i].teamId = i + 1;
        teamList[i].totalPlayers = 0;
        teamList[i].battingPlayerCount = 0;
        teamList[i].averageStrikeRate = 0.0f;
        teamList[i].battingStrikeRateSum = 0.0;
        teamList[i].roleHeads[0] = NULL;
        teamList[i].roleHeads[1] = NULL;
        teamList[i].roleHeads[2] = NULL;
        teamList[i].name[0] = '\0';
    }

    for (i = 0; i < teamCount; i++) {
        int id = getTeamIdByName(teams[i]);
        if (id >= 1 && id <= TEAM_COUNT)
            snprintf(teamList[id - 1].name, sizeof(teamList[id - 1].name), "%s", teams[i]);
    }
}

int binarySearchTeam(int teamId) {
    int low = 0, high = TEAM_COUNT - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (teamList[mid].teamId == teamId)
            return mid;
        if (teamList[mid].teamId < teamId)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

PlayerNode *createPlayerNode(int id, const char *name, int teamId, int role,
                             int runs, float average, float strikeRate,
                             int wickets, float economyRate) {
    PlayerNode *node = (PlayerNode *)malloc(sizeof(PlayerNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    node->playerId = id;
    node->teamId = teamId;
    node->role = role;
    node->totalRuns = runs;
    node->wickets = wickets;
    node->battingAverage = average;
    node->strikeRate = strikeRate;
    node->economyRate = economyRate;
    node->performanceIndex = calculatePerformanceIndex(node);
    node->next = NULL;
    snprintf(node->name, sizeof(node->name), "%s", name);
    return node;
}

void insertPlayerSorted(Team *team, PlayerNode *newNode) {
    int listIndex = newNode->role - 1;
    PlayerNode **head = &team->roleHeads[listIndex];
    PlayerNode *current;

    if (*head == NULL || newNode->performanceIndex > (*head)->performanceIndex) {
        newNode->next = *head;
        *head = newNode;
    } else {
        current = *head;
        while (current->next != NULL &&
               current->next->performanceIndex >= newNode->performanceIndex) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    if (newNode->playerId >= 1 && newNode->playerId <= MAX_PLAYER_ID)
        usedPlayerIds[newNode->playerId] = 1;

    team->totalPlayers++;
    if (newNode->role == BATSMAN || newNode->role == ALL_ROUNDER) {
        team->battingStrikeRateSum += (double)newNode->strikeRate;
        team->battingPlayerCount++;
    }
    if (team->battingPlayerCount > 0)
        team->averageStrikeRate = (float)(team->battingStrikeRateSum / team->battingPlayerCount);
    else
        team->averageStrikeRate = 0.0f;
}

void initializePlayers(void) {
    int i;
    for (i = 0; i < playerCount; i++) {
        int teamId = getTeamIdByName(players[i].team);
        int role = getRoleFromText(players[i].role);
        int teamIndex = binarySearchTeam(teamId);
        PlayerNode *node;

        if (teamIndex == -1 || role == 0) {
            printf("Skipping invalid record: %s\n", players[i].name);
            continue;
        }

        node = createPlayerNode(players[i].id, players[i].name, teamId, role,
                                players[i].totalRuns, players[i].battingAverage,
                                players[i].strikeRate, players[i].wickets,
                                players[i].economyRate);
        if (node != NULL)
            insertPlayerSorted(&teamList[teamIndex], node);
    }
}

int playerIdExists(int playerId) {
    if (playerId >= 1 && playerId <= MAX_PLAYER_ID)
        return usedPlayerIds[playerId] != 0;
    return 0;
}

void printTableHeader(int includeTeam) {
    printf("\n");
    if (includeTeam)
        printf("%-6s %-25s %-16s %-13s %-9s %-8s %-7s %-7s %-9s %-12s\n",
               "ID", "Name", "Team", "Role", "Runs", "Average", "SR", "Wkts", "Economy", "Perf.Index");
    else
        printf("%-6s %-25s %-13s %-9s %-8s %-7s %-7s %-9s %-12s\n",
               "ID", "Name", "Role", "Runs", "Average", "SR", "Wkts", "Economy", "Perf.Index");
}

void printPlayer(const PlayerNode *player, int includeTeam) {
    if (includeTeam) {
        int teamIndex = player->teamId - 1;
        const char *teamName = (teamIndex >= 0 && teamIndex < TEAM_COUNT)
                               ? teamList[teamIndex].name : "Unknown";
        printf("%-6d %-25.25s %-16.16s %-13s %-9d %-8.1f %-7.1f %-7d %-9.1f %-12.2f\n",
               player->playerId, player->name, teamName,
               roleNames[player->role], player->totalRuns,
               player->battingAverage, player->strikeRate, player->wickets,
               player->economyRate, player->performanceIndex);
    } else {
        printf("%-6d %-25.25s %-13s %-9d %-8.1f %-7.1f %-7d %-9.1f %-12.2f\n",
               player->playerId, player->name, roleNames[player->role],
               player->totalRuns, player->battingAverage, player->strikeRate,
               player->wickets, player->economyRate, player->performanceIndex);
    }
}

void displayPlayersOfTeam(void) {
    int teamId, index, role;
    printf("Enter Team ID (1-10): ");
    if (scanf("%d", &teamId) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }

    index = binarySearchTeam(teamId);
    if (index == -1) {
        printf("Team not found.\n");
        return;
    }

    printf("\nPlayers of Team %s (Team ID: %d)\n", teamList[index].name, teamId);
    printTableHeader(0);
    for (role = 0; role < 3; role++) {
        PlayerNode *current = teamList[index].roleHeads[role];
        while (current != NULL) {
            printPlayer(current, 0);
            current = current->next;
        }
    }
    printf("Total Players: %d\n", teamList[index].totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamList[index].averageStrikeRate);
}

void mergeTeams(Team *arr[], int left, int mid, int right) {
    Team *temp[TEAM_COUNT];
    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        if (arr[i]->averageStrikeRate > arr[j]->averageStrikeRate ||
           (arr[i]->averageStrikeRate == arr[j]->averageStrikeRate &&
            arr[i]->teamId < arr[j]->teamId)) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid)
        temp[k++] = arr[i++];
    while (j <= right)
        temp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

void mergeSortTeams(Team *arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortTeams(arr, left, mid);
        mergeSortTeams(arr, mid + 1, right);
        mergeTeams(arr, left, mid, right);
    }
}

void displayTeamsByAverageStrikeRate(void) {
    Team *sortedTeams[TEAM_COUNT];
    int i;
    for (i = 0; i < TEAM_COUNT; i++)
        sortedTeams[i] = &teamList[i];

    mergeSortTeams(sortedTeams, 0, TEAM_COUNT - 1);

    printf("\nTeams Sorted by Average Batting Strike Rate (Descending)\n");
    printf("%-8s %-22s %-15s %-15s\n", "Team ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("---------------------------------------------------------------\n");
    for (i = 0; i < TEAM_COUNT; i++) {
        printf("%-8d %-22s %-15.2f %-15d\n",
               sortedTeams[i]->teamId, sortedTeams[i]->name,
               sortedTeams[i]->averageStrikeRate, sortedTeams[i]->totalPlayers);
    }
}

int readRole(void) {
    int role;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &role) != 1 || role < 1 || role > 3) {
        clearInputBuffer();
        printf("Invalid role.\n");
        return 0;
    }
    return role;
}

void displayTopKPlayers(void) {
    int teamId, k, role, index, shown = 0;
    PlayerNode *current;

    printf("Enter Team ID (1-10): ");
    if (scanf("%d", &teamId) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    index = binarySearchTeam(teamId);
    if (index == -1) {
        printf("Team not found.\n");
        return;
    }

    role = readRole();
    if (role == 0) return;

    printf("Enter number of players (K): ");
    if (scanf("%d", &k) != 1 || k <= 0) {
        clearInputBuffer();
        printf("K must be a positive integer.\n");
        return;
    }

    printf("\nTop %d %s(s) of Team %s\n", k, roleNames[role], teamList[index].name);
    current = teamList[index].roleHeads[role - 1];
    if (current == NULL) {
        printf("No players found for this role.\n");
        return;
    }

    printTableHeader(0);
    while (current != NULL && shown < k) {
        printPlayer(current, 0);
        current = current->next;
        shown++;
    }
    printf("Players displayed: %d\n", shown);
}

int higherPriority(const HeapItem *a, const HeapItem *b) {
    if (a->player->performanceIndex != b->player->performanceIndex)
        return a->player->performanceIndex > b->player->performanceIndex;
    return a->player->playerId < b->player->playerId;
}

void heapPush(HeapItem heap[], int *size, HeapItem item) {
    int child = (*size)++;
    while (child > 0) {
        int parent = (child - 1) / 2;
        if (higherPriority(&heap[parent], &item)) break;
        heap[child] = heap[parent];
        child = parent;
    }
    heap[child] = item;
}

HeapItem heapPop(HeapItem heap[], int *size) {
    HeapItem result = heap[0];
    HeapItem last = heap[--(*size)];
    int parent = 0;

    while (parent * 2 + 1 < *size) {
        int child = parent * 2 + 1;
        if (child + 1 < *size && higherPriority(&heap[child + 1], &heap[child]))
            child++;
        if (higherPriority(&last, &heap[child])) break;
        heap[parent] = heap[child];
        parent = child;
    }
    if (*size > 0)
        heap[parent] = last;
    return result;
}

void displayAllPlayersOfRole(void) {
    int role = readRole();
    HeapItem heap[TEAM_COUNT];
    int heapSize = 0, i;
    long long displayed = 0;

    if (role == 0) return;

    for (i = 0; i < TEAM_COUNT; i++) {
        PlayerNode *head = teamList[i].roleHeads[role - 1];
        if (head != NULL) {
            HeapItem item;
            item.player = head;
            heapPush(heap, &heapSize, item);
        }
    }

    if (heapSize == 0) {
        printf("No players found for this role.\n");
        return;
    }

    printf("\nAll %s(s) Across All Teams by Performance Index\n", roleNames[role]);
    printTableHeader(1);

    while (heapSize > 0) {
        HeapItem item = heapPop(heap, &heapSize);
        PlayerNode *nextPlayer;
        printPlayer(item.player, 1);
        displayed++;

        nextPlayer = item.player->next;
        if (nextPlayer != NULL) {
            HeapItem nextItem;
            nextItem.player = nextPlayer;
            heapPush(heap, &heapSize, nextItem);
        }
    }
    printf("Total %s(s): %lld\n", roleNames[role], displayed);
}

void addPlayer(void) {
    int teamId, teamIndex, playerId, role, runs, wickets;
    char name[MAX_NAME + 1];
    float average, strikeRate, economyRate;
    PlayerNode *node;

    printf("Enter Team ID (1-10): ");
    if (scanf("%d", &teamId) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    teamIndex = binarySearchTeam(teamId);
    if (teamIndex == -1) {
        printf("Team not found.\n");
        return;
    }
    if (teamList[teamIndex].totalPlayers >= MAX_PLAYERS_PER_TEAM) {
        printf("This team has reached the maximum of %d players.\n", MAX_PLAYERS_PER_TEAM);
        return;
    }

    printf("Enter Player ID (1-%d): ", MAX_PLAYER_ID);
    if (scanf("%d", &playerId) != 1 || playerId < 1 || playerId > MAX_PLAYER_ID) {
        clearInputBuffer();
        printf("Invalid Player ID.\n");
        return;
    }
    if (playerIdExists(playerId)) {
        printf("This Player ID already exists. Player IDs must be unique.\n");
        return;
    }

    printf("Enter Player Name: ");
    if (scanf(" %50[^\n]", name) != 1) {
        clearInputBuffer();
        printf("Invalid name.\n");
        return;
    }

    role = readRole();
    if (role == 0) return;

    printf("Total Runs: ");
    if (scanf("%d", &runs) != 1 || runs < 0) {
        clearInputBuffer();
        printf("Invalid total runs.\n");
        return;
    }
    printf("Batting Average: ");
    if (scanf("%f", &average) != 1 || average < 0.0f) {
        clearInputBuffer();
        printf("Invalid batting average.\n");
        return;
    }
    printf("Strike Rate: ");
    if (scanf("%f", &strikeRate) != 1 || strikeRate < 0.0f) {
        clearInputBuffer();
        printf("Invalid strike rate.\n");
        return;
    }
    printf("Wickets: ");
    if (scanf("%d", &wickets) != 1 || wickets < 0) {
        clearInputBuffer();
        printf("Invalid wickets.\n");
        return;
    }
    printf("Economy Rate: ");
    if (scanf("%f", &economyRate) != 1 || economyRate < 0.0f) {
        clearInputBuffer();
        printf("Invalid economy rate.\n");
        return;
    }

    node = createPlayerNode(playerId, name, teamId, role, runs, average,
                            strikeRate, wickets, economyRate);
    if (node == NULL) return;

    insertPlayerSorted(&teamList[teamIndex], node);
    printf("Player added successfully to Team %s!\n", teamList[teamIndex].name);
    printf("Performance Index: %.2f\n", node->performanceIndex);
}

void freeAllPlayers(void) {
    int i, role;
    for (i = 0; i < TEAM_COUNT; i++) {
        for (role = 0; role < 3; role++) {
            PlayerNode *current = teamList[i].roleHeads[role];
            while (current != NULL) {
                PlayerNode *temp = current;
                current = current->next;
                free(temp);
            }
            teamList[i].roleHeads[role] = NULL;
        }
    }
}

void displayMenu(void) {
    printf("\nICC ODI PLAYER PERFORMANCE ANALYZER\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display All Players of a Specific Role Across All Teams\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
}

int main(void) {
    int choice = 0;

    initializeTeams();
    initializePlayers();

    printf("\nICC ODI Player Performance Analyzer initialized successfully.\n");
    printf("Loaded %d predefined player records across %d teams.\n", playerCount, teamCount);

    do {
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Please enter a valid menu option.\n");
            continue;
        }

        switch (choice) {
            case 1:
                addPlayer();
                break;
            case 2:
                displayPlayersOfTeam();
                break;
            case 3:
                displayTeamsByAverageStrikeRate();
                break;
            case 4:
                displayTopKPlayers();
                break;
            case 5:
                displayAllPlayersOfRole();
                break;
            case 6:
                freeAllPlayers();
                printf("Memory released. Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please select 1-6.\n");
        }
    } while (choice != 6);

    return 0;
}