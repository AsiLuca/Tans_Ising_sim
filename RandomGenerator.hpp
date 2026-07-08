#ifndef RANDOM_GENERATOR_HPP
#define RANDOM_GENERATOR_HPP

#include <random>

// Classe per gestire la generazione di numeri casuali utilizzando
// il generatore Mersenne-Twister (std::mt19937).
class RandomGenerator {
private:
  std::mt19937 gen; // Generatore di numeri pseudo-casuali
  std::uniform_real_distribution<double> dist_real;
  // gen è il Generatore di numeri pseudo-casuali
  // dist_real crea distribuzione uniforme in un intervallo

public:
  // Costruttore: inizializza il generatore con un seed
  RandomGenerator(unsigned int seed = 1971) : gen(seed), dist_real(0.0, 1.0) {}

  // Ritorna un numero reale uniformemente distribuito in [0, 1)
  double getReal() { return dist_real(gen); }

  // Ritorna un numero intero uniformemente distribuito in [0, max_val - 1]
  int getInteger(int max_val) {
    std::uniform_int_distribution<int> dist_int(0, max_val - 1);
    return dist_int(gen);
  }
};

#endif
