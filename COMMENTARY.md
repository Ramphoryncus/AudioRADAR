# E.A.R.D.A.R. — Development Commentary

## Environmental Audio Real-Time Detection And Ranging

## Introduction

E.A.R.D.A.R. (Environmental Audio Real-Time Detection And Ranging) is my Final Major Project exploring how spatial audio information in videogames can be translated into a visual format for deaf and hard-of-hearing (DHH) players. The central aim of the project was to investigate whether information that would ordinarily be communicated through hearing, such as the direction and approximate distance of a sound source, could instead be represented through an accessible heads-up display.

The project originated from a simple accessibility problem. Modern games frequently rely on sound to communicate information that is relevant to gameplay. A hearing player may be able to identify that footsteps are approaching from behind, that gunfire is occurring to one side, or that a vehicle is approaching without any of those objects currently being visible. A deaf or hard-of-hearing player may not receive equivalent information if the game does not provide another sensory representation.

E.A.R.D.A.R. was conceived as a possible way of reducing this information disparity. Rather than generating conventional subtitles for every sound, the intention was to create a spatial visualisation system capable of representing the location, identity and eventually the perceptibility of important sounds around the player.

The original scope was ambitious. E.A.R.D.A.R. was planned as an engine-independent concept with implementations for both Unreal Engine and Unity. Sound-emitting objects would carry semantic information identifying what kind of sound they represented. A central manager would determine whether the sound should reasonably be perceptible to a hearing player before transmitting a compact representation to a visual interface.

The intended perceptibility model included distance attenuation, obstruction or occlusion, source loudness, prioritisation, temporary sensitivity changes following loud events, and suppression of unnecessary visual information. For example, an object that was both close to the player and clearly visible might not require an additional icon, while an important object behind the player could benefit from one.

Development ultimately concentrated on a functional Unreal Engine 5.6.1 prototype. The final demonstrated prototype does not implement or validate every part of the original design. Instead, it establishes the core architecture required for E.A.R.D.A.R. to identify configured emitters, convert their three-dimensional relationship to the player into camera-relative spatial data, pass this information from C++ to Unreal Motion Graphics (UMG), and dynamically display visual glyphs representing individual emitters.

This distinction between the original specification and the completed prototype is important. Features such as sophisticated acoustic modelling, live audio analysis, polished iconography, user-configurable accessibility presets and extensive DHH user testing remain areas for future development. The prototype should therefore be considered a proof of the underlying spatial visualisation pipeline rather than a finished accessibility product.

---

# Research and Conceptual Influences

## Accessibility as a Design Principle

The conceptual direction of E.A.R.D.A.R. developed alongside my research into accessibility and inclusive game design. My Critical and Conceptual Influences research investigated how games can be optimised for players with diverse needs and considered accessibility not simply as a collection of optional settings but as part of the wider design process.

One of the most significant conclusions from this research was that accessibility should be considered from the beginning of development rather than added retrospectively. Frameworks including the Game Accessibility Guidelines (GAG), Xbox Accessibility Guidelines (XAG) and Accessible Player Experiences (APX) demonstrate that accessibility encompasses much more than individual features. It includes clarity, predictability, customisation, alternative sensory communication and the ability for players to configure an experience around their requirements.

This had a direct influence on E.A.R.D.A.R. The project is not intended to give a DHH player an advantage over a hearing player. Its purpose is instead to explore **equivalence of information**. If information exists in the audio landscape and can meaningfully influence the decisions of a hearing player, there is an argument for providing an alternative method through which a player unable to access that information can receive it.

This also changed how I thought about the HUD. The goal should not be to expose every event occurring in the game world. Doing so could give the user information that a hearing player would never have received and would also produce an excessively cluttered interface. E.A.R.D.A.R. therefore developed around the idea of a perceptibility model: an event should qualify for visualisation because it represents information that could plausibly have been perceived through sound.

## The Last of Us Part II Remastered

A major case study within my accessibility research was *The Last of Us Part II Remastered*. Naughty Dog's approach demonstrates how information that is conventionally dependent upon one sensory channel can be represented through another. Its accessibility systems include hearing-oriented presets, enhanced subtitles, visual awareness information and directional sound indicators.

This was particularly relevant to E.A.R.D.A.R. because the project is fundamentally concerned with sensory translation. The information already exists in the game simulation; the problem is how it is communicated to the player.

The case study also reinforced the importance of combining accessibility features rather than assuming that one interface can solve every problem. Players can have intersecting requirements, and a visual accessibility tool designed for a DHH player must still consider factors such as contrast, visual complexity, readability and cognitive load.

This influenced my intended design direction for E.A.R.D.A.R. The long-term plan included alternative display configurations such as a DHH mode, Minimal mode and High-Contrast mode. I also considered shape language and redundant visual communication so that icons would not depend entirely on colour.

These features were not completed within the final prototype, but they remain important design requirements rather than discarded ideas.

## Clarity Before Style

Another significant outcome of my research was the principle that clarity should come before stylistic complexity.

This became unexpectedly relevant during implementation. The final prototype uses simple letters such as N, S, E and W as test glyphs rather than polished artwork. Although these are obviously not suitable final icons for an accessibility product, they became useful development tools because their identity is unambiguous.

Using letters allowed me to determine whether an individual emitter retained its identity while its position changed. A generic circle would have shown that something was moving, but it would have been harder to determine whether the correct emitter was appearing in the correct location.

The deliberately simple visual design therefore became part of the testing methodology.

---

# Initial Concept and Technical Planning

## Original System Concept

The initial concept for E.A.R.D.A.R. was considerably broader than the final prototype.

The proposed system consisted of several conceptual stages:

```text
Sound Source
     ↓
Semantic Sound Information
     ↓
E.A.R.D.A.R. Emitter
     ↓
Perceptibility Processing
     ↓
Spatial Blip Data
     ↓
Visual HUD
```


### Software Architecture Diagram

As part of the initial technical planning, I produced a software architecture diagram to visualise the proposed relationship between E.A.R.D.A.R.'s major components and responsibilities.

[Figma software Architecture Diagram](https://www.figma.com/board/OZpTkQ9uJaijjUFIh3gBIR/EARDAR-SADiagram?node-id=0-1&p=f)

The diagram was used as a planning tool for separating the sound-emitter layer, central processing system and HUD representation before these concepts were translated into the engine-specific Unity and Unreal implementations.

---

Initially I considered analysing audio directly and potentially using automatic classification to determine whether a sound represented footsteps, gunfire, vehicles or other events.

This approach would have introduced substantial complexity. Audio classification would require either signal-processing heuristics or a machine-learning solution and would make the behaviour of the system harder to predict.

I therefore refined the design toward **semantic tagging**.

Instead of asking E.A.R.D.A.R. to determine what an arbitrary waveform represents, the developer can explicitly identify a sound-emitting object. The emitter can contain metadata describing its category, maximum range, icon, priority and other properties.

This approach has several advantages for an accessibility system. It is deterministic, inexpensive compared with continuous classification, configurable by the game developer, and easier to test.

For example:

```text
Sound.Footstep
Sound.Gunshot
Sound.Explosion
Sound.Vehicle
Sound.Voice
```

could be associated with different display profiles.

The visual system does not need to infer that an audio waveform represents an explosion. The game already knows that an explosion has occurred.

## Cross-Engine Architecture

Early development explored how the same architecture could be represented in both Unity and Unreal Engine.

The Unity design used four principal scripts:

```text
EardarSoundProfile.cs
EardarEmitter.cs
EardarManager.cs
RadarIcon.cs
```

`EardarSoundProfile` was designed as a `ScriptableObject`. It contained metadata including `IconId`, category, base loudness, maximum effective range, occlusion penalty, persistence, priority and on-screen suppression.

`EardarEmitter` provided a component that could be attached to an object representing a sound source.

`EardarManager` was responsible for finding emitters and calculating potential radar information.

`RadarIcon` represented the visual output and contained the polar positioning calculation for the Unity Canvas.

The architecture was subsequently translated into Unreal Engine concepts:

```text
EardarSoundProfile     → FEardarProfileRow
EardarEmitter          → UEardarEmitterComponent
EardarManager          → AEardarManager
RadarIcon / data       → FEardarBlip + UMG
```

This was useful because it demonstrated that the underlying idea was not dependent on a particular engine API.

However, completing two equivalent production prototypes would have significantly increased the implementation and testing workload. Unreal Engine therefore became the main production target.

---

# Unreal Engine 5.6.1 Architecture

The Unreal prototype was developed using C++ for the underlying system and Blueprint/UMG for presentation.

The major source files were:

```text
EardarTypes.h
EardarEmitterComponent.h
EardarEmitterComponent.cpp
EardarManager.h
EardarManager.cpp
EardarRadarWidget.h
EardarRadarWidget.cpp
```

The visual implementation consisted primarily of:

```text
WBP_EardarRadar
WBP_EardarBlip
```

This separation was intentional. C++ performs the world-space calculations and constructs data describing what should be displayed. UMG is responsible for deciding how that information appears.

The resulting pipeline is:

```text
World Actor
    ↓
UEardarEmitterComponent
    ↓
AEardarManager
    ↓
FEardarBlip
    ↓
UEardarRadarWidget
    ↓
Event UpdateBlips
    ↓
WBP_EardarRadar
    ↓
WBP_EardarBlip
```

This became the central architecture of the working prototype.

---

# Data Structures

## EardarTypes.h

`EardarTypes.h` contains shared data structures used by the E.A.R.D.A.R. system.

`EEardarCategory` provides categories such as:

```cpp
Footstep
Gunshot
Explosion
Vehicle
Voice
Ambient
Custom
```

The intention is for categories to provide broad semantic information while Gameplay Tags can provide more specific identifiers.

`FEardarProfileRow` describes how an emitter should behave. Its planned properties include base loudness, effective range, occlusion penalty, persistence, priority, on-screen suppression and icon identity.

The second important structure is `FEardarBlip`.

A blip represents the compact information required by the interface after an emitter has been processed. Relevant fields include:

```cpp
FVector WorldLocation;
float BearingDeg;
float NormalizedRadius;
FName IconId;
float Alpha;
int32 Priority;
FGameplayTag SoundTag;
EEardarCategory Category;
```

This structure creates a useful boundary between simulation and interface code.

The UMG Blueprint does not need to know how an emitter was discovered or how its world position was converted. It receives a `BearingDeg`, `NormalizedRadius`, `IconId` and other presentation information.

This also makes the system more extensible because the visual interface could theoretically be replaced without rewriting the emitter system.

---

# EardarEmitterComponent

`UEardarEmitterComponent` was designed as an Unreal `UActorComponent`.

This allows E.A.R.D.A.R. behaviour to be added compositionally to an existing actor rather than requiring every compatible actor to inherit from a specific E.A.R.D.A.R. base class.

The component can contain semantic and configuration information including:

```cpp
FGameplayTag SoundTag;
EEardarCategory CategoryOverride;
float BaseLoudnessOverride;
float MaxEffectiveRangeOverride;
float OcclusionPenaltyOverride;
FName IconIdOverride;
```

It can also optionally reference an `AudioComponent`.

The location of an emitter is obtained using:

```cpp
FVector UEardarEmitterComponent::GetEmitterLocation() const
{
    if (AudioComponent)
    {
        return AudioComponent->GetComponentLocation();
    }

    return GetOwner()
        ? GetOwner()->GetActorLocation()
        : FVector::ZeroVector;
}
```

This provides two useful behaviours. If a spatial audio component is explicitly supplied, its location can represent the sound source. Otherwise the owning actor becomes the source.

For the final prototype I used ordinary cube actors as controlled test emitters. This meant that I could validate the spatial system independently of a more complicated gameplay or audio scenario.

---

# EardarManager

`AEardarManager` is the central processing actor.

The design places the more expensive or shared processing in one manager rather than making every emitter perform its own tick-based HUD calculations.

At startup the manager can create the configured radar widget. During runtime it builds a collection of `FEardarBlip` structures and passes them to the interface.

The manager also contains the planned perceptibility architecture. The code includes configuration for:

```cpp
PerceptibilityThreshold
HearingSensitivity
SensitivityRecoverySeconds
MaxVisibleBlips
```

and helper functions including:

```cpp
ResolveProfile()
ComputeBlip()
CheckOcclusion()
```

`ComputeBlip()` contains calculations for distance, attenuation, occlusion, hearing sensitivity, bearing and normalised radius.

The intended audibility model can be summarised as:

```text
Audibility =
    BaseLoudness
  × DistanceAttenuation
  × OcclusionMultiplier
  × HearingSensitivity
  × PlaybackRMS
```

This is important to distinguish from the functionality demonstrated in the final prototype.

The architecture and calculations for these systems exist in the codebase, but my final practical testing concentrated on validating emitter discovery, C++ to Blueprint communication, bearing, radius and HUD representation. I therefore do not consider acoustic occlusion, temporary hearing adaptation or live RMS (Root Mean Square) response to have been sufficiently evaluated as completed features.

`SourcePlaybackRMS`, for example, remains a placeholder value of `1.0`. Consequently the system does not currently analyse the live amplitude of the sound source.

This is an important limitation because a future production version should determine whether a sound is actually playing rather than relying exclusively on configured emitter metadata.

---

# C++ and Blueprint Communication

One of the most important technical milestones was successfully passing the C++ blip array into UMG.

`EardarRadarWidget.h` derives from `UUserWidget` and exposes:

```cpp
UFUNCTION(BlueprintImplementableEvent, Category="EARDAR")
void UpdateBlips(const TArray<FEardarBlip>& Blips);
```

`BlueprintImplementableEvent` was particularly appropriate because I wanted C++ to define the data pipeline without forcing the visual implementation to remain in C++.

The manager can therefore call:

```cpp
RadarWidgetInstance->UpdateBlips(Blips);
```

and Unreal routes the event to the Blueprint implementation in `WBP_EardarRadar`.

The resulting boundary is:

```text
C++ simulation/data
        ↓
UpdateBlips()
        ↓
Blueprint Event UpdateBlips
        ↓
UMG presentation
```

This was one of the first areas that I tested directly.

### Blueprint for WBP_EardarRadar

<img width="594" height="208" alt="Blueprint for WBP_EardarRadar" src="https://github.com/user-attachments/assets/bcda5d1d-e4ff-49c2-aaad-e17af251b3ef" />

The initial widget confirmed that the C++ base class could successfully expose functionality to its Blueprint child.

### Evidence of C++/Blueprint Interface with PrintString

<img width="220" height="198" alt="Evidence of C++ Blueprint interface" src="https://github.com/user-attachments/assets/7529d6a2-79e2-40bf-a2b7-437b99db39af" />

During early integration I deliberately used `Print String` rather than immediately implementing the final visualisation. This provided a simple observable output from the interface and reduced the number of systems being debugged simultaneously.

---

# Dynamic Blip Creation

The next stage was creating a separate widget for an individual blip.

`WBP_EardarBlip` was introduced as a reusable visual element. Rather than hard-coding a fixed number of icons into the radar, `WBP_EardarRadar` can iterate over the `Blips` array and create a widget for each entry.

### Evidence Blueprint for WBP_EardarRadar incorporating WBP_EardarBlip with test PrintString

<img width="1781" height="262" alt="WBP EardarRadar incorporating WBP EardarBlip" src="https://github.com/user-attachments/assets/864848a9-a6b0-4311-a3cd-3fd2ef77f9bd" />

This established the following runtime relationship:

```text
UpdateBlips
    ↓
Clear previous children
    ↓
ForEach FEardarBlip
    ↓
Create WBP_EardarBlip
    ↓
Add Child to Canvas
    ↓
Position child
```

Although rebuilding the children every update is adequate for a small prototype, this would not be my preferred production solution. Repeated creation and destruction of UMG widgets could become expensive with large numbers of emitters.

A future version should maintain a pool of blip widgets or update persistent widgets associated with registered emitters. The Unity scaffold already explored a pooling approach, which could be transferred to Unreal.

---

# Testing Emitter Discovery

An important early test involved checking the length of the blip array.

### Evidence Interface Communicates with WBP_EardarRadar Widget

<img width="247" height="245" alt="No emitter test" src="https://github.com/user-attachments/assets/ba7f5af4-b906-4ed7-9ec4-cf13819538d1" />

With no actor configured as an E.A.R.D.A.R. emitter, the array length was:

```text
0
```

This result was expected but useful. It demonstrated that the Blueprint event was being called without falsely reporting an emitter.

I then added an `EardarEmitterComponent` to a cube actor in the level.

<img width="251" height="330" alt="One emitter test" src="https://github.com/user-attachments/assets/0297f38d-c8b5-44eb-9cd3-eaa9e0c7b018" />

The array subsequently reported:

```text
1
```

This was a small but significant integration test.

The important result was not simply that a number changed. The test showed that a world actor configured with the C++ component could enter the E.A.R.D.A.R. pipeline and result in data reaching the Blueprint interface.

At this point the basic path:

```text
Actor → Component → Manager → Blip Array → Widget
```

had been demonstrated.

---

# Implementing the Spatial HUD

Once emitter discovery was working, I moved onto the defining feature of the prototype: representing world-space sound positions on a two-dimensional HUD.

The manager calculates two particularly important values:

```text
BearingDeg
NormalizedRadius
```

`BearingDeg` represents the direction of the emitter relative to the player's current camera orientation.

`NormalizedRadius` represents relative distance on a scale between zero and one.

In Unreal the bearing calculation uses the camera's forward and right vectors:

```cpp
const FVector Fwd = CamRot.Vector();
const FVector Right = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y);
const FVector ToNorm = To.GetSafeNormal();

const float X = FVector::DotProduct(ToNorm, Right);
const float Y = FVector::DotProduct(ToNorm, Fwd);

OutBlip.BearingDeg =
    FMath::RadiansToDegrees(FMath::Atan2(X, Y));
```

This gives an angle relative to the player's view rather than a fixed world direction.

This distinction is essential for the intended interface.

If a sound remains physically stationary while the player rotates the camera, its icon should move around the radar because its direction **relative to the player** has changed.

Distance is normalised using:

```cpp
OutBlip.NormalizedRadius =
    FMath::Clamp(
        DistM / Profile.MaxEffectiveRangeM,
        0.f,
        1.f
    );
```

This means:

```text
0.0 = radar centre
1.0 = outer radius
```

A sound closer to the player therefore appears nearer to the centre.

---

# Polar Coordinates and Blueprint Positioning

The UMG Blueprint converts the bearing and normalised radius into X and Y coordinates.

Conceptually the calculation is:

```text
X = sin(angle) × radius
Y = -cos(angle) × radius
```

The result is then multiplied by the desired radar radius and added to the centre of the available canvas.

The negative cosine for Y is necessary because screen-space Y coordinates increase downward while the conceptual radar treats forward as upward.

### Functional WBP_EardarRadar Blueprint

<img width="2086" height="633" alt="Functional WBP EardarRadar Blueprint" src="https://github.com/user-attachments/assets/ff632df0-da4d-4c7d-b03b-9b033d17e6e2" />

This Blueprint takes each `FEardarBlip`, breaks the structure into its individual properties and converts the bearing and radius into a two-dimensional position.

The centre of `BlipCanvas` is determined using its local geometry rather than relying on a hard-coded screen resolution. This is preferable because the interface should eventually be able to operate across different viewport sizes.

### Testing Blueprint maths for Bearing Degrees and Normalised Radius

<img width="360" height="565" alt="Testing bearing and radius maths" src="https://github.com/user-attachments/assets/d04f4e2b-3990-4f13-bdb7-3116ae50746f" />

This stage required considerable iteration because several apparent mathematical problems were ultimately caused by UMG layout behaviour rather than the spatial calculations themselves.

---

# UMG Layout Problem and Iteration

One of the clearest examples of iterative problem-solving occurred when the generated blips appeared along the bottom of the viewport.

Initially this suggested that the polar-coordinate calculation was incorrect. The bearing values appeared to respond to player movement, however, which indicated that the underlying data was at least partially functioning.

The problem was eventually traced to the structure of `WBP_EardarBlip`.

The widget initially used a Canvas-based layout that interacted poorly with the dynamically assigned Canvas slot. As a result, the generated widgets did not behave like small independent glyphs and their visual positions were offset in unexpected ways.

I changed the root of `WBP_EardarBlip` to a `SizeBox`.

<img width="351" height="374" alt="WBP EardarBlip SizeBox fix" src="https://github.com/user-attachments/assets/d61e42d7-ceb6-4682-bd2e-d2e53a868512" />

Following this change, the glyphs moved into the intended central radar area.

This was a useful debugging lesson because the visible symptom suggested a mathematical error while the actual problem existed in the UI hierarchy.

It reinforced the importance of isolating systems when debugging. The spatial calculation, Canvas positioning and child-widget layout are separate stages even though their failures can produce very similar visual results.

---

# Final Functional Radar Blueprint

After correcting the child-widget layout, the radar was able to create and position multiple glyphs dynamically.

### Final functional WBP_EardarRadar

<img width="1993" height="637" alt="Final functional WBP EardarRadar" src="https://github.com/user-attachments/assets/adf8b77b-2cdb-4621-82b5-813aad73ba8b" />

At this stage the important functionality was no longer simply the appearance of a widget.

The system now demonstrated:

```text
Multiple configured emitters
        ↓
Individual FEardarBlip structures
        ↓
Camera-relative bearing
        ↓
Normalised radial distance
        ↓
Dynamic child widgets
        ↓
Screen-space polar positioning
```

This represents the central technical proof of concept for E.A.R.D.A.R.

---

# Directional Glyph Testing

The initial `WBP_EardarBlip` used the letter `F` as a placeholder.

This created a problem when testing multiple emitters because every source looked identical. Although their positions could be observed, it was difficult to know which glyph represented which actor.

I therefore used `IconIdOverride` to assign four test emitters the identifiers:

```text
N
S
E
W
```

These labels represented their initial placement around the player and were deliberately simple test values rather than final icon designs.

### Evidence of Glyph Rotation on Camera Movement

<img width="495" height="365" alt="Directional glyph test position one" src="https://github.com/user-attachments/assets/38020fa8-fb7a-4e4d-943d-c53afbbd0f00" />

<img width="383" height="389" alt="Directional glyph test position two" src="https://github.com/user-attachments/assets/1855734c-5612-4877-9323-25a149574718" />

This provided much clearer evidence of the spatial behaviour.

When the player camera rotated, the N, S, E and W glyphs changed their screen positions while retaining their identities.

This demonstrated that the system was not simply displaying fixed compass points. The positions were being recalculated relative to the player's current viewpoint.

Moving the test actors to different distances also produced visible differences in radial placement.

This was particularly important because E.A.R.D.A.R. is intended to communicate both:

```text
Where is the sound?
```

and:

```text
Approximately how far away is it?
```

The final prototype therefore establishes a functioning basis for directional and distance-based visualisation.


### Video Evidence — Functional E.A.R.D.A.R. Prototype

The following video demonstrates the Unreal Engine 5.6.1 prototype running in Play In Editor. Four test emitters labelled N, S, E and W are positioned around the player. As the camera rotates and the player moves, the corresponding HUD glyphs update their positions using the calculated bearing and normalised radial distance.

https://github.com/user-attachments/assets/facdbbc5-73cc-4c10-a9d6-9d38490d1ad9

---

# Gameplay Tags and Profiles

The architecture also includes support for Unreal Gameplay Tags.

A planned sound could use tags such as:

```text
Sound.Footstep
Sound.Gunshot
Sound.Explosion
Sound.Vehicle
```

The original design intended these tags to identify corresponding `FEardarProfileRow` entries from a DataTable.

This would allow designers to configure sound behaviour without modifying C++.

For example, an explosion might have a high base loudness, long effective range and high priority, while footsteps might have a lower range and priority.

The source code contains the infrastructure for this approach, but the final demonstrated prototype used the simpler per-emitter override system, particularly `IconIdOverride`, rather than completing a production DataTable library.

Gameplay Tags should therefore be considered part of the implemented architecture but not a fully authored content system within the final prototype.

One technical issue encountered during implementation was that using `FGameplayTag` required the Unreal module dependency to be added to the project build configuration.

The project initially produced a linker error referencing:

```text
Z_Construct_UScriptStruct_FGameplayTag
```

The problem was resolved by adding:

```text
"GameplayTags"
```

to the appropriate module dependencies in `AudioRADAR.Build.cs`.

This was a useful reminder that including a header is not sufficient when working with Unreal modules. The build system also needs to know that the project links against the module implementing the type.

---

# Unreal Project Structure and Development Workflow

Another technical issue concerned source organisation.

I initially experimented with reorganising the E.A.R.D.A.R. classes into `Public` and `Private` subdirectories. Although this is a conventional Unreal module organisation, introducing the structure into the existing project created unnecessary problems with include paths, Rider integration and generated project behaviour during a time-critical stage of development.

I therefore returned to the flatter source structure already used by the project.

This was a pragmatic decision rather than an architectural requirement of E.A.R.D.A.R. The more formal folder organisation could be restored in a future refactoring pass, but during prototype development it was more important to maintain a working project and concentrate on functionality.

A related compile problem occurred because an early class declaration still contained the placeholder export macro:

```cpp
YOURGAME_API
```

rather than the project's actual module export macro.

Correcting this to the AudioRADAR module macro resolved the resulting undefined-class errors.

Both issues reinforced the value of making structural changes incrementally, particularly in Unreal projects where Unreal Header Tool, generated source, module definitions and the IDE project model interact.

---

# Unity Exploration

Although Unreal became the primary implementation, a Unity C# architecture was also developed.

The Unity version followed the same overall principles.

`EardarSoundProfile` is a `ScriptableObject` containing properties including:

```csharp
IconId
Category
BaseLoudness
MaxEffectiveRangeMeters
OcclusionPenalty
PersistenceSeconds
Priority
OnScreenSuppression
```

`EardarEmitter` represents a component attached to a sound-producing GameObject.

`EardarManager` gathers emitters and contains calculations for distance attenuation, line-of-sight checking, hearing sensitivity, bearing, radius, priority and visual alpha.

`RadarIcon` converts the calculated polar coordinates into a Unity `RectTransform` position.

For example:

```csharp
float rad = bearingDeg * Mathf.Deg2Rad;

float x = Mathf.Sin(rad) * r;
float y = Mathf.Cos(rad) * r;

Vector2 half = radarRoot.rect.size * 0.48f;

rt.anchoredPosition =
    new Vector2(x * half.x, y * half.y);
```

The Unity design also included a simple UI pooling strategy.

Rather than continuously creating and destroying icon objects, `EnsurePool()` creates enough `RadarIcon` instances and then reuses them.

This is an approach that would be worth bringing back into the Unreal implementation.

The Unity code should primarily be understood as architectural exploration rather than an equally complete second prototype. The Unreal implementation received the practical integration and visual testing.

Nevertheless, implementing the same conceptual system using two engine APIs helped demonstrate that E.A.R.D.A.R.'s architecture can be separated from its engine-specific implementation.

---

# Audibility and Perceptibility Model

One of the more ambitious aspects of the project is the attempt to distinguish between **the existence of a sound source** and **whether that source should be represented to the player**.

Simply displaying every configured emitter would effectively become a world-information radar rather than an audio accessibility system.

The intended perceptibility calculation combines several factors.

## Distance Attenuation

The prototype code uses a simple linear approximation:

```text
Attenuation = 1 - Distance / MaximumRange
```

with the result clamped between zero and one.

This is intentionally simple.

A production implementation should ideally use or approximate the actual attenuation behaviour of the game's audio system rather than maintaining an unrelated second attenuation model.

## Occlusion

The manager also contains a line-trace based `CheckOcclusion()` function.

The concept is that an obstacle between listener and source reduces the effective audibility of that source.

The profile's `OcclusionPenalty` determines the intended reduction.

This is only an approximation of acoustic obstruction. Real audio propagation depends on material, geometry, diffraction, reflections and the game's audio implementation.

Therefore, while the code establishes an occlusion mechanism, I do not claim that the prototype provides a physically accurate acoustic simulation.

More importantly, I did not complete sufficient practical testing of this behaviour during the final prototype stage to present it as validated functionality.

## Hearing Sensitivity

Another experimental property is `HearingSensitivity`.

The original concept proposed that a very loud event such as an explosion or flashbang could temporarily reduce the visibility of quieter audio events before gradually recovering.

This was intended to model the fact that E.A.R.D.A.R. should represent what a hearing player could perceive rather than providing permanently perfect information.

The manager contains:

```cpp
void ApplyLoudEvent(float EventIntensity)
```

and recovery logic in `Tick()`.

However, this remained a code-level feature and was not integrated into a sufficiently developed gameplay scenario for meaningful evaluation.

It should therefore be considered an experimental extension of the architecture rather than a demonstrated final feature.

---

# Testing and Evaluation

Testing during the prototype concentrated on validating individual links in the E.A.R.D.A.R. pipeline.

Because several systems interact, testing them incrementally was considerably more useful than attempting to build the entire interface before running it.

## Test 1 — Widget Creation

The first requirement was verifying that the C++ manager could create the correct `WBP_EardarRadar` class and add it to the viewport.

The visible E.A.R.D.A.R. interface confirmed this stage.

## Test 2 — C++ to Blueprint Event

`Print String` was connected to `Event UpdateBlips`.

Repeated output confirmed that the C++ manager was calling the Blueprint event.

This isolated the communication boundary before dynamic widgets were introduced.

## Test 3 — Empty Emitter Collection

With no configured emitter, the array length reported:

```text
0
```

This confirmed that the system did not invent a blip when none existed.

## Test 4 — Single Emitter

A cube actor was given an `EardarEmitterComponent`.

The reported value changed to:

```text
1
```

This confirmed successful integration between a world actor, its component, the manager and the widget.

## Test 5 — Multiple Emitters

Four emitters were placed around the player.

Multiple widgets were generated, demonstrating that the system was not limited to a single source.

## Test 6 — Radial Distance

The four emitters were deliberately positioned at different distances.

Their glyphs appeared at different distances from the centre of the HUD.

This provided visual evidence that `NormalizedRadius` was influencing the UI.

## Test 7 — Camera Rotation

The camera was rotated while the emitters remained in the world.

The N, S, E and W glyphs changed position around the centre.

This demonstrated the camera-relative nature of `BearingDeg`.

Together, these tests validate the main functional goal achieved by the prototype:

> configured world-space emitters can be transformed into dynamic player-relative visual information.

---

# Limitations

The final prototype is significantly smaller in scope than the original E.A.R.D.A.R. proposal.

It is therefore important to distinguish between demonstrated functionality and planned architecture.

## Live Audio Detection

The prototype does not currently perform full live analysis of arbitrary game audio.

`SourcePlaybackRMS` remains a placeholder.

Consequently an emitter is primarily a configured semantic object rather than the result of analysing the game's mixed audio output.

Future development should connect the system to actual playback state and amplitude data.

## Acoustic Validation

Distance attenuation and occlusion calculations exist in the source architecture, but they have not received sufficient controlled testing to demonstrate that the resulting visibility accurately represents what a hearing player would perceive.

This would require a more rigorous test environment containing known distances, obstacles and sound levels.

## Temporary Hearing Sensitivity

The code supports an experimental `HearingSensitivity` value and `ApplyLoudEvent()`, but this was not demonstrated through a completed explosion or flashbang scenario.

## DataTable Profiles

The DataTable profile architecture exists in the C++ design, but a comprehensive set of production sound profiles was not authored and tested.

The final spatial tests instead relied on emitter overrides.

## Placeholder Glyphs

N, S, E and W are development glyphs rather than final accessible iconography.

A finished system would require a carefully designed icon language representing events such as footsteps, speech, gunfire, explosions and vehicles.

These should be evaluated for readability at different sizes and ideally tested with DHH users.

## Accessibility Configuration

The planned DHH, Minimal and High-Contrast presets were not implemented.

Neither were configurable icon scale, opacity, radar radius or alternative contrast settings.

This is particularly significant because my accessibility research emphasised customisation. A future version must address this before E.A.R.D.A.R. could reasonably be described as a mature accessibility tool.

## User Testing

The largest limitation is the absence of structured testing with the intended DHH audience.

Technical correctness does not automatically mean that an accessibility interface is useful.

Only players can determine whether the visualisation is understandable, distracting, tiring, helpful or overwhelming during real gameplay.

Consequently, the final prototype validates a technical concept rather than validating its accessibility effectiveness.

## Performance

The Unreal prototype has only been tested with a small number of emitters.

The manager architecture potentially performs work for each emitter every update, and the current Blueprint implementation recreates child widgets.

A production implementation should use event-driven emitter registration, update throttling, spatial filtering and widget pooling.

---

# Reflection

The development of E.A.R.D.A.R. changed significantly from its initial concept.

At the beginning I approached the project largely as an interesting programming problem: detect sound information, calculate its position and draw it on a radar.

My accessibility research made the problem more complex but also more meaningful.

The question changed from:

> How can I display sound?

to:

> What information should be displayed so that the interface provides useful equivalence without creating an unfair or overwhelming information advantage?

This distinction affects almost every part of the architecture.

It is why concepts such as perceptibility thresholds, attenuation, occlusion, on-screen suppression and hearing sensitivity appeared in the design.

The project also reinforced the importance of separating technical systems.

`UEardarEmitterComponent` identifies a potential source.

`AEardarManager` processes the source.

`FEardarBlip` describes the result.

`UEardarRadarWidget` provides the interface boundary.

`WBP_EardarRadar` displays it.

This separation made debugging possible when the prototype began producing incorrect visual results. In particular, the UMG positioning problem demonstrated that incorrect output does not necessarily mean that the underlying mathematical data is wrong.

The N/S/E/W test was another useful lesson in prototype design. Placeholder content can be more useful than polished assets when it is selected to answer a specific technical question. The four letters allowed me to observe emitter identity, bearing and movement simultaneously.

There were also lessons in scope management.

Attempting to develop equivalent Unity and Unreal versions alongside acoustic modelling, sophisticated UI, accessibility presets and user testing was too ambitious for the final implementation period.

The Unreal prototype is therefore narrower than the original specification.

However, it establishes an important vertical slice through the system:

```text
Configured emitter
       ↓
World position
       ↓
Manager
       ↓
Bearing + distance
       ↓
C++ / Blueprint interface
       ↓
Dynamic visual glyph
```

This provides a foundation that can be developed incrementally rather than requiring the entire accessibility system to be redesigned.

---

# Future Development

The next stage of E.A.R.D.A.R. should focus on turning the current spatial prototype into a genuine audio-driven system.

The first priority would be connecting emitters to actual sound playback.

A blip should only qualify while a relevant sound is active or within its persistence window.

The second priority would be systematic testing of attenuation and occlusion.

A controlled test map could contain emitters at known distances with different types of obstruction. The resulting E.A.R.D.A.R. output could then be compared against Unreal's audio behaviour.

The profile system should then be completed.

Gameplay Tags and `FEardarProfileRow` DataTables could allow designers to define categories such as:

```text
Sound.Footstep
Sound.Gunshot
Sound.Explosion
Sound.Vehicle
Sound.Voice
```

without editing C++.

The HUD would then need to move beyond placeholder letters.

A consistent icon set should communicate sound category through shape, with colour used only as redundant information rather than the sole differentiator.

Accessibility options should include at minimum:

- HUD scale;
- glyph scale;
- high-contrast presentation;
- opacity;
- maximum displayed blips;
- visual intensity;
- radar radius;
- optional distance information;
- alternative display modes.

The planned on-screen suppression system should also be evaluated. Avoiding redundant icons for objects that are already clearly visible could reduce clutter, but it should not remove information that the user still finds useful.

Performance should be improved by replacing continuous discovery and widget recreation with registration and pooling.

Finally, the most important future step would be user testing.

DHH players should be involved in evaluating the interface and shaping subsequent iterations. Questions should include whether the radar is understandable without explanation, whether icons can be interpreted during fast gameplay, how much information becomes overwhelming, which events deserve prioritisation, and whether the interface actually improves situational awareness.

The results may require changing assumptions made during the prototype.

That would not represent failure. Iteration based on the intended users is an essential part of accessibility design.

---

# Conclusion

E.A.R.D.A.R. began as an ambitious proposal for an audio accessibility system spanning Unreal Engine and Unity. The original concept included semantic sound classification, perceptibility modelling, distance attenuation, occlusion, temporary hearing sensitivity, prioritisation and configurable visualisation.

The completed Final Major Project prototype does not realise every aspect of that specification.

Instead, development concentrated on establishing the technical foundation in Unreal Engine 5.6.1.

The resulting prototype demonstrates that actors configured with `UEardarEmitterComponent` can enter a central processing system, be represented as `FEardarBlip` data, cross the C++/Blueprint boundary through `UEardarRadarWidget`, and appear dynamically in `WBP_EardarRadar`.

Most importantly, the system successfully converts the three-dimensional relationship between player and emitter into a camera-relative bearing and normalised radial distance. Testing with four individually identifiable N, S, E and W emitters demonstrated that glyph positions respond as the camera rotates and that differences in emitter distance are represented radially.

This establishes the core spatial visualisation principle on which the wider E.A.R.D.A.R. concept depends.

The project also changed my understanding of accessibility development. My research demonstrated that accessibility is not simply achieved by adding more information to the screen. An accessible interface must consider clarity, equivalence, customisation, cognitive load and the needs of the players for whom it is intended.

For that reason, I consider the current E.A.R.D.A.R. implementation a **functional technical prototype rather than a completed accessibility solution**.

Its greatest value is the foundation it provides for subsequent work: integrating real audio playback, validating perceptibility rules, designing accessible glyphs, implementing configurable display modes, improving performance and, most importantly, testing the system with deaf and hard-of-hearing players.

Although the final implementation is smaller than originally planned, it demonstrates a complete path from a configured environmental source to a dynamic visual representation of its spatial relationship to the player. This gives E.A.R.D.A.R. a working technical basis from which its original accessibility goals can continue to be developed.
