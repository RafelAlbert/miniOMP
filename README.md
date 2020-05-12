# Programació i Arquitectures Paral·leles (PAP)

Implementació d'un runtime simple d'OpenMP de PAP:
* __Parallel__: fonamentalment en els fitxers [parallel.c](https://github.com/RafelAlbert/miniOMP/blob/master/parallel.c) i [parallel.h](https://github.com/RafelAlbert/miniOMP/blob/master/parallel.h).
* __Critical__: bàsicament en el fitxer [synchronization.c](https://github.com/RafelAlbert/miniOMP/blob/master/synchronization.c).
* __Barrier__ (simple): essencialment en el fitxer [synchronization.c](https://github.com/RafelAlbert/miniOMP/blob/master/synchronization.c).
* __Single__: principalment en els fitxers [single.c](https://github.com/RafelAlbert/miniOMP/blob/master/single.c) i [single.h](https://github.com/RafelAlbert/miniOMP/blob/master/single.h).
* __For__ (amb __wait__ i __nowait__): primordialment en els fitxers [loop.c](https://github.com/RafelAlbert/miniOMP/blob/master/loop.c) i [loop.h](https://github.com/RafelAlbert/miniOMP/blob/master/loop.h).
* __Barrier__ (amb operacions atòmiques intrínsiques): elementalment en el fitxer [synchronization.c](https://github.com/RafelAlbert/miniOMP/blob/master/synchronization.c).

Document de l'exposició feta del treball: [Document](https://github.com/RafelAlbert/miniOMP/blob/master/expo.pdf).
