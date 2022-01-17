#ifndef LOGGING_HPP
#define LOGGING_HPP
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#define LogInfo(msg) BOOST_LOG_TRIVIAL(info) << msg
#define LogDebug(msg) BOOST_LOG_TRIVIAL(debug) << msg

void init() {
    logging::add_file_log("looper_host.log");
}

#endif
