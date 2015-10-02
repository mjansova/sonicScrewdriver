How to
======

0. Key principles
-----------------

The tool is designed to be interfaced easily with a collection of TTrees, 
referred later as "datasets" in the corresponding method names. However,
no assumption is made about the exact way data are stored. It should therefore 
be easy to use it as long as you are able to provide event-by-event (or
jet-by-jet, depending on what is your basic object) values of your variables.

Using the tool is done in four major steps :
  
1.  [Define physics objects](#DefiningPhysicsObjects) (variables, processes, datasets, cuts, regions, channels and figures) ;
2.  [List the plots you](#PlotConfiguration) want to be produced ;
3.  [Loop on whatever your data comes from](#LoopOnYourData), automatically feeding it to the tool ;
4.  [Produce and browse the outputs](#ProduceOutput).

In appendix A, I show how the user can access histograms and produce plots his own way when the current feature are not enough for the user-case.

App. A. [About user-custom plots and manipulation](#CustomPlots)


<a name="DefiningPhysicsObjects"></a>
1. Defining physics objects
----------------------------

### Variables

```C++
s.AddVariable("tag", "label", "unit", numberOfBins, min, max,      pointerToValue, "options");
s.AddVariable("tag", "label", "unit", numberOfBins, customBinning, pointerToValue, "options");
```

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
 
### Process classes

```C++
s.AddProcessClass("tag", "label", "type", color, "options");
```

where :
- `tag` is the shortcut name of the process class (to be used by the system) ;
- `label` is the fancy name of the process class (to be used on plots) ;
- `type` is string being either `background`, `signal` or `data` ;
- `color` is the ROOT color code to be used on plots for thie process class (see predefined colors in [`interface/Common.h`](interface/Common.h) ;
- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this process class. Currently supported options include :
    - `no1DPlots` : process class will not appear on 1D-type plots

### Datasets

```C++
s.AddDataset("tag", "processClass", initialNumberOfEvents, xSecOrLumi, "options");
```

where :
- `tag` is the shortcut name of the dataset (to be used by the system) ;
- `processClass` is the tag of the process class associate to this process class ;
- `initialNumberOfEvents` is an optional parameter that can be used later for weighting, corresponding the initial number of events to consider ;
- `xSecOrLumi` is also and optional parameter that can be used later for weighting, corresponding to either the cross section of the sample or the luminosity of the dataset ;
- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this dataset.

### Cuts

One can define cuts on previously defined variable using the following syntax :

```C++
Cut("variable", 'cutType', cutValue);
```

where
- `variable` is the tag of the variable targeted by the cut
- `cutType` is a single character, either '>', '<' or '=' related to the direction of the cut
- `cutValue` is the value of the cut

### Regions

```C++
s.AddRegion("tag", "label", "parentRegionTag", listOfCuts, "options");
s.AddRegion("tag", "label", "parentRegionTag", selector,   "options");
```

where :
- `tag` is the shortcut name of the region (to be used by the system) ;
- `label` is the fancy name of the region (to be used on plots) ;
- `parentRegionTag` is an optional argument such that this region will inherit of the cut of the parent ;
- `listOfCuts` is the vector of cuts that defines the region. Alternatively, one can use `selector` being a pointer to a bool-returning function telling if the currently treated event should go or not into the region.
- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this region. Currently supported options include :
    - `showCuts` will, for a given 1D plot showing a variable, remove the cut associated to that variable such as one can see the effect of that cut
    - `blinded` will make sure that histograms corresponding to data will not be filled for this region.

### Channels

```C++
s.AddChannel("tag", "label", selector, "options");
```

where :
- `tag` is the shortcut name of the channel (to be used by the system) ;
- `label` is the fancy name of the channel (to be used on plots) ;
- `selector` is a pointer to a bool-returning function telling if the currently treated event should go or not into the channel.
- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this channel.

### Figures

Figures are a special kind of variable as they are not event-by-event variables, but rather global numbers such as scale factors or yields, but that might depend on the process, region and channel. First, you need to declare the figure :

```C++
s.AddFigurePerProcess("tag", "label", "options"); // Depends of process x region x channel
s.AddFigure          ("tag", "label", "options"); // Depends of region x channel only
```

where :
- `tag` is the shortcut name of the figure (to be used by the system) ;
- `label` is the fancy name of the figure (to be used on plots).
- `options` is a optional string containing options separated by comas, that aim to customize the behavior of this figure.

Then you can fill the values for each process/region/channel with :

```C++
s.SetFigure("figureTag","processTag","regionTag","channelTag",valueWithError)
```

where :
- `figureTag` is the tag of the figure as declared before
- `processTag`, `regionTag` and `channelTag` are the tag of the process x region x channel (process being ommited if the figure doesnt depend on process)
- `valueWithError` is a variable with Figure type, e.g. `Figure(3.14,1.0)` corresponds to 3.14 +/- 1.0

<a name="PlotConfiguration"></a>
2. Configure the plots
----------------------

### Preparing the histograms

First, the user must tell the sonic screwdriver to create all the 1-dimensionnal histograms that will be needed, using :

```C++
s.Create1DHistos();
```

If you will want to have a look at 2-dimensionnal plots (i.e. one variable vs an other one), you need to manually ask for each one of them to be create, this way :

```C++
s.Add2DHisto("var_1","var_2");
s.Add2DHisto("var_2","var_3");
```

### Scheduling plots

Then, the user can list the plots that shall be produced, using the following syntax :

```C++
myScrewdriver.SchedulePlots("plotType","options");
```

where :
- `plotType` is the type of plot to be produced ;
- `options`  is an optional field that contain information about how this particular kind of plot should be produced. Note that for some type of plots, some information are actually mandatory. Options are written with the form `field=value` and separated by comas (`,`) ;

The current supported kind of plots are :

|   Name                     |  Description                                                                        | Options |
| -------------------------- | ----------------------------------------------------------------------------------- | ------- |
| `1DStack`                  | Weighted backgrounds are stacked on top of each other                               |         |
| `1DSuperimosed`            | Backgrounds are renormalized to unity and superimosed, aiming to compare the shapes |         |
| `1DDataMCComparison`       | Weighted backgrounds are stacked then data are superimposed and a data/MC ratio is drawn |    |
| `2D`                       | All created 2D histograms will lead to a 2D plot, one for each process class        |         |
| `2DSuperimposed`           | Not supported yet because of ROOT issues                                            |         |
| `1DFigureOfMerit`          | Create a plot showing the evolution of a figure of merit after cutting on a given variable | `var` to indicate the variable tag to cut on, and `cutType` should be set to `keepHighValues` or `keepLowValues` |
| `1DFrom2DProjection`       | (Badly supported at this point) Aiming to create 1D plots from 2D ones, in particular to compute efficiency curves. | See with the developers or in the code.. |
| `1DFigure`                 | For a given list of figures and a given channel, produce a plot of the figures as function of the regions | `name` to indicate the tag of the plot, `figures` to list the figures separated by `:`,`channel` to specify the channel, `min` and `max` to fix the y-range. |
| `1DDataMCComparisonFigure` | For each FigurePerProcess declared, produce a data/MC comparison as function of the regions | |

### Further configuration of the plots

#### Include signal in plots

| Method                  | Range              | Option               | Values                                |
| ----------------------- | ------------------ | -------------------- | ------------------------------------- |
| s.SetGlobalBoolOption   | "1DSuperimposed"   | "includeSignal"      | `true` or `false`                     |
| s.SetGlobalStringOption | "1DStack"          | "includeSignal"      | `"no"`, `"superimposed"` or `"stack"` |
| s.SetGlobalFloatOption  | "1DStack"          | "factorSignal"       | A float                               |
| s.SetGlobalStringOption | "DataMCComparison" | "includeSignal"      | `"no"`, `"superimposed"` or `"stack"` |
| s.SetGlobalFloatOption  | "DataMCComparison" | "factorSignal"       | A float                               |

#### Figure of Merit

| Method                  | Range              | Option               | Values                               |
| ----------------------- | ------------------ | -------------------- | ------------------------------------ |
| s.SetGlobalFloatOption  | "FigureOfMerit"    | "backgroundSystematicUncertainty" | A float                 |

#### Plot information

| Method                  | Range              | Option               | Values                               |
| ----------------------- | ------------------ | -------------------- | ------------------------------------ |
| s.SetGlobalStringOption | "Plot"             | "infoTopRight"       | A string (ex : `"CMS internal"`)
| s.SetGlobalStringOption | "Plot"             | "infoTopLeft"        | A string (ex : `"#sqrt{s} = 8 TeV"`)

#### Plot export

| Method                  | Range              | Option               | Values                               |
| ----------------------- | ------------------ | -------------------- | ------------------------------------ |
| s.SetGlobalBoolOption   | "Plot"             | "exportPdf"          | true of false                        |
| s.SetGlobalBoolOption   | "Plot"             | "exportEps"          | true or false                        |
| s.SetGlobalBoolOption   | "Plot"             | "exportPng"          | true of false (warning : png export takes time) |

<a name="LoopOnYourData"></a>
3. Loop on your data
--------------------

This is typically done with something like :

```C++
vector<string> datasetsList;
s.GetDatasetList(&datasetsList);

cout << "   > Reading datasets " << endl;

for (unsigned int d = 0 ; d < datasetsList.size() ; d++)
{
    // Get current dataset
    string currentDataset = datasetsList[d];
    string currentProcessClass = s.GetProcessClass(currentDataset); 
    
    // Open tree corresponding to currentDataset
    TTree* theTree = ...

    // Loop over the events
    int nEntries = theTree->GetEntries();
    for (int i = 0 ; i < nEntries ; i++)
    {
        theTree->GetEntry(i);

        // This weight, or whatever weight you want to use
        float weight = s * GetDatasetLumiWeight(currentDataset);

        // Then use black magic to fill the right histograms with current event
        s.AutoFillProcessClass(currentProcessClass,weight);
    }
}
```

<a name="ProduceOutput"></a>
4. Produce and browse the outputs
---------------------------------

After looping on your data, but before producing any plots, you might want to apply scale factors to your (1D) histograms (warning : it doesnt affects 2D or 3D histograms).
To do so, use

```C++
s.ApplyScaleFactor("processTag","regionTag","channelTag",scaleFactor)
```

where :
- `processTag`, `regionTag`, `channelTag` is the relevant process x region x channel
- `scaleFactor` is the scale factor to apply to all variable plots corresponding to the process x region x channel, e.g. `Figure(3.14,1.0)` corresponding to 3.14 +/- 1.0 

Producing and writing the plots is done with

```C++
s.MakePlots();
s.WritePlots("./folder/for/plots/")
```

which after the end of the program you can then browse. In root format for instance, with

```C++
root -l ./folder/for/plots/*.root
```

You may also produce and export yield tables, for a given channel, with :

```C++
TableDataMC(&s,{ "regionTag1", "regionTag2" }, "channelTag").Print("yieldTable.tab",precision)
```

where :
- `&s` is the pointer of the screwdriver
- `{ "regionTag1", "regionTag2" }` is the list of regions to consider
- `"channelTag"` is the channel to consider
- `"yieldTable.tab"` is the output file name
- `precision` is an integer stating the precision to use when printing the yields (e.g. 2 for 2 decimal precision)
- Additionnaly, as an additional argument after `"channelTag"`, you may add `"includeSignal"` to include signal process classes in the tab

<a name="CustomPlots"></a>
Appendix A. About user-custom plots and manipulation
----------------------------------------------------
