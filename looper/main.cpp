#include <jack/jack.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include "graphics.hpp"
#include "channel.hpp"

#define DEFAULT_CHANNELS 4

using std::vector;
using std::thread;

static bool recording = false;

static vector<float> channel;

struct termios old_tio;

jack_port_t *output;
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
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
    cleanup_display();
    exit(0);
}

void configure_terminal() {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;
    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    setup_display();
}

int get_user_input(ChannelRack &master) {
    while (1) {
        display_recording(master.get_active_channel().recording);
        unsigned char c = getchar();
        if (c == ' ') {
            master.get_active_channel().recording = !master.get_active_channel().recording;
        } else if (c == 'b') {
            master.decrement_active_channel();
        } else if (c == 'n') {
            master.increment_active_channel();
        }
    }
    std::cout << "thread exit" << std::endl;
}

int process_channels(jack_nframes_t nframes, void *arg) { 
    ChannelRack *channels = static_cast<ChannelRack*>(arg);
    jack_time_t current_time = jack_get_time();
    static jack_time_t cycle_time = 0;
    float *data = (float*)jack_port_get_buffer(input, nframes);    
    int idx = channels->get_longest_channel();
    if (channels->get_active_channel().recording) {
        channels->get_active_channel().write_channel(data, nframes, cycle_time);
    } else if (idx != -1) {
        for (int i = 0; i < DEFAULT_CHANNELS; ++i) {
            if ((*channels)[i].recorded) {
                (*channels)[i].copy_to_output(nframes, cycle_time);
            }
        }
        if (cycle_time >= (*channels)[idx].get_total_frame_count()) 
            cycle_time = 0;
    }
    float *buffer =  (float*)jack_port_get_buffer(live_output, nframes);
    std::copy(data, data+nframes, buffer);
    return 0;
}

int process(jack_nframes_t nframes, void *arg) {
    vector<Channel> *channels = static_cast<vector<Channel>*>(arg);
    jack_time_t current_time = jack_get_time();
    static jack_time_t cycle_time = 0;
    float *data = (float*)jack_port_get_buffer(input, nframes);
    if (recording) {
        channel.insert(channel.end(), data, data+nframes);
    } else if (channel.size() >= nframes) {
        if (cycle_time >= channel.size())
            cycle_time = 0;
        float *buffer = (float*)jack_port_get_buffer(output, nframes);
        auto channel_sample_start = channel.begin()+(unsigned int)cycle_time;
        std::copy(channel_sample_start, channel_sample_start+nframes, buffer);
        cycle_time += nframes; 
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

    client = jack_client_open(client_name, options,&status, server_name);
    input = jack_port_register(
        client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    output = jack_port_register(
        client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    live_output = jack_port_register(
        client, "live_output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    vector<Channel> channel_rack(DEFAULT_CHANNELS, Channel(client));

    ChannelRack master = ChannelRack(client, channel_rack);

    if(jack_set_process_callback(client, process_channels, &master))
        panic("Could not set callback");
    
    int active = jack_activate(client);
    if (active != 0)
        panic("Can not activate client");

    thread io_thread(get_user_input, std::ref(master));
    configure_terminal();

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

    if(jack_connect(client, jack_port_name(output), playback_ports[0]))  {
        std::cout << playback_ports[0] << std::endl;
        panic("Could not connect output port");
    }
    
    if(jack_connect(client, jack_port_name(live_output), playback_ports[0]))  {
        std::cout << playback_ports[0] << std::endl;
        panic("Could not connect output port");
    }

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
    jack_free(input);
    jack_free(capture_ports);
    jack_free(playback_ports);
    return 0;
}
