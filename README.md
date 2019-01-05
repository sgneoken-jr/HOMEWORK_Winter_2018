# HOMEWORK - Winter 2018

## Ambito del problema
Programmazione in linguaggio C nell'ambito della concorrenza, su piattaforma Linux.

## Descrizione del problema
Lo scopo del problema è fornire uno strumento di controllo e visualizzazione (nella realtà virtuale, i.e. terminale) di uno strumento riabilitativo costituito da un carrello che viene mosso su una rotaia da un paziente. Nella semplificazione dell'assegnemento, l'input è simulato tramite un file (di default `device.txt`) in cui, per ogni riga, figurano tempo e incremento spaziale della posizione del carrello. Tali dati devono essere letti tramite un __Interfaccia__ (`[Interface]`) che, sulla base del  pattern _model-viewer-controller_ noto nell'ingegneria del software, metta i dati a disposizione del __Modello__ (`[Model]`) in un buffer `DeviceInput`. Da tali dati "grezzi", vienie ricostruita la posizione del carrello tramite un'apposita logica di controllo insita nel Modello. Per ogni istante temporale interessato, viene riempito un buffer `DevicePosition` da cui il __Visualizzatore__ (`Viewer`) e il __Controllore__ (`Controller`) possono estrarre i valori per renderli, rispettivamente, visualizzabili a video e elencati in un file `output.txt`.
Sostanzialmente, si ha: un problema "produttore - consumatore semplice" su `DeviceInput` e un "produttore - 2 consumatori" su `DevicePosition`.

## Vincoli di I/O
### Riga di comando
Il formato della riga di input deve essere il seguente:
`./haptic [posMin] [posMax] [ctrlPer] [viewPer]`
Su iniziativa personale, ho fatto in modo che un file di input diverso da quello di default (`device.txt`), potesse essere letto senza ricompilare il sorgente nel seguente modo:
`./haptic [posMin] [posMax] [ctrlPer] [viewPer] --file [fileName]`

### File di input
Il formato dei dati deve essere:
`[t_i]   [incr_i]`
per ogni riga.

### File di output
Il formato dei risultati sarà incolonnato sotto `Time     Position` in maniera analoga.

## Strategia di risoluzione
Per risolvere questo problema ho scelto di utilizzare 6 thread in esecuzione concorrente:
* `Main`, che esegue la funzione `main()` (vedere file `main.c`): si occupa di ricevere l'input dalla riga di comando e di inizializzare, gestire e terminare mutex, condition variables e le altre thread sorelle;
* `Timing`, che esegue la funzione `timing()` (vedere file `timing.c` e `timing.h`): scandisce la temporizzazione (tramite __timer__) e gestisce i segnali;
* `Interface`, che esegue la funzione `interface()` (vedere file `interface.c` e `interface.h`): legge il file di input e fa da produttore per il buffer `DeviceInput`;
* `Model`, che esegue la funzione `model()` (vedere file `model.c` e `model.h`): contiene la logica del controllo di posizione, fa da consumatore per il buffer `DeviceInput` e da produttore per `DevicePosition`;
* `Controller`, che esegue la funzione `controller()` (vedere file `controller.c` e `controller.h`): permette di dare in output (in questo caso, stampare su file) i _setpoint_ da dare come riferimento al PLC che attua il carrello (consumatore per `DevicePosition');
* `Viewer`, che esegue la funzione `viewer()` (vedere file `viewer.c` e `viewer.h`): permette la visualizzazione in veste grafica del progresso spazio-temporale del carrello sulla guida (consumatore per `DevicePosition').

La thread `Timing` è l'unica la cui maschera dei segnali non sia piena e l'unica per cui sono stati installati gli handler per i segnali `SIGUSR1` (utilizzato per la temporizzazione), `SIGINT` (utilizzato per la _graceful degradation_) e `SIGQUIT` (per emergenza e debugging).
Tale thread, a ogni unità di tempo (scadenza del timer), sveglia (tramite `pthread_cond_signal()`) `Interface` che produce un dato, la quale a sua volta sveglia (allo stesso modo) `Model`. A multipli dell'unità di tempo decisi dall'utente, la thread `Timing` sveglia anche `Viewer` e `Controller` (sempre con `pthread_cond_signal()`).

## Osservazioni
* L'aver implementato i buffer `DeviceInput` e `DevicePosition` con delle liste concatenate ha permesso una gestione rapida e snella delle sezioni critiche della concorrenza (lettura/scrittura sui buffer), dato che non ci sono i problemi di blocchi e sblocchi di mutex sugli array circolari. In lettura, specialmente, una volta salvato l'indirizzo del nodo, si possono eseguire tutte le altre operazioni in sezione non critica.
* Si suppone che il tempo sia non negativo strettamente crescente.
* Il compito di pulire `DevicePosition` viene affidato al controller per convenzione, ma utilizza variabili globali di ultima lettura sia del controllore che del viewer.

## Istruzioni per la compilazione
Una volta estratto l'archivio in un'opportuna cartella, procedere come segue:
* aprire un terminale e posizionarsi nella cartella scelta tramite la navigazione col comando `cd`
* entrare nella cartella sorgente: `cd HOMEWORK_Winter_2018/src/`
* compilare con: `make` (o `make debug_haptic` se si intende utilizzare un programma di debugging, e.g. `gdb`)
* eseguire con la sintassi: `./haptic [posMin] [posMax] [ctrlPer] [viewPer]` o `./haptic [posMin] [posMax] [ctrlPer] [viewPer] --file [fileName]`

Entrando nel file `config.h` e togliendo i commenti alla macro `DEBUG` e ricompilando il programma, l'esecuzione genererà a video indicazioni utili sullo svolgimento delle operazioni. Nel medesimo file sono impostabili anche altre opzioni, quali la granularità temporale, per esempio.

## Test effettuati
Alcuni esempi di test effettuati:
* __Valgrind__: `valgrind --leak-check=yes -v --track-origins=yes ./haptic -30 20 3 5`. Non ci sono memory leak, ma alcuni problemi di mancate inizializzazioni.
