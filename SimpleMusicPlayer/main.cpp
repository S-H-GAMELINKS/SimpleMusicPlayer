// Simple Music Player
// coding by S.H. (GAMELINKS)
// This source code license is MIT, see the license

// include DxLib
#include "DxLib.h"

// include Boost library
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace fs = boost::filesystem;

// include openCV
#include <opencv2/opencv.hpp>

// include standard library
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

// One (and only one) of your C++ files must define CVUI_IMPLEMENTATION
// before the inclusion of cvui.h to ensure its implementaiton is compiled.
#define CVUI_IMPLEMENTATION
#include "cvui.h"

// Hide Console Window
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Set Software Title
constexpr const char* WindowName = "Simple Music Player";

// Set Wait Time
constexpr int WaitTime = 20;

//Read All MP3 File Path
std::vector<std::string> AllFilePath() {

	std::vector<std::string> Container;

	std::string Path = "mp3", str = "/";

	const fs::path path(Path);

	std::int32_t i = 0;

	BOOST_FOREACH(const fs::path& p, std::make_pair(fs::directory_iterator(path),
		fs::directory_iterator())) {
		if (!fs::is_directory(p)) {
			Container.emplace_back(std::move(Path + str + p.filename().string()));
			std::cout << p.filename() << std::endl;
			i++;
		}
	}

	return Container;
}

// Loading music
std::vector<int> LoadMusic(const std::vector<std::string> Container) {

	// Loading Sound Data Type Set
	DxLib::SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMPRESS);

	std::vector<int> MusicContainer;
	
	for (auto&& c : Container)
		MusicContainer.emplace_back(std::move(DxLib::LoadSoundMem(c.c_str())));

	return MusicContainer;
}

// Draw button
int DrawButton(cv::Mat frame, std::int32_t Num, std::vector<int> MusicContainer) {

	if (cvui::button(frame, 20, 40, "Prev") || DxLib::CheckHitKey(KEY_INPUT_LEFT)){
		Num = (Num == 0) ? 0 : Num--;

		DxLib::StopSoundMem(MusicContainer[Num + 1]);
		DxLib::PlaySoundMem(MusicContainer[Num], DX_PLAYTYPE_BACK);

		std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime));
	}

	if (cvui::button(frame, 80, 40, "Play") || DxLib::CheckHitKey(KEY_INPUT_SPACE)) {
		DxLib::PlaySoundMem(MusicContainer[Num], DX_PLAYTYPE_BACK);

		std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime));
	}

	if (cvui::button(frame, 140, 40, "Stop") || DxLib::CheckHitKey(KEY_INPUT_BACK)) {
		DxLib::StopSoundMem(MusicContainer[Num]);

		std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime));
	}

	if (cvui::button(frame, 200, 40, "Next") || DxLib::CheckHitKey(KEY_INPUT_RIGHT)) {

		Num = (static_cast<unsigned int>(Num) == MusicContainer.size()) ? MusicContainer.size() : Num++;

		DxLib::StopSoundMem(MusicContainer[Num - 1]);
		DxLib::PlaySoundMem(MusicContainer[Num], DX_PLAYTYPE_BACK);

		std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime));
	}

	return Num;
}

// Change Volumes
double VolumeTrackBar(cv::Mat frame, double value) {
	cvui::trackbar(frame, 40, 70, 220, &value, (double)0.0, (double)100.0);

	if (DxLib::CheckHitKey(KEY_INPUT_UP)) {
		if (value >= 100.0)
			value = 100.0;
		else
			value++;
	}

	if (DxLib::CheckHitKey(KEY_INPUT_DOWN)) {
		if (value <= 0.0)
			value = 0.0;
		else
			value--;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime));

	return value;
}

int main(int argc, const char *argv[])
{

	DxLib::SetNotWinFlag(TRUE);	// Do not draw Window for DxLib

	if (DxLib::DxLib_Init() == -1)    // DxLib Init!
	{
		return -1;    // Error!
	}

	std::vector<std::string> Container = AllFilePath();

	// Create a frame where components will be rendered to.
	cv::Mat frame = cv::Mat(180, 280, CV_8UC3);

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
	cvui::init(WindowName);

	// Set Volumnes
	double value = 100.0;

	const std::vector<int> MusicContainer = LoadMusic(Container);

	std::int32_t Num = 0;

	while (true) {
		// Fill the frame with a nice color
		frame = cv::Scalar(49, 52, 49);

		Num = DrawButton(frame, Num, MusicContainer);

		value = VolumeTrackBar(frame, value);

		ChangeVolumeSoundMem(255 * static_cast<int>(value) / 100, MusicContainer[Num]);

		if (DxLib::CheckSoundMem(MusicContainer[Num]) == 1) {
			cvui::text(frame, 40, 150, "Now Playing Number is");
			cvui::text(frame, 190, 150, Container[Num]);
		}

		// Update cvui stuff and show everything on the screen
		cvui::imshow(WindowName, frame);

		// Exit to ESC Key!
		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	DxLib::DxLib_End();        // DxLib End!

	return 0;
}