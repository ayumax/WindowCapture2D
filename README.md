# WindowCapture2D
Library for capturing and displaying windows in real time with UnrealEngine.

Captures in specified window units.


![ed99cc1218473bb4accc3c5d217454c4](https://user-images.githubusercontent.com/8191970/59857826-b31e1400-93b4-11e9-80df-13e39a0e1ec6.gif)



# Quick Start
Please check "show engine content"

![showenginecontent](https://user-images.githubusercontent.com/8191970/60768227-80d31d00-a0fd-11e9-8ad8-7fa00682732e.png)

## When Actor is placed
1. Put "WindowCapturePlane" at the level

![setactor](https://user-images.githubusercontent.com/8191970/60768239-afe98e80-a0fd-11e9-989f-445cf6779517.png)

## When Widget is placed
1. Place "WindowCaptureUMG" in UMG

![umg](https://user-images.githubusercontent.com/8191970/60768308-ffc85580-a0fd-11e9-8e2f-7d99cc2893c5.png)

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
