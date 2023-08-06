#include <jack/jack.h>
#include <string.h>
#include <vector>

using std::vector;

struct Channel { 
public:
    bool recording;
    bool recorded;
    vector<float> buffer;
    jack_port_t *output_port;
    jack_nframes_t frame_offset;

    static jack_port_t *input;

    inline Channel() {}
    inline Channel(jack_port_t *_output_port) {
        output_port = _output_port;
    }
    ~Channel() {
        jack_free(output_port);
    }
    Channel(const Channel& rhs) {
        this->recording = rhs.recording;
        this->recorded = rhs.recorded;
        this->frame_offset = rhs.frame_offset;
        memcpy(this->output_port, rhs.output_port, sizeof(rhs.output_port));
    }
    Channel& operator=(Channel rhs) {
        if (this != &rhs) {
            this->recording = rhs.recording;
            this->recorded = rhs.recorded;
            this->frame_offset = rhs.frame_offset;
            memcpy(this->output_port, rhs.output_port, sizeof(rhs.output_port));
        }
        return *this;
    }
    inline void clear() { buffer.clear(); }
    inline void copy_to_output(float *arg, jack_nframes_t nframes, jack_nframes_t cycle_time) {
        float *phys_output = (float*)jack_port_get_buffer(output_port, nframes);
        std:copy(buffer.begin()+cycle_time , buffer.begin()+nframes+cycle_time, phys_output);
    }
    inline jack_nframes_t write_channel(float *input_buffer, jack_nframes_t nframes, jack_nframes_t offset) {
        
    }
}; 
