#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

#include "opencv2/core.hpp"
#include <opencv2/videoio.hpp>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#ifndef VIDEO_PATH
#define VIDEO_PATH "../"
#endif

struct FrameBuffer {
    std::mutex mutexLock;
    cv::Mat Frame;

    int frameNum = -1;
    bool eof = false;
};

struct ProducerArgs {
    FrameBuffer* frameBuffer;
    std::string FilePath;
    std::string msgPath;
};

void frameProducer(ProducerArgs *producerArgs);

int main(int argc, char** argv) {
    ros::init(argc, argv, "PylonTest");

    // Prep Producer
    FrameBuffer frameBufferL;
    ProducerArgs producerArgsL;
    producerArgsL.frameBuffer = &frameBufferL;
    producerArgsL.FilePath = std::string(VIDEO_PATH).append("track3_L.mp4");
    producerArgsL.msgPath = "/mur/stereo_cam/left_image";

    FrameBuffer frameBufferR;
    ProducerArgs producerArgsR;
    producerArgsR.frameBuffer = &frameBufferR;
    producerArgsR.FilePath = std::string(VIDEO_PATH).append("track3_R.mp4");
    producerArgsR.msgPath = "/mur/stereo_cam/right_image";

    std::thread producerL(frameProducer, &producerArgsL);
    std::thread producerR(frameProducer, &producerArgsR);

    producerL.join();
    producerR.join();

    return 0;
}

void frameProducer(ProducerArgs *producerArgs) {
    ros::NodeHandle nh;

    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise(producerArgs->msgPath, 1);

    FrameBuffer *frameBuffer = producerArgs->frameBuffer;
    cv::VideoCapture VideoFile(producerArgs->FilePath);

    auto frameTime = std::chrono::milliseconds(static_cast<int64_t>(1000.0/VideoFile.get(cv::CAP_PROP_FPS)));
    // auto frameTime = std::chrono::milliseconds(100);

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << frameTime.count() << std::endl;

    while (ros::ok()) {
        auto now = std::chrono::high_resolution_clock::now();

        frameBuffer->mutexLock.lock();
        VideoFile.read(frameBuffer->Frame);
        frameBuffer->frameNum++;
        frameBuffer->mutexLock.unlock();

        if (frameBuffer->Frame.empty() ) {
            frameBuffer->eof = true;
            return;
        }

        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frameBuffer->Frame).toImageMsg();

        pub.publish(msg);
        ros::spinOnce;

        auto now2 = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(frameTime - (now2 - now));
    }
}