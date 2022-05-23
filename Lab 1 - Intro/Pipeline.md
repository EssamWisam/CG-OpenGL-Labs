## <span class="colour" style="color:rgb(39, 39, 39)"></span>Vertex Shader

```
A vertex shader is a graphics processing function, which manipulates vertex data values on an X (length), Y (height) 
and Z (depth) 3D plane through mathematical operations on an object.These variations can be differences in color,
texture coordinates, orientations in space,...

It can only manipulate the vertices. For every vertex to be processed, the shader program executes.
```
<br>
## Primitive Assembly
<br>
```
The primitive assembly stage takes as input all the vertices from the vertex shader
that form a primitive and assembles all the point(s) in the primitive shape given (e.g. triangle).
```
<br>
## Geomtery Shader
<br>
```
The output of the primitive assembly stage is passed to the geometry shader. The geometry shader takes as 
input a collection of vertices that form a primitive and has the ability to generate other shapes by emitting
new vertices to form new (or other) primitive(s).
```
<br>
<span class="colour" style="color:rgb(17, 17, 17)">`The output of the geometry shader is then passed on to the `</span>`rasterization stage`<span class="colour" style="color:rgb(17, 17, 17)">` where it maps the resulting primitive(s) to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.`</span>

<br>
## Fragment(Pixel) Shader
<br>
```
The main purpose of the fragment shader is to calculate the final color of a pixel and this is usually the stage where
all the advanced OpenGL effects occur. Usually the fragment shader contains data about the 3D scene that it can use to 
calculate the final pixel color (like lights, shadows, color of the light and so on).

There's also an alpha blending stage that modifies the colors according to the alpha value.
```
<br>
<span class="colour" style="color:rgb(17, 17, 17)">`In modern OpenGL we are `</span>**`required`**<span class="colour" style="color:rgb(17, 17, 17)">` to define at least a vertex and fragment shader of our own (there are no default vertex/fragment shaders on the GPU).`</span>