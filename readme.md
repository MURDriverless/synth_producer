# synth_producer
Currently used as a playground for testing of the ros stereo pipeline.

## Requirements/Compiled with
 - `OpenCV` 4.1.1, Compiled with CUDA, CUDNN and Non-free addons
 - `vision_opencv` from [https://github.com/MURDriverless/vision_opencv](https://github.com/MURDriverless/vision_opencv)

 ## Usage
 - Currently video tracks are hardcoded in `src/synth_producer_node.cpp`, change the code to retarget different video streams.
 - `synth_producer_node` produces to `/mur/stereo_cam/left_image` and `/mur/stereo_cam/right_image`