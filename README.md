---
title: libITS
keywords: gal composite
tags: [gal]
sidebar: home_sidebar
permalink: libits.html
summary: A C++ library for efficient Symbolic Model Checking.
---

# libITS C++ library 

## What is libITS 

**libITS** is a C++ library for symbolic model-checking of various formalisms using the efficient 
symbolic encodings of **libDDD**. It defines Instantiable Transition System as a framework, 
enabling hierarchical composition of components specified in diverse formalisms and their symbolic
 manipulation.

It is able to read a variety of formalisms and encode them into efficient symbolic 
representations of sets of states and of a symbolic Successor relation. 
These elements make it highly suitable to quickly experiment new symbolic
 model-checking algorithms, while still producing competitive solutions.

libits is a C++ library that takes care of nasty I/O and low level encoding of states and transition relations for you,
 and lets you harness the power of homomorphisms and the fixpoint operator.
Based on libddd, the root class ITSModel offers the critical getNextRel(), getPredRel(), getInitialState() and getPredicate() 
  that allow to write most model checking algorithms just using Identity, fixpoint and homomorphism compositions. 
  
Automatic homomorphism rewritings will take place transparently to enable efficient algorithms such as saturation where possible. 
The its-reach, CTL and LTL sources can be used as examples of how to use and link to the libits library.

Main features include:

*   Instantiable Transition System as a framework, allow hierarchical composition of components specified in diverse formalisms.
*   Support for GAL format input which offers rich data manipulation.
*   Composite ITS types offer a great flexibility in the definition of synchronized products of transitions systems.
*   Optimized implementation taking full advantage of the features of libDDD, notably automatic saturation and hierarchy.
*   Scalar and Circular compositions express common symmetric synchronization patterns (a pool of processes, a ring topology),
    and are exploited by the tool to provide superior performance.
*   Support for Petri nets and some of their extensions (Time Petri nets, Queues, inhibitor arcs, rest arcs, self-modifying nets...)
*   Support for discrete time in models such as Time Petri nets and their compositions
*   Support for ETF format input which is produced by the tool LTSmin from diverse formalisms.
*   Support for Divine format input which is native to the tool Divine and used in BEEM models.
*   Easily extensible

LibITS tools is distributed under the terms of the GNU Public License [GPL](http://www.gnu.org/licenses/gpl.html). 

We have integrated modified versions of sources taken from other open-source projects such as Divine, VIS or LTSmin. 
See acknowledgement section below.

The GAL language complements ITS by providing a model for high-level data manipulation. It offers a rich C-like syntax and direct support for arithmetic and arrays. 
Systems can be provided using mixed formalisms, such as TPN for the control part and GAL for the data part synchronized using Composite ITS types. 
     
ITS-tools also support Divine models natively, thanks to an internal translation to GAL. 
Last but not least, ETF files produced by LTSmin from several high-level formalisms can be used as input for symbolic model-checking.

## Getting started

libITS is a library for symbolic model-checking. 

The full its-tools distribution contains three separate programs. All these tools take as input Instantiable Transition Systems. 
This general compositional semantic framework allows to define models in parts, and uses the powerful algorithms and data structures of libDDD for state space representation.

Different input formalisms and formats are supported, even within a given model, provided they match the requirements of an ITS type definition : what is a symbolic set of states ? what is your symbolic transition relation ?.

With this semantic definition of the model, its-reach tests reachability of a given state, its-ctl checks whether the initial state satisfies a given branching time CTL property, and its-ltl checks linear-time temporal LTL properties of the language (set of infinite runs) of a system.

All three tools share common options for specifying the target ITS model, then each has a specific option to specify respectively the state to reach, the CTL property, or the LTL property. 
Additional options specific to each tool allow to refine the behavior (i.e. select a specific model-checking algorithm or strategy).

The package libits includes a tool to compute reachable state space and answer reachability queries, called its-reach.

Recent and up to date definitions relating to ITS and GAL can be taken from this document [.pdf](https://media.githubusercontent.com/media/lip6/ITSTools-web/master/files/gal.pdf)

Yann's habilitation thesis also contains up to date definitions and presents an overview of this work [.pdf](https://pages.lip6.fr/Yann.Thierry-Mieg/hdr-ytm.pdf)

The [Documents](bib.md) page holds many related papers which give a formal definition to all the concepts of the library.


Although we'll be generally happy to help you if encounter issues with the code, there is no "developer guide" yet. 
oxygen documentation can however be generated giving some overview of the code. 
When looking at the source, we recommend you start from the central ITSModel referential class and at the ITS Type contract.

The distribution also includes a documentation folder doc/ in which a detailed developer documentation can be generated (requires [doxygen](www.doxygen.org)). 
A version of the technical User Documentation can thus be built using Doxygen from the source code. 
Please browse the ["its/ITSModel"](https://github.com/lip6/libITS/blob/master/its/ITSModel.hh) class and ["its/Type"](https://github.com/lip6/libITS/blob/master/its/Type.hh) for a high level API.

You can also look at the [main of its-reach](https://github.com/lip6/libITS/blob/master/bin/main.cpp#L75-L94) it's not too complex, the highlighted code deals with 
exploring K depth in BMC.    

## Obtaining LibITS

LibITS is free software distributed under the terms of Gnu Public License GPL. It is a C++ library to manipulate ITS symbolically.

libits is our model-checking library built on top of libddd and offering support specific to transition relations and their manipulation using SDD and homomorphisms. 

LibITS is packaged using gnu autotools.

You can download the current release from here: [LibITS download page](https://lip6.github.io/libITS/)

The package distribution uses GNU autotools, simply extract then in the root installation folder type.

<code>
./configure  
make
</code>

Build has been tested on Linux (32 and 64 bit), MacOS X on PPC and intel, and Cygwin/MinGW 32 and 64 environments. If you encounter any problems please mail me (mailto:ddd@lip6.fr)  

In fact, even if you don't encounter problems, we are always interested in feedback and simply knowing about our users so please mail us (mailto:ddd@lip6.fr) if you are using the library in your project.

## Obtaining the latest development version

### Prerequisites

The latest version of libITS can be obtained from GitHub at _https://github.com/lip6/libITS_. 

Note that to build from the git, you need autotools and to invoke "autoreconf -vfi" to create the "configure" script. 
Due to various dependencies, building from git requires some configuration settings. 

Remember, configure --help is your friend, and you can also have a look at the configuration settings of 
our [Travis continuous integration server](https://github.com/lip6/libITS/blob/master/.travis.yml) 
or [AppVeyor for Windows](https://github.com/lip6/libITS/blob/master/appveyor.yml) for inspiration on how to invoke configure.

This is the recommended approach for developers to download the ITS related tools.

Current pre-requisites for libITS : 
*   Autotools in relatively recent versions (autoconf >= 2.19, automake >= 2.61), packaged on most distributions.
*   C++ build tools (GNU toolchain recommended g++, sh, make and dependencies). We need relatively recent gcc/g++, compliant with C++11, e.g. **g++ 5.0** or better.
*   libDDD &gt;= v1.9 
*   A git client, command line or graphical (eGit provided in Eclipse for instance).
*   libExpat : a lightweight C library to parse XML documents. You can get it here [Official LibExpat Home](http://sourceforge.net/projects/expat/) or packaged in most linux distributions.
*   Antlr : a parser generator. You can get it here [Official ANTLR Home](http://antlr.org) or packaged in most linux distributions. 
*   libGMP : the GNU multiple precision library for math with large numbers, such as exact state space size reporting
Complete build of libITS from source requires full antlr distribution, but the "easy-install" script below takes care of obtaining this package during install using "wget".

To obtain and install ANTLR we provide this small script. 
Then pass configure options "--with-antlrc="$tmp"/antlr-3.4-bin --with-antlrjar="$tmp"/antlr-3.4-bin/antlr-3.4-complete.jar" to libits, ctl and ltl builds.
[antlr.sh](https://github.com/lip6/libITS/blob/master/antlr.sh)  


Full installation follows these guidelines:

### Install of libITS

Follow the usual <code> ./configure ; make ; make install </code> mojo.
Pass <code>--prefix=/home/me/alocalpath/</code> to __configure__ to install in a non standard location (or if you are not root).


## Common options of its-tools

### Using LibITS Flags and Options

LibITS already handles many options that are useful in any model-checking procedure.
These flags and options are honored by all the libITS based tools, [its-reach](reach.md), [its-ctl](ctl.md), [its-ltl](ltl.md).
The source dealing with them is in [Options](https://github.com/lip6/libITS/blob/master/its/Options.hh) or look at [usage example](https://github.com/lip6/libITS/blob/master/bin/main.cpp#L182-L194)

Models can be given to its-tools as:

*   Extended ordinary Petri nets: place/transition nets with pre, post, inhibitor, and test arcs. Syntax of input files can be any of Romeo xml format, PROD format, CAMI format, CAMI+JSON. A clean programmer API designed to facilitate generation of (large) nets is also provided.
*   Composite types and their variants, ScalarSet and CircularSet. These types offer a general support for the composition of ITS instances. A dedicated XML format (as generated from the Coloane ITS plugin) or a programmer API can be used to input Composite types.
*   Discrete time models: Time Petri nets in the XML format of Romeo, and TimedComposite allow to build representations of timed models. TA support is currently in development.
*   Models input in the [ETF format](http://fmt.cs.utwente.nl/tools/ltsmin/etf.html) as produced by [LTSmin](http://fmt.cs.utwente.nl/tools/ltsmin/). This means we can analyze (CTL, LTL) the numerous formalisms that are seen as front-ends by LTSmin.
*   Models input in the [GAL format](gal.php). With this rich format for data manipulation a wide class of systems can be described.
*   Models input in the [Divine format](http://divine.org) by internal translation to GAL. This means we can analyze (CTL, LTL) divine models natively. Efficient ordering heuristics (based on GAL structure) are also provided.

### Input Options (mandatory):

*   -i path : specifies the path to the input model file.
*   -t {CAMI,PROD,ROMEO,ITSXML,ETF,DVE,GAL,DLL,NDLL} : specifies format of the input model file :
    *   CAMI : CAMI format (for P/T nets) is the native Petri net format of [CPN-AMI](http://move.lip6.fr/software/CPNAMI/MANUAL_SERV/index.html)
    *   PROD : PROD format (for P/T nets) is the native format of [PROD](http://www.tcs.hut.fi/Software/prod/)
    *   ROMEO : an XML format (for Time Petri nets) that is the native format of [Romeo](http://romeo.rts-software.org/)
    *   ITSXML : a native XML format (for ANY kind of ITS) for this tool. These files allow to point to other files and are used as main file for composite definitions. See [this example](https://github.com/lip6/libITS/blob/master/Samples/ITSXML/philo4/modelMain.xml), the list of formalism/format supported is described [here](https://github.com/lip6/libITS/blob/master/its/ITSModelXMLLoader.cpp).
    *   ETF : [Extended Table Format](http://fmt.cs.utwente.nl/tools/ltsmin/etf.html) is the native format used by LTSmin, built from many front-ends.
    *   DVE : Divine is a modelling language similar to Promela. Input file should be in [Divine format](http://divine.fi.muni.cz/manual.html#quick-guide-the-dve-specification-language).
    *   GAL : Guarded Action Language. Input file should be in [GAL syntax](gal.php).
    *   DLL : use a dynamic library that provides a function "void loadModel (Model &,int)" typically written using the manipulation APIs. See demo/ folder.
    *   NDLL : same as DLL, but expect input formatted as size:lib.so. See [demo/ folder](https://github.com/lip6/libITS/blob/master/demo/philoSumo.cpp) for a usage example. Both DLL and NDLL are used to inject of arbitrary C++ ITS types into the ITSModel.

### Options related to Variable Order

libits uses static variable orders. These options allow to view or modify the variable order used.

The syntax of a variable order file is a sequence of : (on one line) : #TYPE nameoftype  
followed by variable names one per line, followed by #ENDTYPE on a single line. 
A single ordering file may contain order of many ITS types. 

[An example order file.](https://github.com/lip6/libITS/blob/master/tests/test_models/train4.ord) 
See also the output of "--dump-order".

*   --load-order path : load the variable order from the file designated by path. This order file can be produced with --dump-order. Note this option is not exclusive of --json-order; the model is loaded as usual, then the provided order is applied a posteriori.
*   --dump-order path : dump the currently used variable order to file designated by path and exit.

Besides these general options valid for all ITS models, (time) Petri nets can be loaded with a hierarchical variable order in JSON format. See the [PNXDD](https://projets-systeme.lip6.fr/trac/research/NEOPPOD/wiki/Framekit_guide_stand_alone#a4HeuristicDescription) tool which embeds various heuristics that produce these files. A Petri net loaded with a hierarchical order is re-encoded internally as a composition of subnets. See [this March 2012 TopNoc paper](http://pagesperso-systeme.lip6.fr/Fabrice.Kordon/pdf/2012-toponc-pnxdd.pdf) that describes these heuristics in detail.  
This option is Petri net specific:

*   --json-order path : use a JSON encoded hierarchy description file for a Petri net model (CAMI, PROD or ROMEO), such as produced using Neoppod/PNXDD heuristic ordering tools. Note that this option modifies the way the model is loaded.

For models that use GAL or Divine formalisms, we have adapted [Aloul et al's FORCE heuristic.](http://web.eecs.umich.edu/~karem/papers/Aloul-GLSVLSI03.pdf). Use option --gen-order for fine control on these heuristics, or leave the default that is a good compromise for a broad range of models.

*   --gen-order L?Q?S? : Invoke ordering heuristic to compute a static ordering. Three types of constraints can be specified:
    *   L try to improve locality of transitions.
    *   Q try to reduce the number of queries.
    *   S try get the state variables higher in the structure.

It defaults to LQ, a fair compromise between improving locality and reducing the need to query the lower part of the structure when resolving the actions. If you want to use the default (lexicographical) ordering, use --gen-order "" to deactivate all the constraints

### Options related to Encoding of ITS types

For Petri nets, there is a choice between an SDD encoding, where to each variable is associated the set of integer values it can take, or a DDD encoding, where each edge of the decision diagram is labeled by a single value (i.e. like MDD). The default used is the SDD encoding, but option --ddd sometimes works better when markings (or transition latest firing times) have large values.

*   --sdd : privilege SDD storage (Petri net models only).[DEFAULT]
*   --ddd : privilege DDD (no hierarchy) encoding (Petri net models only).

For Scalar and Circular symmetric composite types, the following options allow to use recursive state encodings. The default setting is "-ssD2 1", i.e. for a Scalar or Circular set of size n, define n SDD variables, one per subcomponent.

*   -ssD2 INT : (DEFAULT: -ssD2 1) (depth 2 levels) use 2 level depth for scalar sets. Integer provided defines level 2 block size.
*   -ssDR INT : (depth recursive) use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels.
*   -ssDS INT : (depth shallow recursive) use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level.

Default setting -ssD2 with block size of 1 for a system of size 9 produces encoding (P//P//P//P//P//P//P//P//P) . For instance, with -ssD2, if size is 9 and block size is 2, 5 variables are introduced, 4 of which represent the state of two subcomponents and one with a single subcomponent state. Hence encoding is : ((P//P)//(P//P)//(P//P)//(P//P)//(P)).  
For instance, with -ssDR, if size is 9 and block size is 2, 2 variables representing the states of 4 and 5 subcomponents are introduced. The 4 subcomponents are encoded as 2 subcomponents containing each 2 instances. The 5 subcomponents are encoded as 2 subsystems of 2 and 3 instances. etc... Hence encoding is : ((P//P)//(P//P))//((P//P)//((P//P)//(P))).  
For a system of size 9, with -ssDR 4, the encoding is ((P//P)//(P//P)//(P//P)//(P//P//P)), with four variables at the outer level and 2 or 3 variables at internal level.  
For the same example of size 9, with -ssDS 4, encoding is : ((P//P//P//P)//(P//P//P//P)//(P)), with 3 variables at outer level and 1 to 4 variables at internal level.

A good approach to determine a good setting of these options for a particular model is to first try "-ssD2 1" and "-ssD2 2". If building blocks of size two reduces the complexity, higher values can be tried. Choosing values for -ssD2 above half the system size is not useful as these all provide the same encoding.

If having larger block size helps, and the size of the system is large enough, try either -ssDR or -ssDS. Note that due to the form of the decomposition, results for different values of block size may be irregular. An ideal case is when choosing a block size k is such that the system size n is a power of k. In that case, -ssDS and -ssDR provide the same encoding. Having k divide n is also a favorable case, as it helps build similar subsystems (same number of instances in each subsystem).

For more details on this recursive encoding, see the [2011 ITS definition paper](http://ceur-ws.org/Vol-726/04-componet-2.pdf) where Scalar and Circular composite and their encodings are defined (page 8-9).

### Options related to gc policy

libddd has an optimistic garbage collection policy, where a memory threshold is set to start running the gc. Below this threshold, no gc will occur, trading faster runtimes for higher memory footprint. Above this threshold, at certain steps of the symbolic algorithms (when the data structures are in a stable intermediate step), the gc will be triggered. When invoking the gc, a new threshold is defined as the max of the previous threshold and the memory usage when invoking the gc. Use a very low value of the threshold to minimize the memory footprint, or leave the default at 1.3 Gb, that provides very fast results for small models, but still allows to scale up on larger ones. Avoid setting the threshold above 60% of physical memory, as this may lead to swapping before the first gc call.

Garbage collection options :  

*   --no-garbage : disable garbage collection (may be faster, more memory)*   --gc-threshold INT : set the threshold for first starting to do gc [DEFAULT:13000 kB=1.3GB]

### Options related to saturation

Two variants of saturation are possible, controlling how clusters are applied at a given level. The BFS variant chains application of each transition cluster using a round robin, while the DFS variant chains applications to a fixpoint of each cluster. The DFS variant can be very efficient when a given transition cluster can be fired several times in a row. This could be the case for a transition with a high level of non-determinism, or a transition such as "time elapses" which can often be chained without firing other transitions. Petri nets with large marking values also often exhibit this behavior where a transition can be fired several times in a row.

The default setting is BFS, except if loading Time Petri nets where the default becomes DFS (more efficient in our experiments). Manually setting the strategy overrules these defaults.

*   --fixpoint {BFS,DFS} : this options controls which kind of saturation algorithm is applied. Both are variants of saturation not really full DFS or BFS. [default: BFS]


## Authors, Acknowledgement

The code base for libITS is loosely based on the program called pnddd by Denis Poitrenaud and Jean-Michel Couvreur. 

The code for ITS and their manipulation is due to Yann Thierry-Mieg. 

Code for manipulation of GAL and Expressions was written by Yann Thierry-Mieg and Maximilien colange. 

Input parsers were contributed by Silien Hong (JSON) and Didier Lime (Romeo). 

The code for manipulation of ETF files is heavily based on LTSmin, by Stefan Blom, Alfons Laarman, Elwin Pater, Jaco van de Pol, Michael Weber et al. of the Formal Methods and Tools group of the University of Twente (NL). 
Essentially a subset of the files of LTSmin managing ETF models are duplicated in the src/etf/ folder of libits, and some refactoring was performed to remove dependencies to things we do not use in this scenario. 
A first proof of concept implementation for interaction between SDD and LTSmin is due to Jeroen Ketema of Univ. Twente; this served as an early basis to build the ITS wrapper around ETF files. 

The code for handling parse of Divine models is directly extracted (and adapted) from the divine source distribution. 

Translation of Divine to GAL is loosely based on the code of a patch (divine compile -l) for divine provided by the Twente group.

For further credits and version history please browse the repository history.

