# Version 1.0

## New Features:

-   The Plug-in has been completely rewritten with support for Unreal Engine 4.20–4.23 and 4.24. The Plug-in for 4.24 is separate because of API changes in Unreal Engine.

-   This Plug-in is intended for higher quality rendering of Unreal Engine scenes with native lights and materials. The lights and materials are translated during the render time to Radeon ProRender. Please use it with your existing Unreal Engine scenes.
    
-   Rendering uses OpenCL path tracing or Vulkan for Low, Medium, and High quality view modes.
    
-   Machine Learning Denoising is now available to be enabled in the render view
    
-   Adaptive sampling now automatically focuses render samples on areas of noise in the rendered image.