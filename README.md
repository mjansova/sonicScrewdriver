sonicScrewdriver
================

A plotting tool for high-energy physics using Root


Goal
----

The Sonic Screwdriver is designed to generate high-energy physics plots easily by providing the user with a clean, flexible front-end aimed to manipulate physics concepts instead of ROOT-related technical mess.

Name inspiration : https://en.wikipedia.org/wiki/Sonic_screwdriver

Features
--------

- Add variables, process classes, regions and channels
- Create the histograms that will be needed for your plots in one line
- List the type of plots you want to be produced
- Add options where you want to change the standard behavior
- Run over your events and fill all the histograms in one line
- Ask for the plots to be produced
- Run your macro, browse your plots and do actual physics


To-do list / ideas
------------------

- Update the example in test/
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

Changelog
---------

### 13-06-05

- Fixed eps/png plot export
- Updated README

### 13-06-04

- Removed 1D/2DCustom plots (now replaced by 2Dto1D and 3Dto2D projections)
- Added 1D-cut significance plots
- Removed mechanism that associate variables to region (turned out to be useless)
