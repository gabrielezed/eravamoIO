# eravamoIO

__ Eravamo IO, Nicotina Nick, Grimy Joe e Megan Chill. __

eravamoIO (eravamoInputOutput) è un modulo backend scritto in C che sposta sul livello enterprise un inside joke dei Nivra. 
La modalità di input permette di salvare un nuovo nome su un database SQLite. 
La modalità di output permette di generare una frase del tipo "Eravamo IO, NomeX, NomeY e NomeZ" pescando dei nomi casuali dal database SQLite.
Il caso d'uso è integrarlo a un front-end web per generare i meme e renderli accessibili come fatto su [quà ci sarà un link del risultato finale]

Perché in C?
Perché il C è velocissimo e permette di iterare e generare molto più velocemente di un linguaggio di scripting come Python. 
 Non è vero, è perché sto studiando C e fare queste stupidaggini mi aiuta.
 
## Dependencies
This project uses [SQLite](https://www.sqlite.org/index.html) for data storage. 
The database engine is integrated using the [amalgamation](https://www.sqlite.org/amalgamation.html) 
source files (`sqlite3.c` and `sqlite3.h`), which are included directly in the repository.
