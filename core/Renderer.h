#pragma once

#include <OpenGLProgram.h>
#include <stdio.h>
#include <EventHandler.h>
#include <Scene.h>

namespace ssCore
{
    template <class T>
    class Renderer
    {
    /*
    * This class is a template class that must be inherited by the renderer class
    * The derived class must implement the following functions:
    * Start()
    * RenderFirstPass()
    * RenderMainPass()
    * End()
    * The derived class must also have a constructor and destructor
    */
    public:
        Renderer(std::shared_ptr<Scene> scene)
		    :scene(scene)
        {
        }
        ~Renderer()
        {
        }

        /*
         * Called before the application loop starts
         */
        void Initialize()
        {
            // Initialize OpenGL
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                spdlog::info("Failed to initialize OpenGL context");
                return;
            }
            GL_CALL(glDebugMessageCallback(GLDebugMessageCallback, NULL));
            program = std::make_unique<OpenGLProgram>();

		    static_cast<T*>(this)->Start();
        }

        /*
         * Called every frame in the application loop
         */
        void Render()
        {
            // Swap the buffers
            EventHandler::GetInstance().SwapBuffers();

            // Clear the screen
            program->Clear();

            // First render pass: Shadows, reflections etc.
            static_cast<T*>(this)->RenderFirstPass();

            // Main render pass
            static_cast<T*>(this)->RenderMainPass();
            
            frameCount++;
        }

        void Terminate()
        {
            // Clean up here
            static_cast<T*>(this)->End();
        }

    protected:
        std::unique_ptr<OpenGLProgram> program;
	    std::shared_ptr<Scene> scene;
        long int frameCount = 0;

    //==============================================================
    // These functions must be implemented by the derived class
        /*
        * Called before application loop inside Initialize
        */
        virtual void Start() = 0;
        /*
        * Called in application loop inside Render before RenderMainPass
        */
        virtual void RenderFirstPass() = 0;
        /*
        * Called in application loop inside Render after RenderFirstPass
        */
        virtual void RenderMainPass() = 0;
        /*
        * Called after application loop inside Terminate
        */
        virtual void End() = 0;
    //================================================================
    };

    class TestRenderer : public Renderer<TestRenderer>
    {
    public:
        TestRenderer(std::shared_ptr<Scene> scene)
            : Renderer<TestRenderer>(scene)
        {
        }
        ~TestRenderer()
        {
        }

        void Start()
        {
            spdlog::debug("TestRenderer::Start()");
        }

        void RenderFirstPass()
        {
        }

        void RenderMainPass()
        {
            const glm::vec3 clearColor1(0.09f, 0.30f, 0.55f);
            const glm::vec3 clearColor2(1.0f, 0.76f, 0.03f);
            float time = EventHandler::GetInstance().GetTime() * 10.0f;
            //put time into a sin wave to get a value between 0 and 1
            float value = (sin(time) + 1.0f) / 2.0f;
            //lerp between the two colors
            program->SetClearColor(glm::vec4(glm::mix(
                clearColor1,
                clearColor2,
                value), 1.0f));
        }

        void End()
        {
        }
    };
} // namespace ssCore