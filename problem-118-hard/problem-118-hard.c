#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define POPULATION_SIZE 50
#define MAX_GENERATIONS 1000
#define CULL_RATE 10
#define PARENT_RATE 5
#define MUTATION_RATE .3

#define STANDARD_DEVIATION_CUTOFF 5

#define CIPHER_LENGTH 26
#define WORD_LENGTH 6
#define NUM_WORDS 7260

#define WORDS_PATH "problem-118-hard.txt"
const char IN_ORDER[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                          'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                          'y', 'z'};



typedef struct {
  char* cipher;  //cipher is a 26 character mapping string
  int   fitness; //fitness indicates the fitness of cipher
}cipher_ind;

typedef struct {
  cipher_ind *individuals[POPULATION_SIZE]; //individuals contains the members of population
  int         age;                          //age is how old the population is in generations
}population;

//genetic algorithm
char *best_cipher(void);
bool  criteria_met(population *);

//statistical functions
double std_dev(population *);
double mean(population *);
int sum(population *);

//new generation functions
cipher_ind *crossover(const cipher_ind *, const cipher_ind *);
cipher_ind *mutate(const cipher_ind *);

//fitness related functions
int fitness(const char *);
char *apply(const char *, const char *);
bool alphabetical(const char *);

//constructor functions
population *new_random_population(void);
cipher_ind *copy_individual(const cipher_ind *);
cipher_ind *new_random_individual(void);
cipher_ind *new_individual(void);
char *new_string(int);

//destructor functions
void free_population(population *);
void free_individual(cipher_ind *);
void free_string(char *);

//utility functions
bool in(int, int*, size_t);
void swap(cipher_ind *, cipher_ind *);
void str_swap(char *, int, int);

int main(void){
  char *str = best_cipher();
  printf("%d generations\n", MAX_GENERATIONS);
  printf("%s %d\n", str, fitness(str) );
  return 0;
}

//GENETIC ALGORITHM
//////////////////////

//best_cipher uses a genetic algorithm to choose the best cipher
char *best_cipher(void){
  srand(time(NULL));

  int i, j, k;

  //choose initial population
  population *p = new_random_population();

  while(!criteria_met(p)){
    //prune population with worst fitness
    for(i=0; i<CULL_RATE; i++){
      int min_index = 0;
      for(j=0; j<(POPULATION_SIZE-i); j++){
        if(p->individuals[j]->fitness < p->individuals[min_index]->fitness){
          min_index = j;
        }
      }
      swap(p->individuals[min_index], p->individuals[POPULATION_SIZE-i-1]);
    }

    for(i=1; i<=CULL_RATE; i++){
      free_individual(p->individuals[POPULATION_SIZE-i]);
    }
      
    //select pairs from best to mate
    int s = sum(p);
    bool cont;

    for(i=0; i<PARENT_RATE; i++){
      int pick = rand() % s;
      
      for(j=0; j<POPULATION_SIZE-CULL_RATE; j++){
        
        if(pick <= p->individuals[j]->fitness){
          swap(p->individuals[j], p->individuals[i]);
          break;
        }
          
        pick -= p->individuals[j]->fitness;
      }
    }
    
    //replenish population with said pairs
    for(i=1; i<=CULL_RATE; i++){
      ////with crossover
      int orig = rand() % PARENT_RATE;
      cipher_ind *p1 = p->individuals[orig],
          *p2, *new;
      
      int pick = rand() % (PARENT_RATE-1);
      j=0;
      while(pick > 0){
        j++;
        if(j != orig)
          pick--;
        else
          continue;
      }
      p2 = p->individuals[j];
      
      new = crossover(p1, p2);
      
      ////with mutation
      if( ((double)rand()/(double)RAND_MAX) < MUTATION_RATE ){
        cipher_ind *mtd = mutate(new);
        free_individual(new);
        new = mtd;
      }

      p->individuals[POPULATION_SIZE-i] = new;
    }

    p->age++;
  }
  

  int fittest = 0;
  for(i=0; i<POPULATION_SIZE; i++)
    if(p->individuals[i]->fitness > p->individuals[fittest]->fitness)
      fittest = i;

  char *fittest_cipher = new_string(CIPHER_LENGTH);
  strcpy(fittest_cipher, p->individuals[fittest]->cipher);

  free_population(p);

  return fittest_cipher;
}


//criteria_met tests population p and returns true if its termination criteria are met.
bool criteria_met(population *p){
  if(p->age > MAX_GENERATIONS)
    return true;
  if(std_dev(p) < STANDARD_DEVIATION_CUTOFF)
    return true;

  return false;
}

//STATISTICAL FUNCTIONS
///////////////////////

//std_dev calculates the standard deviation of the fitness of the population.
double std_dev(population *p){
  double total = 0;
  double m = mean(p);

  int i;
  for(i=0; i<POPULATION_SIZE; i++){
    total += pow( p->individuals[i]->fitness - m, 2);
  }

  total /= POPULATION_SIZE;
  return sqrt(total);
}

//mean calculates the average fitness of a population
double mean(population *p){
  return sum(p)/POPULATION_SIZE;
}

int sum(population *p){
  int total = 0;

  int i;
  for(i=0; i<POPULATION_SIZE; i++){
    total += p->individuals[i]->fitness;
  }

  return total;
}

//NEW_GENERATION FUNCTIONS
///////////////////////////

//crossover selects a random point in the cipher string and swaps the values,
// such as in a chromosome. It then returns one of the two swapped strings.
cipher_ind *crossover(const cipher_ind *p1, const cipher_ind *p2){
  cipher_ind *cp1 = copy_individual(p1);
  cipher_ind *cp2 = copy_individual(p2);

  int i;
  for(i=(rand() % CIPHER_LENGTH); i<CIPHER_LENGTH; i++){
    char temp = cp1->cipher[i];
    cp1->cipher[i] = cp2->cipher[i];
    cp2->cipher[i] = temp;
  }

  i=rand() % 2;
  if(i == 1){
    free_individual(cp2);

    cp1->fitness = fitness(cp1->cipher);
    return cp1;
  }

  free_individual(cp1);

  cp2->fitness = fitness(cp2->cipher);
  return cp2;
}

//mutate returns a randomly mutated individual
cipher_ind *mutate(const cipher_ind *ind){
  cipher_ind *mtd = copy_individual(ind);

  do{
    str_swap(mtd->cipher, rand() % CIPHER_LENGTH, rand() % CIPHER_LENGTH);
  } while( (rand() % 3) != 0 );

  mtd->fitness = fitness(mtd->cipher);

  return mtd;
}

//FITNESS RELATED FUNCTIONS
////////////////////////////

//fitness returns the number of words in the provided file
// that are in alphabetical order for cipher.
int fitness(const char *cipher){
  int count = 0;

  FILE *fp = fopen(WORDS_PATH, "r");
  char *current = new_string(WORD_LENGTH);

  int i;
  for(i=0; i<NUM_WORDS; i++){
    fscanf(fp, "%s\n", current);
    count += alphabetical(apply(current, cipher)) ? 1 : 0;
  }

  fclose(fp);
  
  free_string(current);
  return count;
}

//apply encodes word using cipher.
char *apply(const char *word, const char *cipher){
  char *code = new_string(WORD_LENGTH);

  int i;
  for(i=0; i<WORD_LENGTH; i++){
    code[i] = cipher[word[i] - 'a'];
  }
  code[WORD_LENGTH] = '\0';

  return code;
}

//alphabetical returns true if the letters in s are in alphabetical order.
bool alphabetical(const char *s){
  int i;
  for(i=(strlen(s) - 1); i>0;){
    if(s[i] <= s[--i])
      return false;
  }
  return true;
}

//CONSTRUCTOR FUNCTIONS
////////////////////////

//new_random_population creates a new population with a set of random individuals.
population *new_random_population(void){
  population *p = (population *) malloc(sizeof(population));

  int i;
  for(i=0; i<POPULATION_SIZE; i++){
    p->individuals[i] = new_random_individual();
  }

  p->age = 0;

  return p;
}

//copy_individual creates a new individual from the passed one.
cipher_ind *copy_individual(const cipher_ind *ind){
  cipher_ind *copy = new_individual();
  strcpy(copy->cipher, ind->cipher);
  copy->fitness = ind->fitness;

  return copy;
}

//new_random_individual creates a new individual with a random cipher.
cipher_ind *new_random_individual(void){
  int picked[CIPHER_LENGTH];
  
  cipher_ind *ind = (cipher_ind *) malloc(sizeof(cipher_ind));
  ind->cipher = new_string(CIPHER_LENGTH);
  
  int i;
  for(i=0; i<CIPHER_LENGTH; i++){

    int index = rand() % CIPHER_LENGTH;
    while(in(index, picked, i)){
      index = rand() % CIPHER_LENGTH;
   }
    picked[i] = index;
  }

  for(i=0; i<CIPHER_LENGTH; i++){
    ind->cipher[i] = IN_ORDER[picked[i]];
  }

  ind->cipher[CIPHER_LENGTH] = '\0';

  ind->fitness = fitness(ind->cipher);
  
  return ind;
}

//new_individual creates a new individual with a garbage cipher.
cipher_ind *new_individual(void){
  cipher_ind *ind = (cipher_ind *) malloc(sizeof(cipher_ind));
  ind->cipher = new_string(CIPHER_LENGTH);
  ind->fitness = -1;

  return ind;
}

//new_string allocates a new string with room for len useful characters.
char *new_string(int len){
  return (char *) calloc(len+1, sizeof(char));
}

//DESTRUCTOR FUNCTIONS
//////////////////////

//free_population deletes a population.
void free_population(population *p){
  int i;
  for(i=0; i<POPULATION_SIZE; i++){
    free_individual(p->individuals[i]);
  }

  free(p);
}

//free_individual deletes an individual.
void free_individual(cipher_ind *individual){
  free_string(individual->cipher);
  free(individual);
}

//free_string frees a string.
void free_string(char *str){
  free(str);
}

//UTILITY FUNCTIONS

//in returns true if an int is in an int array.
bool in(int i, int* a, size_t s){
  int j;
  for(j=0; j<s; j++)
    if(i == a[j])
      return true;
  return false;
}

void swap(cipher_ind *a, cipher_ind *b){
  cipher_ind *temp = a;
  a = b;
  b = temp;
}

void str_swap(char *str, int a, int b){
  char temp = str[a];
  str[a] = str[b];
  str[b] = temp;
}

