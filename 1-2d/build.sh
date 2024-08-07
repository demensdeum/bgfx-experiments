clear
rm ./bgfx-2d

shaderc -f "VertexShader.vs" -o "VertexShader.spirv" --type "v" -p "spirv"
if [ $? -ne 0 ]; then
    exit 1
fi

shaderc -f "FragmentShader.fs" -o "FragmentShader.spirv" --type "f" -p "spirv"
if [ $? -ne 0 ]; then
    exit 1
fi

shaderc -f "VertexShader.vs" -o "VertexShader.glsl" --type "v" -p "440"
if [ $? -ne 0 ]; then
    exit 1
fi

shaderc -f "FragmentShader.fs" -o "FragmentShader.glsl" --type "f" -p "440"
if [ $? -ne 0 ]; then
    exit 1
fi

cmake .
make