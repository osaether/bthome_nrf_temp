# BTHome temperature sensor with nRF52

This project implements a [BTHome](https://bthome.io/) temperature sensor using the nRF52840.

# Building

You need the [nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html) to build this sample.

Clone this repo in a folder under the samples folder in the nRF Connect SDK.
To build for the nRF52840 DK run the west build command:

```shell
west build -b nrf52840dk_nrf52840
```

To build for other devices/boards replace nrf52840dk_nrf52840 with the correct one.