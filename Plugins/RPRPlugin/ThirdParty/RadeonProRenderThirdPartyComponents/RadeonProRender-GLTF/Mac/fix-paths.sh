#!/bin/bash

# These commands change load paths on the dependencies

install_name_tool -id /Users/Shared/RadeonProRender/lib/libProRenderGLTF.dylib ./lib/libProRenderGLTF.dylib

install_name_tool -change @rpath/libRadeonProRender64.dylib /Users/Shared/RadeonProRender/lib/libRadeonProRender64.dylib ./lib/libProRenderGLTF.dylib
install_name_tool -change @rpath/libRprSupport64.dylib /Users/Shared/RadeonProRender/lib/libRprSupport64.dylib ./lib/libProRenderGLTF.dylib






