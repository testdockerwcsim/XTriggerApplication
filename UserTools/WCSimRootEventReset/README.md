# WCSimRootEventReset

WCSimRootEventReset clears WCSimRootEvents in the DataModel, in order to prevent memory leaks when there are multiple triggers in the object

## Data

Calls `Clear()` on these objects in the DataModel
`IDWCSimEvent_Triggered`
`ODWCSimEvent_Triggered`



## Configuration

```
use_stopwatch BOOL
stopwatch_file FILENAME
verbose LEVEL
```
* `verbose` Verbosity level. Runs from 0 (low verbosity) to 9 (high verbosity)
* `use_stopwatch` Use the Stopwatch functionality implemented for this tool?
* `stopwatch_file` Save the time it takes for each run of `Execute()` to a histogram. Should end in .pdf, .eps, etc.
