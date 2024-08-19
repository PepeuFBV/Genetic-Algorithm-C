#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POPULATION_SIZE 4  // Number of individuals in the population per generation
#define MIN_VALUE 0.1f
#define MAX_VALUE 7.0f
#define MUTATION_RATE 0.2f    // 20% chance of mutation
#define MUTATION_AMOUNT 1.0f  // Maximum adjustment amount for mutation

float func(float x, float y) {
    float pow1 = pow(x, 3);
    float pow2 = pow(y, 4);
    float sum = pow1 + (2 * pow2);
    return sqrt(sum);
}

float getRandomFloat(float min, float max) {
    return min + (float)rand() / (float)(RAND_MAX / (max - min));
}

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

typedef struct {
    float x;
    float y;
    float functionResult;
    float fitness;  // chance of being selected as a parent
} Individual;

/*
Apply the function to each individual in the population
*/
void calculateFunctionResults(Individual* population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population[i].functionResult = func(population[i].x, population[i].y);
    }
}

/*
Generate the initial population of individuals
*/
Individual* generateInitialPopulation() {
    Individual* population = (Individual*)malloc(POPULATION_SIZE * sizeof(Individual));
    if (population == NULL) {
        printf("Memory allocation for initial population failed\n");
        return NULL;
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        population[i].x = getRandomFloat(MIN_VALUE, MAX_VALUE);
        population[i].y = getRandomFloat(MIN_VALUE, MAX_VALUE);
        population[i].functionResult = 0.0f;
        population[i].fitness = 0.0f;
    }

    return population;
}

/*
Calculate the fitness of each individual in the population
*/
void fitness(Individual* individuals) {
    calculateFunctionResults(individuals);

    float sumResults = 0;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        if (individuals[i].functionResult > 0.0f) {
            sumResults += 1.0f / individuals[i].functionResult;
        } else {
            individuals[i].functionResult = 1.0e-6f;  // Avoid division by zero
            sumResults += 1.0f / individuals[i].functionResult;
        }
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        individuals[i].fitness = (1.0f / individuals[i].functionResult) / sumResults;
    }
}

/*
Choose the two parents with the highest fitness in the population group
*/
void chooseParents(Individual* population, int* parent1, int* parent2) {
    *parent1 = 0;
    *parent2 = 1;
    if (population[1].fitness > population[0].fitness) {
        *parent1 = 1;
        *parent2 = 0;
    }
    for (int i = 2; i < POPULATION_SIZE; i++) {
        if (population[i].fitness > population[*parent1].fitness) {
            *parent2 = *parent1;
            *parent1 = i;
        } else if (population[i].fitness > population[*parent2].fitness) {
            *parent2 = i;
        }
    }
}

/*
Mutate a random bit in the individual at the given index TODO: fix this function
*/
void mutate(Individual* population, int index) {
    // Decide whether to mutate x and/or y
    if ((float)rand() / RAND_MAX < MUTATION_RATE) {
        float oldX = population[index].x;

        // Mutate x
        float mutationX = (float)rand() / RAND_MAX * 2 * MUTATION_AMOUNT - MUTATION_AMOUNT;  // Random value between -MUTATION_AMOUNT and MUTATION_AMOUNT
        population[index].x += mutationX;
        population[index].x = clamp(population[index].x, MIN_VALUE, MAX_VALUE);  // Ensure x is within bounds

        printf("Mutated x from %.2f to %.2f\n", oldX, population[index].x);
    }

    if ((float)rand() / RAND_MAX < MUTATION_RATE) {
        float oldY = population[index].y;

        // Mutate y
        float mutationY = (float)rand() / RAND_MAX * 2 * MUTATION_AMOUNT - MUTATION_AMOUNT;  // Random value between -MUTATION_AMOUNT and MUTATION_AMOUNT
        population[index].y += mutationY;
        population[index].y = clamp(population[index].y, MIN_VALUE, MAX_VALUE);  // Ensure y is within bounds

        printf("Mutated y from %.2f to %.2f\n", oldY, population[index].y);
    }
}

/*
Crossover two parents to create children to be the new population
*/
void crossover(Individual* population, int parent1, int parent2, Individual* children) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        float alpha = (float)rand() / RAND_MAX;  // Random value between 0 and 1

        // Perform single-point crossover with blending
        children[i].x = alpha * population[parent1].x + (1 - alpha) * population[parent2].x;
        children[i].y = alpha * population[parent1].y + (1 - alpha) * population[parent2].y;

        // Clamp the values to be within the range [MIN_VALUE, MAX_VALUE]
        children[i].x = clamp(children[i].x, MIN_VALUE, MAX_VALUE);
        children[i].y = clamp(children[i].y, MIN_VALUE, MAX_VALUE);
    }
}

/*
Main training function
*/
void train(int generations) {
    srand(time(NULL));

    Individual* population = generateInitialPopulation();
    fitness(population);  // calculate fitness of the starting population

    printf("Initial population\n");
    for (int i = 0; i < POPULATION_SIZE; i++) {
        printf("Individual %d: x = %.2f, y = %.2f, fitness = %f, function = %f\n",
               i, population[i].x, population[i].y, population[i].fitness, population[i].functionResult);
    }
    printf("\n");

    for (int i = 1; i < generations; i++) {
        int parent1, parent2;                           // indexes of the parents
        chooseParents(population, &parent1, &parent2);  // choose the parents with the highest fitness

        Individual* children = (Individual*)malloc(POPULATION_SIZE * sizeof(Individual));  // new children
        if (children == NULL) {
            printf("Memory allocation for children failed\n");
            free(population);
            return;
        }

        printf("Parents chosen were:\n");
        printf("Parent 1: x = %.2f, y = %.2f, fitness = %f, function = %f\n",
               population[parent1].x, population[parent1].y, population[parent1].fitness, population[parent1].functionResult);
        printf("Parent 2: x = %.2f, y = %.2f, fitness = %f, function = %f\n", population[parent2].x,
               population[parent2].y, population[parent2].fitness, population[parent2].functionResult);

        crossover(population, parent1, parent2, children);  // create children from parents

        // 10% chance of mutation for each child
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (rand() % (int)(MUTATION_RATE * 10) == 0) {
                printf("Mutating child %d\n", i);
                mutate(children, i);
                printf("Child became %d: x = %.2f, y = %.2f\n", i, children[i].x, children[i].y);
            }
        }

        // replace the population with the children
        for (int i = 0; i < POPULATION_SIZE; i++) {
            population[i] = children[i];
        }

        // calculate fitness of the current population
        fitness(population);

        printf("Generation %d\n", i);
        for (int i = 0; i < POPULATION_SIZE; i++) {
            printf("Individual %d: x = %.2f, y = %.2f, fitness = %f, function = %f\n",
                   i, population[i].x, population[i].y, population[i].fitness, population[i].functionResult);
        }
        printf("\n");

        free(children);
    }

    free(population);
}