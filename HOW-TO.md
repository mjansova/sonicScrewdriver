How to
======

Key principles
--------------

The tool is designed to be interfaced easily with a collection of TTrees, 
referred later as "datasets" in the corresponding method names. However,
no assumption is made about the exact way data are stored. It should therefore 
be easy to use it as long as you are able to provide event-by-event (or
jet-by-jet, depending on what is your basic object) values of your variables.

The user must first create an instance of the front-end :
    
    SonicScrewdriver s;

Then, applying methods on the front-end, the user can define physics objects such 
as variables, process classes, datasets, cuts, regions, channels and figures.

Variables
---------

    s.AddVariable(tag, label, unit, numberOfBins, min, max,      pointerToValue, options);
    s.AddVariable(tag, label, unit, numberOfBins, customBinning, pointerToValue, options);

where :
- `tag` is the shortcut name of the variable (to be used by the system) ;
- `label` is the fancy name of the variable (to be used on plots) ;
- `numberOfBins` is the number of bins to be used for the histograms corresponding to the variables ;
- `min` and `max` are the boundaries of the histogram. Alternatively, on can use `customBinning` which is
  a float pointer to an array of numbersOfBins+1 floats corresponding to use-defined individual bins boundaries ;
- `pointerToValue` can be a double, a float or an int pointer, pointing to the value that will be used
  when filling histograms.

- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this variable. Currently supported options include :
    - `logY` : plot related to this variable will be plotted in log-Y scale ;
    - `noUnderflowInFirstBin` : do not automatically include the underflow in first bin of the thistogram ;
    - `noOverflowInLastBin` : do not automatically include the overflow in last bin of the histogram.
 
Process classes
---------------

    s.AddProcessClass(tag, label, type, color, options)

where :
- `tag` is the shortcut name of the process class (to be used by the system) ;
- `label` is the fancy name of the process class (to be used on plots) ;
- `type` is string being either `background`, `signal` or `data` ;
- `color` is the ROOT color code to be used on plots for thie process class ;

- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this process class. Currently supported options include :
    - `no1DPlots` : process class will not appear on 1D-type plots

Datasets
--------

    s.AddDataset(tag, processClass, initialNumberOfEvents, xSecOrLumi, options) ;

where :
- `tag` is the shortcut name of the dataset (to be used by the system) ;
- `processClass` is the tag of the process class associate to this process class ;
- `initialNumberOfEvents` is an optional parameter that can be used later for weighting, corresponding the initial number of events to consider ;
- `xSecOrLumi` is also and optional parameter that can be used later for weighting, corresponding to either the cross section of the sample or the luminosity of the dataset ;

- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this dataset.

Cuts
----

One can define cuts on previously defined variable using the following syntax :

    Cut(variable, cutType, cutValue);

where
- `variable` is the tag of the variable targeted by the cut
- `cutType` is a single character, either '>', '<' or '=' related to the direction of the cut
- `cutValue` is the value of the cut

Regions
-------

    s.AddRegion(tag, label, parentRegionTag, listOfCuts, options)
    s.AddRegion(tag, label, parentRegionTag, selector,   options)

where :
- `tag` is the shortcut name of the region (to be used by the system) ;
- `label` is the fancy name of the region (to be used on plots) ;
- `parentRegionTag` is an optional argument such that this region will inherit of the cut of the parent ;
- `listOfCuts` is the vector of cuts that defines the region. Alternatively, one can use `selector` being a pointer to a bool-returning function
telling if the currently treated event should go or not into the region.

- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this region. Currently supported options include :
    - `showCuts` will, for a given 1D plot showing a variable, remove the cut associated to that variable such as one can see the effect of that cut
    - `blinded` will make sure that histograms corresponding to data will not be filled for this region.

Channels
--------

    s.AddChannel(tag, label, selector, options)

where :
- `tag` is the shortcut name of the channel (to be used by the system) ;
- `label` is the fancy name of the channel (to be used on plots) ;
- `selector` is a pointer to a bool-returning function telling if the currently treated event should go or not into the channel.

- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this channel.

2.g) Figures

    s.AddFigurePerProcess(tag, label, options)
    s.AddFigure          (tag, label, options)

where :
- `tag` is the shortcut name of the figure (to be used by the system) ;
- `label` is the fancy name of the figure (to be used on plots).

- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this figure.

