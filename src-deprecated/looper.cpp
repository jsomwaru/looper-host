/** @file simple_client.c
 *
 * @brief This simple client demonstrates the basic features of JACK
 * as they would be used by many applications.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <vector>
#include <jack/jack.h> 

#include "looper.hpp"

using std::vector;

jack_client_t *jc;
vector<jack_port_t*> output_ports(2, nullptr);

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)


static bool RECORDING = false;



static void signal_handler(int sig)
{
	jack_client_close(jc);
	fprintf(stderr, "signal received, exiting ...\n");
	exit(0);
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client follows a simple rule: when the JACK transport is
 * running, copy the input port to the output.  When it stops, exit.
 */

int process_file(jack_nframes_t nframes, void * arg) {
	jack_default_audio_sample_t *out; 
	out = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(output_ports[0], nframes));
    //out1 = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(output_ports[1], nframes));
	auto *data = static_cast<vector<vector<float>>*>(arg);
    vector<float> data1 = (*data)[0]; 
    for(unsigned int i = 0; i < nframes; ++i) {
		out[i] = (jack_default_audio_sample_t)(data1[i]);
	}
	return 0;
	
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown (void *arg) { 
	exit (1);
}


static void start(int signal) {
    if(RECORDING == true) {
        RECORDING = false
    }
    
}

int looper_main (json buffer) {
    const char **ports;
    const char *client_name = "looper";
	const char *server_name = NULL;
    jack_options_t options = JackNullOption;
	jack_status_t status;
    auto left = buffer["left"].get<vector<float>>();
    auto right = buffer["right"].get<vector<float>>();
    vector<vector<float>> lr_void;
    
    lr_void.push_back(left);
    lr_void.push_back(right);

	/* open a client connection to the JACK server */

	jc = jack_client_open(client_name, options, &status, server_name);
	if (jc == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(jc);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	jack_set_process_callback (jc, process_file, &lr_void);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown(jc, jack_shutdown, 0);

	/* create two ports */

	for(unsigned int i = 0; i < output_ports.size(); ++i) {
		// looper_out_
		char name[13];
		sprintf(name, "looper_out_%u", (i+1));
		auto *tmp_port = jack_port_register (jc, name,
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);
        if(tmp_port == NULL) {
            fprintf(stderr, "output port not available");
            exit(1);
        }
        output_ports[i] = tmp_port;
    }


	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (jc)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */
 	
	ports = jack_get_ports (jc, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}
    for (unsigned int i =  0; i < output_ports.size(); ++i) {
	    if (jack_connect(jc, jack_port_name(output_ports[i]), ports[i])) {
		    fprintf (stderr, "cannot connect output ports\n");
	    }
    }

	jack_free (ports);
    
    /* install a signal handler to properly quits jack client */
#ifdef WIN32
	signal(SIGINT, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGTERM, signal_handler);
#else
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, start);
	signal(SIGINT, signal_handler);
#endif

	/* keep running until the Ctrl+C */

	while (1) {
	#ifdef WIN32 
		Sleep(1000);
	#else
		sleep (1);
	#endif
	}

	jack_client_close (jc);
	exit (0);
}
