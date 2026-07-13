#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <iostream>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TAxis.h>

class Analyzer {

private:

    std::vector<double> L_values;
    std::vector<double> Tc_values;
    std::vector<double> runtime_values;

public:

    Analyzer() = default;

    // Estrae automaticamente Tc dal massimo della suscettività
    void addSimulation(int L,
                       const std::vector<double>& T,
                       const std::vector<double>& Chi)
    {
        if(T.empty() || Chi.empty()) return;

        size_t index_max = 0;

        for(size_t i=1;i<Chi.size();i++){

            if(Chi[i] > Chi[index_max])
                index_max = i;

        }

        L_values.push_back(L);
        Tc_values.push_back(T[index_max]);

        std::cout << "L = " << L
                  << "   Tc = " << T[index_max]
                  << std::endl;
    }

    void finiteSizeScaling()
    {

        if(L_values.size()<2){

            std::cout<<"Servono almeno due simulazioni."<<std::endl;
            return;

        }

        int N = L_values.size();

        std::vector<double> invL(N);
        std::vector<double> errX(N,0.0);
        std::vector<double> errY(N,0.0);

        for(int i=0;i<N;i++)
            invL[i]=1.0/L_values[i];

        TCanvas *c =
            new TCanvas("Scaling",
                        "Finite Size Scaling",
                        800,
                        600);

        TGraphErrors *gr =
            new TGraphErrors(N,
                             invL.data(),
                             Tc_values.data(),
                             errX.data(),
                             errY.data());

        gr->SetTitle("Finite Size Scaling");
        gr->GetXaxis()->SetTitle("1/L");
        gr->GetYaxis()->SetTitle("T_{c}(L)");

        gr->SetMarkerStyle(20);
        gr->SetMarkerSize(1.2);

        TF1 *fit =
            new TF1("fit",
                    "[0]+[1]*x",
                    0,
                    invL.front());

        fit->SetParameters(2.27,1.0);

        gr->Draw("AP");

        gr->Fit(fit,"R");

        gStyle->SetOptFit(0);

        c->SaveAs("FiniteSizeScaling.pdf");
        c->SaveAs("FiniteSizeScaling.png");

        std::cout << "\nTc(infinito) = "
                  << fit->GetParameter(0)
                  << std::endl;

        delete fit;
        delete gr;
        delete c;
    }

    void addRuntime(int L,double runtime){
     runtime_values.push_back(runtime);
     }

     void runtimeScaling()
{
    if(runtime_values.size() < 2){

        std::cout << "Servono almeno due simulazioni." << std::endl;
        return;

    }

    int N = runtime_values.size();

    std::vector<double> errX(N,0.0);
    std::vector<double> errY(N,0.0);

    TCanvas *c =
        new TCanvas("Runtime",
                    "Runtime Scaling",
                    800,
                    600);

    TGraphErrors *gr =
        new TGraphErrors(N,
                         L_values.data(),
                         runtime_values.data(),
                         errX.data(),
                         errY.data());

    gr->SetTitle("Tempo di esecuzione");
    gr->GetXaxis()->SetTitle("L");
    gr->GetYaxis()->SetTitle("Runtime (s)");

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.2);
    gr->SetMarkerColor(kBlue+1);
    gr->SetLineColor(kBlue+1);

    // Fit con una legge di potenza
    TF1 *fit =
        new TF1("runtime_fit",
                "[0]*pow(x,[1])",
                L_values.front(),
                L_values.back());

    fit->SetParameters(1e-4,2.0);

    gr->Draw("AP");

    gr->Fit(fit,"R");

    gStyle->SetOptFit(0);

    c->SaveAs("RuntimeScaling.pdf");
    c->SaveAs("RuntimeScaling.png");

    std::cout << "\nAnalisi del runtime:" << std::endl;
    std::cout << "t(L) = a * L^alpha" << std::endl;
    std::cout << "a = "
              << fit->GetParameter(0)
              << std::endl;

    std::cout << "alpha = "
              << fit->GetParameter(1)
              << std::endl;

    delete fit;
    delete gr;
    delete c;
}

};

//ciao funzionano questi cambi?
//ciao ciao luca
// come va?

#endif