# WindowCapture2D
Library for capturing and displaying windows in real time with UnrealEngine.

Captures in specified window units.



![c23c1fcde580c2bc76db322a174c4c08](https://user-images.githubusercontent.com/8191970/59691589-33f6d780-921e-11e9-8f19-e61b6bc81d52.gif)


# Quick Start

1. Put "WindowCapturePlane" at the level

![image](https://user-images.githubusercontent.com/8191970/59691952-e4fd7200-921e-11e9-8afe-8585339f8ae3.png)

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
