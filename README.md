# CHIP-8

[![Build](https://github.com/FrederikTobner/CHIP-8/actions/workflows/build.yml/badge.svg)](https://github.com/FrederikTobner/CHIP-8/actions/workflows/build.yml)
[![Analyze](https://github.com/FrederikTobner/CHIP-8/actions/workflows/codeql.yml/badge.svg)](https://github.com/FrederikTobner/CHIP-8/actions/workflows/codeql.yml)
> **NOTE**: 🚧 Under active development. 🚧

Simple [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C, with a custom assembly language.

![Breakout game](./assets/Breakout.png)

## Table of Contents

* [Dependencies](#dependencies)
* [License](#license)

## Key concepts

1. Programming in binary is quite complex, therefor we provide an assembly language
2. The assembly language should stay close to the actual opcode's representation in memory

## Dependencies

The emulator has the following dependencies

* [SDL 2.0](https://github.com/libsdl-org/SDL) a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware.

## IDE Integration

There is a plugin for vim and neovim. Another alternative is to use my own text editor YATE that has built in language support.

[![Vim](https://github-readme-stats-frederiktobner.vercel.app/api/pin/?username=FrederikTobner&repo=chip8.vim&theme=dark)](https://github.com/FrederikTobner/chip8.vim)
[![YATE](https://github-readme-stats-frederiktobner.vercel.app/api/pin/?username=FrederikTobner&repo=YATE&theme=dark)](https://github.com/FrederikTobner/YATE)

## License

This project is licensed under the [GNU General Public License](LICENSE)
