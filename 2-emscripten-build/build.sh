# USE EMSCRIPTEN 3.1.51
# https://github.com/bkaradzic/bgfx/discussions/3266

source /home/demensdeum_stream/Apps/emsdk-3.1.51/emsdk_env.sh

clear
rm ./bgfx-2d

shaderc -f "VertexShader.vs" -o "VertexShader.glsl" --type "v" -p "120"
if [ $? -ne 0 ]; then
    exit 1
fi

shaderc -f "FragmentShader.fs" -o "FragmentShader.glsl" --type "f" -p "120"
if [ $? -ne 0 ]; then
    exit 1
fi

emcmake cmake .
emmake make

rm -rf /srv/http/BGFX-2D
mkdir /srv/http/BGFX-2D
cp index.html /srv/http/BGFX-2D
cp bgfx-2d.data /srv/http/BGFX-2D
cp bgfx-2d.js /srv/http/BGFX-2D
cp bgfx-2d.wasm /srv/http/BGFX-2D