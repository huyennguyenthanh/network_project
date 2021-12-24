#ifndef _WORKSPACE_H_
#define _WORKSPACE_H_


#define MAX_ROOM 10
#define MAX_USERS_IN_WSP 50

#include<stdio.h>

#include "user.h"
#include "room.h"



typedef struct WorkSpace{
    int ID;
    char name[32];
    
    User *host;
    User *users[MAX_USERS_IN_WSP];
    //Room *rooms[MAX_ROOM];
    int num_of_users;
    int num_of_rooms;
    struct WorkSpace *next;
} WorkSpace;

WorkSpace * createWorkSpace(User host, char* wsp_name);
void addUserToWSP(WorkSpace workspace, User user);
//void updateMessage(WorkSpace workspace, Room room);

void kick(WorkSpace workspace, User user);
void join(WorkSpace workspace);

#endif