#pragma once
#include <string>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TStyle.h>

class IsingPlotter {
public:
  IsingPlotter() = default;

  // Configura stile globale ROOT (va chiamato una volta sola, tipicamente nel main)
  static inline void setupGlobalStyle() {
    ROOT::GetROOT()->SetBatch(kTRUE);
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetTitleFont(42, "T");
    gStyle->SetOptStat(0);
  }

  // Crea, disegna e salva i 4 pannelli per una data dimensione L
  inline void plotAndSave(int L,
                          const std::vector<double>& T_vals,
                          const std::vector<double>& E_vals,
                          const std::vector<double>& M_vals,
                          const std::vector<double>& Cv_vals,
                          const std::vector<double>& Chi_vals,
                          const std::string& outputDir = "plots") {
    TCanvas c1("c1", "Risultati Ising 2D", 1200, 1000);
    c1.Divide(2, 2);

    TGraph* gr_M = makeGraph(T_vals, M_vals,
        "Magnetizzazione Media assoluta per spin; Temperatura T; <|M|>", kBlue);
    TGraph* gr_E = makeGraph(T_vals, E_vals,
        "Energia Media per spin; Temperatura T; <E>", kRed);
    TGraph* gr_Cv = makeGraph(T_vals, Cv_vals,
        "Calore Specifico per spin; Temperatura T; C_{v}", kGreen + 2);
    TGraph* gr_Chi = makeGraph(T_vals, Chi_vals,
        "Suscettivita' Magnetica per spin; Temperatura T; #chi", kMagenta);

    c1.cd(1); gr_M->Draw("APL");
    c1.cd(2); gr_E->Draw("APL");
    c1.cd(3); gr_Cv->Draw("APL");
    c1.cd(4); gr_Chi->Draw("APL");

    std::string png = outputDir + "/Ising_L" + std::to_string(L) + ".png";
    c1.SaveAs(png.c_str());

    delete gr_M;
    delete gr_E;
    delete gr_Cv;
    delete gr_Chi;
    // c1 è locale (stack), viene distrutta automaticamente
  }

private:
  // Helper privato per non ripetere lo styling 4 volte
  static inline TGraph* makeGraph(const std::vector<double>& x,
                                   const std::vector<double>& y,
                                   const char* title,
                                   int color) {
    TGraph* gr = new TGraph(x.size(), x.data(), y.data());
    gr->SetTitle(title);
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(0.6);
    gr->SetMarkerColor(color);
    gr->SetLineColor(color);
    return gr;
  }
};