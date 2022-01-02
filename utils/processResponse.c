
#include "processResponse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../models/workspace.h"
#include "../models/room.h"
#include "../models/user.h"
#include "../models/message.h"
#include "../models/utils.h"

char *processResponseForViewWSP(User *user)
{

    static char information[512];
    int count = 0;
    WorkSpace *root = readWorkspaceData("db/workspaces.txt");
    int *list_wps = findWSPForUser(root, user->ID, &count);
    printf("Num Wsp of %s : %d\n", user->name, count);
    sprintf(information, "%d\n", count);
    if (count != 0)
    {
        for (int i = 0; i < count; i++)
        {
            {
                WorkSpace *wsp = searchWSPByID(root, list_wps[i]);
                char temp[3];
                sprintf(temp, "%d ", wsp->ID);
                strcat(information, temp);
                strcat(information, wsp->name);
                strcat(information, " ");

                if (wsp->host_id == user->ID)
                {
                    sprintf(temp, "%d\n", 1); // 1 is admin
                }
                else
                    sprintf(temp, "%d\n", 0);
                strcat(information, temp);
            }
        }
    }

    printf("Information for wsp list: \n %s\n", information);
    return information;
}

char *processResponseForJoinWSP(User *user, int wsp_id)
{
    static char information[512];

    WorkSpace *wsp = readOneWSPData("db/workspaces.txt", wsp_id);
    User *root = readUserData("db/users.txt");
    char temp[3];
    strcat(information, wsp->name);
    strcat(information, "\n");

    //process rooms
    sprintf(temp, "%d\n", wsp->num_of_rooms);
    strcat(information, temp);
    for (int i = 0; i < wsp->num_of_rooms; i++)
    {

        sprintf(temp, "%d ", wsp->room_id[i]);
        strcat(information, temp);
        strcat(information, wsp->room_name[i]);
        strcat(information, "\n");
    }

    //process user
    sprintf(temp, "%d\n", wsp->num_of_users);
    strcat(information, temp);
    for (int i = 0; i < wsp->num_of_users; i++)
    {
        User *p = searchUserByID(root, wsp->user_id[i]);
        char temp[3];
        sprintf(temp, "%d ", p->ID);
        strcat(information, temp);
        strcat(information, p->name);
        strcat(information, " ");
    }

    printf("Information for wsp list:  %s", information);
    return information;
}

char *processResponseForJoinRoom(User *user, int wsp_id, int room_id)
{
    static char information[4096];

    char filename[32];
    strcpy(filename, createMessFilename(wsp_id, room_id));
    Message *root = readMessData(filename);
    User *u_root = readUserData("db/users.txt");
    Message *p = root;

    int count_mess = 0;
    while (p!=NULL)
    {
        count_mess += 1;
        p = p->next;
    }
    
    p = root;
    char temp[3];
    printf("Number of mess: %d\n", count_mess);
    while (p != NULL)
    {
        if (count_mess - 10 >0)
        {
            count_mess--;
            p = p->next;
            continue;
        }
        sprintf(temp, "%d\n", p->ID);
        strcat(information, temp);

        char timestr[64];
        strcpy(timestr, convertTimeTtoString(p->datetime, 1));
        strcat(information, timestr);
        strcat(information,"\n");
        if (p->send_id != user->ID)
        {
            User *tmp = searchUserByID(u_root, p->send_id);
            strcat(information, tmp->name);
            strcat(information, "\n");
        }
        else
        {
            strcat(information, "0\n");
        }
        if (p->parent_id != 0)
        {
            Message *tmp = findMessByID(root, p->parent_id);
            strcat(information, tmp->content);
            strcat(information, "\n");
        }
        else
        {
            strcat(information, "0\n");
        }
        strcat(information, p->content);
        strcat(information, "\n");
        p = p->next;
    }

    printf("Information message:\n  %s\n", information);
    return information;
}