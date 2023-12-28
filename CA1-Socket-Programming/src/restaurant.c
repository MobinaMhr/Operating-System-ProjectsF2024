#include "restaurant.h"

char *NAME;

void initRstrnt(RestaurantInfo *rstrnt, unsigned short udpPort, const char *udpIp)
{
    rstrnt->udpPort = udpPort;
    rstrnt->udpFd = createUdpSckt(rstrnt->udpPort, udpIp, &rstrnt->udpAddr);
    rstrnt->tcpPort = genUniqueTcpPort();
    rstrnt->tcpFd = createTcpSckt(rstrnt->tcpPort);
    rstrnt->name = uniqueUsrName(rstrnt->udpFd, rstrnt->tcpFd, rstrnt->udpAddr, rstrnt->tcpPort);
    NAME = rstrnt->name;
    logFile(NAME, "Initiated\n");
}

int isInKitchn(RestaurantInfo *rstrnt, char *ingName)
{
    int size = rstrnt->kitchenIngrdntCount;
    for (int i = 0; i < size; i++)
    {
        if (strcmp(rstrnt->kitchenIngrdnt[i].name, ingName) == 0)
        {
            return 1;
        }
    }
    return 0; // no
}

void initKitchen(RestaurantInfo *rtnt)
{
    rtnt->status = Closed;
    cmdLineInterface(rtnt, START_WORKING);

    for (int i = 0; i < MAX_FOOD_NUM; i++)
        memset(&rtnt->orderedFoods[i], 0, sizeof(OrderedFood));
    rtnt->numOrderedFoods = 0;

    rtnt->menu = getRecipes();
    rtnt->recipes = getRecipes();

    int size = rtnt->menu.foodCount;
    rtnt->kitchenIngrdntCount = 0;
    for (int i = 0; i < size; i++)
    {
        int size2 = rtnt->menu.foods[i].ingredientAmount;
        for (int j = 0; j < size2; j++)
        {
            if (isInKitchn(rtnt, rtnt->menu.foods[i].ingredients[j].name) == 0)
            {
                int indx = rtnt->kitchenIngrdntCount;
                rtnt->kitchenIngrdntCount++;
                rtnt->kitchenIngrdnt[indx].name = (char *)malloc(strlen(rtnt->menu.foods[i].ingredients[j].name));
                strcpy(rtnt->kitchenIngrdnt[indx].name, rtnt->menu.foods[i].ingredients[j].name);
                rtnt->kitchenIngrdnt[indx].count = 0;
            }
            rtnt->menu.foods[i].ingredients[j].count = 0;
        }
    }
}

void display(const OrderedFood *orderedFood)
{
    char buff[INP_BUFF_SIZE];
    memset(buff, '\0', INP_BUFF_SIZE);
    sprintf(buff, "\n%s %u %s", orderedFood->cstmrName, orderedFood->cstmrPort, orderedFood->foodName);
    setPromptReadInput(STDOUT_FILENO, buff, NULL, 0);
}

void displayRequestsList(RestaurantInfo *rstrnt)
{
    startDisply();
    setPromptReadInput(STDOUT_FILENO, DISPLAY_REQUEST_LIST_TITLE, NULL, 0);
    for (int i = 0; i < rstrnt->numOrderedFoods; i++)
    {
        if (strcmp(rstrnt->orderedFoods[i].status, FOOD_STATUS_PENDING) != 0)
            continue;
        display(&rstrnt->orderedFoods[i]);
    }
    endDisply();
    logFile(NAME, "Requests list displayed\n");
}

void desplaySalesHistory(RestaurantInfo *rstrnt)
{
    startDisply();
    setPromptReadInput(STDOUT_FILENO, DISPLAY_SALES_HISTORY_TITLE, NULL, 0);
    for (int i = 0; i < rstrnt->numOrderedFoods; i++)
    {
        if (strcmp(rstrnt->orderedFoods[i].status, FOOD_STATUS_PENDING) == 0)
            continue;
        if (strcmp(rstrnt->orderedFoods[i].status, NO) == 0)
            continue;
        display(&rstrnt->orderedFoods[i]);
    }
    endDisply();
    logFile(NAME, "Sales history displayed\n");
}

OrderedIngredient orderIngrdnt()
{
    OrderedIngredient ingrdnt;
    char buff[INP_BUFF_SIZE];
    memset(buff, '\0', INP_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "\n", NULL, 0);
    setPromptReadInput(STDOUT_FILENO, "port of supplier: ", buff, INP_BUFF_SIZE);
    ingrdnt.spplrPort = atoi(buff);

    memset(buff, '\0', INP_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "name of ingredient: ", buff, INP_BUFF_SIZE);
    strcpy(ingrdnt.ingrdntName, buff);

    memset(buff, '\0', INP_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "number of ingredient: ", buff, INP_BUFF_SIZE);
    ingrdnt.ingrdntAmount = atoi(buff);
    return ingrdnt;
}

void displayFoodRecepies(Recipes recps)
{
    int recepieCount = recps.foodCount;
    for (int i = 0; i < recepieCount; i++)
    {
        char *foodName = (char *)malloc(2 * INP_BUFF_SIZE * sizeof(char));
        sprintf(foodName, "%d- %s:\n", i + 1, recps.foods[i].name);
        foodName = (char *)realloc(foodName, strlen(foodName) + 1);
        write(1, foodName, strlen(foodName));
        for (int j = 0; j < recps.foods[i].ingredientAmount; j++)
        {
            char *ingrdnts = (char *)malloc(2 * INP_BUFF_SIZE * sizeof(char));
            sprintf(ingrdnts, "\t\t%s : %d\n", recps.foods[i].ingredients[j].name,
                    recps.foods[i].ingredients[j].count);
            ingrdnts = (char *)realloc(ingrdnts, strlen(ingrdnts) + 1);
            setPromptReadInput(STDOUT_FILENO, ingrdnts, NULL, 0);
        }
    }
    logFile(NAME, "Food recipes displayed\n");
}

void sendIngrdntReq(RestaurantInfo *rstrnt)
{
    OrderedIngredient ordered = orderIngrdnt();
    ordered.spplrPort;
    int fd = connectTcpClient(ordered.spplrPort); // port supplier doroste
    if (fd < 0)
    {
        setPromptReadInput(STDERR_FILENO, "you entered wrong supplier port.\n", NULL, 0);
        return;
    }

    char contnt[INP_BUFF_SIZE];
    memset(contnt, '\0', INP_BUFF_SIZE);
    sprintf(contnt, "%d", fd);
    const char *encMsg = encodeMsg(creatMsg(rstrnt->name, rstrnt->tcpPort, contnt, REQUEST_INGREDIENT));

    send(fd, encMsg, strlen(encMsg), 0);

    setPromptReadInput(STDOUT_FILENO, WAIT_TILL_SUPPLIR_RESPONSE, NULL, 0);
    logFile(NAME, "ingredient request sent\n");

    if (getIngrdntRspns(rstrnt, fd) != YES)
        return;

    for (int i = 0; i < rstrnt->kitchenIngrdntCount; i++)
    {
        memset(contnt, '\0', INP_BUFF_SIZE);
        sprintf(contnt, "%s", rstrnt->kitchenIngrdnt[i].name);
        if (strcmp(contnt, ordered.ingrdntName) != 0)
            continue;
        rstrnt->kitchenIngrdnt[i].count += ordered.ingrdntAmount;
    }
}

void showSuppliers(int rstrntTcpFd, char *rstrntName)
{
    setPromptReadInput(STDOUT_FILENO, SHOW_SUPPLIER_TITLE, NULL, 0);

    char buff[INP_BUFF_SIZE];
    signal(SIGALRM, alarmHndlr);
    siginterrupt(SIGALRM, 1);
    while (1)
    {
        alarm(1);
        int fd = accTcpClient(rstrntTcpFd);
        memset(buff, '\0', INP_BUFF_SIZE);
        int rcv = recv(fd, buff, INP_BUFF_SIZE, 0);
        alarm(0);
        if (rcv < 0)
            break;
        Message msg = decodeMsg(buff);
        showUser(msg.senderName, msg.senderTcpPort);
    }
    logFile(NAME, "Suppliers list displayed\n");
}

char *getIngrdntRspns(RestaurantInfo *rstrnt, int createdFd)
{
    signal(SIGALRM, alarmHndlr);
    siginterrupt(SIGALRM, 1);

    alarm(90);
    consoleLock();
    char buff[INP_BUFF_SIZE];
    memset(buff, '\0', INP_BUFF_SIZE);
    int rcv = recv(createdFd, buff, INP_BUFF_SIZE, 0);
    alarm(0);
    consoleUnlock();

    if (rcv <= 0)
    {
        setPromptReadInput(STDOUT_FILENO, TIMEOUT_MESSAGE, NULL, 0);
        char *encMsg = encodeMsg(creatMsg(rstrnt->name, rstrnt->tcpPort, TIMEOUT_MESSAGE, TIMEOUT_MESSAGE));
        send(createdFd, encMsg, strlen(encMsg), 0);
        return NO;
    }

    Message rspMsg = decodeMsg(buff);
    return showOrdrIngrdntRspnse(rspMsg.content, rstrnt->name, rspMsg.senderName);
}

void displayIngredients(RestaurantInfo *rstrnt)
{
    startDisply();
    setPromptReadInput(STDOUT_FILENO, DISPLAY_INGREDIENT_TITLE, NULL, 0);

    char buff[INP_BUFF_SIZE];
    for (int i = 0; i < rstrnt->kitchenIngrdntCount; i++)
    {
        if (rstrnt->kitchenIngrdnt[i].count == 0)
            continue;

        memset(buff, '\0', INP_BUFF_SIZE);
        sprintf(buff, "\n%s %d", rstrnt->kitchenIngrdnt[i].name, rstrnt->kitchenIngrdnt[i].count);

        setPromptReadInput(STDOUT_FILENO, buff, NULL, 0);
    }

    endDisply();
}

int isEnogh(RestaurantInfo *rstrnt, char *ingName, int amount)
{
    int size = rstrnt->kitchenIngrdntCount;
    for (int i = 0; i < size; i++)
    {
        if (strcmp(rstrnt->kitchenIngrdnt[i].name, ingName) != 0)
            continue;
        if (rstrnt->kitchenIngrdnt[i].count < amount)
        {
            return 0;
        }
        return 1;
    }
    return 0;
}

int hasEnoghIngredient(RestaurantInfo *rstrnt, char *foodName)
{
    for (int i = 0; i < rstrnt->recipes.foodCount; i++)
    {

        if (strcmp(rstrnt->recipes.foods[i].name, foodName) != 0)
            continue;
        int size = rstrnt->recipes.foods[i].ingredientAmount;
        for (int j = 0; j < size; j++)
        {
            int neededAmount = rstrnt->recipes.foods[i].ingredients[j].count;
            if (isEnogh(rstrnt, rstrnt->recipes.foods[i].ingredients[j].name, neededAmount) == 0)
                return 0; // no
        }
    }
    return 1; // yes
}

void cmdLineInterface(RestaurantInfo *rstrnt, const char *cmd)
{
    logFile(NAME, "CLI command received.");
    if (cmd == NULL || strlen(cmd) == 0)
        return;
    else if (strcmp(cmd, START_WORKING) == 0 && rstrnt->status == Closed)
    {
        rstrnt->status = Open;
        Message bcMsg = creatMsg(rstrnt->name, rstrnt->tcpPort, START_WORKING, START_WORKING);
        char *encMsg = encodeMsg(bcMsg);
        sendto(rstrnt->udpFd, encMsg, strlen(encMsg), 0, (struct sockaddr *)&rstrnt->udpAddr, sizeof(rstrnt->udpAddr));
    }
    else if (strcmp(cmd, BREAK) == 0 && rstrnt->status == Open)
    {
        if (rstrnt->numOrderedFoods > 0)
        {
            setPromptReadInput(STDOUT_FILENO, "you haven't answer all requests yet!\n", NULL, 0);
            return;
        }
        rstrnt->status = Closed;
        Message bcMsg = creatMsg(rstrnt->name, rstrnt->tcpPort, BREAK, BREAK);
        char *encMdg = encodeMsg(bcMsg);
        sendto(rstrnt->udpFd, encMdg, strlen(encMdg), 0, (struct sockaddr *)&rstrnt->udpAddr, sizeof(rstrnt->udpAddr));
    }
    else if (strcmp(cmd, SHOW_INGREDIENTS) == 0 && rstrnt->status == Open)
    {
        displayIngredients(rstrnt);
    }
    else if (strcmp(cmd, SHOW_RECIPES) == 0 && rstrnt->status == Open)
    {
        startDisply();
        displayFoodRecepies(rstrnt->recipes);
        endDisply();
    }
    else if (strcmp(cmd, SHOW_SUPPLIERS) == 0 && rstrnt->status == Open)
    {
        char *msg = encodeMsg(creatMsg(rstrnt->name, rstrnt->tcpPort, SHOW_SUPPLIERS, SHOW_SUPPLIERS));
        sendto(rstrnt->udpFd, msg, strlen(msg), 0, (struct sockaddr *)&rstrnt->udpAddr, sizeof(rstrnt->udpAddr));
        startDisply();
        showSuppliers(rstrnt->tcpFd, rstrnt->name);
        endDisply();
    }
    else if (strcmp(cmd, REQUEST_INGREDIENT) == 0 && rstrnt->status == Open)
    {
        sendIngrdntReq(rstrnt);
    }
    else if (strcmp(cmd, SHOW_REQUESTS_LIST) == 0 && rstrnt->status == Open)
    {
        displayRequestsList(rstrnt);
    }
    else if (strcmp(cmd, ANSWER_REQUEST) == 0 && rstrnt->status == Open)
    {
        Response rsp = answerOrderReq();
        for (int i = 0; i < MAX_FOOD_NUM; i++)
        {
            if (rstrnt->orderedFoods[i].cstmrPort != rsp.cstmrPort)
                continue;
            if (strcmp(rstrnt->orderedFoods[i].status, NO) == 0)
            {
                printf("This order is already set to No (NO | Time Out).\n");
                break;
            }
            if (strcmp(rstrnt->orderedFoods[i].status, YES) == 0)
            {
                printf("This order is already set to Yes.\n");
                break;
            }
            if (hasEnoghIngredient(rstrnt, rstrnt->orderedFoods[i].foodName) == 0)
            {
                printf("There is no enogh ingredient for this food right now.\n");
                break;
            }

            int fd = rstrnt->orderedFoods[i].fd;
            printf("rstrnt->orderedFoods[i].fd:%d\n", fd);
            strcpy(rstrnt->orderedFoods[i].status, rsp.answer);

            char *encoded = encodeMsg(creatMsg(rstrnt->name, rstrnt->tcpPort, rsp.answer, ORDER_FOOD));
            printf("bf send\n");
            int a = send(fd, encoded, strlen(encoded), 0);
            printf("this is send : %d\n", a);
            if (rsp.answer == NO)
                break;
            makeFood(rstrnt, rstrnt->orderedFoods[i].foodName);
        }
    }
    else if (strcmp(cmd, SHOW_SALES_HISTORY) == 0 && rstrnt->status == Open)
    {
        desplaySalesHistory(rstrnt);
    }
    else if (rstrnt->status == Closed)
    {
        setPromptReadInput(STDOUT_FILENO, "you are closed!\n", NULL, 0);
    }
    else
    {
        setPromptReadInput(STDOUT_FILENO, "invalid command\n", NULL, 0);
    }
}

char *showOrdrIngrdntRspnse(char *rspns, char *rstrntName, char *spplrName)
{
    if (strcmp(rspns, YES) == 0)
    {
        logFile(NAME, "Supplier accepted\n");
        printResponse(spplrName, "Supplier accepted !\n");
        return YES;
    }
    else if (strcmp(rspns, NO) == 0)
    {
        logFile(NAME, "Supplier denied\n");
        printResponse(spplrName, "Supplier denied!\n");
        return NO;
    }
    else
    {
        logFile(NAME, "Invalid income message\n");
        setPromptReadInput(STDOUT_FILENO, "invalid income msg\n", NULL, 0);
        return NO;
    }
}

int udbHndlr(Message *msg, RestaurantInfo *rstrnt)
{
    logFile(NAME, "UDP message received\n");
    char buff[INP_BUFF_SIZE];
    if ((strcmp(msg->title, CHECK_USERNAME_COMMAND) == 0) && msg->senderTcpPort != rstrnt->tcpPort)
    {
        logFile(NAME, "Check username command received\n");
        if (strcmp(rstrnt->name, msg->senderName) != 0)
            return 0;
        Message bcMsg = creatMsg(rstrnt->name, rstrnt->tcpPort, CHECK_USERNAME_COMMAND, CHECK_USERNAME_COMMAND);
        int serverFd = connectTcpClient(msg->senderTcpPort);
        char *encoded = encodeMsg(bcMsg);
        send(serverFd, encoded, strlen(encoded), 0);
        close(serverFd); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    if (strcmp(msg->title, SHOW_RESTAURANTS) == 0 && rstrnt->status == Open)
    {
        logFile(NAME, "Show restaurants command received\n");
        Message tcpMsg = creatMsg(rstrnt->name, rstrnt->tcpPort, SHOW_RESTAURANTS, SHOW_RESTAURANTS);
        int serverFd = connectTcpClient(msg->senderTcpPort);
        char *encoded = encodeMsg(tcpMsg);
        int a = send(serverFd, encoded, strlen(encoded), 0);
    }
    return 0;
}

Response answerOrderReq()
{
    Response response;
    char buff[PORT_BUFF_SIZE];

    memset(buff, '\0', PORT_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "\n-> port of request: ", buff, INP_BUFF_SIZE);
    response.cstmrPort = atoi(buff);

    memset(buff, '\0', PORT_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "-> your answer (yes/no): ", buff, INP_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "\n", NULL, 0);
    strcpy(response.answer, buff);

    return response;
}

void makeFood(RestaurantInfo *rst, char *foodName)
{
    int ingCount = rst->kitchenIngrdntCount;
    int amount = 0;
    int foodCount = rst->recipes.foodCount;
    for (int i = 0; i < foodCount; i++)
    {
        if (strcmp(rst->recipes.foods[i].name, foodName) != 0)
            continue;
        int size2 = rst->recipes.foods[i].ingredientAmount;
        for (int j = 0; j < size2; j++)
        {
            for (int z = 0; z < ingCount; z++)
            {
                if (strcmp(rst->recipes.foods[i].ingredients[j].name, rst->kitchenIngrdnt[z].name) != 0)
                    continue;

                amount = rst->recipes.foods[i].ingredients[j].count;
                rst->kitchenIngrdnt[z].count -= amount;
            }
        }
    }
    logFile(NAME, "Food made\n");
}

void tcpHndlr(RestaurantInfo *rstrnt, Message *msg)
{
    logFile(NAME, "TCP message received\n");
    if (strcmp(msg->title, ORDER_FOOD) == 0)
    {
        logFile(NAME, "Order food received\n");
        OrderedFood ordFood;
        strcpy(ordFood.cstmrName, msg->senderName);
        ordFood.cstmrPort = msg->senderTcpPort;

        char *token;
        token = strtok(msg->content, "|");
        strcpy(ordFood.foodName, token);
        token = strtok(NULL, "|");
        ordFood.fd = atoi(token);

        if (rstrnt->status == Closed || hasEnoghIngredient(rstrnt, ordFood.foodName) == 0)
        {
            Response rsp;
            strcpy(rsp.answer, NO);
            rsp.cstmrPort = msg->senderTcpPort;
            char *encoded = encodeMsg(creatMsg(rstrnt->name, rstrnt->tcpPort, rsp.answer, ORDER_FOOD));
            send(ordFood.fd, encoded, strlen(encoded), 0);
            return;
        }

        strcpy(ordFood.status, FOOD_STATUS_PENDING);
        rstrnt->orderedFoods[rstrnt->numOrderedFoods] = ordFood;
        strcpy(rstrnt->orderedFoods[rstrnt->numOrderedFoods].foodName, ordFood.foodName);
        strcpy(rstrnt->orderedFoods[rstrnt->numOrderedFoods].foodName, ordFood.foodName);
        rstrnt->numOrderedFoods++;
        setPromptReadInput(STDOUT_FILENO, "new order!\n", NULL, 0);
    }
    else if (strcmp(msg->title, TIMEOUT_MESSAGE) == 0 && strcmp(msg->content, ORDER_FOOD) == 0)
    {
        logFile(NAME, "Timeout message received\n");

        for (int i = 0; i < rstrnt->numOrderedFoods; i++)
        {
            if (strcmp(rstrnt->orderedFoods[i].status, FOOD_STATUS_PENDING) != 0)
                continue;
            if (rstrnt->orderedFoods[i].cstmrPort != msg->senderTcpPort)
                continue;
            if (strcmp(rstrnt->orderedFoods[i].cstmrName, msg->senderName) != 0)
                continue;
            strcpy(rstrnt->orderedFoods[i].status, NO);
            break;
        }
    }
}

int main(int argc, char const *argv[])
{
    int filFd = open("Restaurant", O_RDONLY);

    RestaurantInfo rstrnt;
    initRstrnt(&rstrnt, atoi(argv[1]), IP_ADDR);
    welcome(rstrnt.name, Restaurant);
    initKitchen(&rstrnt);

    fd_set masterSet, workingSet;
    struct sockaddr_in addrOut;

    int maxSd = (rstrnt.tcpFd > rstrnt.udpFd) ? rstrnt.tcpFd : rstrnt.udpFd;
    FD_ZERO(&masterSet);
    FD_SET(rstrnt.udpFd, &masterSet);
    FD_SET(rstrnt.tcpFd, &masterSet);
    FD_SET(STDIN_FILENO, &masterSet);
    char buffer[INP_BUFF_SIZE];
    while (1)
    {
        workingSet = masterSet;
        select(maxSd + 1, &workingSet, NULL, NULL, NULL);

        for (int i = 0; i <= maxSd; i++)
        {
            if (!FD_ISSET(i, &workingSet))
                continue;
            memset(buffer, '\0', INP_BUFF_SIZE);
            if (i == rstrnt.udpFd)
            {
                recv(rstrnt.udpFd, buffer, INP_BUFF_SIZE, 0);
                Message msg = decodeMsg(buffer);
                udbHndlr(&msg, &rstrnt);
            }
            else if (i == rstrnt.tcpFd)
            {
                int new_socket = accTcpClient(rstrnt.tcpFd);
                FD_SET(new_socket, &masterSet);
                if (new_socket > maxSd)
                    maxSd = new_socket;
            }
            else if (i == STDIN_FILENO)
            {
                setPromptReadInput(STDIN_FILENO, NULL, buffer, INP_BUFF_SIZE);
                cmdLineInterface(&rstrnt, buffer);
            }
            else
            {
                recv(i, buffer, INP_BUFF_SIZE, 0);
                Message msg = decodeMsg(buffer);
                tcpHndlr(&rstrnt, &msg);
            }
        }
    }
    close(filFd);
}