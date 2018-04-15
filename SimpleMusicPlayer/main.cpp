#include "DxLib.h"

#include <opencv2/opencv.hpp>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <iomanip>

// One (and only one) of your C++ files must define CVUI_IMPLEMENTATION
// before the inclusion of cvui.h to ensure its implementaiton is compiled.
#define CVUI_IMPLEMENTATION
#include "cvui.h"

// Hide Console Window
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

constexpr const char* WindowName = "Simple Music Player";

constexpr int MaterialMax = 99;

namespace {

	//各種素材ファイル確認関数
	bool CheckMaterialExistence(const std::string& FilePath) noexcept {
		std::ifstream Material(FilePath, std::ios_base::in);
		return Material.is_open();
	}

	//各種素材ファイルパス処理
	std::string MaterialPathCalc(const std::int32_t& i, const std::string& FilePath, const std::string& FileFormat) noexcept {
		std::ostringstream Num;

		Num << std::setfill('0') << std::setw(2) << i + 1;

		return (FilePath + Num.str() + FileFormat);
	}

	//各種素材読込テンプレート関数
	template <typename T, typename Func>
	void MaterialLoadTemplate(std::array<T, MaterialMax>& Material, const std::string& FilePath, const std::string& FileFormat, Func&& Loader) noexcept {
		for (std::int32_t i = 0; i < MaterialMax; i++) {
			if (CheckMaterialExistence(MaterialPathCalc(i, FilePath, FileFormat)))
				Material[i] = Loader(MaterialPathCalc(i, FilePath, FileFormat));
		}
	}
}

std::array<int, MaterialMax> LoadMusic() {

	std::array<int, MaterialMax> MusicContainer;

	MaterialLoadTemplate(MusicContainer, "mp3/bgm", ".mp3", [](const std::string& Path) {return DxLib::LoadSoundMem(Path.c_str()); });

	return MusicContainer;
}

int DrawButton(cv::Mat frame, std::int32_t Num, std::array<int, MaterialMax> MusicContainer) {

	if (cvui::button(frame, 20, 40, "Prev")) {
		if (Num == 0)
			Num = 0;
		else
			Num--;

		DxLib::StopSoundMem(MusicContainer[Num + 1]);
		DxLib::PlaySoundMem(MusicContainer[Num], DX_PLAYTYPE_BACK);

		return Num;
	}

	if (cvui::button(frame, 80, 40, "Play")) {
		DxLib::PlaySoundMem(MusicContainer[Num], DX_PLAYTYPE_BACK);

		return Num;
	}

	if (cvui::button(frame, 140, 40, "Stop")) {
		DxLib::StopSoundMem(MusicContainer[Num]);

		return Num;
	}

	if (cvui::button(frame, 200, 40, "Next")) {
		if (Num == MaterialMax)
			Num = MaterialMax;
		else
			Num++;

		DxLib::StopSoundMem(MusicContainer[Num - 1]);
		DxLib::PlaySoundMem(MusicContainer[Num], DX_PLAYTYPE_BACK);

		return Num;
	}

	return Num;
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

	const std::array<int, MaterialMax> MusicContainer = LoadMusic();

	std::int32_t Num = 0;

	while (true) {
		// Fill the frame with a nice color
		frame = cv::Scalar(49, 52, 49);

		Num = DrawButton(frame, Num, MusicContainer);

		value = VolumeTrackBar(frame, value);

		ChangeVolumeSoundMem(255 * static_cast<int>(value) / 100, MusicContainer[Num]);

		if (DxLib::CheckSoundMem(MusicContainer[Num]) == 1) {
			cvui::text(frame, 40, 150, "Now Playing Number is");
			cvui::text(frame, 190, 150, std::to_string(Num));
		}

		// Update cvui stuff and show everything on the screen
		cvui::imshow(WindowName, frame);

		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	DxLib::DxLib_End();        // ＤＸライブラリ使用の終了処理

	return 0;
}