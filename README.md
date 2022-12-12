# Quizzettino
"Quizzettino" è un progetto di retrofit del "Quizzettone Elettronico"® con Arduino Nano.

## Introduzione
Il "Quizzettone Elettronico"®, distribuito da Giochi Preziosi, è stato un giocattolo molto diffuso negli anni '90 in quanto univa ai consueti giochi da tavolo un apparecchio che emulava le pulsantiere di molti dei più famosi quiz.

Con questo "retrofit" possiamo non solo far tornare in funzione un vecchio Quizzettone anche non più funzionante (su eBay se ne trovano ancora a pochi Euro, purché integro e con i pulsanti funzionanti) e quindi donargli una nuova vita utilizzandolo come originariamente previsto, ma lo trasformiamo in una "pulsantiera intelligente" grazie ad Arduino consentendo di collegarlo ad un Personal Computer e comandarlo tramite un semplice protocollo seriale.

E' quindi possibile implementare su PC dei giochi che siano in grado di interagire con la pulsantiera ed i relativi concorrenti.

Per ulteriori informazioni sul progetto, vedere il sito Instructables.

## Operatività

Una volta acceso o collegandolo ad un PC via USB o con un caricabatteria o persino un power bank, dopo una inizializzazione durante la quale si verifica il funzionamento dei sei LED, il Quizzettino sarà in attesa della pressione di un pulsante da parte di un concorrente non appena si accenderà il LED centrale.

Quando il concorrente più rapido premerà il proprio pulsante per prenotarsi si accenderà il corrispondente LED, verrà emesso un tono, il LED centrale si spegnerà, e nessun altro potrà prenotarsi. Quando il presentatore deve passare alla domanda successiva, premerà il pulsante di reset con il quale il Quizzettone si metterà nuovamente in attesa di un pulsante, ed il LED centrale si accenderà nuovamente.

## Protocollo seriale
Il protocollo seriale implementato è molto semplice e si compone da singoli caratteri inviati ad Arduino Nano della pulsantiera. 

Impostando la comunicazione a 115200 bps i comandi e notifiche sono i seguenti:

Caratteri inviati dal Quizzettone al PC:
- '*' Inizializzazione completata, Quizzettino pronto
- 'R' Pulsante "reset" premuto (torna in attesa di un pulsante)
- '1'-'6' Numero del pulsante premuto (e relativo LED acceso)
- '+' Premuto il tasto verde (OK)
- '-' Premuto il testo rosso (ERRORE)
- 'E' Dump dei dati di configurazione in EEPROM (vedi "Dump EEPROM")
- '#' Indica un commento o una stringa di debug, da ignorare fino al primo '\n' (ignorare '\r')

Comandi dal PC al Quizzettone:
- 'R' Reset (come pulsante)
- '1'-'6' Simula il pulsante premuto da un concorrente
- '+' Riproduci il suono del tasto verde (OK)
- '-' Riproduci il suono del tasto rosso (ERRORE)
- 'A'/'a' Attiva/disattiva l'Autoreset
- 'S'/'s' Attiva/disattiva i suoni
- '?' Mostra il contenuto della EEPROM (vedi "Configurazione")

## Configurazione
La configurazione viene salvata in EEPROM, ma solamente nel caso in cui si apportano modifiche ad almeno uno dei parametri configurabili. Segue l'elenco dei byte (con "E(x)" si intende il byte della EEPROM all'indirizzo "x"):

- E(0) contiene 1 se è presente una configurazione, o 0 se non contiene la configurazione
- E(1) contiene 0 se non è attivo l'Autoreset, 1 se è attivo
- E(2) contiene 0 se non sono attivi i suoni, 1 se sono attivi

### Dump EEPROM
Al comando '?' il Quizzettone risponde con un elenco dei valori di configurazione presenti in EEPROM. Il formato della risposta prevede una 'E' seguita da un elenco di valori esadecimali separati da ';'. 

Esempio:

`E1;1;0;`

In questo caso è presente una configurazione (il primo byte è 1), è attivo l'AutoReset, ed i suoni sono disattivati.
