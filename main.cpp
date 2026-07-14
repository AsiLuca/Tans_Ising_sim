#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <chrono>

#include "IsingSim.hpp"
#include "Analyzer.hpp"
#include "IsingPlotter.hpp"

using namespace std;

int main(int argc, char **argv) {
  // Crea la cartella per i plot se non esiste
  system("mkdir -p plots");
  IsingPlotter::setupGlobalStyle();
  
  Analyzer analyzer;
  IsingPlotter plotter;

  // Parametri della simulazione
  vector<int> lattice_sizes = {8,16,24,32,48,64,96,128};              // Dimensioni del reticolo (L x L)
  int therm_sweeps = 20000; // Passi per termalizzare il sistema
  int meas_sweeps = 30000; // Passi per misurare le osservabili

  cout << "***********************************************************" << endl;
  cout << "--- Inizio Simulazione Modello di Ising 2D (Metropolis) ---" << endl;
  cout << "***********************************************************" << endl;
  cout << endl;
  
  // Crea l'oggetto per gestire la simulazione
  for(int L : lattice_sizes){
    cout << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << "Reticolo L: " << L << "x" << L << " spin" << endl;
    cout << "Termalizzazione: " << therm_sweeps << " passi" << endl;
    cout << "Misurazione: " << meas_sweeps << " passi" << endl;
    cout << "-----------------------------------------------------------" << endl;
    auto start = chrono::high_resolution_clock::now();
    IsingSim sim(L, therm_sweeps, meas_sweeps, 1971); // seed = 42

    // Array per memorizzare i dati calcolati e poi passarli a ROOT
    vector<double> T_vals;
    vector<double> M_vals;
    vector<double> E_vals;
    vector<double> Cv_vals;
    vector<double> Chi_vals;

    // Definiamo i punti di temperatura.
    // Usiamo passi più fitti vicino a T_c (circa 2.269) per cogliere la transizione di fase
    vector<double> temperatures;
    for (double T = 1.0; T <= 4.0; T += 0.1) {
      if (T > 2.0 && T < 2.6) {
        // Vicino al punto critico, usiamo un passo di 0.02
        for (double fine_T = T; fine_T < T + 0.1; fine_T += 0.02) {
          temperatures.push_back(fine_T);
        }
      } else {
        temperatures.push_back(T);
      }
    }

    cout << "\nSimulazione in corso..." << endl;
    cout << fixed << setprecision(4);
    cout << "T\t<E>\t<|M|>\tC_v\tChi\n";

    // Ciclo principale sulle temperature
    for (double T : temperatures) {
      SimResult res = sim.runForTemperature(T);

      T_vals.push_back(res.T);
      M_vals.push_back(res.M_abs_mean);
      E_vals.push_back(res.E_mean);
      Cv_vals.push_back(res.Cv);
      Chi_vals.push_back(res.Chi);

      // Output in tempo reale a terminale
      cout << res.T << "\t" << res.E_mean << "\t" << res.M_abs_mean << "\t"
           << res.Cv << "\t" << res.Chi << endl;
    }

    analyzer.addSimulation(L, T_vals, Chi_vals);
    auto stop = chrono::high_resolution_clock::now();

    double runtime =
        chrono::duration<double>(stop - start).count();

    analyzer.addRuntime(L, runtime);
    cout << "\nRuntime simulazione: " << runtime << " s\n" << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << endl;

     plotter.plotAndSave(L, T_vals, E_vals, M_vals, Cv_vals, Chi_vals);
  }

  analyzer.finiteSizeScaling();
  analyzer.runtimeScaling();  

  return 0;
}