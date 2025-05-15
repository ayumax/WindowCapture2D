# WindowCapture2D

## Overview
WindowCapture2D is an Unreal Engine plugin that enables low-latency, high-performance window-based capture on Windows.

It supports capturing background windows, and the captured images can be attached to meshes or UMG in 3D space.

**If you want to confirm whether this plugin works in your environment, you can download the Tester application from the Releases of this repository and check if the capture works correctly.**

![ed99cc1218473bb4accc3c5d217454c4](https://user-images.githubusercontent.com/8191970/59857826-b31e1400-93b4-11e9-80df-13e39a0e1ec6.gif)

## Supported UE Versions
| UE Version | Support Status | Branch Name |
|------------|---------------|-------------|
| UE 5.5     | ✅ Supported  | UE5.5       |
| UE 5.4     | ✅ Supported  | UE5.4       |
| UE 5.3     | ✅ Supported  | UE5.3       |
| Others     | 🔄 Check docs | master      |

With support for UE5.x, the capture method has changed compared to the previous WindowCapture2D plugin.

The UE5.x compatible plugin is designed to deliver even higher performance than the previous UE4.x version.

For UE4 support, please use the UE4.x branch.

## How to Obtain
- [Unreal Engine Marketplace](https://www.fab.com/ja/listings/96541798-870b-40a5-b5ec-b3981e88272f)
- GitHub: https://github.com/ayumax/WindowCapture2D

## Installation Guide

If you don't have a C++ build environment, download the pre-built package from the Marketplace.

### From Marketplace
1. Purchase and download the plugin from the Marketplace.
2. Add it to your project from the "Library" section in the UE Launcher.
3. Enable WindowCapture2D from "Edit" → "Plugins" in the UE editor.

### From GitHub
1. Clone this repository: `git clone https://github.com/ayumax/WindowCapture2D`
2. Copy the `Plugins` directory into your project folder.
3. Build the plugin (requires a C++ build environment).
4. Enable WindowCapture2D from "Edit" → "Plugins" in the UE editor.

### Compatibility Check
Select the appropriate branch for your Unreal Engine version.

## Quick Start

1. Enable "show engine content" and "show plugin content".

![showcontent](https://user-images.githubusercontent.com/8191970/60973786-dc9edf80-a363-11e9-8eb4-c01deb0135c5.png)

### When placing an Actor
1. Place "WindowCapturePlane" in your level.

![setactor](https://user-images.githubusercontent.com/8191970/60768239-afe98e80-a0fd-11e9-989f-445cf6779517.png)

### When placing a Widget
1. Place "WindowCaptureUMG" in your UMG.

![umg](https://user-images.githubusercontent.com/8191970/60768308-ffc85580-a0fd-11e9-8e2f-7d99cc2893c5.png)

2. Set various properties.

![image](https://user-images.githubusercontent.com/8191970/59692049-18400100-921f-11e9-9e09-c945c744c05c.png)

| Property                   | Description                                      |
|:---------------------------|:-------------------------------------------------|
| Capture Target Title       | Title string of the window you want to capture   |
| Title Matching Window Search | Window title matching method                    |
| Frame Rate                 | Capture frame rate (fps)                         |

## Feature Details

- Low-latency, high-performance window capture for Windows
- Supports capturing background windows
- Captured images can be attached to 3D meshes or UMG
- Title matching (Perfect, Forward, Partial, Backward, Regular Expression)
- Supports both Unreal Engine C++ and Blueprint

### Title Matching Methods

| Type              | Description                                  |
|:------------------|:---------------------------------------------|
| PerfectMatch      | Exact match with the input string             |
| ForwardMatch      | Forward match with the input string           |
| PartialMatch      | Partial match with the input string           |
| BackwardMatch     | Backward match with the input string          |
| RegularExpression | Match using regular expressions               |

## Usage Examples

### Blueprint Example
- See "Quick Start" above.
- Easy to use with Blueprint.

### C++ Example
```cpp
// Create a CaptureMachine and start capturing
UCaptureMachine* Capture = NewObject<UCaptureMachine>();
Capture->Start();
// ... Release with Dispose() when done
Capture->Dispose();
```

## Notes
- Does not work on environments other than Windows.
- Capturing windows displayed on large screens such as 4K monitors requires a high-performance CPU and GPU.
  - If the frame rate is low, please lower the screen resolution.
- Some types of windows may not be capturable.

## WinRT
This plugin uses WinRT to capture windows.

WinRT is used under the MIT license.

```
C++ for the Windows Runtime (cppwinrt)

Copyright (c) Microsoft Corporation
All rights reserved. 

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

```

Therefore, to build this plugin in your environment, Windows SDK v10.0.22000.0 or later is required.

Additionally, this plugin includes the WinRT headers within the Plugin directory and uses them via include.

This is because, in rare cases where an older version of WinRT is present in the environment, building the plugin may fail.

If you want to build using the WinRT installed in your environment, please modify the following line in `WindowCapture2D.Build.cs` to add the directory on your system to the include path.

```csharp
var winrtDirectory = Path.Combine(ModuleDirectory, "Private", "cppwinrt");
if (Directory.Exists(winrtDirectory))
{
    PrivateIncludePaths.Add(winrtDirectory);
}
```

## License
This plugin is provided under the MIT License.  
However, if you download and use it from the Epic Games Marketplace, the Epic Games license terms will apply.

## Contributing
Bug reports, feature requests, and pull requests are welcome.  
Please select the appropriate branch for your Unreal Engine version.