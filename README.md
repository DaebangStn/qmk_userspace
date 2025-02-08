# QMK Userspace

My QMK keymap repository.

## Build

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`
1. Build with the following command:
`qmk userspace-compile` (for all targets) or `qmk flash -kb (keyboard) -km (keymap)` (build and flash specific target)
1. To create layout.json, run 
```
qmk c2json --no-cpp keyboards/crkbd/rev4_1/standard/keymaps/daebangstn/keymap.c > keyboard.json
```
you can leverage qmk [configurator](https://config.qmk.fm/) to create visual keymap

## Available keymaps (qmk.json)
1. crkbd/rev4_1/standard
- `daebangstn`: Default keymap

## Development


see [Configure VS code for QMK](https://docs.qmk.fm/other_vscode#configuring-vs-code)
