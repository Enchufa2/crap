# onoff

This is an `ath9k` power-save tester, a small module to be compiled within the
`ath9k` source tree and to play with the sleep state.

- put `onoff.*` under `ath9k/`
- include `onoff.h` in `ath9k/init.c`
- call `onoff_register_ah()` at the end of `ath9k_init_device()`
- compile, install & have fun!
