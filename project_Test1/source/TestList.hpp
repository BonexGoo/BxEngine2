#pragma once

class TestListClass
{
public:
	enum {ButtonLength = 2};
	bool IsButtonPushed[ButtonLength];
	bool IsExitButtonPushed;

	id_font font1;
	id_font font2;
	id_sound sound1;
	id_sound sound2;

public:
	TestListClass()
	{
		for(int i = 0; i < ButtonLength; ++i)
			IsButtonPushed[i] = false;
		IsExitButtonPushed = false;

		font1 = BxCore::Font::Open("font/hy.ttf", 15);
		font2 = BxCore::Font::Open("font/hys.ttf", 11);
		sound1 = BxCore::Sound::Create("sound/popdown.wav");
		sound2 = BxCore::Sound::Create("sound/popup.wav");
	}

	~TestListClass()
	{
		BxCore::Font::Close(font1);
		BxCore::Font::Close(font2);
		BxCore::Sound::StopEffect();
		BxCore::Sound::Release(sound1);
		BxCore::Sound::Release(sound2);
	}
};
