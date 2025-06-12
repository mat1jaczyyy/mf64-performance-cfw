#include "key.h"
#include "led.h"

void app_init(void) {
	led_setup();
	//fastrgb_clear();
	key_setup();
	
	_delay_ms(20);
	
	key_update();
	
	// Check if we need to reboot to bootloader now
	if ((key_state & 0x01)) {
		led_set_state_dfu(); // bad LED code can soft brick?
		Jump_To_Bootloader();
		while (1);
	}
	
	//eeprom_setup();
	//midi_setup();
	//config_setup();
}

#define USB_RX_FAIL_LIMIT 120
#define USB_RX_PACKET_LIMIT 512

void app_task(void) {
	// Midifighter is not completely enumerated by the USB Host
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	//Midifighter_GetIncomingUsbMidiMessages();

	key_update();
	
	// TODO: Implement Handling Button Presses
	//{
	//	uint64_t key_bit = 0x0001;
	//	// update sleep timer (if necessary)
	//	if (key_down) {
	//		sleep_minute_counter=0;
	//	}
	//	// Service Each Individual Button
	//	for(uint8_t i=0; i<64; ++i) {
	//		if (key_down & key_bit) {
	//			// There's a key down, put a NoteOn and/or CC event into the stream.
	//			uint8_t note = midi_64_key_to_note(i);
	//
	//			if (G_EE_MIDI_OUTPUT_MODE < MIDI_OUTPUT_MODE_CCS_ONLY) {
	//				midi_stream_note(note, true);
	//			}
	//
	//			if (G_EE_MIDI_OUTPUT_MODE > MIDI_OUTPUT_MODE_NOTES_ONLY)
	//			{
	//				midi_stream_raw_cc(G_EE_MIDI_CHANNEL,note,127);
	//			}
	//		}
	//		if (key_up & key_bit) {
	//			// There's a key up, put a NoteOff event onto the stream.
	//			uint8_t note = midi_64_key_to_note(i);
	//			// Adjust channel
	//			uint8_t channel = G_EE_MIDI_CHANNEL;
	//			// Output Note Message
	//			if (G_EE_MIDI_OUTPUT_MODE < MIDI_OUTPUT_MODE_CCS_ONLY) {
	//				midi_stream_note_ch(channel, note, false);
	//			}
	//			// Output CC Message
	//			if (G_EE_MIDI_OUTPUT_MODE > MIDI_OUTPUT_MODE_NOTES_ONLY)
	//			{
	//				midi_stream_raw_cc(G_EE_MIDI_CHANNEL,note,0);
	//			}
	//		}
	//		key_bit <<= 1;
	//	}
	//}
	
	// Finished generating MIDI events, flush the endpoints. (otherwise it won't send until it's full!)
	MIDI_Device_Flush(g_midi_interface_info);

	// Handle 'Note Off Delay', since it only actually matters here
	update_note_off_feedback_delay();

	// TODO: Render display buffer
	default_display_run();

	// Send Data to the LEDs
	led_update_pixels(g_display_buffer);
}
