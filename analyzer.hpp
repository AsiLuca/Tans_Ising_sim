#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <iostream>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>

class Analyzer {

private:
  std::vector<double> L_values;
  std::vector<double> Tc_values;

public:
  Analyzer() = default;

  // Estrae automaticamente Tc dal massimo della suscettività
  void addSimulation(int L, const std::vector<double> &T,
                     const std::vector<double> &Chi) {
    if (T.empty() || Chi.empty())
      return;

    size_t index_max = 0;

    for (size_t i = 1; i < Chi.size(); i++) {

      if (Chi[i] > Chi[index_max])
        index_max = i;
    }

    L_values.push_back(L);
    Tc_values.push_back(T[index_max]);

    std::cout << "L = " << L << "   Tc = " << T[index_max] << std::endl;
  }

  void finiteSizeScaling() {

    if (L_values.size() < 2) {

      std::cout << "Servono almeno due simulazioni." << std::endl;
      return;
    }

    int N = L_values.size();

    std::vector<double> invL(N);
    std::vector<double> errX(N, 0.0);
    std::vector<double> errY(N, 0.0);

    // Crea un grafico in cui sull'asse X c'è 1/L e sull'asse Y c'è T_c(L)
    for (int i = 0; i < N; i++)
      invL[i] = 1.0 / L_values[i];

    TCanvas *c = new TCanvas("Scaling", "Finite Size Scaling", 800, 600);

    TGraphErrors *gr = new TGraphErrors(N, invL.data(), Tc_values.data(),
                                        errX.data(), errY.data());

    gr->SetTitle("Finite Size Scaling");
    gr->GetXaxis()->SetTitle("1/L");
    gr->GetYaxis()->SetTitle("T_{c}(L)");

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.2);

    // Effettua un fit per trovare il valore dell'intercetta
    // che corrisponde al limite per $1/L \to 0$, ovvero per $L \to \infty$.
    // Questo valore stimato è la temperatura critica reale del sistema
    // infinito.

    TF1 *fit = new TF1("fit", "[0]+[1]*x", 0, invL.front());

    fit->SetParameters(2.27, 1.0);

    gr->Draw("AP");

    gr->Fit(fit, "R");

    gStyle->SetOptFit(1111);

    c->SaveAs("plots/FiniteSizeScaling.pdf");
    c->SaveAs("plots/FiniteSizeScaling.png");

    std::cout << "\nTc(infinito) = " << fit->GetParameter(0) << std::endl;

    delete fit;
    delete gr;
    delete c;
  }
};

#endif