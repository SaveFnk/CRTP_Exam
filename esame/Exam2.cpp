#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Replace with the IP address of the server you want to send frames to
const std::string SERVER_IP = "127.0.0.1";

int main() {
  // Open the default webcam
  cv::VideoCapture webcam(0);
  if (!webcam.isOpened()) {
    std::cerr << "Error: Could not open webcam" << std::endl;
    return 1;
  }

  // Create a socket for sending data to the server
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cerr << "Error: Could not create socket" << std::endl;
    return 1;
  }

  // Connect to the server
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  if (inet_pton(AF_INET, SERVER_IP.c_str(), &server_addr.sin_addr) <= 0) {
    std::cerr << "Error: Invalid server IP address" << std::endl;
    return 1;
  }
  if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "Error: Could not connect to server" << std::endl;
    return 1;
  }

  // Send the filename to the server
  const std::string filename = "webcam_frames.avi";
  if (send(sock, filename.c_str(), filename.size(), 0) < 0) {
    std::cerr << "Error: Could not send filename to server" << std::endl;
    return 1;
  }

  // Create a video writer for storing the frames on disk
  cv::VideoWriter writer("webcam_frames.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0,
                         cv::Size(webcam.get(cv::CAP_PROP_FRAME_WIDTH),
                                  webcam.get(cv::CAP_PROP_FRAME_HEIGHT)));
  if (!writer.isOpened()) {
    std::cerr << "Error: Could not create video writer" << std::endl;
    return 1;
  }

  // Capture and save frames from the webcam, and send them to the server
  while (true) {
    cv::Mat frame;
    if (!webcam.read(frame)) {
      std::cerr << "Error: Could not read frame from webcam" << std::endl;
      break;
    }

    // Write the frame to disk
    writer.write(frame);

    // Send the frame to the server
    if (send(sock,
