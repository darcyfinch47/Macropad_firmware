## Compile and flash firmware

Example command to compile the firmware and flash the keyboard:

    qmk flash -kb macropad_firmware -km via

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Run HID client

Install dependencies for client fron the qmk-hid-display-client directory:

    npm i

Run the client:

    node .