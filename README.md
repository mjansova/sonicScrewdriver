Sonic screwdriver
================

A plotting tool for high-energy physics using Root

Goal
----

This tool is designed to make it easy to generate high-energy physics plots by providing the user with a clean, flexible front-end aimed to manipulate physics concepts instead of ROOT-related technical mess.

```
                                              +--------------+                 
                                              |              |                 
                             describe         |  Physics     |                 
                        ----------------->    |    concepts  |                 
    +---------------+                         |              | --+             
    |               |                         +--------------+   |             
    |   Particle    |                                            |     Sonic   
    |   Physicist   |                                            |  screwdriver
    |               |                         +--------------+   |             
    +---------------+                         |              | <-+             
                        <-----------------    |  Plots and   |                 
                                get           |    tables    |                 
                                              |              | <-->  ROOT      
                                              +--------------+                 
```

[Name inspiration](https://en.wikipedia.org/wiki/Sonic_screwdriver) (warning : it doesn't work on wood)

Features
--------

- Describe the physics : variables, datasets & process classes, cuts, regions & channels, figures, ...
- List the type of plots you want to be produced : superimposed, stacked, data/MC comparisons, 2D, ...
- Use options where you want to change the standard behavior and personalize the outputs
- Run over your datasets, automatically feeding the histograms all at once
- Ask for your plots and tables to be produced
- Browse the outputs, do actual physics

Installation
------------

After setting up ROOT, just do :

    make 
    
To get a quick idea of how the tool is working, you can check [`plotGenerationExample.cc`](test/plotGenerationExample.cc) provided in the `test` folder. To run it, type :

    cd test && make test

Then manually browse the plots in `test/plots/` or just do

    make browse

It will open a `TBrowser` with the root files already loaded.

Usage / how-to
--------------

Pieces of documentation are provided in the [HOW-TO](HOW-TO.md) file.

To-do list / ideas
------------------

Important :
- Documentation
- More global options to change font, line width, ... maybe add a config file to read stuff from.
- Use C++ maps to better handle the various vectors of variables/processes/regions/histos/plots
- Have an extra optional argument for datasets to be splitted in different process classes
- Rethink the way lumi is managed, more generally define weighting via a function pointer

Little improvements :
- For custom binning, option to redivide the content of the bins to simulate a fixed bin width (to keep having an interpretation in term of pdf).
- Modify the way operations between figures are made (should use actual stat formulas instead of root intermediate histos)
- Maybe possibility to link variable to vectors (of float, ints..). ...And maybe to TLorentzVector ? Not a big fan of this though.

Big stuff :
- Possibility to separate the reading from the actual plotting, ie export raw histograms and be able to import them.
- Web-based interface to generate the code ?

Eric suggestions :
- Add Chi2 or Kolmogorov test for data/MC comparison + display value on the plot
- Add 2D plot showing the evolution of correlation between two variables after lower/upper cuts
- Add data and SM yield on data/MC plot
