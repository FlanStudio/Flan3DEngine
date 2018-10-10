# Flan3DEngine

3D Engine made by Jonathan Molina and Oriol de Dios during the third year of Game Design and Development degree at CITM/UPC.

Link to [Github Repository](https://github.com/FlanStudio/Flan3DEngine)

Link to [**Jonathan Molina** Github](https://github.com/Jony635)

Link to [**Oriol de Dios** Github](https://github.com/orioldedios)

## Game Engine Instructions:

### Main Menu Bar
 **Application**

* _**Quit**_: Quit the engine.

 **View**

* _**Demo Window**_: Opens a UI demo window.

* _**MGL Window**_: For spawning spheres and knowing if they are colliding.

* _**LOG**_: Opens a window that shows LOGs, WARNINGs and ERRORs.

* _**Config**_

 _Application_: Shows Engine name, Organization, Max FPS, current FPS and last ms.

 _Window_: Shows Refresh Rate and allows to change config values (Brightness, Width, Height, Fullscreen, Fullscreen Desktop, Borderless and Resizable).

 _Renderer*_: Shows some renderer modes.

 _Hardware_: Shows some Engine hardware information.

* _**About**_: Shows a window with a description of the engine and links to authors Githubs, Libraries used and License.

## Innovation:

- **ImGui_Tabs** module from [HERE](https://github.com/scottmudge/imgui_tabs), implemented in the Log Window and updated to the actual version of ImGui.
- **FileSystem** using [PHYSFS](https://icculus.org/physfs/) and implemented in all textures and meshes loading.
- **LogWindow** using [this](https://github.com/ocornut/imgui/issues/300) example the creator of ImGui posted in an issue.
