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

An example is available in test/. 
After setting up your environment, just do :

    ./cmdCompile.sh && ./makeTreeForExample && ./plotGenerationExample

And browse the plots/ folder.

Disclaimer
----------

This tool doesn't work on WOOD.

To-do list / ideas
------------------

- Optimise AutoFill : should check only once the regions and channel in AutoFill before dispatch
- Actual documentation
- Fix infos displayed on plots (energy/lumi, + add region, channel, change emplacement)
- Possibility to add custom lines/infos on plots via external user-defined templates 
- Underflow/overflow management by default
- Custom binning for variables (ex : for discrete values like # of jets)
- Improve tables (auto-scale of column width ?)
- Add daughter class to table for Data/MC or bkg/sig comparison
- Add or fix latex/ascii export/import for tables
- Easy access to a given plot or histo
- Improve ratio plots (lines, gap, scale) for data/MC comparisons plots
- Lumi reweighting should be done per channel
- Add surcharge operator +, -, *, and / for (int,Figure)
- Modify the way operations between figures are made (should use actual stat formulas instead of root histos)
- Global options configuration to set/change the default behavior (ex : font, line width, text colors ...)


