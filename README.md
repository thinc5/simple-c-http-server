# simple-c-http-server
A simple http server written in C to consume http requests and do things if those requests meet criteria specified in `filter.h`

## Rationale
Can I build a small binary that largely follows the [suckless philosophy](https://suckless.org/philosophy/) which can handle my limited continuous integration needs?

## Building

### Requirements
- C compiler
- Make (unless you want to use something else)

To build run `make`

## Configuration

All configuration takes place in `config.h`, `filter.h` and `actions.h`

You can use the existing filters and actions as templates, and modify the rest of the code to suit your needs.

## To do
- Multi-threading?
- More complex example actions
