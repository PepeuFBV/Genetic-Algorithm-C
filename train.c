#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float function(int x, int y) {
    return sqrt(pow(x, 3) + 2 * pow(y, 4));
}

typedef struct {
    int x;
    int y;
    float functionResult;
    float fitness;  // chance of being selected as a parent
} Individual;

void calculateFunctionResults(Individual* population, int populationSize) {
    for (int i = 0; i < populationSize; i++) {
        population[i].functionResult = function(population[i].x, population[i].y);
    }
}

Individual* generateInitialPopulation(int populationSize) {
    Individual* population = (Individual*)malloc(populationSize * sizeof(Individual));
    if (population == NULL) {
        printf("Memory allocation for initial population failed\n");
        return NULL;
    }

    srand(time(NULL));  // Seed the random number generator
    for (int i = 0; i < populationSize; i++) {
        population[i].x = rand() % 7;  // random number between 0 and 7
        population[i].y = rand() % 7;  // random number between 0 and 7
    }

    return population;
}

void fitness(Individual* individuals, int populationSize) {
    calculateFunctionResults(individuals, populationSize);

    for (int i = 0; i < populationSize; i++) {
        float current = 1 / individuals[i].functionResult;
        float sumResults = 0;
        for (int j = 0; j < populationSize; j++) {
            sumResults += 1 / individuals[j].functionResult;
        }
        individuals[i].fitness = current / sumResults;
    }
}

void chooseParents(Individual* population, int populationSize, int* parent1, int* parent2) {
    *parent1 = 0;
    *parent2 = 1;
    if (population[1].fitness > population[0].fitness) {
        *parent1 = 1;
        *parent2 = 0;
    }
    for (int i = 2; i < populationSize; i++) {
        if (population[i].fitness > population[*parent1].fitness) {
            *parent2 = *parent1;
            *parent1 = i;
        } else if (population[i].fitness > population[*parent2].fitness) {
            *parent2 = i;
        }
    }
}

Individual* mutate(Individual* population, int index) {
    int mutationPoint = rand() % 2;  // chooses x or y to mutate
    int mutationPlace = rand() % 3;  // chooses the bit to mutate (0 to 2)
    if (mutationPoint == 0) {        // x
        population[index].x ^= 1 << mutationPlace;
        population[index].x = population[index].x % 7;  // ensure x is within [0, 7]
    } else {                                            // y
        population[index].y ^= 1 << mutationPlace;
        population[index].y = population[index].y % 7;  // ensure y is within [0, 7]
    }

    return population;
}

Individual* crossover(Individual* population, int parent1, int parent2, Individual* children, int numChildren) {
    srand(time(NULL));

    for (int i = 0; i < numChildren; i++) {
        int crossoverPoint = rand() % 8;  // from number 0 to 7

        // create masks for the crossover point
        int mask1 = (1 << crossoverPoint) - 1;
        int mask2 = ~mask1;

        // combine bits from both parents
        int* parent1x = (int*)&population[parent1].x;
        int* parent2x = (int*)&population[parent2].x;
        int* parent1y = (int*)&population[parent1].y;
        int* parent2y = (int*)&population[parent2].y;

        int* childx = (int*)&children[i].x;
        int* childy = (int*)&children[i].y;

        *childx = (*parent1x & mask1) | (*parent2x & mask2);
        *childy = (*parent1y & mask1) | (*parent2y & mask2);
    }

    return children;
}

void train(int generations, int populationSize) {
    Individual* population = generateInitialPopulation(populationSize);
    fitness(population, populationSize);  // calculate fitness of the starting population

    for (int i = 0; i < generations; i++) {
        int parent1, parent2;  // indexes of the parents
        chooseParents(population, populationSize, &parent1, &parent2);

        Individual* children = (Individual*)malloc(populationSize * sizeof(Individual));  // new children
        if (children == NULL) {
            printf("Memory allocation for children failed\n");
            return;
        }

        children = crossover(population, parent1, parent2, children, populationSize);  // create children from parents

        // choose a random children to mutate, two children choosen with only x or y mutated
        int childToMutate = rand() % populationSize;
        mutate(children, childToMutate);
        childToMutate = rand() % populationSize;
        mutate(children, childToMutate);

        // replace the population with the children
        for (int i = 0; i < populationSize; i++) {
            population[i] = children[i];
        }

        fitness(population, populationSize);  // calculate fitness of the current population

        printf("Generation %d\n", i);
        for (int i = 0; i < populationSize; i++) {
            printf("Children %d: x = %d, y = %d", i, children[i].x, children[i].y);
            printf(" | ");
        }
        printf("\n");

        free(children);
    }
}
