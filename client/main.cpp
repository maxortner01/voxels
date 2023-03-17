#include "livre/livre.h"

#include <iostream>

using namespace livre;

int main()
{
    Window window(1280, 720);

    GraphicsPipeline pipeline(window.getInstance());
    pipeline.getVertexShader().fromFileAsGLSL("vertex.glsl");
    pipeline.getFragmentShader().fromFileAsGLSL("fragment.glsl");
    pipeline.create();

    Renderer::CreateInfo info;
    Renderer renderer(info, window.getInstance());

    ModelObject object;

    /*
    Shader shader(Shader::TYPE::VERTEX | Shader::TYPE::FRAGMENT);
    
    shader.fromFile(Shader::TYPE::VERTEX, "vertex.glsl");
    shader.fromFile(Shader::TYPE::FRAGMENT, "fragment.glsl");
    shader.link();

    Vec3f vertices[] = {
        Vec3f(-0.5, 0.5, 0), Vec3f(0.5, 0.5, 0), Vec3f(0.5, -0.5, 0), Vec3f(-0.5, -0.5, 0)
    };

    uint32_t indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0
    };

    VertexObject object(BufferObject::BUFFERTYPE::VERTEX);
    VertexObject::STATUS i = object.loadIndices(&indices[0], 8);
    if (i != VertexObject::STATUS::SUCCESS)
    {
        std::cout << "error loading indices\n";
    }

    VertexObject::STATUS c = object.loadVertices(&vertices[0], 4);
    if (c != VertexObject::STATUS::SUCCESS)
    {
        std::cout << "error loading vertices\n";
    }

*/
    window.setTitle("Main");

    while (window.open())
    {
        window.pollEvents();
        //object.draw(pipeline);
        //renderer.draw();

        renderer.startFrame();
        renderer.draw(object, pipeline);
        renderer.endFrame();
    }

    window.waitForIdle();

    return 0;
}