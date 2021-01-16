#Consuela

Progetto di Reti: implementazione di un server FTP in modalità passiva
  - Server.c gestisce la parte di socketing a lato server
  - Client.c gestisce la parte a lato server invece
  - modules.h è il modulo che contiene tutte le variabili globali e i prototipi dei vari metodi usati in modules.c
  - modules.c c'è l'intera implementazione delle varie procedure in particolare le funzioni get e put che definiscono l'essenza di ftp

Particolare bug: dopo aver fatto una put, il buffer di input interpreta anche il carriege return come comando => bug non ancora risolto
