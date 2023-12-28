#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct Ingridient
{
    char *name;
    int count;
} Ingridient;

typedef struct
{
    char ingrdntName[51];
    int ingrdntAmount;
    unsigned short spplrPort;
} OrderedIngredient;

typedef struct FoodInfo
{
    char *name;
    Ingridient *ingredients;
    int ingredientAmount;
} FoodInfo;

typedef struct Recipes
{
    FoodInfo *foods;
    int foodCount;
} Recipes;

typedef struct Kitchen
{
    int ingNum;
    Ingridient ing; 
} Kitchen;

typedef struct RstrntMenu /////////////////////////jadid zadam
{
    char *name;
    Ingridient ingridients;

} RstrntMenu;

typedef enum Status
{
    Closed,
    Open
} Status;

typedef enum SupplierStatus
{
    Busy,
    Free
} SupplierStatus;

typedef struct Response
{
    unsigned short cstmrPort;
    char answer[5];
} Response;

typedef enum Client
{
    Customer,
    Restaurant,
    Supplier
} Client;

typedef struct Message
{
    unsigned short senderTcpPort;
    char *senderName;
    char title[100];
    char content[100];
} Message;

typedef struct SupplierInfo
{
    char *name;
    unsigned short int udpPort;
    int udpFd;
    unsigned short int tcpPort;
    int tcpFd;
    struct sockaddr_in udpAddr;
    unsigned short int currDmndrFd;
    char **allIngredients;
    SupplierStatus status;
    

} SupplierInfo;

typedef struct OrderedFood
{
    char cstmrName[100];
    char foodName[100];
    // char *foodName;
    unsigned short cstmrPort;
    char status[10];
    int fd;
} OrderedFood;

typedef struct RestaurantInfo
{
    char *name;
    unsigned short int udpPort;
    int udpFd;
    unsigned short int tcpPort;
    int tcpFd;
    Status status;
    OrderedFood orderedFoods[MAX_FOOD_NUM];
    int numOrderedFoods;
    struct sockaddr_in udpAddr;
    Recipes menu;
    Recipes recipes;
    Ingridient kitchenIngrdnt[MAX_INGREDIENT_NUM];
    int kitchenIngrdntCount;

} RestaurantInfo;

typedef struct CustomerInfo
{
    char *name;
    unsigned short int udpPort;
    unsigned short int tcpPort;
    int udpFd;
    int tcpFd;
    struct sockaddr_in udpAddr;
} CustomerInfo;

typedef struct
{
    char name[51];
    unsigned short int rstrntPort;
} OdrFood;

#endif