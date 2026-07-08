#ifndef ISING_SIM_HPP
#define ISING_SIM_HPP

#include "Lattice2D.hpp"
#include "Metropolis.hpp"
#include <cmath>

// Struct che contiente le osservabili calcolate a una certa T.
struct SimResult {
  double T;
  double M_abs_mean; // <|M|> per spin
  double E_mean;     // <E> per spin
  double Cv;         // Calore specifico per spin
  double Chi;        // Suscettività magnetica per spin
};

// Classe coordinatrice che gestisce termalizzazione, misurazione e calcolo
// delle medie.
class IsingSim {
private:
  RandomGenerator rng;
  Lattice2D lattice;
  Metropolis metropolis;
  int thermalization_sweeps;
  int measurement_sweeps;

public:
  // Costruttore della classe IsingSim.
  IsingSim(int L, int therm_swp, int meas_swp, unsigned int seed = 1971)
      : rng(seed), lattice(L, rng), metropolis(lattice, rng, 1.0),
        thermalization_sweeps(therm_swp), measurement_sweeps(meas_swp) {}

  // Esegue la simulazione per una specifica temperatura
  SimResult runForTemperature(double T) {
    metropolis.setTemperature(T);

    // Fase 1: Termalizzazione
    // Scartiamo le configurazioni iniziali per portare il sistema
    // all'equilibrio termodinamico.
    for (int i = 0; i < thermalization_sweeps; ++i) {
      metropolis.sweep();
    }

    // Fase 2: Misurazione
    // Accumuliamo i valori di E, E^2, |M|, M^2 per calcolare le fluttuazioni.
    double E_sum = 0, E2_sum = 0;
    double M_sum = 0, M2_sum = 0;

    for (int i = 0; i < measurement_sweeps; ++i) {
      metropolis.sweep();

      double E = lattice.getTotalEnergy();
      double M = std::abs(lattice.getTotalMagnetization()); // Usiamo il modulo

      E_sum += E;
      E2_sum += E * E;
      M_sum += M;
      M2_sum += M * M;
    }

    // Calcolo delle medie statistiche.
    double N = lattice.getN();
    double E_mean = E_sum / measurement_sweeps;
    double E2_mean = E2_sum / measurement_sweeps;
    double M_mean = M_sum / measurement_sweeps;
    double M2_mean = M2_sum / measurement_sweeps;

    SimResult res;
    res.T = T;
    res.E_mean = E_mean / N;     // Energia media per spin
    res.M_abs_mean = M_mean / N; // Magnetizzazione media per spin

    // Teorema di Fluttuazione-Dissipazione:
    // C_v = (<E^2> - <E>^2) / (T^2 * N)
    res.Cv = (E2_mean - E_mean * E_mean) / (T * T * N);

    // Suscettività:
    // chi = (<M^2> - <|M|>^2) / (T * N)
    res.Chi = (M2_mean - M_mean * M_mean) / (T * N);

    return res;
  }
};

#endif
