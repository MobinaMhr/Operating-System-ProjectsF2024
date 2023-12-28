#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <termios.h>

// #define O_RDWR 0x0002

// #define O_APPEND 0x0008

// #define O_CREAT 0x0100

// num defs
#define INP_BUFF_SIZE 1024
#define PORT_BUFF_SIZE 16
#define MAX_FOOD_NUM 80
#define MAX_INGREDIENT_NUM 120

#define IP_ADDR "192.168.72.255"
#define RECIPES_JSON_FILE_PATH "./recipes.json"

// food status defs
#define FOOD_STATUS_YES "yes"
#define FOOD_STATUS_NO "no"
#define FOOD_STATUS_PENDING "pending"

// prompts consts
#define SIGNIN_PROMPT "Please enter (unique) username: "
#define INVALID_SIGNIN_PROMPT "Please enter another username: "
#define OPENED_RESTAURANT_PROMPT " restaurant opend!"

// errors
#define PORT_NOT_ENTERED_ERROR "please enter your port"
#define INVALID_USERNAME_ERROR "invalid username"

// UDP commands
#define CHECK_USERNAME_COMMAND "check_username"

////
#define START_WORKING "start working"
#define BREAK "break"
#define SHOW_INGREDIENTS "show ingredients"
#define SHOW_RECIPES "show recipes"
#define SHOW_SUPPLIERS "show suppliers"
#define REQUEST_INGREDIENT "request ingredient"
#define SHOW_REQUESTS_LIST "show requests list"
#define ANSWER_REQUEST "answer request"
#define SHOW_SALES_HISTORY "show sales history"

////
#define SHOW_RESTAURANTS "show restaurants"
#define SHOW_MENU "show menu"

#define ORDER_FOOD "order food"
////
#define YES "yes"
#define NO "no"
#define TIMEOUT_MESSAGE "Time out!\n"
#define NEW_ORDER_MESSAGE "new order!\n\n"
#define RESTAURANT_OPPENED_MESSAGE "Restaurant opened!"
#define RESTAURANT_CLOSED_MESSAGE "Restaurant closed!"

//
#define SHOW_RESTAURANT_TITLE "\nusername/port"
#define SHOW_SUPPLIER_TITLE "\nusername/port"
#define DISPLAY_REQUEST_LIST_TITLE "\nusername/port/order"
#define DISPLAY_SALES_HISTORY_TITLE "\nusername/port/order"
#define DISPLAY_INGREDIENT_TITLE "\ningredient/amount"

#define WAIT_TILL_RESTRNT_RESPONSE "waiting for the restaurant's response ...\n\n"
#define WAIT_TILL_SUPPLIR_RESPONSE "waiting for the supplier's response ...\n\n"

#endif