**Reminder**
`Any transformation can be described in terms of a rotation/reflection, scale then another rotation. One rotation only would not be enough for all transformations (e.g. shear)`

**Depth Test**
`demonstrated last lab.`
`Just be reminded that the right approach is to consider the depth value of each fragment pixel and then decide if it should be drawn or not (painter's algorithm assumed one depth value over the whole object which isn't necessarily the case.)`

**1 - Depth & Color Mask**
<span class="colour" style="color:rgb(92, 99, 112)">`suppose a transparent object was closer to another one then even though it passes the depth test we don't`</span>
<span class="colour" style="color:rgb(92, 99, 112)">`want it to overwrite the opaque object behind it. Thus we would disable writing to the depth buffer in this case.`</span>
<span class="colour" style="color:rgb(92, 99, 112)">`This is just for illustration won't use it this lab (the 2 lines are set to default, can comment them)`</span>

**2 - Culling**
`A cube has 6 faces but we never see them all at once.`
`The computer draws all 6 (but then uses depth test to decide which to be rendered).`

`Side note: Sometimes, 2 objects might might have the same depth value. In this case you'd see the window mixing between which of them to render depending on floating point errors. This is known as z-fighting and is not a favourable feature.`

<br>
`Now back to the cube. Wouldn't it improve performance if we don't draw the whole cube -> Use back face culling (mainly used for optimization).`
`This would for instance cause us to see nothing if we try to see the cube from inside (or a 2D triangle from the back). The order of the vertices is`
`what decides which face is the front and which is the back. Given a specific order of vertices (e.g. red -> green -> blue triangle vertices) in the`
`vertex buffer then for one of the faces they will be ordered in clockwise fashion and for the other they will be ordered in anti-clockwise fashion.`
`If we define the anti-clockwise ordering to be the back face then whenever they are ordered in that way we won't see the face if back face culling is enabled.`

**Note)**
`In the EBO in the code if we graph each triangle we will see that the vertices are aligned in anti-clockwise fashion.`
`So if we enable back or front face culling both would be enabled/disabled together.`
`Now if we let the second triangle use the vertices in an order such that 2, 0, 3 it will this time lead to a clockwise order in the graph.`

`This means that enabling back/front face culling would only show one of the two triangles at a time.`

<br>
<br>
**3 - Scene Graphs**
// No code written for this part.
`Suppose our world space is earth and a human is walking on it while moving their forearm up and down.`
`It might seem complex to model the movement of such human (the forearm undergoes two types of motion relative to the world: up and down, horizontally).`
`Dealing with the problem in hierarchical fashion (tree <-> scene graph) solves it. Use a matrix that takes you from the forearm space to the arm space (this one should capture forearm movement) then from the arm space to the human space then from the human space to the world space (this one to capture human movement).``So we can apply a transformation (motion) on a child object relative to its parent and then observe the overall effect on the whole object by multiplying`
`all transformation matrices up to the root of the tree (yields the effect in the world space).`
`Will probably need this in project.`

<br>
**4 - ViewPort Transformations**
`We mentioned before that it's automatically handled by OpenGL.`
`Can still modify it so that we for instance draw different things in different parts of the screen.`
`e.g. split screen gaming`

`Simply use`
`glViewport(x, y, width, height) to define the section before drawing. (x, y) is for lower left corner.`
`use coordinates in the screen space (lower left is (0,0)). (meanwhile in NDC (0,0) is at the middle -> we're coming from this)`

`Now we can do:`
`glViewPort for first section`
`draw`
`glViewPort for second section`
`draw`

`Suppose we want as well to let them have different background colors. glClear won't respect that.`

`Solution is to use glScissors. Use it with the same input as glViewport.`

`Now we can do:`
`glViewPort for first section`
`glScissor for first section`
`clear (now it will respect the section)`
`draw`
`glScissor for second section`
`clear (now it will respect the section)`
`draw`

`Note about scissors)`
`Can be used to further cut a part from the viewport section (by passing a smaller quad than that of viewPort).`

<br>
**5 - More Notes**
`More Notes)`

`Transform camera by translation to the right has same effect as transforming whole world to the left (inverse transformation.) `
`In the scene graph, we may write a function that transforms the local space of the object (whatever it is) to the world space. For the camera object this would give them "camera space to world space transformation matrix". Now since we know `
`that later we will need to transform all objects to the camera space we can derive the corresponding matrix easily by `
`taking the inverse of camera to world matrix to get a matrix that takes us from world space to camera space and then apply it on all.(Thus, Mcam in the slides is the inverse of Mcam in the lecture).`

`Orthographic height -> distance between top and bottom planes in the orthographic view volume.`
`Can do orthographic  projection on glm (provide the 6 planes for orthographic view volume)`

`Perspective matrix is the only one which modifies the last row (relating a variable to w) which we scale by it (and hence the related variable affects all coordinates.)`

`In the vertex shader don't divide by w. It's needed to correct interpolation. Otherwise we would notice unexpected wraping.`
<span class="colour" style="color:rgb(35, 38, 41)">` "The rasterizer `</span>*`needs`*<span class="colour" style="color:rgb(35, 38, 41)">` the perspective information provided by the W component to do its job. With a W of 1, the interpolation is done in window space, without any regard to perspective." which indeed intuitively would cause wrapping.`</span>

`In the project inheritance is not recommended (complicates/restricts things).Entity Component System (data-oriented) is what we should consider.`
`Each entity has components defining the data and there is a seperate system involving logic (it would look for the needed components to apply logic on in the world (which contains many entities)). It as well makes converting the overall structure of the current contex into a file (serialization) or vice versa (deserialization) easy. `

<span class="colour" style="color:rgb(35, 38, 41)"></span>