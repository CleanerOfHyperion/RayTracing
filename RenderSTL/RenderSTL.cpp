#include <cstdio>
#include <cstdlib>
#include <GL/_OpenGL.h>
#include <GL/_Window.h>
#include <_Math.h>
#include <_Time.h>
#include <_Array.h>
#include <_Pair.h>
#include <_STL.h>

namespace OpenGL
{
	struct RenderSTL :OpenGL
	{
		struct Renderer :Program
		{
			STLData model;
			Transform trans;
			Buffer<ArrayBuffer> buffer;
			Buffer<UniformBuffer> transformBuffer;
			VertexAttrib positions;

			Renderer(SourceManager*);
			void refreshBuffer();
			virtual void setBufferData()override
			{
			}
			virtual void run() override
			{
				glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDrawArrays(GL_TRIANGLES, 0, model.stl.verticesRepeated.length);
			}
			virtual void resize(int _w, int _h)override
			{
				trans.resize(_w, _h);
				glViewport(0, 0, _w, _h);
			}
		};

		SourceManager sm;
		Renderer renderer;

		RenderSTL();
		virtual void init(FrameScale const&) override;
		virtual void run() override;
		virtual void frameSize(int, int) override;
		virtual void framePos(int, int) override;
		virtual void frameFocus(int) override;
		virtual void mouseButton(int, int, int) override;
		virtual void mousePos(double, double) override;
		virtual void mouseScroll(double, double) override;
		virtual void key(GLFWwindow*, int, int, int, int) override;
	};

	inline RenderSTL::RenderSTL()
		:
		sm(),
		renderer(&sm)
	{
	}
	inline void RenderSTL::init(FrameScale const& _size)
	{
		glViewport(0, 0, _size.w, _size.h);
		glEnable(GL_DEPTH_TEST);
		renderer.trans.init(_size);
		renderer.transformBuffer.dataStore();
		renderer.buffer.dataStore();
	}
	inline void RenderSTL::run()
	{
		/*
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);*/
		renderer.use();
		renderer.refreshBuffer();
		renderer.run();
	}
	inline void RenderSTL::frameSize(int _w, int _h)
	{
		renderer.resize(_w, _h);
	}
	inline void RenderSTL::framePos(int, int)
	{
	}
	inline void RenderSTL::frameFocus(int)
	{
	}
	inline void RenderSTL::mouseButton(int _button, int _action, int _mods)
	{
		switch (_button)
		{
			case GLFW_MOUSE_BUTTON_LEFT:renderer.trans.mouse.refreshButton(0, _action); break;
			case GLFW_MOUSE_BUTTON_MIDDLE:renderer.trans.mouse.refreshButton(1, _action); break;
			case GLFW_MOUSE_BUTTON_RIGHT:renderer.trans.mouse.refreshButton(2, _action); break;
		}
	}
	inline void RenderSTL::mousePos(double _x, double _y)
	{
		renderer.trans.mouse.refreshPos(_x, _y);
	}
	inline void RenderSTL::mouseScroll(double _x, double _y)
	{
		if (_y != 0.0)
			renderer.trans.scroll.refresh(_y);
	}
	inline void RenderSTL::key(GLFWwindow * _window, int _key, int _scancode, int _action, int _mods)
	{
		switch (_key)
		{
			case GLFW_KEY_ESCAPE:
				if (_action == GLFW_PRESS)
					glfwSetWindowShouldClose(_window, true);
				break;
			case GLFW_KEY_A:renderer.trans.key.refresh(0, _action); break;
			case GLFW_KEY_D:renderer.trans.key.refresh(1, _action); break;
			case GLFW_KEY_W:renderer.trans.key.refresh(2, _action); break;
			case GLFW_KEY_S:renderer.trans.key.refresh(3, _action); break;
		}
	}

	RenderSTL::Renderer::Renderer(SourceManager * _sourceManage)
		:
		Program(_sourceManage, "Triangle", Vector<VertexAttrib*>{&positions}),
		model(_sourceManage->folder.find("resources/star.stl").readSTL()),
		trans({ {80.0,0.1,200},{0.5,0.8,0.05},{2},500.0 }),
		transformBuffer(&trans.bufferData, 0),
		buffer(&model),
		positions(&buffer, 0, VertexAttrib::three, VertexAttrib::Float, false, sizeof(Math::vec3<float>), 0)
	{
		model.stl.removeUseless();
		model.stl.getVerticesRepeated();
		init();
	}
	void RenderSTL::Renderer::refreshBuffer()
	{
		trans.operate();
		if (trans.updated)
		{
			transformBuffer.refreshData();
			trans.updated = false;
		}
	}

}


int main()
{
	OpenGL::OpenGLInit init(4, 5);
	Window::Window::Data winParameters
	{
		"Ahh",
		{
			{400,400},
			true,false
		}
	};
	Window::WindowManager wm(winParameters);
	OpenGL::RenderSTL test;
	wm.init(0, &test);
	glfwSwapInterval(0);
	FPS fps;
	fps.refresh();
	while (!wm.close())
	{
		wm.pullEvents();
		wm.render();
		wm.swapBuffers();
		fps.refresh();
		fps.printFPS(1);
	}
	return 0;
}

