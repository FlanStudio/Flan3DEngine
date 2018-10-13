# Flan3DEngine

3D Engine made by Jonathan Molina and Oriol de Dios during the third year of Game Design and Development degree at CITM/UPC.

Link to [Github Repository](https://github.com/FlanStudio/Flan3DEngine)

Link to [**Jonathan Molina** Github](https://github.com/Jony635)

Link to [**Oriol de Dios** Github](https://github.com/orioldedios)

## Game Engine Instructions:

### Main Menu Bar
 **Application**

* _**Quit**_: Quit the engine.

 **View** (All windows quit pressing **ESC**).

* _**Demo Window**_: Opens a UI demo window.

* _**MGL Window**_: For spawning spheres and knowing if they are colliding.

* _**LOG**_: Opens a window that shows LOGs, WARNINGs and ERRORs.

* _**Config**_

 _Application_: Shows Engine name, Organization, Max FPS, current FPS and last ms.

 _Window_: Shows Refresh Rate and allows to change config values (Brightness, Width, Height, Fullscreen, Fullscreen Desktop, Borderless and Resizable).

 _Renderer*_: Shows some renderer modes.

 _Hardware_: Shows some Engine hardware information.
 
 * _**Properties**_: Opens a window that shows information of the object **transformations**, **Geometry** (triangles and vertices) and **Textures**.

* _**About**_: Shows a window with a description of the engine and links to authors Githubs, Libraries used and License.

### Game
 **Camera Movement**

  * _**W, A, S, D, Q, E**_: Move **forward**, **left**, **backward**, **right**, **downwards** or **upwards**.
    
  * _**Mouse Scroll**_: Makes a **zoom-in**/**zoom-out**.
  
  * _**Alt + Left Click + Mouse Movement**_: Centers the object and moves around it.
  
  * _**Right Click + Mouse Movement**_: Camera rotates.
  
  * _**F**_: Centers the view to the object.
  
 **Other functionalities**
  * _**Drag and Drop**_: an FBX will be rendered if it's dropped into the engine.Also a texture can be dropped to (PNG, DDF).
 
## Innovation:

- **ImGui_Tabs** module from [HERE](https://github.com/scottmudge/imgui_tabs), implemented in the Log Window and updated to the actual version of ImGui.
- **FileSystem** using [PHYSFS](https://icculus.org/physfs/) and implemented in all textures and meshes loading.
- **LogWindow** using and modifying [this](https://github.com/ocornut/imgui/issues/300) example the creator of ImGui posted in an issue.

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
