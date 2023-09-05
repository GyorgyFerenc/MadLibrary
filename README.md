# MadLibrary

It is a header based library

## Layout
<!-- 
```
┌
├── include
│   └── Variable.hpp
``` -->

```
┌── documentation // The documentation of everything
├── example // Set of examples using the library
├── experimental // The location of experimental modules 
│   ├── ExperimentalModule1 // The location of an experimental module
│   ... // rest of experimental modules
│
└── include // The location of the includes
    ├── Log // The location of the logging module
    ... // rest of the modules
```

## Versioning

__Curent version:__ V0.0.1.

The version format is V1.2.3 where 1 is the __major__ version, 2 is the __minor__ version, 3 is the __patch__ version.
- __Major__ version: Signals backward-incompatible public API changes. This release carries no guarantee that it will be backward compatible with preceding __major__ versions.
- __Minor__ version: Signals backward-compatible public API changes. This release guarantees backward compatibility and stability.
- __Patch__ version: Signals changes that don't affect the module's public API or its dependencies. This release guarantees backward compatibility and stability 

## License

The curent version (V0.x.x) is licensed under the Creative Commons Attribution 4.0 International License. 
To view a copy of this license, visit [this link](https://creativecommons.org/licenses/by/4.0/.), here is the [legal code](https://creativecommons.org/licenses/by/4.0/legalcode).

## Developer documentation

Developer documentation is [here](documentation/Developer.md)
