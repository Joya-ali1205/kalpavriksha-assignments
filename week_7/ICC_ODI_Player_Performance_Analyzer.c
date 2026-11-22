#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

typedef struct PlayerRecord{
    int id;
    const char* name;
    const char* team;
    const char* role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerRecord *next;
    struct PlayerRecord *nextRole;
} PlayerRecord;

typedef struct Team{
    int id;
    const char* name;
    int totalPlayers;
    float avgBattingStrikeRate;
    struct PlayerRecord *head;
    struct PlayerRecord *tail;
    PlayerRecord *batsmanHead;
    PlayerRecord *batsmanTail;
    PlayerRecord *bowlerHead;
    PlayerRecord *bowlerTail;
    PlayerRecord *allRounderHead;
    PlayerRecord *allRounderTail;

}Team;

typedef struct HeapNode
{
    PlayerRecord *player;
    int teamIndex;
} HeapNode;

Team *Team_arr;
HeapNode *heap;
int heapSize=0;

void swap(HeapNode *a, HeapNode *b){
    HeapNode temp=*a;
    *a=*b;
    *b=temp;
}

void insertHeap(PlayerRecord *player, int teamIndex){
    int i;
    i=heapSize;
    heap[i].player=player;
    heap[i].teamIndex=teamIndex;
    heapSize++;
    while (i>0 &&heap[(i-1)/2].player->performanceIndex<heap[i].player->performanceIndex){
        swap(&heap[i],&heap[(i-1)/2]);
        i=(i-1)/2;
    }
}

HeapNode deleteMax(){
    HeapNode maxNode;
    int i=0;
    int largest;
    int left,right;
    maxNode=heap[0];
    heap[0]=heap[heapSize-1];
    heapSize--;
    while(1){
        left=2*i+1;
        right=2*i+2;
        largest=i;
        if(left<heapSize&&heap[left].player->performanceIndex>heap[largest].player->performanceIndex){
            largest=left;
        }
        if(right<heapSize&&heap[right].player->performanceIndex>heap[largest].player->performanceIndex){
            largest=right;
        }
        if(largest==i){
            break;
        }
        swap(&heap[i],&heap[largest]);
        i=largest;
    }
    return maxNode;
}

void initializeTeams(){
    Team_arr=malloc(teamCount*sizeof(Team));
    if(Team_arr==NULL){
        printf("Memory allocation failed.");
        return;
    }
    for(int i=0;i<teamCount;i++){
        Team_arr[i].id=i+1;
        Team_arr[i].name=teams[i];
        Team_arr[i].totalPlayers = 0;
        Team_arr[i].avgBattingStrikeRate = 0;
        Team_arr[i].head = NULL;
        Team_arr[i].tail = NULL;
        Team_arr[i].batsmanHead = NULL;
        Team_arr[i].batsmanTail = NULL;
        Team_arr[i].bowlerHead = NULL;
        Team_arr[i].bowlerTail = NULL;
        Team_arr[i].allRounderHead = NULL;
        Team_arr[i].allRounderTail = NULL;
    }    
}

int searchByTeamName(const char *name){
    int low = 0;
    int high = teamCount - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int result = strcmp(Team_arr[mid].name, name);
        if (result == 0) {
            return mid;
        }
        else if (result < 0) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }
    return -1;
}

void insertRoleSorted(PlayerRecord **head, PlayerRecord **tail,PlayerRecord *newPlayer){
    PlayerRecord *temp;
    if (*head==NULL||newPlayer->performanceIndex>(*head)->performanceIndex){
        newPlayer->nextRole=*head;
        *head=newPlayer;
        if (*tail==NULL){
            *tail=newPlayer;
        }
        return;
    }
    temp=*head;
    while (temp->nextRole!=NULL&&temp->nextRole->performanceIndex>=newPlayer->performanceIndex){
        temp=temp->nextRole;
    }
    newPlayer->nextRole=temp->nextRole;
    temp->nextRole=newPlayer;
    if (newPlayer->nextRole==NULL)
    {
        *tail=newPlayer;
    }
}

float performanceIndexCalculator(PlayerRecord *player){
    float pi=0.0;
    if(strcmp(player->role,"Batsman")==0){
        pi=((player->battingAverage)*(player->strikeRate))/100;
    }
    if(strcmp(player->role,"Bowler")==0){
        pi=((player->wickets)*2)+(100-(player->economyRate));
    }
    if(strcmp(player->role,"All-rounder")==0){
        pi=(((player->battingAverage)*(player->strikeRate))/100)+((player->wickets)*2);
    }
    return pi;
}

void initializePlayers(){
    for(int i=0;i<playerCount;i++){
        PlayerRecord *newPlayer=malloc(sizeof(PlayerRecord));
        if(newPlayer==NULL){
            printf("Memory allocation failed.");
            return;
        }
        newPlayer->id=players[i].id;
        newPlayer->totalRuns=players[i].totalRuns;
        newPlayer->battingAverage=players[i].battingAverage;
        newPlayer->strikeRate=players[i].strikeRate;
        newPlayer->wickets=players[i].wickets;
        newPlayer->economyRate=players[i].economyRate;
        newPlayer->name=players[i].name;
        newPlayer->team=players[i].team;
        newPlayer->role=players[i].role;
        newPlayer->performanceIndex=performanceIndexCalculator(newPlayer);
        newPlayer->next=NULL;
        int index=searchByTeamName(newPlayer->team);
        Team_arr[index].totalPlayers++;
        if(Team_arr[index].head==NULL){
            Team_arr[index].head=newPlayer;
            Team_arr[index].tail=newPlayer;
        }
        else{
            Team_arr[index].tail->next=newPlayer;
            Team_arr[index].tail=newPlayer;
        }
        if (strcmp(newPlayer->role, "Batsman") == 0){
            insertRoleSorted(&Team_arr[index].batsmanHead,&Team_arr[index].batsmanTail,newPlayer);
        }
        else if (strcmp(newPlayer->role, "Bowler") == 0){
            insertRoleSorted(&Team_arr[index].bowlerHead,&Team_arr[index].bowlerTail,newPlayer);
        }
        else if (strcmp(newPlayer->role, "All-rounder") == 0){
            insertRoleSorted(&Team_arr[index].allRounderHead,&Team_arr[index].allRounderTail,newPlayer);
        }
    }
}

void calculateTeamAverages(){
    int i, count;
    float total;
    PlayerRecord *temp;
    for (i = 0; i < teamCount; i++){
        total = 0;
        count = 0;
        temp = Team_arr[i].head;
        while (temp != NULL){
            if (strcmp(temp->role,"Batsman")==0||strcmp(temp->role,"All-rounder")==0){
                total+=temp->strikeRate;
                count++;
            }
            temp=temp->next;
        }
        if(count>0){
            Team_arr[i].avgBattingStrikeRate=total/count;
        }
        else{
            Team_arr[i].avgBattingStrikeRate=0;
        }
    }
}

void addPlayer(){
    PlayerRecord *newPlayer=malloc(sizeof(PlayerRecord));
    if(newPlayer==NULL){
        printf("Memory allocation failed.");
        return;
    }
    int team_id;
    int role_choice;
    char name[100];

    printf("Enter Team ID to add player: ");
    scanf("%d", &team_id);
    printf("Enter Player Details:\n");
    printf("Player ID: ");
    scanf("%d",newPlayer->id);
    printf("Name: ");
    scanf(" %[^\n]",name);
    newPlayer->name=malloc(strlen(name)+1);
    if(newPlayer->name==NULL){
        printf("Memory allocation failed.");
        return;
    }
    newPlayer->name=name;
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role_choice);
    printf("Total Runs: ");
    scanf("%d", newPlayer->totalRuns);
    printf("Batting Average: ");
    scanf("%f", newPlayer->battingAverage);
    printf("Strike Rate: ");
    scanf("%f", newPlayer->strikeRate);
    printf("\nWickets: ");
    scanf("%d", newPlayer->wickets);
    printf("Economy Rate: ");
    scanf("%f", newPlayer->economyRate);
    if(role_choice == 1) {
        newPlayer->role=malloc(strlen("Batsman")+1);
        if(newPlayer->role==NULL){
            printf("Memory allocation failed.");
            return;
        }
        newPlayer->role = "Batsman";
    }
    else if(role_choice == 2) {
        newPlayer->role=malloc(strlen("Bowler")+1);
        if(newPlayer->role==NULL){
            printf("Memory allocation failed.");
            return;
        }
        newPlayer->role = "Bowler";
    }
    else if(role_choice == 3) {
        newPlayer->role=malloc(strlen("All-rounder")+1);
        if(newPlayer->role==NULL){
            printf("Memory allocation failed.");
            return;
        }
        newPlayer->role = "All-rounder";
    }
    newPlayer->team=Team_arr[team_id - 1].name;
    Team_arr[team_id-1].tail->next=newPlayer;
    Team_arr[team_id-1].tail=newPlayer;
    if (strcmp(newPlayer->role, "Batsman") == 0){
        insertRoleSorted(&Team_arr[team_id-1].batsmanHead,&Team_arr[team_id-1].batsmanTail,newPlayer);
    }
    else if (strcmp(newPlayer->role, "Bowler") == 0){
        insertRoleSorted(&Team_arr[team_id-1].bowlerHead,&Team_arr[team_id-1].bowlerTail,newPlayer);
    }
    else if (strcmp(newPlayer->role, "All-rounder") == 0){
        insertRoleSorted(&Team_arr[team_id-1].allRounderHead,&Team_arr[team_id-1].allRounderTail,newPlayer);
    }
    printf("Player added successfully to Team %s!\n",Team_arr[team_id-1].name);
}
void displayTeamPlayers(int id){
    PlayerRecord *temp=Team_arr[id-1].head;
    if (temp==NULL){
        printf("List is empty.\n");
        return;
    }
    printf("Players of Team India:\n");
    printf("===========================================================================\n");
    printf("ID   Name         Role       Runs     Avg    SR    Wkts    ER     Perf. Index\n");
    printf("===========================================================================\n");
    while(temp!=NULL){
        printf("%d  %s    %s    %d   %.2f   %.2f   %d   %.2f   %.2f\n",temp->id,temp->name,temp->role,temp->totalRuns,temp->battingAverage,temp->strikeRate,temp->wickets,temp->economyRate,temp->performanceIndex);
        temp=temp->next;
    }
}
void sortTeamsByStrikeRate(){
    calculateTeamAverages();
    Team **sortedTeams;
    int i,j;
    Team *key;
    sortedTeams=malloc(teamCount*sizeof(Team *));
    if(sortedTeams==NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    for(i=0;i<teamCount;i++){
        sortedTeams[i]=&Team_arr[i];
    }
    for(i=1;i<teamCount;i++){
        key=sortedTeams[i];
        j=i-1;
        while (j>=0&&sortedTeams[j]->avgBattingStrikeRate<key->avgBattingStrikeRate){
            sortedTeams[j+1]=sortedTeams[j];
            j--;
        }
        sortedTeams[j+1]=key;
    }
    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("===========================================================================\n");
    printf("ID   Team Name       Avg Bat SR    Total Players\n");
    printf("===========================================================================\n");
    for (i=0;i<teamCount;i++){
        printf("%d  %s       %.2f    %d\n",i+1,sortedTeams[i]->name,sortedTeams[i]->avgBattingStrikeRate,sortedTeams[i]->totalPlayers);
    }
    free(sortedTeams);
}

void displayTopKPlayers(){
    int team_id,choice,k;
    int count=0;
    PlayerRecord *temp=NULL;
    printf("Enter Team ID: ");
    scanf("%d",&team_id);
    printf("\nSelect Role:\n");
    printf("1. Batsman\n");
    printf("2. Bowler\n");
    printf("3. All-rounder\n");
    printf("Enter your choice: ");
    scanf("%d",&choice);
    printf("Enter K: ");
    scanf("%d",&k);
    if (team_id<1||team_id>teamCount||k<=0){
        printf("Invalid input.\n");
        return;
    }
    if (choice==1){
        temp=Team_arr[team_id - 1].batsmanHead;
    }
    else if (choice==2){
        temp=Team_arr[team_id - 1].bowlerHead;
    }
    else if (choice==3){
        temp=Team_arr[team_id - 1].allRounderHead;
    }
    else{
        printf("Invalid role choice.\n");
        return;
    }
    printf("===========================================================================\n");
    printf("ID   Name         Role       Runs     Avg    SR    Wkts    ER     Perf. Index\n");
    printf("===========================================================================\n");
    while (temp!=NULL&&count<k){
        printf("%d  %s    %s    %d   %.2f   %.2f   %d   %.2f   %.2f\n",temp->id,temp->name,temp->role,temp->totalRuns,temp->battingAverage,temp->strikeRate,temp->wickets,temp->economyRate,temp->performanceIndex);
        temp=temp->next;
        temp=temp->nextRole;
        count++;
    }
    if(count==0){
        printf("No players found for this role.\n");
    }
}

void displayPlayersByRole(){
    int choice,i;
    PlayerRecord *temp;
    HeapNode maxNode;
    printf("\nSelect Role:\n");
    printf("1. Batsman\n");
    printf("2. Bowler\n");
    printf("3. All-rounder\n");
    printf("Enter your choice: ");
    scanf("%d",&choice);
    heap=malloc(teamCount*sizeof(HeapNode));
    if (heap==NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    heapSize=0;
    for(i=0;i<teamCount;i++){
        temp=NULL;
        if(choice==1){
            temp=Team_arr[i].batsmanHead;
        }
        else if(choice==2){
            temp=Team_arr[i].bowlerHead;
        }
        else if(choice==3){
            temp=Team_arr[i].allRounderHead;
        }
        else{
            printf("Invalid role choice.\n");
            free(heap);
            return;
        }
        if (temp != NULL){
            insertHeap(temp, i);
        }
    }
    printf("===========================================================================\n");
    printf("ID   Name         Team         Role       Runs     Avg    SR    Wkts    ER     Perf. Index\n");
    printf("===========================================================================\n");
    while(heapSize > 0){
        maxNode=deleteMax();
        printf("%d  %s    %s    %s    %d   %.2f   %.2f   %d   %.2f   %.2f\n",temp->id,temp->name,temp->team,temp->role,temp->totalRuns,temp->battingAverage,temp->strikeRate,temp->wickets,temp->economyRate,temp->performanceIndex);
        temp=temp->next;
        temp=maxNode.player->nextRole;
        if (temp != NULL){
            insertHeap(temp, maxNode.teamIndex);
        }
    }
    free(heap);
}

void freeMemory(){
    int i;
    PlayerRecord *temp,*nextNode;
    for (i=0;i<teamCount;i++){
        temp=Team_arr[i].head;
        while (temp!=NULL){
            nextNode=temp->next;
            free(temp);
            temp=nextNode;
        }
    }
    free(Team_arr);
}

int main(){
    int choice;
    initializeTeams();
    initializePlayers();
    calculateTeamAverages();

    printf("\n");
    do
    {
        printf("====================================================\n");
        printf("       ICC ODI Player Performance Analyzer\n");
        printf("====================================================\n");
        printf("\n1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of Specific Role Across All Teams\n");
        printf("6. Exit\n");
        printf("====================================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                addPlayer();
                break;
            case 2:
                int id;
                printf("Enter Team ID: ");
                scanf("%d",&id);
                displayTeamPlayers(id);
                break;
            case 3:
                sortTeamsByStrikeRate();
                break;
            case 4:
                displayTopKPlayers();
                break;
            case 5:
                displayPlayersByRole();
                break;
            case 6:
                printf("\nExiting ICC ODI Player Performance Analyzer...\n");
                break;
            default:
                printf("\nInvalid choice! Please enter a number between 1 and 6.\n");
        }
    } while(choice != 6);
    freeMemory();
    return 0;
}