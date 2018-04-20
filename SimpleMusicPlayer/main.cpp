// Simple Music Player
// coding by S.H. (GAMELINKS)
// This source code license is MIT, see the license

// include DxLib
#include "DxLib.h"

// include Boost library
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace fs = boost::filesystem;

// include openCV and standard library
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
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Set Software Title
constexpr const char* WindowName = "Simple Music Player";

// loading music max 
constexpr int MaterialMax = 99;

//�S�t�@�C�����`��֐�
std::array<std::string, MaterialMax> AllFilePath() {

	std::array<std::string, MaterialMax> Container;

	std::string Path = "mp3", str = "/";

	const fs::path path(Path);

	std::int32_t i = 0;

	BOOST_FOREACH(const fs::path& p, std::make_pair(fs::directory_iterator(path),
		fs::directory_iterator())) {
		if (!fs::is_directory(p)) {
			Container[i] = Path + str + p.filename().string();
			std::cout << p.filename() << std::endl;
			i++;
		}
	}

	return Container;
}

// music material load func's
namespace {

	//�e��f�ރt�@�C���m�F�֐�
	bool CheckMaterialExistence(const std::string& FilePath) noexcept {
		std::ifstream Material(FilePath, std::ios_base::in);
		return Material.is_open();
	}

	//�e��f�ރt�@�C���p�X����
	std::string MaterialPathCalc(const std::int32_t& i, const std::string& FilePath, const std::string& FileFormat) noexcept {
		std::ostringstream Num;

		Num << std::setfill('0') << std::setw(2) << i + 1;

		return (FilePath + Num.str() + FileFormat);
	}

	//�e��f�ޓǍ��e���v���[�g�֐�
	template <typename T, typename Func>
	void MaterialLoadTemplate(std::array<T, MaterialMax>& Material, const std::string& FilePath, const std::string& FileFormat, Func&& Loader) noexcept {
		for (std::int32_t i = 0; i < MaterialMax; i++) {
			if (CheckMaterialExistence(MaterialPathCalc(i, FilePath, FileFormat)))
				Material[i] = Loader(MaterialPathCalc(i, FilePath, FileFormat));
		}
	}
}

// loading music
std::array<int, MaterialMax> LoadMusic(const std::array<std::string, MaterialMax> Container) {

	//�T�E���h�f�[�^�̓ǂݍ��݌`��
	DxLib::SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMPRESS);

	std::array<int, MaterialMax> MusicContainer;

	for (std::size_t i = 0; i < MaterialMax; i++)
		MusicContainer[i] = DxLib::LoadSoundMem(Container[i].c_str());

	return MusicContainer;
}

// Draw button
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

// Change Volumes
double VolumeTrackBar(cv::Mat frame, double value) {
	cvui::trackbar(frame, 40, 70, 220, &value, (double)0.0, (double)100.0);
	return value;
}

int main(int argc, const char *argv[])
{

	DxLib::SetNotWinFlag(TRUE);	// �E�C���h�E�֘A�̏������s��Ȃ�

	if (DxLib::DxLib_Init() == -1)    // �c�w���C�u��������������
	{
		return -1;    // �G���[���N�����璼���ɏI��
	}

	std::array<std::string, MaterialMax> Container = AllFilePath();

	// Create a frame where components will be rendered to.
	cv::Mat frame = cv::Mat(180, 280, CV_8UC3);

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
	cvui::init(WindowName);

	// Set Volumnes
	double value = 100.0;

	const std::array<int, MaterialMax> MusicContainer = LoadMusic(Container);

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

	DxLib::DxLib_End();        // �c�w���C�u�����g�p�̏I������

	return 0;
}