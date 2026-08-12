# AudioRADAR

Developed with Unreal Engine 5

Final Major Project "information/research" repository [https://github.com/Ramphoryncus/FinalMajorProject]


# E.A.R.D.A.R.

## Environmental Audio Real-Time Detection And Ranging

E.A.R.D.A.R. is an accessibility-focused Final Major Project exploring how spatial audio information in videogames can be represented visually for deaf and hard-of-hearing (DHH) players.

The project investigates whether information normally communicated through hearing — such as the direction and approximate distance of a sound source — can be translated into a visual HUD without simply revealing every event occurring in the game world.

The current submission is a **functional prototype developed primarily in Unreal Engine 5.6.1**. A corresponding Unity/C# architecture was also explored during development.

---

## Project Aim

A hearing player can often obtain useful information from the audio landscape without seeing its source. Examples include:

- footsteps approaching from behind;
- gunfire from another direction;
- nearby explosions;
- vehicles approaching or moving out of view;
- voices and other important environmental sounds.

E.A.R.D.A.R. explores providing an alternative visual representation of this information.

The long-term objective is not to create an omniscient radar. Instead, E.A.R.D.A.R. is intended to represent sounds that a hearing player could reasonably perceive.

The original design therefore considered factors including:

- sound identity/category;
- direction;
- distance;
- attenuation;
- occlusion;
- perceptibility;
- priority;
- temporary hearing sensitivity following loud events;
- suppression of unnecessary visual information.

Not all of these features have been fully tested in the current prototype. See **Current Limitations** below.

---

# Current Unreal Prototype

The submitted Unreal Engine prototype demonstrates the core spatial visualisation pipeline.

Configured actors can be given an `EardarEmitterComponent`. The central `EardarManager` processes these emitters and creates `FEardarBlip` data which is passed from C++ to the Blueprint/UMG interface.

The current demonstrated pipeline is:

```text
Actor
  ↓
EardarEmitterComponent
  ↓
EardarManager
  ↓
FEardarBlip
  ↓
EardarRadarWidget
  ↓
WBP_EardarRadar
  ↓
WBP_EardarBlip
  ↓
HUD
```

The prototype currently demonstrates:

- multiple E.A.R.D.A.R. emitters in the level;
- emitter registration and processing;
- C++ to Blueprint/UMG communication;
- dynamic HUD blip generation;
- camera-relative bearing calculation;
- radial representation of emitter distance;
- individual `IconId` values;
- N/S/E/W test emitters;
- live HUD updates as the player's viewpoint changes.

The N/S/E/W glyphs are deliberately simple development placeholders used to make the behaviour of individual test emitters easy to identify.

---

# Software Architecture

The planned E.A.R.D.A.R. software architecture and relationship between the major system components is documented in the following Figma diagram:

[Figma software Architecture Diagram](https://www.figma.com/board/OZpTkQ9uJaijjUFIh3gBIR/EARDAR-SADiagram?node-id=0-1&p=f)

The architecture separates sound/emitter configuration, perceptibility processing and visual presentation so that the underlying E.A.R.D.A.R. concept can be adapted across different game engines.

---

# Unreal Engine Source

The main E.A.R.D.A.R. C++ files are:

```text
EardarTypes.h
EardarEmitterComponent.h
EardarEmitterComponent.cpp
EardarManager.h
EardarManager.cpp
EardarRadarWidget.h
EardarRadarWidget.cpp
```

## `EardarTypes.h`

Contains shared E.A.R.D.A.R. data structures, including:

- `EEardarCategory`
- `FEardarProfileRow`
- `FEardarBlip`

`FEardarBlip` acts as the main data structure passed from the C++ processing system to the visual HUD.

## `EardarEmitterComponent`

An `ActorComponent` that can be added to an Unreal actor to identify it as a potential E.A.R.D.A.R. source.

The component contains configurable metadata and overrides such as:

```text
SoundTag
CategoryOverride
BaseLoudnessOverride
MaxEffectiveRangeOverride
OcclusionPenaltyOverride
IconIdOverride
```

## `EardarManager`

The central processing actor.

Its responsibilities include gathering registered emitters, resolving their configuration, calculating spatial information and supplying `FEardarBlip` data to the radar interface.

The codebase also contains experimental/scaffolded support for:

- distance attenuation;
- occlusion;
- perceptibility thresholds;
- priority;
- hearing sensitivity and recovery following loud events;
- Gameplay Tag/DataTable sound profiles.

These systems are present in the architecture but were **not all sufficiently validated during the final prototype testing**.

## `EardarRadarWidget`

Provides the C++/Blueprint interface:

```cpp
UFUNCTION(BlueprintImplementableEvent, Category="EARDAR")
void UpdateBlips(const TArray<FEardarBlip>& Blips);
```

This allows C++ to provide processed spatial data while UMG controls its visual presentation.

---

# UMG Interface

Two main Widget Blueprints are used:

```text
WBP_EardarRadar
WBP_EardarBlip
```

`WBP_EardarRadar` receives the array of `FEardarBlip` structures and creates the required visual elements.

`WBP_EardarBlip` represents an individual source.

The HUD converts each emitter's bearing and normalised radius into polar screen coordinates.

Conceptually:

```text
X = sin(Bearing) × Radius
Y = -cos(Bearing) × Radius
```

This allows an emitter to move around the player-centred HUD as the camera rotates.

Its normalised distance determines how far from the centre its glyph appears.

---

# Test Setup

The final spatial test uses four cube actors positioned around the player.

Each contains an `EardarEmitterComponent` and has an individual `IconIdOverride`:

```text
N
S
E
W
```

The letters identify the individual test emitters rather than acting as a fixed compass.

When the player changes viewing direction, the glyphs move around the HUD while retaining their identities.

Emitters positioned at different distances also appear at different radial distances from the centre.

This test was used to validate the core spatial visualisation system.

---

# Running the Unreal Prototype

## Requirements

- Unreal Engine 5.6.1
- C++ development environment compatible with Unreal Engine
- Visual Studio or JetBrains Rider with Unreal Engine support

## Setup

1. Clone or download this repository.
2. Ensure Unreal Engine 5.6.1 is installed.
3. Right-click the `.uproject` file and generate project files if required.
4. Open the project in Unreal Engine.
5. Compile the C++ project if prompted.
6. Open the E.A.R.D.A.R. test level.
7. Run the project using **Play In Editor (PIE)**.
8. Rotate/move the player camera and observe the N/S/E/W glyphs updating on the HUD.

If the project requires rebuilding after cloning, ensure the `GameplayTags` module is present in the module dependencies in `AudioRADAR.Build.cs`.

---

# Unity Exploration

A corresponding Unity/C# implementation was explored during development.

The main scripts are:

```text
EardarSoundProfile.cs
EardarEmitter.cs
EardarManager.cs
RadarIcon.cs
```

The Unity version follows the same conceptual architecture as the Unreal implementation:

```text
GameObject
    ↓
EardarEmitter
    ↓
EardarManager
    ↓
Spatial / perceptibility data
    ↓
RadarIcon
    ↓
Canvas
```

`EardarSoundProfile` uses a `ScriptableObject` to store configurable emitter metadata.

The Unity implementation also explores UI pooling so that radar icons can be reused rather than repeatedly instantiated.

The Unreal Engine implementation became the primary functional prototype, so the Unity code should be considered an **architectural/prototyping exploration rather than an equally complete final implementation**.

---

# Development Documentation

The full development process, research influences, implementation, testing, limitations and reflection are documented in:

```text
COMMENTARY.md
```

The project source code also contains Doxygen-compatible documentation describing the purpose and behaviour of the E.A.R.D.A.R. C++ classes and functions.

---

# Video Evidence

A short video demonstration of the Unreal Engine prototype is included/linked as submission evidence.

The demonstration shows:

- the prototype running in Unreal Engine 5.6.1;
- multiple configured E.A.R.D.A.R. emitters;
- the N/S/E/W test glyphs;
- camera-relative movement of the HUD glyphs;
- radial differences based on emitter position.

https://github.com/user-attachments/assets/ff395c30-b5b6-4235-8544-79594e05eb59

---

# Current Limitations

E.A.R.D.A.R. is currently a technical prototype and should not be considered a completed accessibility product.

Current limitations include:

- live audio amplitude/RMS is not yet connected to the final visualisation;
- attenuation behaviour requires further controlled testing;
- occlusion support exists in the architecture but has not been sufficiently validated;
- temporary hearing sensitivity/loud-event behaviour has not been demonstrated in the final test scenario;
- Gameplay Tag/DataTable-driven sound profiles require further integration and testing;
- N/S/E/W are placeholder development glyphs rather than final accessible iconography;
- accessibility configuration options such as high-contrast modes and HUD scaling remain future work;
- the Unity version has not reached the same level of integration as the Unreal prototype;
- performance with large numbers of simultaneous emitters has not been evaluated;
- structured testing with deaf and hard-of-hearing players has not yet been undertaken;
- a packaged final build is not included with the submission.

The supplied video, screenshots, source code and development commentary provide evidence of the functional Unreal prototype.

---

# Future Development

Future development would focus on:

1. connecting E.A.R.D.A.R. directly to active audio playback;
2. replacing the placeholder RMS value with live audio-level information;
3. validating attenuation and occlusion against the game's actual audio behaviour;
4. completing Gameplay Tag and DataTable-driven sound profiles;
5. adding categories such as footsteps, gunfire, explosions, vehicles and voices;
6. designing accessible iconography using shape as well as colour;
7. implementing high-contrast and configurable HUD modes;
8. reducing redundant icons when their source is already clearly visible;
9. introducing persistent/pool-based blip management for performance;
10. completing and evaluating the Unity implementation;
11. conducting iterative usability testing with deaf and hard-of-hearing players.

The most important future step is user evaluation. A technically functioning visualisation does not by itself demonstrate that the interface is genuinely useful or accessible to its intended audience.

---

# Declared Assets

## Original Project Work

The E.A.R.D.A.R. concept, accessibility-system design, prototype implementation, testing and project-specific integration were produced for this Final Major Project.

The Unreal Engine implementation includes project-specific work across:

```text
EardarTypes.h
EardarEmitterComponent.h
EardarEmitterComponent.cpp
EardarManager.h
EardarManager.cpp
EardarRadarWidget.h
EardarRadarWidget.cpp
WBP_EardarRadar
WBP_EardarBlip
```

The Unity exploration includes:

```text
EardarSoundProfile.cs
EardarEmitter.cs
EardarManager.cs
RadarIcon.cs
```

The N/S/E/W glyphs and cube actors used in the final demonstration are placeholder test elements and are not presented as original graphical assets.

## Epic Games / Unreal Engine Assets

The Unreal prototype was created using **Unreal Engine 5.6.1** and the standard **Third Person template**.

Any template characters, animations, meshes, materials, level elements or other content originating from the Unreal Engine Third Person template remain Epic Games/Unreal Engine assets and are not claimed as original work.

## Unity Technologies Assets

Unity was used during the cross-engine exploration of E.A.R.D.A.R.

Any standard Unity engine components, APIs or editor-provided resources remain the property of their respective creators and are not claimed as original assets.

## Artificial Intelligence Assistance

Generative AI assistance was used during development as a programming, debugging, documentation and learning aid.

ChatGPT was used to assist with activities including:

- discussing and refining the E.A.R.D.A.R. architecture;
- explaining C++ and C# concepts;
- generating and iterating example/scaffold code;
- debugging Unreal Engine and Blueprint integration;
- discussing spatial/polar-coordinate calculations;
- annotating source code and preparing Doxygen documentation;
- structuring and editing project documentation;

AI-generated suggestions were integrated, modified, tested and debugged within the project rather than being treated as independently validated functionality.

Where functionality remained untested or incomplete, this is identified in the Development Commentary and project documentation.

The final project decisions, implementation within the engine, integration, testing, evidence gathering, COMMENTARY.md, README.md and submission remain the responsibility of the student.

---

# Author

**Henry Taylor**

BSc (Hons) Games Development  
Final Major Project

**E.A.R.D.A.R. — Environmental Audio Real-Time Detection And Ranging**
