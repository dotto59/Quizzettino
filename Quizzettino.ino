/*
Name:		 QUIZZETTINO
Created: 09/12/2022
Author:	 Alex Palmese
Contact: alex.palmese@gmail.com
Copyright (C) 2022 Alex Palmese
Website:	https://github.com/dotto59/Quizzettino/

Version 1.0.0

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

// ***************************************************
// CONFIGURAZIONE
// ***************************************************
// Pin di led e pulsanti
//                     VI BL VE GI AR RO
const byte P_LED[6] = { 7, 6, 5, 4, 3, 2};
const byte P_PUL[6] = {A5,A4,A3,A2,A1,A0};
const byte P_RESET = 8; // Pulsante di reset
const byte P_READY = 9; // Led che indica che è pronto a ricevere
const byte P_NO = 10;   // Pulsante rosso
const byte P_SI = 11;   // Pulsante verde
const byte P_SPEAKER =12; // Beeper

// Tempo per l'auto reset
const unsigned long AUTO_RESET = 4000;

// ***************************************************
// SUONI
// ***************************************************
#include "pitches.h"
// Suono all'accensione
const byte START_NOTE = 16;
int StartNote[] = { 
    NOTE_E6, NOTE_E6, 0, NOTE_E6, 
    0, NOTE_C6, NOTE_E6, 0,
    NOTE_G6, 0, 0, 0,
    NOTE_G5, 0, 0, 0
};
int StartTempo[] = { 
    12, 12, 12, 12, 
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12
};
// Sono pulsante
const byte PULS_NOTES = 2;
int PulsNote[] = {
  NOTE_D5, NOTE_E5
};
int PulsTempo[] = {
  12, 6
};
// Suono (Hz) e durata (ms) dei tasti SI e NO
const unsigned int SI_NOTE = 740;
const unsigned int NO_NOTE = 120;
const unsigned long BEEP_DURATION = 1000;
// ***************************************************
// Gestione configurazione in EEPROM
// ***************************************************
#include <EEPROM.h>
// Indirizzi dei campi della EEPROM
const int E_SET = 0;        // Se la configurazione è stata salvata vale 1
const int E_AUTORESET = 1;  // bAutoReset
const int E_SUONI = 2;      // bSuoni

const int E_MAX = 2;        // Indirizzo massimo della zona di configurazione
// ***************************************************

// Auto reset (default = NO)
bool bAutoReset = false;
// Indica se emettere i suoni (default = SI)
bool bSuoni = true;

unsigned long tmrAutoReset = 0;
// ID pulsante acceso (1-6, 0=nessuno)
byte idAcceso = 0;
// Indica se è attivo un pulsante
bool bPremuto = false;

void setup()
{
  Serial.begin(115200);
  pinMode(P_RESET, INPUT_PULLUP);
  pinMode(P_NO, INPUT_PULLUP);
  pinMode(P_SI, INPUT_PULLUP);
  pinMode(P_READY, OUTPUT);
  digitalWrite(P_READY, LOW);
  pinMode(P_SPEAKER, OUTPUT);
  for (int i=0; i<6; ++i) 
  {
  	pinMode(P_LED[i], OUTPUT);
    pinMode(P_PUL[i], INPUT_PULLUP);
    digitalWrite(P_LED[i],HIGH);
    delay(200);
    digitalWrite(P_LED[i],LOW);
    delay(100);
  }

  if (EEPROM.read(E_SET) == 255) EEPROM.write(E_SET, 0);
  
  // Leggo configurazione, se presente
  if (EEPROM.read(E_SET) == 1) {
    bAutoReset = (EEPROM.read(E_AUTORESET) == 1);
    bSuoni = (EEPROM.read(E_SUONI) == 1);
  }
    
  // Melodia all'accensione
  play(StartNote, START_NOTE, StartTempo);

  digitalWrite(P_READY, HIGH);
  Serial.println("*"); // Conferma attivazione
}

void WriteConfig() {
  if (EEPROM.read(E_SET) == 0 || EEPROM.read(E_SET) == 255) EEPROM.write(E_SET, 1);
  EEPROM.write(E_AUTORESET, bAutoReset ? 1: 0);
  EEPROM.write(E_SUONI, bSuoni ? 1: 0);
}

void EDump() {
  Serial.print("E");
  for (int a=0; a <= E_MAX; ++a) {
    //Serial.print(a); Serial.print("="); 
    Serial.print(EEPROM.read(a), HEX); Serial.print(";");
  }
  Serial.println();
}

void loop()
{
  // Flag per indicare l'azione richiesta (da pulsanti o da protocollo seriale)
  bool bClear = false; // Reset
  bool bSI = false;    // Pulsante SI (verde)
  bool bNO = false;    // Pulsante NO (rosso)
  if (Serial.available()) {
    char c = Serial.read();
    if (c != '\n' && c != '\r') {
      switch (c) {
        case 'R': // Comando reset
          bClear = true;
          break;
        case '1' ... '6':
          // Se non è già acceso un led accetto la richiesta
          if (idAcceso == 0) idAcceso = c - '0';
          break;
        case '+': // Suono SI
          bSI = true;
          break;
        case '-': // Suono NO
          bNO = true;
          break;

        // VALORI CONFIGURABILI        
        case 'A': // Abilita auto reset
          Serial.println("A");
          bAutoReset = true;
          WriteConfig();
          break;
        case 'a': // Disabilita auto reset
          Serial.println("a");
          bAutoReset = false;
          WriteConfig();
          break;
        case 'S': // Abilita suoni
          Serial.println("S");
          bSuoni = true;
          WriteConfig();
          break;
        case 's': // Disabilita suoni
          Serial.println("s");
          bSuoni = false;
          WriteConfig();
          break;
        case '?': // Dump EEPROM
          EDump();
      }
    }
  }
  // Pulsante di reset
  if (digitalRead(P_RESET) == LOW) {
    bClear = true;
    delay(50);
    while(digitalRead(P_RESET) == LOW)
      delay(50);
  }
  // Auto reset?
  if (bAutoReset && tmrAutoReset > 0 && millis()-tmrAutoReset >= AUTO_RESET) {
    bClear = true;
  }
  
  // Devo quindi resettare?
  if (bClear) {
    // Spegne tutti i led
  	for (int i=0; i<6; ++i) 
  	  digitalWrite(P_LED[i], LOW);
	  idAcceso = 0;
    bClear = false;
    bPremuto = false;
    // Conferma sonora
    if (bSuoni) tone(P_SPEAKER, NOTE_A5, 200);
    digitalWrite(P_READY, HIGH);
    Serial.println("R");
    tmrAutoReset = 0;
  }
  
  // Controllo se qualche concorrente ha premuto il pulsante
  if (!bPremuto) {
    for (int i=0; i<6; ++i) 
    {
      if (digitalRead(P_PUL[i]) == LOW || idAcceso == i+1) {
        digitalWrite(P_LED[i], HIGH);
        digitalWrite(P_READY, LOW);
        Serial.println(i+1);
        // Suono pulsante
        play(PulsNote, PULS_NOTES, PulsTempo);
        bPremuto = true;
        if (bAutoReset > 0) tmrAutoReset = millis();
        break;
      }
    }
  }
  if (digitalRead(P_SI) == LOW || bSI) {
    Serial.println("+");
    if (bSuoni) tone(P_SPEAKER, SI_NOTE, BEEP_DURATION);
    delay(BEEP_DURATION);
    bSI = false;
  }
  if (digitalRead(P_NO) == LOW || bNO) {
    Serial.println("-");
    if (bSuoni) tone(P_SPEAKER, NO_NOTE, BEEP_DURATION);
    delay(BEEP_DURATION);
    bNO = false;
  }
}

void play(int melody[], int size, int tempo[]){
  if (!bSuoni) return;
  for (int thisNote = 0; thisNote < size; thisNote++) {
    int noteDuration = 1000/tempo[thisNote];
    tone(P_SPEAKER, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    tone(P_SPEAKER, 0, noteDuration);
  }
}
