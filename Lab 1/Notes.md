***OpenGL***
<span class="colour" style="color:rgb(77, 81, 86)">`(Open Graphics Library) is a cross-language, cross-platform application programming interface (API) for rendering 2D and 3D vector graphics.`</span>

- - -

**GLFW (Graphics Library Framework)**<span class="colour" style="color:rgb(77, 81, 86)"></span>
<span class="colour" style="color:rgb(77, 81, 86)">`An Open Source, multi-platform library for OpenGL (C language).`</span>
So essentially, OpenGL is the API and GLFW is the library built on top of it.

- - -

**GLSL**
`GL Shader language. Used to write the vertex and fragment shaders. We say that it's a C-style language (more high level than C)`

- - -

**GLAD**
<span class="colour" style="color:rgb(92, 99, 112)">`An OpenGL loading library that manages function pointers for OpenGL. It abstracts away the difference between the loading mechanisms on different platforms. (There are many other loading libraries as well.)`</span>

<br>
Some more terminology:

**Buffer Object**
`An object that represents a linear array of memory, which is stored in the GPU. There are numerous ways to have the GPU access data in a buffer object.`

**OpenGL Context**
<span class="colour" style="color:rgb(32, 33, 34)">`A context stores all of the state associated with this instance of OpenGL. Involves all the bindings, shaders, textures,... part of the current state`</span>