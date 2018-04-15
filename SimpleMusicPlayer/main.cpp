#include "DxLib.h"

#include <opencv2/opencv.hpp>
#include <string>

// One (and only one) of your C++ files must define CVUI_IMPLEMENTATION
// before the inclusion of cvui.h to ensure its implementaiton is compiled.
#define CVUI_IMPLEMENTATION
#include "cvui.h"

// Hide Console Window
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

constexpr const char* WindowName = "Simple Music Player";

void DrawButton(cv::Mat frame) {

	if (cvui::button(frame, 20, 40, "Prev")) {
	}

	if (cvui::button(frame, 80, 40, "Play")) {
	}

	if (cvui::button(frame, 140, 40, "Stop")) {
	}

	if (cvui::button(frame, 200, 40, "Next")) {
	}
}

double VolumeTrackBar(cv::Mat frame, double value) {
	cvui::trackbar(frame, 40, 70, 220, &value, (double)0.0, (double)100.0);
	return value;
}

int main(int argc, const char *argv[])
{

	DxLib::SetNotWinFlag(TRUE);	// ウインドウ関連の処理を行わない

	if (DxLib::DxLib_Init() == -1)    // ＤＸライブラリ初期化処理
	{
		return -1;    // エラーが起きたら直ちに終了
	}

	// Create a frame where components will be rendered to.
	cv::Mat frame = cv::Mat(180, 280, CV_8UC3);

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
	cvui::init(WindowName);

	double value = 100.0;

	while (true) {
		// Fill the frame with a nice color
		frame = cv::Scalar(49, 52, 49);

		DrawButton(frame);

		value = VolumeTrackBar(frame, value);

		// Update cvui stuff and show everything on the screen
		cvui::imshow(WindowName, frame);

		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	DxLib::DxLib_End();        // ＤＸライブラリ使用の終了処理

	return 0;
}