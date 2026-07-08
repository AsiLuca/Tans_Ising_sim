#ifndef LATTICE2D_HPP
#define LATTICE2D_HPP

#include "RandomGenerator.hpp"
#include <vector>

// Classe che rappresenta il reticolo quadrato 2D degli spin.
class Lattice2D {
private:
  int L;                  // Lato del reticolo
  int N;                  // Numero totale di spin (L * L)
  std::vector<int> spins; // Array 1D (ottimizzato).
  // Usiamo la formula [indice = riga * larghezza_riga + colonna= x*L + y]
  // per calcolare l'indice in base alle coordinate (x,y) degli spin.

public:
  // Inizializza il reticolo con spin casuali (T = infinito)
  Lattice2D(int L, RandomGenerator &rng) : L(L), N(L * L), spins(N) {
    for (int i = 0; i < N; i++) {
      spins[i] = (rng.getReal() < 0.5) ? 1 : -1; // operatore ternario if-else
    }
  }

  int getL() const { return L; }
  int getN() const { return N; }

  // Ritorna il valore dello spin applicando le Condizioni al Contorno
  // Periodiche (PBC). Se si esce dal reticolo, somma o sottrae L. Rendiamo il
  // reticolo toroidale. inline = copia la funzione (alleggerisce operazioni
  // ripetute)
  inline int getSpin(int x, int y) const { //
    if (x < 0)
      x += L;
    else if (x >= L)
      x -= L;
    if (y < 0)
      y += L;
    else if (y >= L)
      y -= L;
    return spins[x * L + y];
  }

  // Capovolge uno spin (applicando le PBC).
  inline void flipSpin(int x, int y) {
    if (x < 0)
      x += L;
    else if (x >= L)
      x -= L;
    if (y < 0)
      y += L;
    else if (y >= L)
      y -= L;
    spins[x * L + y] *= -1;
  }

  // Calcola l'energia totale del sistema.
  // L'Hamiltoniana è H = -J * sum_<i,j> (S_i * S_j).
  // Assumiamo J = 1.
  double getTotalEnergy() const {
    double E = 0;
    for (int x = 0; x < L; x++) {
      for (int y = 0; y < L; y++) {
        int s = getSpin(x, y);
        // Per evitare di contare due volte le coppie, guardiamo solo
        // ai vicini "destro" e "inferiore".
        int nn = getSpin(x + 1, y) + getSpin(x, y + 1);
        E += -s * nn;
      }
    }
    return E;
  }

  // Calcola la magnetizzazione totale (somma degli spin).
  double getTotalMagnetization() const {
    double M = 0;
    for (int s : spins) {
      M += s;
    }
    return M;
  }

  // Calcola la variazione di energia (Delta E) se lo spin in (x,y) venisse
  // capovolto. Delta E = 2 * J * S_i * sum(S_nn). Con J=1.
  inline int getDeltaE(int x, int y) const {
    int s = getSpin(x, y);
    int sum_nn = getSpin(x + 1, y) + getSpin(x - 1, y) + getSpin(x, y + 1) +
                 getSpin(x, y - 1);
    return 2 * s * sum_nn;
  }
};

#endif
