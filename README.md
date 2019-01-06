# HOMEWORK - Winter 2018

[Per le istruzioni di compilazione vedere la fine del presente file]

## Ambito del problema
Programmazione in linguaggio C nell'ambito della concorrenza, su piattaforma Linux.

## Descrizione del problema
Lo scopo del problema è fornire uno strumento di controllo e visualizzazione (nella realtà virtuale, i.e. terminale) di uno strumento riabilitativo costituito da un carrello che viene mosso su una rotaia da un paziente. Nella semplificazione dell'assegnemento, l'input è simulato tramite un file (di default `device.txt`) in cui, per ogni riga, figurano tempo dell'acquisizione e incremento spaziale della posizione del carrello. Sulla base del pattern _Model, View and Controller_ noto nell'ingegneria del software, dapprima tali dati devono essere letti tramite un'__Interfaccia__ (`Interface`) che li metta, quindi, a disposizione del __Modello__ (`Model`) in un buffer `DeviceInput`. Da tali dati "grezzi", il `Model` ricostruisce la posizione del carrello tramite un'apposita logica di controllo. Per ogni istante temporale interessato, viene di seguito riempito un buffer `DevicePosition` da cui il __Visualizzatore__ (`Viewer`) e il __Controllore__ (`Controller`) possono estrarre i valori per renderli, rispettivamente, visualizzabili a video e fornirli in uscita, in questo caso, in un file `output.txt`.
Sostanzialmente, si ha: un problema "produttore - consumatore" semplice su `DeviceInput` e un "produttore - 2 consumatori" su `DevicePosition`.

## Vincoli di I/O
### Riga di comando
Il formato della riga di input deve essere il seguente:
`./haptic [posMin] [posMax] [ctrlPer] [viewPer]`,
dove:
* `posMin` è la posizione del muro di sinistra
* `posMax` è la posizione del muro di destra
* `ctrlPer` è il multiplo dell'unità di tempo a cui viene ricorrentemente svegliato il Controller
* `viewPer` è il multiplo dell'unità di tempo a cui viene ricorrentemente svegliato il Viewer

### File di input
Il formato dei dati deve essere:
`[t_i]   [incr_i]`
per ogni riga.

### File di output
Il formato dei risultati sarà incolonnato sotto `Time  |  Position` in maniera analoga all'input.

## Strategia di risoluzione
Per risolvere questo problema ho scelto di utilizzare 6 thread in esecuzione concorrente:
* `Main`, che esegue la funzione `main()` (vedere file `main.c`): si occupa di ricevere l'input dalla riga di comando e di inizializzare, gestire e terminare mutex, condition variables e le altre thread sorelle. Installa una maschera di segnale completa che farà ereditare a tutte le altre thread che genera;
* `Timing`, che esegue la funzione `timing()` (vedere file `timing.c` e `timing.h`): scandisce la temporizzazione (tramite __timer__) e gestisce i segnali (di interesse) in arrivo, essendosi liberata della maschera ereditata da `Main` e avendone installata un'altra permeabile a tutti i segnali;
* `Interface`, che esegue la funzione `interface()` (vedere file `interface.c` e `interface.h`): legge il file di input e fa da produttore per il buffer `DeviceInput`;
* `Model`, che esegue la funzione `model()` (vedere file `model.c` e `model.h`): contiene la logica del controllo di posizione, fa da consumatore per il buffer `DeviceInput` e da produttore per `DevicePosition`;
* `Controller`, che esegue la funzione `controller()` (vedere file `controller.c` e `controller.h`): permette di dare in output (in questo caso, stampare su file) i _setpoint_ da dare come riferimento al PLC che attua il carrello (consumatore per `DevicePosition');
* `Viewer`, che esegue la funzione `viewer()` (vedere file `viewer.c` e `viewer.h`): permette la visualizzazione in veste grafica del progresso spazio-temporale del carrello sulla guida (consumatore per `DevicePosition'), tramite la collocazione di una `X` su una "barra" di caratteri su terminale (`bash`) di lunghezza fissa e settabile nel file `config.h`.

Da notare che, esaurito il file di input, il programma non è pensato per terminare, bensì resterà in attesa del segnale dell'utente per terminare in modo "sicuro" (_graceful degradation_, `ctrl + C`), oppure "brutale" (`ctrl + \`).

### Ulteriori specificazioni
La thread `Timing` è l'unica la cui maschera dei segnali non sia piena e l'unica per cui sono stati installati gli handler per i segnali `SIGUSR1` (utilizzato per la temporizzazione), `SIGINT` (utilizzato per la _graceful degradation_) e `SIGQUIT` (per emergenza e debugging).
Tale thread, a ogni unità di tempo (scadenza del timer), sveglia (tramite `pthread_cond_signal()`) `Interface` che produce un dato, la quale a sua volta sveglia (allo stesso modo) `Model`.
A multipli dell'unità di tempo decisi dall'utente, la thread `Timing` sveglia anche `Viewer` e `Controller` (sempre con `pthread_cond_signal()`).

La scelta di "svegliare" le thread tramite dei _signalC_ su delle variabili condizione dedicate è stata fatta per evitare _busy waiting_ e, quindi, aumentare l'efficienza del programma.

## Osservazioni
* L'aver implementato i buffer `DeviceInput` e `DevicePosition` con delle liste concatenate ha permesso una gestione rapida e snella delle sezioni critiche della concorrenza (lettura/scrittura sui buffer), dato che non ci sono i problemi di blocchi e sblocchi di mutex sugli array circolari. In lettura, specialmente, una volta salvato l'indirizzo del nodo, si possono eseguire tutte le altre operazioni in sezione non critica, basta "catturare" in maniera protetta la posizione dell'ultimo nodo aggiunto e poi procedere a eventuali ricerche tramite la libreria ad hoc implementata in `list.c` e `list.h`.
* Per evitare che la memoria occupata dalle liste sia troppo grande, è stato affidato al `Model` il compito di _pulire_ il buffer `DeviceInput` ad ogni unità di tempo e al `Controller` il compito di _pulire_ il `DevicePosition` al termine di ogni suo ciclo di esecuzione, cancellando a ritroso dalla lista gli elementi più vetusti a partire dal minor ultimo tempo letto tra quello del `Viewer` e del `Controller` stesso.
* Si suppone che il tempo sia _non negativo strettamente crescente_. L'implementazione della scansione temporale è ottenuta con un `timer` che a ogni unità di tempo che scandisce sveglia una o più delle altre thread (nel codice è stata predisposta la presenza di un array di timer in realtà, ignorare tale fatto, dal momento che ne viene usato solo uno);
* Essendo la barra di visualizzazione discreta, alla posizione che corrisponde al suo "100%" (ossia il limite superiore), la matematica imporrebbe il posizionamento della `X` sul carattere successivo a quella che è la sua effettiva lunghezza, uscendo così dalla visualizzazione: a ciò si è rimediato con un artificio che prevede, in tal caso, la visualizzazione della `X` sull'ultimo carattere disponibile, accompagnata però dalla corretta indicazione della posizione al limite superiore.

## Test effettuati
Alcuni esempi di test effettuati:
* dal momento che il programma è stato eseguito su GNOME Terminal, la visualizzazione dei colori del Viewer risulta ivi corretta, ma non posso garantirlo su altri terminali.
* __Valgrind__: `valgrind --leak-check=yes -v --track-origins=yes ./haptic -30 20 3 5`. Non ci sono memory leak, ma alcuni "problemi" di mancate inizializzazioni, che non sembrano compromettere il funzionamento del programma.
* Sono stati provati diversi file di input (vedere la cartelle `input`):
- `device.txt`, fornito assieme alla consegna, per testare il funzionamento di massima del sistema;
- `device1.txt`, creato più lungo per controllare che non vi fossero dei _miss_ del `Viewer` o del `Controller`, oppure delle cancellazioni precoci di dati;
- `device2.txt`, per testare la visualizzazione delle posizioni limite.

## Istruzioni per la compilazione
Una volta estratto l'archivio in un'opportuna cartella, procedere come segue:
* aprire un terminale (testato su GNOME Terminal) e posizionarsi nella cartella scelta tramite la navigazione col comando `cd`
* entrare nella cartella sorgente: `cd HOMEWORK_Winter_2018/src/`
* compilare con: `make` (o `make debug_haptic` se si intende utilizzare un programma di debugging, e.g. `gdb`)
* eseguire con la sintassi: `./haptic [posMin] [posMax] [ctrlPer] [viewPer]`
* Su iniziativa personale, ho fatto in modo che un file di input diverso da quello di default (`device.txt`), potesse essere letto senza ricompilare il sorgente nel seguente modo: `./haptic [posMin] [posMax] [ctrlPer] [viewPer] --file [fileName]`, dove con `fileName` si intende il percorso relativo (dalla cartella attuale a quella dove si trova il file desiderato), e.g. `../input/device1.txt`;
* Per terminare l'esecuzione del programma, premere `ctrl + C` per attivare la graceful degradation, oppure `ctrl + \` per uscire immediatamente.

Entrando nel file `config.h` ed eventualmente togliendo i commenti alla macro `DEBUG` e ricompilando il programma, l'esecuzione genererà a video indicazioni utili sullo svolgimento delle operazioni. Nel medesimo file sono impostabili anche altre opzioni, quale la __granularità temporale__ tramite una risoluzione in secondi e in nanosecondi, per esempio.
