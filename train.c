#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int function2(int x, int y) {
    // return sqrt(pow(x, 3) + (2 * pow(y, 4)));
    return x * x;
}

typedef struct {
    int x;
    int y;
    int functionResult;
    double fitness;  // chance of being selected as a parent
} Individual;

Individual* calculateFunctionResults(Individual* population, int populationSize) {
    for (int i = 0; i < populationSize; i++) {
        population[i].functionResult = function2(population[i].x, population[i].y);
    }
    return population;
}

Individual* generateInitialPopulation(int populationSize) {
    Individual* population = (Individual*)malloc(populationSize * sizeof(Individual));
    if (population == NULL) {
        printf("Memory allocation for initial population failed\n");
        return NULL;
    }

    srand(time(NULL));  // Seed the random number generator
    for (int i = 0; i < populationSize; i++) {
        population[i].x = rand() % 7 + 1;  // random number between 1 and 7
        population[i].y = rand() % 7 + 1;  // random number between 1 and 7
    }

    return population;
}

Individual* fitness(Individual* individuals, int populationSize) {
    individuals = calculateFunctionResults(individuals, populationSize);

    for (int i = 0; i < populationSize; i++) {
        double current = 1 / individuals[i].functionResult;
        double sumResults = 0;
        for (int j = 0; j < populationSize; j++) {
            sumResults += 1 / individuals[j].functionResult;
        }
        individuals[i].fitness = current / sumResults;  // this isn't working ?
        printf("current: %lf", current);
        printf("sum: %lf", sumResults);
        printf("fitness: %d", individuals[i].fitness);
        printf("\n");
    }

    return individuals;
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
    int mutationPlace = rand() % 3;  // chooses the bit to mutate (0 to 2)

    population[index].x ^= 1 << mutationPlace;
    population[index].x = population[index].x % 7;  // ensure x is within [0, 7]

    population[index].y ^= 1 << mutationPlace;
    population[index].y = population[index].y % 7;  // ensure y is within [0, 7]

    return population;
}

Individual* crossover(Individual* population, int parent1, int parent2, Individual* children, int numChildren) {
    srand(time(NULL));

    for (int i = 0; i < numChildren; i++) {
        int crossoverPoint = rand() % 2;  // choose a random crossover point (0 or 1

        // create masks for the crossover point
        int mask1 = (1 << crossoverPoint) - 1;  // 00000011 for crossoverPoint = 2
        int mask2 = ~mask1;                     // 11111100 for crossoverPoint = 2

        // combine bits from both parents, half from each
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

    for (int i = 0; i < generations; i++) {
        population = fitness(population, populationSize);  // calculate fitness of the starting population

        int parent1, parent2;  // indexes of the parents
        chooseParents(population, populationSize, &parent1, &parent2);

        Individual* children = (Individual*)malloc(populationSize * sizeof(Individual));  // new children
        if (children == NULL) {
            printf("Memory allocation for children failed\n");
            return;
        }

        children = crossover(population, parent1, parent2, children, populationSize);  // create children from parents

        // 10% chance of mutation for each child
        // for (int i = 0; i < populationSize; i++) {
        //     if (rand() % 10 == 0) {
        //         children = mutate(children, i);
        //     }
        // }

        // replace the population with the children
        for (int i = 0; i < populationSize; i++) {
            population[i] = children[i];
        }

        population = fitness(population, populationSize);  // calculate fitness of the current population

        printf("Generation %d\n", i);
        for (int i = 0; i < populationSize; i++) {
            printf("Children %d: x = %d, y = %d, fitness = %lf, function = %lf", i, children[i].x, children[i].y, children[i].fitness, children[i].functionResult);
            printf(" | ");
        }
        printf("\n\n");

        free(children);
    }
}
