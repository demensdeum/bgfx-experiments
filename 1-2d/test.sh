./bgfx-2d OPENGL
if [ $? -ne 0 ]; then
    exit 1
fi

./bgfx-2d VULKAN
if [ $? -ne 0 ]; then
    exit 1
fi