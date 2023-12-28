#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "cJSON.h"
#include "defines.h"

#include "utils.h"

char *readFile(const char *filename);
char **getMenu();
char **getIngridients();
void parseFood(cJSON *foodJson, FoodInfo *food_struct);
Recipes getRecipes();