#include <jack/jack.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include "graphics.hpp"
#include "channel.hpp"

#define DEFAULT_CHANNELS 4

using std::vector;
using std::thread;

static bool recording = false;

jack_port_t *input;
jack_port_t *live_output;
jack_client_t *client;

void panic(const char *message) {
    std::cerr << message << '\n';
    exit(1);
}

void jack_shutdown(int sig) {
    std::cout << "exiting" << std::endl;
    jack_client_close(client);
    tcsetattr(STDIN_FILENO,TCSANOW, &old_tio);
    cleanup_display();
    exit(0);
}

int process_channels(jack_nframes_t nframes, void *arg) { 
    ChannelRack *channels = static_cast<ChannelRack*>(arg);
    static jack_nframes_t cycle_time = 0;
    float *data = (float*)jack_port_get_buffer(input, nframes);    
    int idx = channels->get_longest_channel();
    Channel &active_channel = channels->get_active_channel();
    if (channels->get_active_channel().recording) {
        if (!channels->get_active_channel().recorded) {
            channels->get_active_channel().recorded = true;
            channels->get_active_channel().frame_offset = cycle_time;
        }
        channels->get_active_channel().write_channel(data, nframes);
    } else if (idx != -1) {
        channels->schedule(nframes, cycle_time);
        cycle_time += nframes;
        if (cycle_time >= (*channels)[idx].get_total_frame_count()) 
            cycle_time = 0;
    }
    float *buffer =  (float*)jack_port_get_buffer(live_output, nframes);
    std::copy(data, data+nframes, buffer);
    return 0;
}

int main() {    
    const char *client_name = "looper";
    const char *server_name = nullptr;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    client = jack_client_open(client_name, options, &status, server_name);
    input = jack_port_register(
        client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    live_output = jack_port_register(
        client, "live_output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    ChannelRack master(client, DEFAULT_CHANNELS);

    if(jack_set_process_callback(client, process_channels, &master))
        panic("Could not set callback");
    
    int active = jack_activate(client);
    if (active != 0)
        panic("Can not activate client");

    master.connect(client);
    configure_terminal();
    thread io_thread(get_user_input, std::ref(master));

    const char **capture_ports = jack_get_ports(
        client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
    const char **playback_ports = jack_get_ports(
        client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);

    if (capture_ports == NULL || playback_ports == NULL) 
        panic("Capture or Playback ports not found");

    if(jack_connect(client, capture_ports[0], jack_port_name(input))) {
        std::cout << capture_ports[0] << std::endl;
        panic("Could not connect input port");
    }

    if(jack_connect(client, jack_port_name(live_output), playback_ports[0]))  {
        std::cout << playback_ports[0] << std::endl;
        panic("Could not connect output port");
    }

    jack_free(capture_ports);
    jack_free(playback_ports);

    signal(SIGINT, jack_shutdown);
    
    while (1) {
        #ifdef WIN32
            Sleep(1000);
        #else
            sleep(1);
        #endif
	};

	jack_client_close(client);
    io_thread.join();

    return 0;
}
