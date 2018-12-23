#include <errno.h>
#include <fcntl.h>
#include <pulse/error.h>
#include <pulse/simple.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

extern char _binary_bell_wav_start;
extern char _binary_bell_wav_end;
int play_sound(int nLoop) {
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
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, "play_sound", PA_STREAM_PLAYBACK, NULL,
                            "playback", &ss, NULL, NULL, &error))) {
        std::cerr << "pa_simple_new failed: " << pa_strerror(error) << "\n";
        goto finish;
    } else {
        for (int i = 0; i < nLoop; i++) {
            if (pa_simple_write(s, byte_string.c_str(), audio_size, &error) <
                0) {
                std::cerr << "pa_simple_write() failed, error: "
                          << pa_strerror(error) << '\n';
                goto finish;
            }
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

int main() {
    auto status = play_sound(3);
    if (status) {
        std::cerr << "failed to play sound.\n";
    }
}
