/*
 * Beispiel-Implementierung für die erweiterte FastRGB Dual-LED Funktionalität
 * 
 * Dieses Beispiel zeigt verschiedene Anwendungen der neuen LED-Funktionen:
 * - Separate LED-Steuerung
 * - Dual-LED-Effekte
 * - Kompatibilitätsfunktionen
 */

#include "fastrgb.h"
#include "display.h"

// Beispiel 1: Einfache separate LED-Steuerung
void example_separate_leds(void) {
    // Button 0: LED 0 rot, LED 1 blau
    fastrgb_set_led(0, 0, 63, 0, 0);  // Erste LED rot
    fastrgb_set_led(0, 1, 0, 0, 63);  // Zweite LED blau
    
    // Button 1: LED 0 grün, LED 1 aus
    fastrgb_set_led(1, 0, 0, 63, 0);  // Erste LED grün
    fastrgb_set_led(1, 1, 0, 0, 0);   // Zweite LED aus
    
    // Button 2: Beide LEDs gleichzeitig mit dual-Funktion
    fastrgb_set_dual(2, 63, 63, 0, 0, 63, 63); // LED 0: gelb, LED 1: cyan
}

// Beispiel 2: Fortschrittsanzeige mit beiden LEDs
void example_progress_bar(uint8_t progress_percent) {
    uint8_t total_leds = NUM_BUTTONS * 2; // 64 Buttons × 2 LEDs = 128 LEDs
    uint8_t filled_leds = (progress_percent * total_leds) / 100;
    
    for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
        uint8_t led0_index = button * 2;
        uint8_t led1_index = button * 2 + 1;
        
        // LED 0 Zustand
        if (led0_index < filled_leds) {
            fastrgb_set_led(button, 0, 0, 63, 0); // Grün
        } else {
            fastrgb_set_led(button, 0, 0, 0, 0);  // Aus
        }
        
        // LED 1 Zustand
        if (led1_index < filled_leds) {
            fastrgb_set_led(button, 1, 0, 63, 0); // Grün
        } else {
            fastrgb_set_led(button, 1, 0, 0, 0);  // Aus
        }
    }
}

// Beispiel 3: Blinkende LEDs mit unterschiedlichen Phasen
void example_alternating_blink(uint8_t phase) {
    for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
        if (phase % 2 == 0) {
            // Phase 0: LED 0 an, LED 1 aus
            fastrgb_set_dual(button, 63, 0, 0, 0, 0, 0); // LED 0: rot, LED 1: aus
        } else {
            // Phase 1: LED 0 aus, LED 1 an
            fastrgb_set_dual(button, 0, 0, 0, 0, 0, 63); // LED 0: aus, LED 1: blau
        }
    }
}

// Beispiel 4: Regenbogen-Effekt mit beiden LEDs
void example_rainbow_dual(uint8_t offset) {
    for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
        // Berechne zwei verschiedene Hue-Werte
        uint8_t hue1 = (button * 4 + offset) % 256;
        uint8_t hue2 = (hue1 + 128) % 256; // 180° versetzt
        
        // Vereinfachte HSV-zu-RGB-Konvertierung für Demonstration
        uint8_t r1, g1, b1, r2, g2, b2;
        
        // LED 0 (vereinfachter Regenbogen)
        if (hue1 < 85) {
            r1 = 63 - (hue1 * 63) / 85;
            g1 = (hue1 * 63) / 85;
            b1 = 0;
        } else if (hue1 < 170) {
            r1 = 0;
            g1 = 63 - ((hue1 - 85) * 63) / 85;
            b1 = ((hue1 - 85) * 63) / 85;
        } else {
            r1 = ((hue1 - 170) * 63) / 85;
            g1 = 0;
            b1 = 63 - ((hue1 - 170) * 63) / 85;
        }
        
        // LED 1 (versetzter Regenbogen)
        if (hue2 < 85) {
            r2 = 63 - (hue2 * 63) / 85;
            g2 = (hue2 * 63) / 85;
            b2 = 0;
        } else if (hue2 < 170) {
            r2 = 0;
            g2 = 63 - ((hue2 - 85) * 63) / 85;
            b2 = ((hue2 - 85) * 63) / 85;
        } else {
            r2 = ((hue2 - 170) * 63) / 85;
            g2 = 0;
            b2 = 63 - ((hue2 - 170) * 63) / 85;
        }
        
        fastrgb_set_dual(button, r1, g1, b1, r2, g2, b2);
    }
}

// Beispiel 5: LED-Status auslesen und verarbeiten
void example_read_led_state(void) {
    for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
        // Aktuelle LED-Werte auslesen
        uint8_t led0_r = fastrgb_get_led_r(button, 0);
        uint8_t led0_g = fastrgb_get_led_g(button, 0);
        uint8_t led0_b = fastrgb_get_led_b(button, 0);
        
        uint8_t led1_r = fastrgb_get_led_r(button, 1);
        uint8_t led1_g = fastrgb_get_led_g(button, 1);
        uint8_t led1_b = fastrgb_get_led_b(button, 1);
        
        // Beispiel: Dimme alle LEDs um 50%
        fastrgb_set_dual(button, 
                        led0_r / 2, led0_g / 2, led0_b / 2,  // LED 0 gedimmt
                        led1_r / 2, led1_g / 2, led1_b / 2); // LED 1 gedimmt
    }
}

// Beispiel 6: Ableton-Palette mit einzelnen LEDs
void example_ableton_dual_palette(void) {
    for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
        // LED 0: Verwende Palette-Index basierend auf Button-Nummer
        fastrgb_ableton_single_led(button, 0, button % 128);
        
        // LED 1: Verwende komplementäre Palette-Farbe
        fastrgb_ableton_single_led(button, 1, (button + 64) % 128);
    }
}

// Beispiel 7: Kompatibilitätstest - alte und neue Funktionen gemischt
void example_compatibility_test(void) {
    // Erste Hälfte der Buttons: Verwende alte Funktionen (beide LEDs)
    for (uint8_t button = 0; button < NUM_BUTTONS / 2; button++) {
        fastrgb_set(button, 63, 0, 0); // Beide LEDs rot
    }
    
    // Zweite Hälfte: Verwende neue Funktionen (einzelne LEDs)
    for (uint8_t button = NUM_BUTTONS / 2; button < NUM_BUTTONS; button++) {
        fastrgb_set_led(button, 0, 0, 63, 0); // LED 0 grün
        fastrgb_set_led(button, 1, 0, 0, 63); // LED 1 blau
    }
}

// Haupt-Demo-Funktion
void run_dual_led_demo(uint8_t demo_mode, uint8_t animation_frame) {
    // Alle LEDs löschen
    fastrgb_clear();
    
    switch (demo_mode) {
        case 0:
            example_separate_leds();
            break;
        case 1:
            example_progress_bar(animation_frame * 2); // 0-100% über 50 Frames
            break;
        case 2:
            example_alternating_blink(animation_frame / 10); // Alle 10 Frames wechseln
            break;
        case 3:
            example_rainbow_dual(animation_frame);
            break;
        case 4:
            example_read_led_state();
            break;
        case 5:
            example_ableton_dual_palette();
            break;
        case 6:
            example_compatibility_test();
            break;
        default:
            // Standard: Alle LEDs aus
            break;
    }
    
    // Display-Buffer mit beiden LEDs aktualisieren
    fastrgb_state(g_display_buffer);
}
