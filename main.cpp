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
#define BUFSIZE 1024
int main(int argc, char *argv[]) {
    /* The Sample format to use */
    static const pa_sample_spec ss = {PA_SAMPLE_S16LE, 44100, 2};
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* replace STDIN with the specified file if needed */
    if (argc > 1) {
        int fd;
        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            std::cerr << "failed to open " << argv[1]
                      << ", error: " << strerror(errno) << "\n";
            goto finish;
        }
        if (dup2(fd, STDIN_FILENO) < 0) {
            std::cerr << "failed to duplicate stream, error: "
                      << strerror(errno) << "\n";
            goto finish;
        }
        close(fd);
    }
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback",
                            &ss, NULL, NULL, &error))) {
        std::cerr << "pa_simple_new failed: " << pa_strerror(error) << "\n";
        goto finish;
    }
    for (;;) {
        uint8_t buf[BUFSIZE];
        ssize_t r;
#if 0
        pa_usec_t latency;
        if ((latency = pa_simple_get_latency(s, &error)) == (pa_usec_t) -1) {
            fprintf(stderr, __FILE__": pa_simple_get_latency() failed: %s\n", pa_strerror(error));
            goto finish;
        }
        fprintf(stderr, "%0.0f usec    \r", (float)latency);
#endif
        /* Read some data ... */
        if ((r = read(STDIN_FILENO, buf, sizeof(buf))) <= 0) {
            if (r == 0) /* EOF */
                break;
            std::cerr << "read() from stdin failed: " << strerror(errno)
                      << '\n';
            goto finish;
        }
        /* ... and play it */
        if (pa_simple_write(s, buf, (size_t)r, &error) < 0) {
            std::cerr << "pa_simple_write() failed, error: "
                      << pa_strerror(error) << '\n';
            goto finish;
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
    return ret;
}
