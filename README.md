sonicScrewdriver
================

A plotting tool for high-energy physics using Root

Goal
----

The Sonic Screwdriver is designed to generate high-energy physics plots easily by providing the user with a clean, flexible front-end aimed to manipulate physics concepts instead of ROOT-related technical mess.

[Name inspiration](https://en.wikipedia.org/wiki/Sonic_screwdriver)

Features
--------

- Add variables, process classes, regions and channels
- Create the histograms that will be needed for your plots (one line of code)
- List the type of plots you want to be produced
- Eventually add options where you want to change the standard behavior
- Run over your events and fill all the histograms (one line of code)
- Ask for the plots to be produced (two lines of code)
- Run your macro
- Browse your plots, do actual physics

Usage
-----

An example is provided in `test/`. 
After setting up your environment, just do :

    make && make test

And manually browse the plots in `test/plots/` or just do

    make browse

from the `test` folder. It will open a TBrowser with the root files already loaded.

Disclaimer
----------

This tool doesnt work on WOOD.

To-do list / ideas
------------------

Important :
- Documentation
- More global options to change font, line width, ... maybe add a config file to read stuff from.
- Converge on what to do for the lumi reweighting per channel...

Little improvements :
- For custom binning, option to redivide the content of the bins to simulate a fixed bin width (to keep having an interpretation in term of pdf).
- Modify the way operations between figures are made (should use actual stat formulas instead of root histos)
- Maybe possibility to link variable to vectors (of float, ints..). ...And maybe to TLorentzVector ? Not a big fan of this though.

Mid-long term :
- Add table for data/MC comparisons
- Add surcharge operator +, -, x, and / for (int,Figure)

Big stuff :
- Possibility to separate the reading from the actual plotting, ie export raw histograms and be able to import them.
- Web-based interface to generate the code..

Eric stuff :
- Add Chi2 or Kolmogorov test for data/MC comparison + display value on the plot
- Add 2D plot showing the evolution of correlation between two variables after lower/upper cuts
- Add data and SM yield on data/MC plot
