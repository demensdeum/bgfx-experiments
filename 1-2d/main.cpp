#include <iostream>
#include <vector>
#include <fstream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

const int screenWidth = 1920;
const int screenHeight = 1080;

std::vector<char> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        std::string error = "failed to open file: " + filename;
        throw std::runtime_error(error);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

struct Vertex3D_UV
{
    float x;
    float y;
    float z;
    float u;
    float v;

    static void init()
    {
        Vertex3D_UV::ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout Vertex3D_UV::ms_layout;

bgfx::TextureHandle loadTexture(const char *filepath)
{
    SDL_Surface *surface = SDL_LoadBMP(filepath);
    if (!surface)
    {
        throw std::runtime_error("Failed to load texture: " + std::string(SDL_GetError()));
    }

    const bgfx::Memory *mem = bgfx::alloc(surface->w * surface->h * 4);
    uint8_t *dst = (uint8_t *)mem->data;
    uint8_t *src = (uint8_t *)surface->pixels;

    for (int y = 0; y < surface->h; ++y)
    {
        for (int x = 0; x < surface->w; ++x)
        {
            uint8_t *pixel = &src[(y * surface->pitch) + (x * surface->format->BytesPerPixel)];
            dst[(y * surface->w + x) * 4 + 0] = pixel[2]; // R
            dst[(y * surface->w + x) * 4 + 1] = pixel[1]; // G
            dst[(y * surface->w + x) * 4 + 2] = pixel[0]; // B
            dst[(y * surface->w + x) * 4 + 3] = 255;      // A
        }
    }

    bgfx::TextureHandle textureHandle = bgfx::createTexture2D(
        uint16_t(surface->w),
        uint16_t(surface->h),
        false,
        1,
        bgfx::TextureFormat::RGBA8,
        0,
        mem);

    SDL_FreeSurface(surface);
    return textureHandle;
}

int main(int argc, char **argv)
{
    std::string windowTitle = "Bgfx 2D Test";
    if (argc != 2) {
        std::cout << "Argument must be OPENGL or VULKAN" << std::endl;
        exit(1);
    }
    std::string mode = std::string(argv[1]);

   Uint32 flags = SDL_WINDOW_OPENGL;

    if (mode == "OPENGL")
    {
        flags = SDL_WINDOW_OPENGL;
        windowTitle += " (OpenGL)";
    }
    else if (mode == "VULKAN")
    {
        flags = SDL_WINDOW_VULKAN;
        windowTitle += " (Vulkan)";
    }

    SDL_Vulkan_LoadLibrary(nullptr);

    SDL_Window *window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screenWidth,
        screenHeight,
        flags);

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi))
    {
        throw std::runtime_error(std::string("Can't get wmi"));
        exit(1);
    }

    bgfx::PlatformData platformData{};
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    platformData.ndt = wmi.info.x11.display;
    platformData.nwh = (void *)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
    pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
    pd.ndt = wmi.info.vivante.display;
    pd.nwh = wmi.info.vivante.window;
#endif
    platformData.context = NULL;
    platformData.backBuffer = NULL;
    platformData.backBufferDS = NULL;

    bgfx::Init init;
    if (mode == "OPENGL")
    {
        init.type = bgfx::RendererType::OpenGL;
    }
    else if (mode == "VULKAN")
    {
        init.type = bgfx::RendererType::Vulkan;
    }
    init.resolution.width = screenWidth;
    init.resolution.height = screenHeight;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData = platformData;

    if (!bgfx::init(init))
    {
        throw std::runtime_error("Failed to initialize bgfx");
    }

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, screenWidth, screenHeight);

    Vertex3D_UV::init();

    std::vector<char> vertexShaderCode;

    if (mode == "OPENGL")
    {
        vertexShaderCode = readFile("VertexShader.glsl");
    }
    else if (mode == "VULKAN")
    {
        vertexShaderCode = readFile("VertexShader.spirv");
    }

    bgfx::ShaderHandle vertexShader = bgfx::createShader(bgfx::makeRef(vertexShaderCode.data(), vertexShaderCode.size()));
    if (!bgfx::isValid(vertexShader))
    {
        throw std::runtime_error("Failed to create vertex shader");
    }
    else
    {
        std::cout << "Vertex shader load success!" << std::endl;
    }

    std::vector<char> fragmentShaderCode;

    if (mode == "OPENGL")
    {
        fragmentShaderCode = readFile("FragmentShader.glsl");
    }
    else if (mode == "VULKAN")
    {
        fragmentShaderCode = readFile("FragmentShader.spirv");
    }
    bgfx::ShaderHandle fragmentShader = bgfx::createShader(bgfx::makeRef(fragmentShaderCode.data(), fragmentShaderCode.size()));
    if (!bgfx::isValid(fragmentShader))
    {
        throw std::runtime_error("Failed to create fragment shader");
    }
    else
    {
        std::cout << "Fragment shader load success!" << std::endl;
    }

    bgfx::ProgramHandle program = bgfx::createProgram(vertexShader, fragmentShader, true);

    if (!bgfx::isValid(program))
    {
        throw std::runtime_error("Failed to create program");
    }

    bool isRun = true;
    SDL_Event e;

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);

    std::vector<Vertex3D_UV> vertices;

    float Z = 0.0;
    vertices.push_back(Vertex3D_UV{-0.5f, -0.5f, Z, 0.0f, 0.0f});
    vertices.push_back(Vertex3D_UV{0.5f, -0.5f, Z, 1.0f, 0.0f});
    vertices.push_back(Vertex3D_UV{0.0f, 0.5f, Z, 0.5f, 1.0f});

    bgfx::VertexBufferHandle vertexBuffer = bgfx::createVertexBuffer(
        bgfx::makeRef(
            vertices.data(),
            sizeof(decltype(vertices)::value_type) * vertices.size()),
        Vertex3D_UV::ms_layout);

    bgfx::TextureHandle texture = loadTexture("brick.texture.bmp");
    bgfx::UniformHandle s_texture0 = bgfx::createUniform("s_texture0", bgfx::UniformType::Sampler);

    while (isRun)
    {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT)
        {
            isRun = false;
        }
        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                isRun = false;
            }
        }

        bgfx::setVertexBuffer(0, vertexBuffer);
        bgfx::setTexture(0, s_texture0, texture);

        bgfx::setState(BGFX_STATE_DEFAULT);

        bgfx::submit(0, program);
        bgfx::frame();
    }

    bgfx::destroy(vertexBuffer);
    bgfx::destroy(program);
    bgfx::destroy(texture);
    bgfx::destroy(s_texture0);

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();    

    exit(0);
}