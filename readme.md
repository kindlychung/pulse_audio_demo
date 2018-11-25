# Example of playing an audio file using libpulse

## Branches:

* `master`: read audio file and play it.
* `use_data_obj`: the same audio file is converted to a ELF object file, and thus embeded into the binary executable.

## Build & Run:

On the master branch:

`./make.sh && ./a.out ~/bell.wav`

On the use_data_obj branch:

`./make.sh && ./a.out`