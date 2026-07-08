# Modello di Ising 2D con CERN ROOT

Questa cartella contiene un'implementazione del Modello di Ising 2D su un reticolo quadrato, simulato tramite l'algoritmo Monte Carlo con algoritmo di Metropolis. Il programma utilizza il framework ROOT per calcolare e generare i grafici delle principali osservabili termodinamiche in funzione della temperatura.

## Struttura del Progetto

Il codice è suddiviso in moduli:

1. **`RandomGenerator.hpp`**: Classe per la generazione dei numeri casuali con `std::mt19937`. Implementa funzioni per generare numeri reali e interi uniformemente distribuiti in un intervallo specificato.
2. **`Lattice2D.hpp`**: Gestisce il reticolo degli spin, l'implementazione delle *Condizioni al Contorno Periodiche (PBC)* e i metodi per calcolare l'energia locale ($\Delta E$) ed estensiva, nonché la magnetizzazione.
3. **`Metropolis.hpp`**: Implementa l'algoritmo. Contiene la logica di **pre-calcolo dei 5 possibili valori del fattore di Boltzmann** ($e^{-\Delta E / T}$), che evita di dover calcolare esponenziali costosi durante i loop di sweep riducendo i tempi di esecuzione. Un singolo step corrisponde sempre a $L^2$ tentativi casuali di flip.
4. **`IsingSim.hpp`**: La classe che coordina le simulazioni. Esegue i passi di termalizzazione e poi accumula le osservabili. Usa il Teorema di Fluttuazione-Dissipazione per ricavare il *Calore Specifico* $C_v$ e la *Suscettività* $\chi$ calcolando le varianze corrette scalate su $N=L^2$.
5. **`main.cpp`**: Imposta i parametri di sweep, esplora le temperature (con step più fitti vicino a $T_c \approx 2.269$), estrae i valori e sfrutta `TGraph` di CERN ROOT per disporli in una griglia visiva (2x2). Genera i plot finali in "batch mode" (senza GUI).
6. **`Makefile`**: Permette di compilare rapidamente il codice tramite il comando `make`.

## Come compilare ed eseguire

### 1. Compilazione
Il codice richiede `g++` (con supporto C++14) e la libreria **CERN ROOT** correttamente installata e configurata.

Puoi compilare tutto semplicemente eseguendo:
```bash
make
```

In alternativa, puoi usare la riga di comando esatta dal terminale:
```bash
g++ -O3 -Wall -std=c++14 main.cpp -o ising_sim $(root-config --cflags --libs)
```
*(Il flag `-O3` è essenziale perché attiva tutte le massime ottimizzazioni del compilatore rendendo i loop del metodo Monte Carlo estremamente veloci).*

### 2. Esecuzione
Per avviare la simulazione, digita:
```bash
./ising_sim
```

### 3. Output
Al termine dell'esecuzione, il terminale mostrerà i dati calcolati passo-passo per ogni temperatura.
Inoltre, verranno generati due file immagine nella stessa cartella:
- `Ising_Simulation_Results.pdf`
- `Ising_Simulation_Results.png`

Questi conterranno i 4 grafici richiesti per:
- Magnetizzazione media assoluta per spin $\langle |M| \rangle$
- Energia media per spin $\langle E \rangle$
- Calore Specifico per spin $C_v$
- Suscettività Magnetica per spin $\chi$
