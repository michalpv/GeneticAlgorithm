#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define POP_SIZE 10

typedef struct {
    uint16_t genome;
    float fitness;
} individual;

uint16_t desiredGenome = 0x1234;

// Returns new individual with random genome and fitness of 0
individual NewIndividual() {
    return (individual) {
        // Randomly generate 16-bit integer values
        .genome = rand() % 65535,
        .fitness = 0
    };
}

// Determines how close the passed individual matches desired genome (bit by bit)
void fitnessFunction(individual* Individual) {
    int fitCount = 0;
    for (int i = 0; i < 16; i++) { // uint16_t has a bit width of 16
        // Build mask
        uint16_t mask = 1 << i;
        if ((mask & Individual->genome) == (mask & desiredGenome)) { // If bit i matches in desiredGenome, increase fitCount
            fitCount++;
        }
    }
    Individual->fitness = (float) fitCount / 16; // uint16_t has a bit width of 16
}

// Sort population from lowest to highest fitness
void sortPopulation(individual pop[]) {
    // Bubble sort (POP_SIZE is small anyway)
    int terminate = 0;
    while (!terminate) {
        terminate = 1;
        for (int i = 0; i < POP_SIZE - 1; i++) {
            if (pop[i].fitness > pop[i+1].fitness) {
                individual temp = pop[i];
                pop[i] = pop[i+1];
                pop[i+1] = temp;
                terminate = 0;
            }
        }
    }
}

int main() {
    srand(time(NULL));

    // Populate population array
    individual population[POP_SIZE];

    for (int i = 0; i < POP_SIZE; i++) {
        population[i] = NewIndividual();
    }

    // Repeat reproduction process until 100% fit individual is found
    unsigned int generation = 1;
    while (population[POP_SIZE - 1].fitness < 1) {
        // Loop through all individuals in population array
        for (int i = 0; i < POP_SIZE; i++) {
            // Process them through the fitness function
            fitnessFunction(&population[i]);
        }
        // Sort population array from lowest to highest fitness
        sortPopulation(population);

        printf("Generation %d:\n\tTop Genome: 0x%x\n\tFitness: %f\n", generation, population[POP_SIZE - 1].genome, population[POP_SIZE - 1].fitness);
        // Selection (Top 2 individuals for now)
        individual new1 = population[POP_SIZE-1];
        individual new2 = population[POP_SIZE-2];

        // Crossover (reproduction)
        int crossover = (rand() % 15) + 1; // Can only bit shift 16 - 1 bits for 16 bit integer
        // Bit shift by 16 - crossover bits to the right
        uint16_t gen1 = new1.genome >> (16 - crossover);
        uint16_t gen2 = new2.genome >> (16 - crossover);
        // Take difference of both genomes
        uint16_t dif = (gen2 > gen1) ? gen2 - gen1 : gen1 - gen2;
        // Bit shift that difference 16 - crossover bits to the left
        dif = dif << (16 - crossover);
        // Add/subtract respectively the difference to the genomes
        new1.genome = (gen1 > gen2) ? new1.genome - dif : new1.genome + dif;
        new2.genome = (gen2 > gen1) ? new2.genome - dif : new2.genome + dif;
        
        // Mutation
        // 70% chance for each new offspring to mutate
        if (rand() % 10 <= 7) {
            // Modify 1 bit at random
            uint16_t mask = 1 << (rand() % 15);
            new1.genome = new1.genome ^ mask;
        }

        if (rand() % 10 <= 7) {
            // Modify 1 bit at random
            uint16_t mask = 1 << (rand() % 15);
            new2.genome = new2.genome ^ mask;
        }

        // Replace least fit individuals with offspring
        population[0] = new1;
        population[1] = new2;

        generation++;
    }

    return 0;
}