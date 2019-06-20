# WindowCapture2D
Library for capturing and displaying windows in real time with UnrealEngine.

Captures in specified window units.


![ed99cc1218473bb4accc3c5d217454c4](https://user-images.githubusercontent.com/8191970/59857826-b31e1400-93b4-11e9-80df-13e39a0e1ec6.gif)



# Quick Start

## When Actor is placed
1. Put "WindowCapturePlane" at the level

![image](https://user-images.githubusercontent.com/8191970/59691952-e4fd7200-921e-11e9-8afe-8585339f8ae3.png)

## When Widget is placed
1. Place "WindowCaptureUMG" in UMG

![umg](https://user-images.githubusercontent.com/8191970/59858054-33447980-93b5-11e9-96bc-ae790e5f721e.png)

1. Set various properties

![image](https://user-images.githubusercontent.com/8191970/59692049-18400100-921f-11e9-9e09-c945c744c05c.png)

| Property | Description |
|:-----------|:------------|
| Capture Target Title | Title string of the window that you want to capture |
| Title Matching Window Search | Title string matching method |
| Frame Rate | Interval to capture the window (fps) |
| Check Window Size | When on, changes the texture size by monitoring the window size change <br/> If enabled, the load will be high|
| Cut Shadow| Remove the shadow of the window from the capture target |

# Title Matching Window Search

| Type | Description |
|:-----------|:------------|
| PerfectMatch | Must match exactly with the input string　|
| ForwardMatch | Compare with the input string in a forward match|
| PartialMatch | Compare with the input string in a partial match|
| BackwardMatch | Compare with the input string in a backward match|
| RegularExpression | Compare using regular expressions |
