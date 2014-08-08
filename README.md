sonicScrewdriver
================

A plotting tool for high-energy physics using Root

Goal
----

This tool is designed to make it easy to generate high-energy physics plots by providing the user with a clean, flexible front-end aimed to manipulate physics concepts instead of ROOT-related technical mess.

[Name inspiration](https://en.wikipedia.org/wiki/Sonic_screwdriver) (disclaimer : it doesn't work on wood)

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

Features
--------

- Describe the physics (variables, datasets & process classes, regions & channels, figures)
- List the type of plots you want to be produced (superimposed, stacked, data/MC comparisons, 2D, ...)
- Use options where you want to change the standard behavior
- Run over your data, automatically filling the histograms all at once
- Ask for the plots to be produced
- Browse your plots, do actual physics

Installation
------------

After setting up ROOT, just do :

    make 
    
To get a quick idea of how the tool is working, you can check `plotGenerationExample.C` in provided in the `test` folder. To run it, type :

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
- Converge on what to do for the lumi reweighting per channel...
- Use C++ maps to better handle the vectors of variables/processes/regions/histos/plots
- Have an extra optional argument for datasets to be splitted in different process classes
- Define weighting via a function pointer
- Add option to choose wether the data/MC ratio is on top or bottom

Little improvements :
- For custom binning, option to redivide the content of the bins to simulate a fixed bin width (to keep having an interpretation in term of pdf).
- Modify the way operations between figures are made (should use actual stat formulas instead of root histos)
- Maybe possibility to link variable to vectors (of float, ints..). ...And maybe to TLorentzVector ? Not a big fan of this though.

Big stuff :
- Possibility to separate the reading from the actual plotting, ie export raw histograms and be able to import them.
- Web-based interface to generate the code..

Eric suggestions :
- Add Chi2 or Kolmogorov test for data/MC comparison + display value on the plot
- Add 2D plot showing the evolution of correlation between two variables after lower/upper cuts
- Add data and SM yield on data/MC plot
