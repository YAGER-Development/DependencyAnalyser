# Dependency Analyser
The Dependency Analyser is a UE5 plugin to measure and analyse bottlenecks in [asset dependency](https://docs.unrealengine.com/5.3/en-US/referencing-assets-in-unreal-engine/) sizes. It allows for quick inspection of all assets in a project and identification of optimisation opportunities, thanks to its integration with the size map tool and the automated testing framework.

![dependency_analyser](https://github.com/YAGER-Development/DependencyAnalyser/assets/17513294/c799d3d2-dbc4-4a32-8d90-6175635345da)

## Table of Contents
- [Introduction](#introduction)
  - [The dependency problem](#the-dependency-problem)
  - [Specifications](#specifications)
  - [Usage instructions](#usage-instructions)
- [Plugin Components](#plugin-components)
  - [Dependency Analyser Tool](#dependency-analyser-tool)
  - [Dependency Analyser Test Suite](#dependency-analyser-test-suite)
  - [Dependency Analyser Settings](#dependency-analyser-settings)
- [Contributions](#contributions)
  - [Known issues](#known-issues)

## Introduction
### The dependency problem
Dependency chains are created by assets referencing other assets, and represent one of the biggest and most common problems with which developers on large projects have to deal when working in Unreal Engine. The reason why they are problematic lies in the way assets are managed in Unreal, which implies that whenever an asset is loaded, all its dependencies are loaded with it. It is not particularly easy to identify growing dependency chains until their size becomes obvious and starts impacting editor loading times, hurting developers' productivity. A great talk that explains the risks of complex dependency chains is [Asset Dependency Chains: The Hidden Danger | Unreal Fest 2022](https://www.youtube.com/watch?v=4-oRyDLfo7M) by Mark Craig.

This plugin simplifies the identification of asset dependencies by pointing a spotlight on their reference count and dependency chain size. It is an ideal tool to integrate into the testing framework and build pipeline of a project. Whilst dependency analysis can be carried out at any point during the development of a project, it is most efficient when performed regularly from the start, therefore an early integration is recommended.

### Specifications
Dependency Analyser plugin version 1.0. Compatibility tested with Unreal Engine version 5.3.2 and 5.1.1.

### Usage instructions
To use the Dependency Analyser plugin:
* Download this repository
* Move it to
  * the Plugins folder of your project, if you want to import it into a specific project
    * i.e. `LyraStarterGame\Plugins`
  * the Plugins folder of your engine, if you want to import it into any project
    * `C:\Program Files\Epic Games\UE_[version]\Engine\Plugins` on Windows
    * `/Users/Shared/Epic Games/UE_[version]/Engine/Plugins` on macOS
* Enable it by following the [Unreal Engine guidelines](https://docs.unrealengine.com/5.0/en-US/working-with-plugins-in-unreal-engine/)
* Compile your C++ project (this repository does not contain binaries - users will need to compile the plugin autonomously) and open Unreal
* Navigate to `Tools > Miscellaneous > Dependency Analyser` to bring up the Dependency Analyser Tool
* Navigate to `Tools > Test Automation` and select `DependencyAnalyser` to inspect and run the Dependency Analyser Test Suite
* Navigate to `Edit > Editor Preferences > Dependency Analyser` to customise the settings, such as which asset types are included in the analysis

## Plugin Components
The Dependency Analyser plugin comprises of two main features:
* The [Dependency Analyser Tool](#dependency-analyser-tool), which allows for an overview of the project asset dependencies
* The [Dependency Analyser Test Suite](#dependency-analyser-test-suite), which uses the Automation System framework to include acceptance limits to asset dependency sizes

The settings of each of these tools can be customised via the [Dependency Analyser Settings](#dependency-analyser-settings).

### Dependency Analyser Tool
The Dependency Analyser Tool comprises of the following features:
1. Optional inclusion of soft references and dev folders into the results
2. Customisation of size in MB at which assets should be considered a warning (orange) or an error (red). These are reset every time the tool is reopened - to set them as default values, see [Dependency Analyser Settings](#dependency-analyser-settings)
3. Export of results of the analysis into a CSV file. Pressing the export button brings up the directory where the CSV file is saved, and a log is printed in console with the relevant path
4. Individual inspection of resulting assets
   * Right-clicking brings up a context menu including the options `Open Size Map`, `Edit Asset` or `Find in Content Browser`
   * Double-clicking bring up the Size Map inspector
5. Searchable results by Name or Type
6. Sortable results by Name, Dependencies Count, Total Size, Type or Path

By default, the tool only analyses assets of type World. Removing limitations to asset types will allow the tool to analyse all assets (which is potentially slow for large projects).

![tool_breakdown](https://github.com/YAGER-Development/DependencyAnalyser/assets/17513294/9878555a-1d8c-406d-aa03-3e4b4bf9c598)

### Dependency Analyser Test Suite
If the project has the [Functional Testing Editor plugin enabled](https://docs.unrealengine.com/5.3/en-US/automation-system-in-unreal-engine/#automationtestsmovedtoplugins), the Dependency Analyser offers a test suite to run a dependency analysis on all assets. The test is executed per-asset, so individual sizes of asset for each failing test can be identified.

![test_tool](https://github.com/YAGER-Development/DependencyAnalyser/assets/17513294/4311e056-ea7f-42db-a314-266b71c273e8)

The tests can be ran both via the Unreal editor and the console.

When running them via the editor, the results can be visualised by:
* Selecting the parent `DependencyAnalyser` test, for an overview of the duration and success/failure/warning rate
* Selecting the individual per-asset test, to visualise information about failure reason and asset size or reference count
* Inspecting the Message Logs, for an overview of failure reasons and asset sizes or reference counts of all assets for which a test ran

![test_log](https://github.com/YAGER-Development/DependencyAnalyser/assets/17513294/8e40af64-bcc3-427c-b47e-e2aab4d79d95)

To run the tests via console command, you can use:

`[PathTo]\UnrealEditor-Cmd.exe [PathTo]\[ProjectName].uproject Params=[LevelName] -ExecCmds=\"Automation RunTests Filter:DependencyAnalyser.[TestName];quit\" -nullrhi -nosound -nosplash -unattended -buildmachine -editortest -stdout`

where [TestName] is either DependencyCountTest or DependencySizeTest. The results can be inspected through the console.

### Dependency Analyser Settings

Users can customise which asset types to analyse, whether to enable memory size calculation, error and warning sizes (both defaults and per asset type), and whether warnings should fail a test or not. These values can be customised by navigating to `Edit > Editor Preferences > Advanced > Dependency Analyser` and modifying the default values (or `Type Limits` for a higher degree of control), and `Fail for Warnings`.

![settings](https://github.com/YAGER-Development/DependencyAnalyser/assets/17513294/50443211-0899-42a7-b971-05a20eb79cd5)

## Contributions
All rights to this plugin belong to YAGER. To contribute to the development of this plugin, feel free to open a [Pull Request](https://github.com/YAGER-Development/DependencyAnalyser/pulls). If you want to actively be part of what we are developing at YAGER, check out and apply to any relevant open positions on our [YAGER website](https://www.yager.de/).
### Known issues
_No known issues at the moment. If you find an issue, feel free to add it to the [Issues](https://github.com/YAGER-Development/DependencyAnalyser/issues) page of this repository._
