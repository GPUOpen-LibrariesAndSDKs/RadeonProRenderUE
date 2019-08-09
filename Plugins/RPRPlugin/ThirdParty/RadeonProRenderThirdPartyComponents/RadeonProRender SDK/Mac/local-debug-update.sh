cp -r /Users/amd/Documents/dev/git/FireRender/dist/debug/bin/x86_64/libRprLoadStore64D.dylib lib/libRprLoadStore64.dylib
cp -r /Users/amd/Documents/dev/git/FireRender/dist/debug/bin/x86_64/libRprSupport64D.dylib lib/libRprSupport64.dylib
cp -r /Users/amd/Documents/dev/git/FireRender/dist/debug/bin/x86_64/libRadeonProRender64D.dylib lib/libRadeonProRender64.dylib
cp -r /Users/amd/Documents/dev/git/FireRender/dist/debug/bin/x86_64/libTahoe64D.dylib lib/libTahoe64.dylib


# install_name_tool -id /Users/Shared/RadeonProRender/lib/libOpenImageIO.1.6.dylib ./lib/libOpenImageIO.1.6.dylib
install_name_tool -id /Users/Shared/RadeonProRender/lib/libTahoe64.dylib ./lib/libTahoe64.dylib
install_name_tool -id /Users/Shared/RadeonProRender/lib/libRadeonProRender64.dylib ./lib/libRadeonProRender64.dylib
install_name_tool -id /Users/Shared/RadeonProRender/lib/libRprLoadStore64.dylib ./lib/libRprLoadStore64.dylib
install_name_tool -id /Users/Shared/RadeonProRender/lib/libRprSupport64.dylib ./lib/libRprSupport64.dylib

# install_name_tool -change /Users/dkozlov/Documents/dev/oiio-bin/src/libOpenImageIO/Release/libOpenImageIO.1.6.dylib /Users/Shared/RadeonProRender/lib/libOpenImageIO.1.6.dylib ./lib/libRadeonProRender64.dylib
# install_name_tool -change /usr/local/lib/libpng16.16.dylib /Users/Shared/RadeonProRender/lib/libpng16.16.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libtiff.5.dylib /Users/Shared/RadeonProRender/lib/libtiff.5.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libjpeg.8.dylib /Users/Shared/RadeonProRender/lib/libjpeg.8.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libIlmImf-2_2.22.dylib /Users/Shared/RadeonProRender/lib/libIlmImf-2_2.22.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libHalf.12.dylib /Users/Shared/RadeonProRender/lib/libHalf.12.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libIex-2_2.12.dylib /Users/Shared/RadeonProRender/lib/libIex-2_2.12.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libImath-2_2.12.dylib /Users/Shared/RadeonProRender/lib/libImath-2_2.12.dylib ./lib/libOpenImageIO.1.6.dylib
# install_name_tool -change /usr/local/lib/libIlmThread-2_2.12.dylib /Users/Shared/RadeonProRender/lib/libIlmThread-2_2.12.dylib ./lib/libOpenImageIO.1.6.dylib

install_name_tool -change @rpath/libRadeonProRender64D.dylib /Users/Shared/RadeonProRender/lib/libRadeonProRender64.dylib ./lib/libRprSupport64.dylib

install_name_tool -change @rpath/libRadeonProRender64D.dylib /Users/Shared/RadeonProRender/lib/libRadeonProRender64.dylib ./lib/libRprLoadStore64.dylib
install_name_tool -change @rpath/libRprSupport64D.dylib /Users/Shared/RadeonProRender/lib/libRprSupport64.dylib ./lib/libRprLoadStore64.dylib


# OpenImageIO 1.7 and its dependencies
# install_name_tool -id /Users/Shared/RadeonProRender/lib/libOpenImageIO.1.7.dylib ./lib/libOpenImageIO.1.7.dylib

# install_name_tool -change /usr/local/opt/openexr/lib/libIlmImf-2_2.22.dylib /Users/Shared/RadeonProRender/lib/libIlmImf-2_2.22.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/ilmbase/lib/libHalf.12.dylib /Users/Shared/RadeonProRender/lib/libHalf.12.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/ilmbase/lib/libIex-2_2.12.dylib /Users/Shared/RadeonProRender/lib/libIex-2_2.12.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/ilmbase/lib/libImath-2_2.12.dylib /Users/Shared/RadeonProRender/lib/libImath-2_2.12.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/ilmbase/lib/libIlmThread-2_2.12.dylib /Users/Shared/RadeonProRender/lib/libIlmThread-2_2.12.dylib ./lib/libOpenImageIO.1.7.dylib
# 
# install_name_tool -change /usr/local/opt/libpng/lib/libpng16.16.dylib /Users/Shared/RadeonProRender/lib/libpng16.16.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/libtiff/lib/libtiff.5.dylib /Users/Shared/RadeonProRender/lib/libtiff.5.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/jpeg/lib/libjpeg.9.dylib /Users/Shared/RadeonProRender/lib/libjpeg.9.dylib ./lib/libOpenImageIO.1.7.dylib

# install_name_tool -change /usr/local/opt/webp/lib/libwebp.7.dylib /Users/Shared/RadeonProRender/lib/libwebp.7.dylib ./lib/libOpenImageIO.1.7.dylib

# install_name_tool -change /usr/local/opt/boost/lib/libboost_filesystem-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_filesystem-mt.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/boost/lib/libboost_regex-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_regex-mt.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/boost/lib/libboost_system-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_system-mt.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/boost/lib/libboost_thread-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_thread-mt.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/boost/lib/libboost_chrono-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_chrono-mt.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/boost/lib/libboost_date_time-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_date_time-mt.dylib ./lib/libOpenImageIO.1.7.dylib
# install_name_tool -change /usr/local/opt/boost/lib/libboost_atomic-mt.dylib /Users/Shared/RadeonProRender/lib/libboost_atomic-mt.dylib ./lib/libOpenImageIO.1.7.dylib

# install_name_tool -change /usr/local/opt/openimageio/lib/libOpenImageIO.1.7.dylib /Users/Shared/RadeonProRender/lib/libOpenImageIO.1.7.dylib ./lib/libRadeonProRender64.dylib
# install_name_tool -change /usr/local/opt/openimageio/lib/libOpenImageIO.1.7.dylib /Users/Shared/RadeonProRender/lib/libOpenImageIO.1.7.dylib ./lib/libTahoe64.dylib



