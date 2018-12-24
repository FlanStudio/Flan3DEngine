# Flan3DEngine

3D Engine made by Jonathan Molina and Oriol de Dios during the third year of Game Design and Development degree at CITM/UPC.

Link to [Github Repository](https://github.com/FlanStudio/Flan3DEngine)

Link to [**Jonathan Molina** Github](https://github.com/Jony635)

Link to [**Oriol de Dios** Github](https://github.com/orioldedios)

## Execution Instructions:
Extract the "Mono.zip" content into the "Game" folder and execute "Flan3DEngine.exe".  
You have all the required files in the Github Releases page.  

Let's build some games!

## If you find a bug or a weird behavior, follow the following steps:
1. Delete the Library folder.
2. Delete all the .meta files in Assets.
3. Press the Windows key and write %appdata%, after that press enter
4. Search for the FlanStudio/Flan3DEngine/lastAssetsState.f file and delete it.

If you followed those steps and the problem didn't solve, feel free to post an issue at the following link, indicating the most accurately possible the steps you have followed until arriving there. Thank you for your feedback!

## Game Engine Instructions:

### Main Menu Bar
 **Application**

* _**Quit**_: Quit the engine.

* _**Rename loaded scene**_: Allows You to rename the current scene.

* _**Save scene**_: Saves current scene.

 **View** (All windows can be closed pressing **ESC**).

* _**Inspector**_: Shows all GameObject components and allows You to add more.

* _**Hierarchy**_: Shows all GameObjects of the scene and allows you to create more (try to Right Click on the window).

* _**FileSystem**_: Shows the Assets folder (inside it you can find different scenes, meshes, textures...).

* _**Demo Window**_: Opens the ImGui demo window.

* _**MGL Window**_: For spawning spheres and knowing if they are colliding.

* _**LOG**_: Opens the console that shows LOGs, WARNINGs and ERRORs.

* _**Config**_

     _Application_: Shows Engine **name, Organization, Max FPS, Game Scale, current FPS** and **last ms**.

     _Window_: Shows Refresh Rate and allows to change config values (**Brightness, Width, Height, Fullscreen, Fullscreen Desktop, Borderless and Resizable**).

     _Renderer_: Shows renderer modes (**Depht Test, Lightning, Texture2D, Normals, Cull Face, Color Material, Wireframe Mode**).

     _Hardware_: Shows Engine hardware information (**SDL Version, CPUs, System Ram, Caps, Brand, GPU, OpenGL version supported, GLSL** and **VRAM usage**).
 
 * _**Properties**_: Opens a window that shows information of the object **Transformations**, **Geometry** (triangles and vertices) and **Textures**.

* _**About**_: Shows a window with a description of the engine and links to authors Githubs, Libraries used and License.

### Game
 **Camera Movement**

  * _**UP, LEFT, DOWN, RIGHT, Y, H**_: Move **forward**, **left**, **backward**, **right**, **downwards** or **upwards**.
    
  * _**Mouse Scroll**_: Makes a **zoom-in**/**zoom-out**.
  
  * _**Alt + Left Click + Mouse Movement**_: Rotate around the loaded geometry.
   
  * _**Right Click + Mouse Movement**_: Camera rotates around itself.
  
  * _**F**_: Centers the view to the object.
  
  * _**A**_: Centers the view to the scene.

**Transformations**

  * _**Q,W,E,R**_: **Pick object**, **Translate object**, **Rotate object**, **Scale object** (also, there's a button for each transformation and one for changing the transformation mode from "local" to "global" and vice versa).
  
**Time**

  * _**Play**_: Join in Game Mode, where all the game-relative subsystems will start working.
   
  * _**Stop**_: Returns to the Editor mode and discards all the changes maked in the Play mode.
  
  * _**Pause**_: Pause the game relative subsystems execution.
  
  * _**Step**_: Advances 1 frame all the paused game relative subsystems
  
 **Other functionalities**
  * _**Drag and Drop**_: FBX meshes and textures (in formats DevIL supports) dropped into the engine are loaded. 

## Main Subsystem: Scripting with C#

### Create new Scripts
  
   Select a GameObject in the Hierarchy, go to the Inspector window and click AddNewComponent. Select Script and choose a name for it. Flan3DEngine will create automatically your .cs file in Assets/Scripts.
   
   In the Root folder you will have a VisualStudio project for all your Scripts, included automatically.
  
### Coding Scripts
   
   Once you press Play, the logic inside your scripts will start executing. Flan3DEngine will automatically call some default callback methods you can override from FlanScript. For now, those callback methods are:
      
   - **OnEnable:** Called every time you enable a ComponentScript (or it's GameObject or one of it's parents).
   - **Awake:** Called only once when the Script is Enabled.
   - **Start:** Called after Awake only if the Script is enabled at the right moment you press Play.
   - **PreUpdate:** Called before Update (once by frame). 
   - **Update:** Called once by frame.
   - **PostUpdate:** Called after Update (once by frame).
   - **OnStop:** Called when you exit the Play Mode.
   - **OnDisable:** Called every time you disable a ComponentScript (or it's GameObject or one of it's parents).
   
### Edit public fields
  
   All your supported public and non-Static fields declared in the script will be serialized in the ComponentScript's OnInspector.        
   
   Their values can be changed in real-time, and they are relatives to each ComponentScript. You can have different values for them in different ComponentScripts using the same .cs file.

### Hot Reloading
  
   You can edit a .cs file while the Engine is executing. These will reset all the edited public fields through the oninspector. Doing this while the Play Mode is Active can cause some weird errors depending of your script. 
  
### Referencing GameObjects, Transforms and Prefabs
  
   If you create a public GameObject or Transform field in your script, you will be able to drag there other GameObjects in the Hierarchy or a Prefab through the Filesystem window.
  
### Utilities implemented in C#
  
   Through a Script you can instanciate an delete GameObjects using Prefabs or another GameObject as a template or simply creating default ones. You can access their names and edit them, also like their transforms (move, rotate and scale them).
   
   You have convenient methods to rotate quaternions, converting Objects to String, accessing the global transformation values and so on.
   
   You can access to the Input class to get user inputs from the keyboard and the mouse. You also have the Time Class for accessing time-related data.    

## Innovation:

- **ImGui_Tabs** module from [HERE](https://github.com/scottmudge/imgui_tabs), implemented in the Log Window and updated to the actual version of ImGui.
- **FileSystem** using [PHYSFS](https://icculus.org/physfs/) and implemented in all textures and meshes loading.
- **LogWindow** using and modifying [this](https://github.com/ocornut/imgui/issues/300) example the creator of ImGui posted in an issue.
- **Serializing in Binary Files**
- **Materials tinting plain colors and textures**
- **HotReloading Scripts**
- **Prefabs** without Apply/Revert functionality.

## Assets

- **Tank & Bullet** from [HERE](https://assetstore.unity.com/packages/essentials/tutorial-projects/tanks-tutorial-46209) by [Tutorial Projects](https://assetstore.unity.com/categories/essentials/tutorial-projects).

## FlanStudio Stuff
![fst](https://user-images.githubusercontent.com/25585362/50057843-59702f00-0170-11e9-82d3-4bb33034a5a6.png)

### Tasks distribution:

   **Jonathan Molina-Prados:** GameObject system, Prefabs, Mono Domain Management, Scripts Compilation, Detect Script Changes, Textures, Textures tint, parent/unparent gameobjects, gameobjects instanciation from C#, destroy gameobjects, refresh gameobjects, management/storagement of gameobject/monoObjects, drawing meshes, FBX exportation, scene serialization, creation and refreshment of the VisualStudio's project including .cs file every time new one is created...
   
   **Oriol de Dios:** Implementing PCG - Random Num Generator, editor Camera Controls, windows Layout/custom theme, enablig/disabling GameObjects/Components behaviour, AABB system for GameObjects, time Module (Play, Pause, Step, all dt calculation and more time stuff...), Mouse Input functionality in C#, calling C# callback methods, Standard C# variables OnInspector, Serializing/deSerializing public fields in ComponentScripts.
    
Check Flan3DEngine's web [here:](https://flanstudio.github.io/Flan3DEngine/)

Check Flan3DEngine's features [Video:](https://www.youtube.com/watch?v=n-EmgHmp-uE&feature=youtu.be)

## License
MIT License

Copyright (c) 2018 Jonathan Molina-Prados and Oriol de Dios.

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
