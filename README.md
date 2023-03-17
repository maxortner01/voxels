# **Li**ghtweight **V**oxel **R**ender **E**ngine
As of right now there isn't any functionality present. But this is here as a reference (later to be moved to the github wiki thing) for the various options and error messages (before I forget).

## Compile Options
There isn't a CMAKE option yet, but removing `-DLIVRE_LOGGING` removes the automatic logging functionality. *This will make error messages non-existant, so if specific debugging info is needed, keep this here*.

## Structure
The `Renderer` object does all the important stuff. A call to `renderer.startFrame()` begins the recording for the frame. This should be followed by the various draw commands. Then `renderer.endFrame()` finishes the command buffer and submits it to the queue. 

### Draw Commands
All draw calls require a renderer as well as a pipeline. The pipeline describes how the drawing is to be done, the renderer is the utility that performs the drawing. Any drawable object can be rendered with `renderer.draw(object, pipeline)`.