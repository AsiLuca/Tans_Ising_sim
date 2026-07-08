#ifndef METROPOLIS_HPP
#define METROPOLIS_HPP

#include "Lattice2D.hpp"
#include "RandomGenerator.hpp"
#include <cmath>

// Classe che implementa l'algoritmo Montecarlo di Metropolis.
class Metropolis {
private:
  Lattice2D &lattice;
  RandomGenerator &rng;
  double T;
  // Array per pre-calcolare i pesi di Boltzmann exp(-Delta E / T).
  // In un reticolo 2D quadrato, i possibili Delta E per un flip sono 5:
  // -8, -4, 0, 4, 8.
  double exp_dE[5];

  // Funzione per riempire l'array dei pesi pre-calcolati.
  // Questo evita di chiamare la funzione costosa std::exp() durante lo sweep.
  void precalculate() {
    for (int i = 0; i < 5; ++i) {
      int dE = -8 + i * 4;
      // Impostiamo k_B = 1
      exp_dE[i] = std::exp(-dE / T);
    }
  }

public:
  Metropolis(Lattice2D &lat, RandomGenerator &r, double temp)
      : lattice(lat), rng(r), T(temp) {
    precalculate();
  }

  // Aggiorna la temperatura e ricalcola i pesi di Boltzmann.
  void setTemperature(double temp) {
    T = temp;
    precalculate();
  }

  // Esegue un singolo "sweep" Monte Carlo, che consiste in N = L^2
  // tentativi di flip di spin. Il tempo è così misurato in unità di sweep.
  void sweep() {
    int N = lattice.getN();
    int L = lattice.getL();
    for (int step = 0; step < N; ++step) {
      // Seleziona un sito a caso
      int x = rng.getInteger(L);
      int y = rng.getInteger(L);

      // Calcola la variazione di energia se flippassimo lo spin
      int dE = lattice.getDeltaE(x, y);

      // Criterio di accettazione Metropolis:
      // Se l'energia diminuisce o rimane uguale, accetta sempre il flip.
      if (dE <= 0) {
        lattice.flipSpin(x, y);
      } else {
        // Se l'energia aumenta, accetta con probabilità proporzionale al
        // fattore di Boltzmann. Mappiamo DeltaE {4, 8} negli indici dell'array
        // {3, 4}.
        int idx = (dE + 8) / 4;
        if (rng.getReal() < exp_dE[idx]) {
          lattice.flipSpin(x, y);
        }
      }
    }
  }
};

#endif
