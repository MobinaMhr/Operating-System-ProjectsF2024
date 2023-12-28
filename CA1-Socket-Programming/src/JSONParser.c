#include "JSONParser.h"

char *readFile(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Cannot open file");
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        perror("Cannot get file size");
        close(fd);
        return NULL;
    }

    char *buffer = (char *)malloc(st.st_size + 1);
    if (buffer == NULL)
    {
        perror("Cannot allocate buffer");
        close(fd);
        return NULL;
    }

    ssize_t bytes_read = read(fd, buffer, st.st_size);
    if (bytes_read != st.st_size)
    {
        perror("Cannot read file");
        free(buffer);
        close(fd);
        return NULL;
    }

    buffer[st.st_size] = '\0';

    close(fd);
    return buffer;
}

char **getMenu()
{
    const char *json = readFile(RECIPES_JSON_FILE_PATH);
    cJSON *root = cJSON_Parse(json);
    if (root == NULL)
    {
        setPromptReadInput(STDERR_FILENO, "Failed to parse JSON.\n", NULL, 0);
        return NULL;
    }

    int dishNums = cJSON_GetArraySize(root);
    char **dish_names = (char **)malloc((dishNums) * sizeof(char *));

    cJSON *dish = root->child;
    int i = 0;
    while (dish != NULL)
    {
        const char *dishName = dish->string;
        dish_names[i] = strdup(dishName);
        i++;

        dish = dish->next;
    }
    cJSON_Delete(root);
    return dish_names;
}

char **getIngridients()
{
    const char *json = readFile(RECIPES_JSON_FILE_PATH);
    cJSON *root = cJSON_Parse(json);
    if (root == NULL)
    {
        setPromptReadInput(STDERR_FILENO, "Failed to parse JSON.\n", NULL, 0);
        return NULL;
    }

    int capacity = 10;
    char **ingredients = (char **)malloc(capacity * sizeof(char *));
    int ingrdntCount = 0;

    cJSON *dish = root->child;
    while (dish != NULL)
    {
        cJSON *Ingridient = dish->child;
        while (Ingridient != NULL)
        {
            const char *ingrdntName = Ingridient->string;

            int found = 0;
            for (int i = 0; i < ingrdntCount; i++)
            {
                if (strcmp(ingredients[i], ingrdntName) == 0)
                {
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                if (ingrdntCount >= capacity)
                {
                    capacity *= 2;
                    char **newDistinctIngredients = (char **)realloc(ingredients, capacity * sizeof(char *));
                    ingredients = newDistinctIngredients;
                }
                ingredients[ingrdntCount] = strdup(ingrdntName);
                ingrdntCount++;
            }

            Ingridient = Ingridient->next;
        }

        dish = dish->next;
    }
    cJSON_Delete(root);

    return ingredients;
}

void parseFood(cJSON *foodJson, FoodInfo *food_struct)
{
    cJSON *jsonIngrdnt = foodJson->child;
    food_struct->name = strdup(foodJson->string);
    food_struct->ingredientAmount = cJSON_GetArraySize(foodJson);
    food_struct->ingredients = (Ingridient *)malloc(sizeof(Ingridient) * food_struct->ingredientAmount);

    int i = 0;
    while (jsonIngrdnt)
    {

        food_struct->ingredients[i].name = strdup(jsonIngrdnt->string);
        food_struct->ingredients[i].count = jsonIngrdnt->valueint;
        i++;
        jsonIngrdnt = jsonIngrdnt->next;
    }
}

Recipes getRecipes()
{
    const char *json = readFile(RECIPES_JSON_FILE_PATH);
    cJSON *root = cJSON_Parse(json);
    if (!root)
    {
        setPromptReadInput(STDERR_FILENO, "Error parsing JSON!\n", NULL, 0);
        exit(EXIT_FAILURE);
    }

    int foodCount = cJSON_GetArraySize(root);
    FoodInfo *foods = (FoodInfo *)malloc(sizeof(FoodInfo) * foodCount);

    int i = 0;
    cJSON *foodJson = root->child;
    while (foodJson)
    {
        parseFood(foodJson, &foods[i]);
        foodJson = foodJson->next;
        i++;
    }
    cJSON_Delete(root);
    Recipes recp;
    recp.foodCount = foodCount;
    recp.foods = foods;
    return recp;
}