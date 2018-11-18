# Flan3DEngine

3D Engine made by Jonathan Molina and Oriol de Dios during the third year of Game Design and Development degree at CITM/UPC.

Link to [Github Repository](https://github.com/FlanStudio/Flan3DEngine)

Link to [**Jonathan Molina** Github](https://github.com/Jony635)

Link to [**Oriol de Dios** Github](https://github.com/orioldedios)

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

  * _**Play**_: Puts the scene in playmode using an scalated dt and showing the view of the main camera.
   
  * _**Stop**_: Returns to the Editor mode and discards all the changes maked in the Play mode.
  
  * _**Pause**_: Pauses the scene (puts dt to 0) when you are in the Play mode (press it again for resuming).
  
  * _**Step**_: Makes one loop and pauses again.
  
 **Other functionalities**
  * _**Drag and Drop**_: FBX meshes and textures (in formats DevIL supports) dropped into the engine are loaded. 
 
## Innovation:

- **ImGui_Tabs** module from [HERE](https://github.com/scottmudge/imgui_tabs), implemented in the Log Window and updated to the actual version of ImGui.
- **FileSystem** using [PHYSFS](https://icculus.org/physfs/) and implemented in all textures and meshes loading.
- **LogWindow** using and modifying [this](https://github.com/ocornut/imgui/issues/300) example the creator of ImGui posted in an issue.
- **Serializing in Binary Files**
- **Materials tinting plain colors and textures**

## Assets

- **Trex** from [HERE](https://www.turbosquid.com/3d-models/t-rex-fbx-free/933905) by [joel3d](https://www.turbosquid.com/Search/Artists/joel3d).
- **Gun** from [HERE](https://www.turbosquid.com/3d-models/3d-model-pbr-flare-gun-1303070) by [soidev](https://www.turbosquid.com/Search/Artists/soidev).

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
