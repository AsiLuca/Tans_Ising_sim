#include "IsingSim.hpp"
#include "analyzer.hpp"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

#include "IsingSim.hpp"
#include "analyzer.hpp"

// librerie ROOT
#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TStyle.h>

using namespace std;

int main(int argc, char **argv) {
  // Crea la cartella per i plot se non esiste
  system("mkdir -p plots");

  // La modalità batch fa sì che il programma esegua le operazioni grafiche in
  // "background", senza aprire alcuna finestra
  ROOT::GetROOT()->SetBatch(kTRUE);

  Analyzer analyzer;

  // Parametri della simulazione
  std::vector<int> lattice_sizes = {16, 32, 64,
                                    128}; // Dimensioni del reticolo (L x L)
  int therm_sweeps = 5000;                // Passi per termalizzare il sistema
  int meas_sweeps = 30000;                // Passi per misurare le osservabili

  cout << "--- Inizio Simulazione Modello di Ising 2D (Metropolis) ---" << endl;

  // Crea l'oggetto per gestire la simulazione
  for (int L : lattice_sizes) {
    cout << "Reticolo L: " << L << "x" << L << " spin" << endl;
    cout << "Termalizzazione: " << therm_sweeps << " passi" << endl;
    cout << "Misurazione: " << meas_sweeps << " passi" << endl;
    IsingSim sim(L, therm_sweeps, meas_sweeps, 42); // seed = 42

    // Array per memorizzare i dati calcolati e poi passarli a ROOT
    vector<double> T_vals;
    vector<double> M_vals;
    vector<double> E_vals;
    vector<double> Cv_vals;
    vector<double> Chi_vals;

    // Definiamo i punti di temperatura.
    // Usiamo passi più fitti vicino a T_c (circa 2.269) per cogliere la
    // transizione di fase.
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

    // ------- INIZIO PLOTTING CON ROOT -------

    // Stile generale
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetTitleFont(42, "T");
    gStyle->SetOptStat(0);

    // Canvas divisa in 4 pannelli (2x2)
    TCanvas *c1 = new TCanvas("c1", "Risultati Ising 2D", 1200, 1000);
    c1->Divide(2, 2);

    // 1. Magnetizzazione
    TGraph *gr_M = new TGraph(T_vals.size(), &T_vals[0], &M_vals[0]);
    gr_M->SetTitle(
        "Magnetizzazione Media assoluta per spin; Temperatura T; <|M|>");
    gr_M->SetMarkerStyle(20);
    gr_M->SetMarkerSize(0.6);
    gr_M->SetMarkerColor(kBlue);
    gr_M->SetLineColor(kBlue);

    // 2. Energia
    TGraph *gr_E = new TGraph(T_vals.size(), &T_vals[0], &E_vals[0]);
    gr_E->SetTitle("Energia Media per spin; Temperatura T; <E>");
    gr_E->SetMarkerStyle(20);
    gr_E->SetMarkerSize(0.6);
    gr_E->SetMarkerColor(kRed);
    gr_E->SetLineColor(kRed);

    // 3. Calore Specifico
    TGraph *gr_Cv = new TGraph(T_vals.size(), &T_vals[0], &Cv_vals[0]);
    gr_Cv->SetTitle("Calore Specifico per spin; Temperatura T; C_{v}");
    gr_Cv->SetMarkerStyle(20);
    gr_Cv->SetMarkerSize(0.6);
    gr_Cv->SetMarkerColor(kGreen + 2);
    gr_Cv->SetLineColor(kGreen + 2);

    // 4. Suscettività
    TGraph *gr_Chi = new TGraph(T_vals.size(), &T_vals[0], &Chi_vals[0]);
    gr_Chi->SetTitle("Suscettivita' Magnetica per spin; Temperatura T; #chi");
    gr_Chi->SetMarkerStyle(20);
    gr_Chi->SetMarkerSize(0.6);
    gr_Chi->SetMarkerColor(kMagenta);
    gr_Chi->SetLineColor(kMagenta);

    // Disegniamo nei pad
    c1->cd(1);
    gr_M->Draw("APL"); // A=Axes, P=Points, L=Line

    c1->cd(2);
    gr_E->Draw("APL");

    c1->cd(3);
    gr_Cv->Draw("APL");

    c1->cd(4);
    gr_Chi->Draw("APL");

    // Salviamo in formato PDF e PNG
    std::string pdf = "plots/Ising_L" + std::to_string(L) + ".pdf";
    std::string png = "plots/Ising_L" + std::to_string(L) + ".png";

    c1->SaveAs(pdf.c_str());
    c1->SaveAs(png.c_str());

    cout << "\nSimulazione per L=" << L << " completata." << endl;
    cout << "I grafici sono stati salvati nella cartella 'plots'." << endl;

    // Pulizia memoria ROOT
    delete gr_M;
    delete gr_E;
    delete gr_Cv;
    delete gr_Chi;
    delete c1;
  }

  analyzer.finiteSizeScaling();

  return 0;
}