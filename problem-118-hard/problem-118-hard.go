package main

import (
	"fmt"
	"io"
	"math"
	"os"
	"string"
)

const POPULATION_SIZE = 50
const MAX_GENERATIONS = 2000
const CULL_RATE = 15
const PARENT_RATE = 5
const MUTATION_RATE = 1

const STANDARD_DEVIATION_CUTOFF = 5

const CIPHER_LENGTH = 26
const WORD_LENGTH = 6
const NUM_WORDS = 7260

const WORDS_PATH = "problem-118-hard.txt"

const IN_ORDER = []byte{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z'}

type Cipher_ind struct {
	cipher  string
	fitness int
}

type Population struct {
	individuals []Cipher_ind
	age         int
}

//GENETIC ALGORITHM
///////////////////
func BestCipher() string {}
func CriteriaMet() bool  {}

//STATISTICAL FUNCTIONS
///////////////////////
func (p *Population) StdDev() float64 {}
func (p *Population) Mean() float64   {}
func (p *Population) Sum() int        {}

//NEW GENERATION FUNCTIONS
//////////////////////////
func (p1 *Cipher_ind) Crossover(p2 *Cipher_ind) *Cipher_ind {}
func (i *Cipher_ind) Mutate() *Cipher_ind                   {}

//FITNESS FUNCTIONS
///////////////////
func Fitness(c string) int {
	file, err := os.Open(WORDS_PATH)
	if err != nil{ panic(err) }
	defer file.Close()

//loop this
	var word string
	_, err := fmt.Fscanf(file, "%s\n", &word)
	if err != nil{ panic(err) }
}

func Apply(cipher, word string) string {
	code := word

	for i, _ := range word {
		code[i] = cipher[word[i]-'a']
	}

	return code
}

func Alphabetical(word string) bool {
	for i, _ := range word {

		if i == len(word) {
			break
		}

		if word[i+1] <= word[i] {
			return false
		}
	}
	return true
}

//CONSTRUCTORS
//////////////
func NewPopulation() *Population {}

func NewIndividual() *Cipher_ind                  {}
func NewRandomCipherInd() *Cipher_ind             {}
func CopyCipherInd(other *Cipher_ind) *Cipher_ind {}
