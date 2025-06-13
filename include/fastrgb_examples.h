/*
 * fastrgb_examples.h
 * 
 * Header-Datei für FastRGB Dual-LED Beispiele
 */

#ifndef _FASTRGB_EXAMPLES_H_INCLUDED
#define _FASTRGB_EXAMPLES_H_INCLUDED

#include <stdint.h>

// Beispiel-Funktionen für die erweiterte FastRGB Dual-LED Funktionalität

// Einfache separate LED-Steuerung
void example_separate_leds(void);

// Fortschrittsanzeige mit beiden LEDs (0-100%)
void example_progress_bar(uint8_t progress_percent);

// Blinkende LEDs mit unterschiedlichen Phasen
void example_alternating_blink(uint8_t phase);

// Regenbogen-Effekt mit beiden LEDs
void example_rainbow_dual(uint8_t offset);

// LED-Status auslesen und verarbeiten
void example_read_led_state(void);

// Ableton-Palette mit einzelnen LEDs
void example_ableton_dual_palette(void);

// Kompatibilitätstest - alte und neue Funktionen gemischt
void example_compatibility_test(void);

// Haupt-Demo-Funktion
// demo_mode: 0-6 für verschiedene Demo-Modi
// animation_frame: Frame-Counter für Animationen
void run_dual_led_demo(uint8_t demo_mode, uint8_t animation_frame);

#endif // _FASTRGB_EXAMPLES_H_INCLUDED
