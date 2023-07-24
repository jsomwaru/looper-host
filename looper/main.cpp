#include <jack/jack.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>

using std::vector;

static bool recording = false;

static vector<float> channel;

jack_port_t *output;
jack_port_t *input;
jack_port_t *live_output;
jack_client_t *client;

void panic(const char *message) {
    std::cerr << message << '\n';
    exit(1);
}

int process(jack_nframes_t nframes, void *arg) {
    jack_nframes_t start = jack_frames_since_cycle_start(client);
    jack_time_t sample_time = jack_frames_to_time(client, channel.size());
    float *data = (float*)jack_port_get_buffer(input, nframes);
    if (recording) {
        channel.insert(channel.end(), data, data+nframes);
    } else {
        float *buffer = (float*)jack_port_get_buffer(output, nframes);
        std::copy(channel.begin(), channel.end(), buffer);
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

    if(jack_set_process_callback(client, process, 0))
        panic("Could not set callback");
    
    int active = jack_activate(client);
    if (active != 0)
        panic("Can not activate client");

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

    while (1) {
        #ifdef WIN32
                Sleep(1000);
        #else
                sleep(1);
        #endif
	};

	jack_client_close(client);

    jack_free(capture_ports);
    jack_free(playback_ports);
    return 0;
}
