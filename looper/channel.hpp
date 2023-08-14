#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <jack/jack.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <mutex>

using std::vector;

typedef unsigned short int channel_count_t;

#define PORT_NAME_LEN 8

struct Channel { 
public:
    bool recording;
    bool recorded;
    vector<float> buffer;
    jack_port_t *output_port;
    jack_nframes_t frame_offset;

    static channel_count_t channel_count;
    static jack_port_t *input;

    inline Channel() {}
    inline Channel(jack_client_t *client) {
        char port_name[PORT_NAME_LEN];
        std::sprintf(port_name, "output%d", channel_count++);
        output_port = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        buffer = vector<float>();
        frame_offset = 0;
        recording = false;
        recorded = false;
    }
    
    inline void clear() { buffer.clear(); }
    
    inline void copy_to_output(jack_nframes_t nframes, jack_nframes_t cycle_time) {
        float *out = (float*)jack_port_get_buffer(output_port, nframes);
        std:copy(buffer.begin()+cycle_time , buffer.begin()+nframes+cycle_time, out);
    }
    
    inline jack_nframes_t write_channel(float *input_buffer, jack_nframes_t nframes) {
        // write sound
        buffer.insert(buffer.end(), input_buffer, input_buffer+nframes);
        return nframes;
    }

    inline jack_nframes_t get_total_frame_count() {
        return frame_offset + buffer.size();
    }

    inline bool get_recording() {
        return recording;
    }
}; 

channel_count_t Channel::channel_count = 0;

struct ChannelRack {
    vector<Channel> rack;
    jack_port_t *master_output;

    static channel_count_t active_channel;  

    inline ChannelRack(vector<Channel> &_rack) {
        rack = _rack;
    }

    inline ChannelRack(jack_client_t *client, channel_count_t num_channels) {
        for (int i = 0; i < num_channels; ++i) {
            rack.emplace_back(client);
        }
    }

    inline int get_longest_channel() {
        auto elm = std::max_element(rack.begin(), rack.end(), [](Channel&a, Channel&b) {
            return a.get_total_frame_count() < b.get_total_frame_count();
        });
        return elm->get_total_frame_count() > 0 ? std::distance(rack.begin(), elm) : -1;
    }

    inline void schedule() {
        int idx = get_longest_channel();
        jack_nframes_t max_length = rack[idx].get_total_frame_count();
        for (int i = 0; i < rack.size(); ++i) {
            
        }
    }

    inline Channel& operator[](int index) {
        if (index < 0 || index > rack.size()) 
            throw std::out_of_range ("Indexed ChannelRack out of range");
        return rack[index];
    }

    inline Channel& get_active_channel() {
        return rack[ChannelRack::active_channel];
    }

    inline void decrement_active_channel() {
        if (active_channel != 0) 
            --active_channel;
    }

    inline void increment_active_channel() {
        if (active_channel < rack.size()-1) 
            ++active_channel;
    }

    inline void connect(jack_client_t *client) {
        const char **playback_ports = jack_get_ports(
            client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
        for (auto &channel : rack) {
            int ret = jack_connect(client, jack_port_name(channel.output_port), playback_ports[0]);
            if (ret != 0) {
                std::cerr << "Could not connect to output port\n";
            }
            std::cerr << "Could connect to output port\n";
        }
        jack_free(playback_ports);
    }

};

channel_count_t ChannelRack::active_channel = 0;

#endif
