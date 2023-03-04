# **Li**ghtweight **V**oxel **R**ender **E**ngine
As of right now there isn't any functionality present. But this is here as a reference (later to be moved to the github wiki thing) for the various options and error messages (before I forget).

## Compile Options
There isn't a CMAKE option yet, but removing `-DLIVRE_LOGGING` removes the automatic logging functionality. *This will make error messages non-existant, so if specific debugging info is needed, keep this here*.

## Status Code Reference
Here is a reference for the various status codes.

### Shaders 

#### `STATUS`
These are the values of `Shader::STATUS` returned after performing various functions.

| Code | Enum                        |
|------|-----------------------------|
| 0    | SUCCESS                     |
| 1    | OPENGL_CREATEPROGRAM_FAILED |
| 2    | SHADER_NOT_MADE             |
| 3    | OPENGL_CREATESHADER_FAILED  |
| 4    | FILE_DOESNT_EXIST           |
| 5    | SHADER_COMPILE_ERROR        |
| 6    | PROGRAM_LINK_ERROR          |
| 7    | COMPILED                    |
| 8    | CREATED                     |
| 9    | DELETED                     |

#### `TYPE`
These are the values of `Shader::TYPE`.

| Code | Enum     |
|------|----------|
| 0    | VERTEX   |
| 1    | FRAGMENT |
| 2    | COMPUTE  |


### VertexObject

#### `STATUS`
These are the values of `VertexObject::STATUS` returned after performing various functions.
| Code | Enum                |
|------|---------------------|
| 0    | SUCCESS             |
| 1    | DOESNT_HAVE_BUFFER  |
| 2    | ALLOCATION_FAILURE  |
| 3    | COPY_FAILURE        |
