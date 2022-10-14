# stmlibs

A set of basic embedded libraries widely used in most of the projects. Just include this repository as a submodule and, if you are not a Squadra Corse member, offer us some beer!

## fsm

### fsmgen

Command line utility to generate strongly typed configuration files and stub
methods starting from a [DOT](https://en.wikipedia.org/wiki/DOT_(graph_description_language)) file.

You can install the tool using:

```
pip install fsm/fsmgen
```

Then create a dot file ([example](fsm/fsmgen/example.dot)) and generated code
for the state machine named "test" using:

```
fsmgen test example.dot
```

Two files, `test.c` and `test.h`, will be generated in you current working
directory. They both depend on the base implementation of fsm in this library.
