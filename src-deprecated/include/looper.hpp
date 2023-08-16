#ifndef LOOPER_HPP
#define LOOPER_HPP

#include <jack/jack.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int looper_main(json);
int process_file(jack_nframes_t, void*);
void jack_shutdown (void *arg);

#endif