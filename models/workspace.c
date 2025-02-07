#include "user.h"
#include "room.h"
#include "workspace.h"
#include "signal.h"
#include "../utils/utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

WorkSpace *createNewWSP(int id, int host_id, char wsp_name[])
{
    WorkSpace *new = (WorkSpace *)malloc(sizeof(WorkSpace));

    new->ID = id;
    new->host_id = host_id;
    strcpy(new->name, wsp_name);
    new->num_of_rooms = 0;
    new->num_of_users = 0;

    // read Each WorkSpace users
    char filename[32] = "db/workspace_users_";
    char tmp[2];
    sprintf(tmp, "%d", id);
    strcat(filename, tmp);
    strcat(filename, ".txt");
    FILE *f;
    if (!(f = fopen(filename, "r")))
    {
        printf("\nCreate Workspace Database failed! File Users not found.\n");
    }
    else
    {
        while (!feof(f))
        {
            fscanf(f, "%d", &new->user_id[new->num_of_users++]);
        }
        fclose(f);
    }

    // read Each WorkSpace room
    strcpy(filename, "db/workspace_rooms_");
    sprintf(tmp, "%d", id);
    strcat(filename, tmp);
    strcat(filename, ".txt");
    printf("File room %s\n", filename);
    if (!(f = fopen(filename, "r")))
    {
        printf("\nCreate Workspace Database failed! File Rooms not found.\n");
    }
    else
    {
        while (!feof(f))
        {
            fscanf(f, "%d\n", &new->room_id[new->num_of_rooms]);
            fscanf(f, "%[^\n]\n", new->room_name[new->num_of_rooms]);
            new->num_of_rooms += 1;
            if (feof(f))
                break;
        }
        fclose(f);
    }
    printf("Read data WSP %s have %d room, %d user\n", new->name, new->num_of_rooms, new->num_of_users);

    return new;
}
void insertWSP(WorkSpace *root, int ID, int host_id, char name[])
{
    WorkSpace *new = createNewWSP(ID, host_id, name);
    if (root == NULL)
    {
        new->next = root;
        root = new;
    }
    else
    {
        WorkSpace *p = root;
        //tro con tro toi cuoi danh sach lien ket
        while (p->next != NULL)
            p = p->next;
        p->next = new;
    }
}
void printAllWSP(WorkSpace *root)
{
    WorkSpace *p = root;
    while (p != NULL)
    {
        printf("Name %s\n", p->name);
        printf("Num of users %d\n", p->num_of_users);
        for (int i = 0; i < p->num_of_users; i++)
        {
            printf("%d ", p->user_id[i]);
        }
        p = p->next;
    }
}
WorkSpace *searchWSPByID(WorkSpace *root, int ID)
{
    WorkSpace *p = root;
    while (p != NULL)
    {
        if (p->ID == ID)
        {
            //WorkSpace *tmp = createNewWSP(p->ID, p->host_id, p->name);
            return p;
        }
        p = p->next;
    }
    return NULL;
}

WorkSpace *readWorkspaceData(char filename[])
{
    WorkSpace *root;

    int id, host_id;
    char wsp_name[32];

    FILE *f;

    //strcpy(filename, "db/users.txt")
    if (!(f = fopen(filename, "r")))
    {
        printf("Create Workspace Database failed! File not found.\n");
    }
    else
    {
        int number_of_workspaces = 0;
        int check = 1;
        fscanf(f, "%d\n", &number_of_workspaces);
        if (number_of_workspaces == 0)
            return NULL;
        while (!feof(f))
        {
            fscanf(f, "%d\n%d\n%s\n", &id, &host_id, wsp_name);

            if (check == 1)
            {
                root = createNewWSP(id, host_id, wsp_name);
                check = 0;
            }
            else
                insertWSP(root, id, host_id, wsp_name);
        }
    }
    fclose(f);
    return root;
}
// input wsp_id and read data of only this workspace
WorkSpace *readOneWSPData(char filename[], int wsp_id)
{
    //WorkSpace *root=NULL;

    int id, host_id;
    char wsp_name[32];

    FILE *f;
    if (!(f = fopen(filename, "r")))
    {
        printf("Create Workspace Database failed! File not found.\n");
    }
    else
    {

        int number_of_workspaces = 0;
        fscanf(f, "%d\n", &number_of_workspaces);
        if (number_of_workspaces == 0)
            return NULL;
        while (!feof(f))
        {

            fscanf(f, "%d\n%d\n%s\n", &id, &host_id, wsp_name);

            if (id == wsp_id)
            {

                fclose(f);
                return createNewWSP(wsp_id, host_id, wsp_name);
            }
        }
    }
    fclose(f);
    return NULL;
}
// int valueInArray(int val, int arr[], int size)
// {

//     for (int i = 0; i < size; i++)
//     {
//         if (arr[i] == val)
//             return 1;
//     }
//     return 0;
// }

// input is a User, find all workspace user belong to
int *findWSPForUser(WorkSpace *root, int user_id, int *count)
{
    WorkSpace *p = root;
    static int list_wps[10];
    *count = 0;
    while (p != NULL)
    {

        if (valueInArray(user_id, p->user_id, p->num_of_users) == 1)
        {
            list_wps[*count] = p->ID;
            *count = *count + 1;
        }
        p = p->next;
    }

    return list_wps; // 0 is not exist
}

// input is a user and a workspace, check if user belong to this wsp
char *checkWSPForUser(WorkSpace *wsp, int user_id, int *flag)
{
    if (valueInArray(user_id, wsp->user_id, wsp->num_of_users) == 1)
    {
        *flag = 2;
        return MESS_JOIN_WSP_SUCCESS;
    }
    return MESS_JOIN_WSP_FAILED;
}

char *checkAvailableID(WorkSpace *wsp, int room_id, int user_id, int *flag)
{
    if (room_id % 2 == 1)
    {
        if (valueInArray(room_id, wsp->user_id, wsp->num_of_users) == 1)
        {
            *flag = 3;
            return MESS_JOIN_ROOM_SUCCESS;
        }
    }
    else
    {
        for (int i = 0; i < wsp->num_of_rooms; i++)
        {
            Room *room = createNewRoom(wsp->room_id[i], wsp->room_name[i], wsp->ID);
            if (valueInArray(user_id, room->user_id, room->num_of_users) == 1)
            {
                *flag = 3;
                return MESS_JOIN_ROOM_SUCCESS;
            }
        }
    }

    return MESS_INVALID_ID;
}

void freeWorkspaceData(WorkSpace *root)
{
    WorkSpace *current = root;
    WorkSpace *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    root = NULL;
}

char *addUserToWSP(int self_id, int wsp_id, int user_id)
{
    WorkSpace *wsp = readOneWSPData("db/workspaces.txt", wsp_id);
    if (wsp == NULL)
    {
        return MESS_JOIN_WSP_FAILED;
    }
    if (wsp->host_id != self_id)
    {
        return MESS_NOT_HOST;
    }
    if (valueInArray(user_id, wsp->user_id, wsp->num_of_users) == 1)
    {
        return MESS_AVAILABLE_IN_WSP;
    }

    wsp->user_id[wsp->num_of_users] = user_id;

    char filename[32] = "db/workspace_users_";
    char tmp[10];
    sprintf(tmp, "%d.txt", wsp_id);
    strcat(filename, tmp);
    FILE *f;
    if (!(f = fopen(filename, "a")))
    {
        printf("\nCreate Workspace Database failed! File Users not found.\n");
    }
    else
    {
        fprintf(f, "%d\n", wsp->user_id[wsp->num_of_users]);
        fclose(f);
    }
    return MESS_ADD_SUCCESS;
}

char *kickUserOutWSP(int self_id, int wsp_id, int user_id)
{
    WorkSpace *wsp = readOneWSPData("db/workspaces.txt", wsp_id);
    if (wsp == NULL)
    {
        return MESS_JOIN_WSP_FAILED;
    }
    if (wsp->host_id != self_id)
    {
        return MESS_NOT_HOST;
    }
    if (valueInArray(user_id, wsp->user_id, wsp->num_of_users) == 0)
    {
        return MESS_NOT_AVAILABLE_IN_WSP;
    }
    // for (int i = 0; i < wsp->num_of_users; i++)
    // {
    //     if (wsp->user_id[i] != user_id)
    //     {
    //         wsp->user_id[i] = NULL;
    //     }
    // }
    char filename[32] = "db/workspace_users_";
    char tmp[10];
    sprintf(tmp, "%d.txt", wsp_id);
    strcat(filename, tmp);
    FILE *f;
    if (!(f = fopen(filename, "a")))
    {
        printf("\nCreate Workspace Database failed! File Users not found.\n");
    }
    else
    {
        for (int i = 0; i < wsp->num_of_users; i++)
        {
            if (wsp->user_id[i] != user_id)
                fprintf(f, "%d\n", wsp->user_id[i]);
        }
    }
    fclose(f);
    return MESS_KICK_SUCCESS;
}