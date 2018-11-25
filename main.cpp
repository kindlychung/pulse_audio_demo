#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <pulse/error.h>
#include <pulse/simple.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#define BUFSIZE 1024
extern char _binary_bell_wav_start;
extern char _binary_bell_wav_end;

int main(int argc, char *argv[]) {
    char *p = &_binary_bell_wav_start;
    std::stringstream byte_stream;
    while (p != &_binary_bell_wav_end) {
        byte_stream << *p++;
    }
    auto byte_string = byte_stream.str();
    size_t audio_size = byte_string.length();
    /* The Sample format to use */
    static const pa_sample_spec ss = {PA_SAMPLE_S16LE, 44100, 2};
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* replace STDIN with the specified file if needed */
    int fd;
    if (argc > 1) {
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            std::cerr << "failed to open " << argv[1]
                      << ", error: " << strerror(errno) << "\n";
            goto finish;
        }
    }
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback",
                            &ss, NULL, NULL, &error))) {
        std::cerr << "pa_simple_new failed: " << pa_strerror(error) << "\n";
        goto finish;
    } else {
        if (pa_simple_write(s, byte_string.c_str(), audio_size, &error) < 0) {
            std::cerr << "pa_simple_write() failed, error: "
                      << pa_strerror(error) << '\n';
            return 1;
        }
    }
    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        std::cerr << "pa_simple_drain() failed, error: " << pa_strerror(error)
                  << '\n';
        goto finish;
    }
    ret = 0;
finish:
    if (s) pa_simple_free(s);
    if (fd > 0) close(fd);
    return ret;
}
