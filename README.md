# Genetic Algorithm

This project implements a simple genetic algorithm in C to optimize a mathematical function. The algorithm evolves a population of candidate solutions over multiple generations to find the best solution according to a given fitness function.

The genetic algorithm is as follows:

1. **Initialization**: Create a population of candidate solutions.

2. **Evaluation**: Evaluate the fitness of each candidate solution.

3. **Selection**: Select the best candidate solutions to reproduce.

4. **Crossover**: Create new candidate solutions by combining the selected solutions.

5. **Mutation**: Mutate the new candidate solutions.

6. **Replacement**: Replace the old population with the new population.

7. **Iteration**: Repeat steps 2-6 for a fixed number of generations.

The function to be optimized is the following:

```c
float func(float x, float y) {
    float pow1 = pow(x, 3);
    float pow2 = pow(y, 4);
    float sum = pow1 + (2 * pow2);
    return sqrt(sum);
}
```

## Key Parameters

- **Individuals**: The candidate solutions in the population. Each individual is represented by the following struct:

```c
typedef struct {
    float x;
    float y;
    float functionResult;
    float fitness;  // chance of being selected as a parent
} Individual;
```

- **Fitness Function**: The fitness of each candidate solution is calculated as the inverse of the function result. The lower the function result, the higher the fitness, meaning the higher the chance of being selected as a parent for the next generation.

- **Population Size**: The number of candidate solutions in the population. Its set to **4**.

- **Mutation Rate**: The probability that a candidate solution will mutate. Its set to **20%**.

- **Solution Space**: The range of values for the candidate solutions. Its set to **[0, 7]**.

- **Generations**: The number of generations to evolve the population. Its set to **100**.

## Usage

To compile the program, run the following command:

```bash
gcc train.c -c
gcc main.c -o main
```

To run the program, execute the following command:

```bash
./main
```

## License

This project is licensed under the MIT License.
