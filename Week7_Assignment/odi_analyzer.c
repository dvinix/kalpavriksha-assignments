
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define MAX_TEAMS 10
#define MAX_PER_TEAM 50
#define MAX_NAME 50
#define MAX_ID 9999

typedef enum {
    BATSMAN = 1,
    BOWLER = 2,
    ALLROUNDER = 3
} Role;

typedef struct PlayerNode {
    int playerId;
    char name[MAX_NAME + 1];
    char teamName[MAX_NAME + 1];
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *nextInTeam;
    struct PlayerNode *nextInRole;
} PlayerNode;

typedef struct {
    int teamId;
    char name[MAX_NAME + 1];
    int totalPlayers;
    double averageBattingStrikeRate;
    double srSum;
    int srCount;
    PlayerNode *allHead;
    PlayerNode *allTail;
    PlayerNode *roleHead[4];
} Team;

static Team teamTable[MAX_TEAMS];
static int numTeams = 0;
static char idUsed[MAX_ID + 1];

static const char *roleName(Role r) {
    switch (r) {
        case BATSMAN: return "Batsman";
        case BOWLER: return "Bowler";
        default: return "All-rounder";
    }
}

static const char *rolePlural(Role r) {
    switch (r) {
        case BATSMAN: return "Batsmen";
        case BOWLER: return "Bowlers";
        default: return "All-rounders";
    }
}

static float computePI(Role r, float avg, float sr, int wk, float er) {
    switch (r) {
        case BATSMAN:
            return (avg * sr) / 100.0f;
        case BOWLER:
            return (wk * 2) + (100.0f - er);
        default:
            return (avg * sr) / 100.0f + (wk * 2);
    }
}

static Team *findTeam(int id) {
    int lo = 0;
    int hi = numTeams - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (teamTable[mid].teamId == id)
            return &teamTable[mid];

        if (teamTable[mid].teamId < id)
            lo = mid + 1;
        else
            hi = mid - 1;
    }

    return NULL;
}

static void linkPlayer(Team *t, PlayerNode *p) {
    p->nextInTeam = NULL;

    if (t->allTail)
        t->allTail->nextInTeam = p;
    else
        t->allHead = p;

    t->allTail = p;

    PlayerNode **pp = &t->roleHead[p->role];

    while (*pp && (*pp)->performanceIndex >= p->performanceIndex)
        pp = &(*pp)->nextInRole;

    p->nextInRole = *pp;
    *pp = p;

    t->totalPlayers++;

    if (p->role == BATSMAN || p->role == ALLROUNDER) {
        t->srSum += p->strikeRate;
        t->srCount++;
        t->averageBattingStrikeRate = t->srSum / t->srCount;
    }

    idUsed[p->playerId] = 1;
}

static PlayerNode *newPlayer(
    int id,
    const char *name,
    const char *team,
    Role role,
    int runs,
    float avg,
    float sr,
    int wk,
    float er
) {
    PlayerNode *p = calloc(1, sizeof(PlayerNode));

    if (!p) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    p->playerId = id;
    snprintf(p->name, sizeof p->name, "%s", name);
    snprintf(p->teamName, sizeof p->teamName, "%s", team);
    p->role = role;
    p->totalRuns = runs;
    p->battingAverage = avg;
    p->strikeRate = sr;
    p->wickets = wk;
    p->economyRate = er;
    p->performanceIndex = computePI(role, avg, sr, wk, er);

    return p;
}

static int parseRole(const char *s, Role *out) {
    if (strcmp(s, "Batsman") == 0) {
        *out = BATSMAN;
        return 1;
    }

    if (strcmp(s, "Bowler") == 0) {
        *out = BOWLER;
        return 1;
    }

    if (strcmp(s, "All-rounder") == 0) {
        *out = ALLROUNDER;
        return 1;
    }

    return 0;
}

static void initData(void) {
    for (int i = 0; i < teamCount && i < MAX_TEAMS; i++) {
        Team *t = &teamTable[numTeams++];
        t->teamId = i + 1;
        snprintf(t->name, sizeof t->name, "%s", teams[i]);
    }

    for (int i = 0; i < playerCount; i++) {
        const Player *h = &players[i];
        Role r;

        if (!parseRole(h->role, &r))
            continue;

        Team *t = NULL;

        for (int k = 0; k < numTeams; k++) {
            if (strcmp(teamTable[k].name, h->team) == 0) {
                t = &teamTable[k];
                break;
            }
        }

        if (!t || h->id < 1 || h->id > MAX_ID ||
            idUsed[h->id] || t->totalPlayers >= MAX_PER_TEAM)
            continue;

        linkPlayer(t, newPlayer(
            h->id,
            h->name,
            h->team,
            r,
            h->totalRuns,
            h->battingAverage,
            h->strikeRate,
            h->wickets,
            h->economyRate
        ));
    }
}

static int readLine(const char *prompt, char *buf, size_t n) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (!fgets(buf, (int)n, stdin))
        return 0;

    size_t len = strlen(buf);

    if (len && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    } else if (len == n - 1) {
        int c;

        while ((c = getchar()) != '\n' && c != EOF) {
        }

        return -1;
    }

    return 1;
}

static int readInt(const char *prompt, long lo, long hi, long *out) {
    char buf[128];

    for (;;) {
        int r = readLine(prompt, buf, sizeof buf);

        if (r == 0)
            return 0;

        char *end;
        long v = strtol(buf, &end, 10);

        while (*end == ' ' || *end == '\t')
            end++;

        if (r == 1 && end != buf && *end == '\0' &&
            v >= lo && v <= hi) {
            *out = v;
            return 1;
        }

        printf("Invalid input. Enter an integer between %ld and %ld.\n",
               lo, hi);
    }
}

static int readFloat(const char *prompt, double lo, double hi, double *out) {
    char buf[128];

    for (;;) {
        int r = readLine(prompt, buf, sizeof buf);

        if (r == 0)
            return 0;

        char *end;
        double v = strtod(buf, &end);

        while (*end == ' ' || *end == '\t')
            end++;

        if (r == 1 && end != buf && *end == '\0' &&
            v >= lo && v <= hi) {
            *out = v;
            return 1;
        }

        printf("Invalid input. Enter a number between %.0f and %.0f.\n",
               lo, hi);
    }
}

#define LINE72 "===================================================================================="
#define LINE86 "======================================================================================"

static void printTeamHeader(void) {
    printf("%s\n", LINE72);
    printf("%-5s %-24s %-12s %6s %6s %6s %5s %5s %10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR",
           "Wkts", "ER", "Perf.Index");
    printf("%s\n", LINE72);
}

static void printTeamRow(const PlayerNode *p) {
    printf("%-5d %-24s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n",
           p->playerId,
           p->name,
           roleName(p->role),
           p->totalRuns,
           p->battingAverage,
           p->strikeRate,
           p->wickets,
           p->economyRate,
           p->performanceIndex);
}

static void printAllHeader(void) {
    printf("%s\n", LINE86);
    printf("%-5s %-24s %-14s %-12s %6s %6s %6s %5s %5s %10s\n",
           "ID", "Name", "Team", "Role", "Runs",
           "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("%s\n", LINE86);
}

static void printAllRow(const PlayerNode *p) {
    printf("%-5d %-24s %-14s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n",
           p->playerId,
           p->name,
           p->teamName,
           roleName(p->role),
           p->totalRuns,
           p->battingAverage,
           p->strikeRate,
           p->wickets,
           p->economyRate,
           p->performanceIndex);
}

static void doAddPlayer(void) {
    long teamId, pid, role, runs, wk;
    double avg, sr, er;
    char name[MAX_NAME + 2];

    if (!readInt("Enter Team ID to add player: ", 1, 1000, &teamId))
        return;

    Team *t = findTeam((int)teamId);

    if (!t) {
        printf("Team with ID %ld not found.\n", teamId);
        return;
    }

    if (t->totalPlayers >= MAX_PER_TEAM) {
        printf("Team %s already has the maximum of %d players.\n",
               t->name, MAX_PER_TEAM);
        return;
    }

    printf("Enter Player Details:\n");

    for (;;) {
        if (!readInt("Player ID: ", 1, MAX_ID, &pid))
            return;

        if (idUsed[pid]) {
            printf("Player ID %ld already exists. Try another.\n", pid);
            continue;
        }

        break;
    }

    for (;;) {
        int r = readLine("Name: ", name, sizeof name);

        if (r == 0)
            return;

        if (r == 1 && name[0] != '\0')
            break;

        printf("Name must be 1-%d characters.\n", MAX_NAME);
    }

    if (!readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ",
                 1, 3, &role))
        return;

    if (!readInt("Total Runs: ", 0, 1000000, &runs))
        return;

    if (!readFloat("Batting Average: ", 0, 1000, &avg))
        return;

    if (!readFloat("Strike Rate: ", 0, 1000, &sr))
        return;

    if (!readInt("Wickets: ", 0, 10000, &wk))
        return;

    if (!readFloat("Economy Rate: ", 0, 100, &er))
        return;

    linkPlayer(t, newPlayer(
        (int)pid,
        name,
        t->name,
        (Role)role,
        (int)runs,
        (float)avg,
        (float)sr,
        (int)wk,
        (float)er
    ));

    printf("Player added successfully to Team %s!\n", t->name);
}

static void doDisplayTeam(void) {
    long teamId;

    if (!readInt("Enter Team ID: ", 1, 1000, &teamId))
        return;

    Team *t = findTeam((int)teamId);

    if (!t) {
        printf("Team with ID %ld not found.\n", teamId);
        return;
    }

    printf("Players of Team %s:\n", t->name);
    printTeamHeader();

    for (PlayerNode *p = t->allHead; p; p = p->nextInTeam)
        printTeamRow(p);

    printf("%s\n", LINE72);
    printf("Total Players: %d\n", t->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",
           t->averageBattingStrikeRate);
}

static int cmpTeamSR(const void *a, const void *b) {
    const Team *x = *(const Team *const *)a;
    const Team *y = *(const Team *const *)b;

    if (x->averageBattingStrikeRate < y->averageBattingStrikeRate)
        return 1;

    if (x->averageBattingStrikeRate > y->averageBattingStrikeRate)
        return -1;

    return x->teamId - y->teamId;
}

static void doDisplayTeamsBySR(void) {
    Team *order[MAX_TEAMS];

    for (int i = 0; i < numTeams; i++)
        order[i] = &teamTable[i];

    qsort(order, numTeams, sizeof order[0], cmpTeamSR);

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-4s %-18s %-14s %s\n",
           "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");

    for (int i = 0; i < numTeams; i++) {
        printf("%-4d %-18s %-14.2f %d\n",
               order[i]->teamId,
               order[i]->name,
               order[i]->averageBattingStrikeRate,
               order[i]->totalPlayers);
    }

    printf("=========================================================\n");
}

static void doTopK(void) {
    long teamId, role, k;

    if (!readInt("Enter Team ID: ", 1, 1000, &teamId))
        return;

    Team *t = findTeam((int)teamId);

    if (!t) {
        printf("Team with ID %ld not found.\n", teamId);
        return;
    }

    if (!readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ",
                 1, 3, &role))
        return;

    if (!readInt("Enter number of players: ", 1, MAX_PER_TEAM, &k))
        return;

    PlayerNode *p = t->roleHead[role];

    if (!p) {
        printf("Team %s has no %s.\n", t->name, rolePlural((Role)role));
        return;
    }

    printf("Top %ld %s of Team %s:\n",
           k, rolePlural((Role)role), t->name);

    printTeamHeader();

    long shown = 0;

    for (; p && shown < k; p = p->nextInRole, shown++)
        printTeamRow(p);

    printf("%s\n", LINE72);

    if (shown < k)
        printf("(Team has only %ld %s.)\n",
               shown, rolePlural((Role)role));
}

static int heapBefore(const PlayerNode *a, const PlayerNode *b) {
    return a->performanceIndex > b->performanceIndex;
}

static void siftDown(PlayerNode **h, int n, int i) {
    for (;;) {
        int l = 2 * i + 1;
        int r = l + 1;
        int m = i;

        if (l < n && heapBefore(h[l], h[m]))
            m = l;

        if (r < n && heapBefore(h[r], h[m]))
            m = r;

        if (m == i)
            return;

        PlayerNode *tmp = h[i];
        h[i] = h[m];
        h[m] = tmp;

        i = m;
    }
}

static void doRoleAcrossTeams(void) {
    long role;

    if (!readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ",
                 1, 3, &role))
        return;

    PlayerNode *heap[MAX_TEAMS];
    int n = 0;

    for (int i = 0; i < numTeams; i++) {
        if (teamTable[i].roleHead[role])
            heap[n++] = teamTable[i].roleHead[role];
    }

    for (int i = n / 2 - 1; i >= 0; i--)
        siftDown(heap, n, i);

    printf("%s of all teams:\n", rolePlural((Role)role));
    printAllHeader();

    int count = 0;

    while (n > 0) {
        PlayerNode *top = heap[0];

        printAllRow(top);
        count++;

        if (top->nextInRole)
            heap[0] = top->nextInRole;
        else
            heap[0] = heap[--n];

        siftDown(heap, n, 0);
    }

    printf("%s\n", LINE86);
    printf("Total %s: %d\n", rolePlural((Role)role), count);
}

static void freeAll(void) {
    for (int i = 0; i < numTeams; i++) {
        PlayerNode *p = teamTable[i].allHead;

        while (p) {
            PlayerNode *nx = p->nextInTeam;
            free(p);
            p = nx;
        }
    }
}

int main(void) {
    initData();

    for (;;) {
        printf("\n==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");

        long choice;

        if (!readInt("Enter your choice: ", 1, 6, &choice))
            break;

        switch (choice) {
            case 1:
                doAddPlayer();
                break;

            case 2:
                doDisplayTeam();
                break;

            case 3:
                doDisplayTeamsBySR();
                break;

            case 4:
                doTopK();
                break;

            case 5:
                doRoleAcrossTeams();
                break;

            case 6:
                freeAll();
                printf("Exiting. Goodbye!\n");
                return 0;
        }
    }

    freeAll();
    return 0;
}
