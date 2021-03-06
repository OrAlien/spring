/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _SOUND_H_
#define _SOUND_H_

#include <string>
#include <vector>

#include "System/Sound/ISound.h"
#include "System/float3.h"
#include "System/UnorderedMap.hpp"
#include "System/Threading/SpringThreading.h"

#include "SoundItem.h"

class CSoundSource;
class SoundBuffer;
class SoundItem;
struct ALCdevice_struct;
typedef struct ALCdevice_struct ALCdevice;

/// Default sound system implementation (OpenAL)
class CSound : public ISound
{
public:
	CSound();
	virtual ~CSound();

	bool HasSoundItem(const std::string& name) const  override;
	size_t GetSoundId(const std::string& name) override;
	SoundItem* GetSoundItem(size_t id) const;

	CSoundSource* GetNextBestSource(bool lock = true) override;

	void UpdateListener(const float3& campos, const float3& camdir, const float3& camup) override;
	void NewFrame() override;

	/// @see ConfigHandler::ConfigNotifyCallback
	void ConfigNotify(const std::string& key, const std::string& value) override;
	void PitchAdjust(const float newPitch) override;

	bool Mute() override;
	bool IsMuted() const override { return mute; }

	void Iconified(bool state) override;

	void PrintDebugInfo() override;

	bool SoundThreadQuit() const { return soundThreadQuit; }
	bool CanLoadSoundDefs() const { return canLoadDefs; }

	bool LoadSoundDefsImpl(const std::string& fileName, const std::string& modes);
	const float3& GetListenerPos() const { return myPos; }

private:
	typedef spring::unordered_map<std::string, std::string> SoundItemNameMap;
	typedef spring::unordered_map<std::string, SoundItemNameMap> SoundItemDefsMap;

private:
	void InitThread(int cfgMaxSounds);
	void UpdateThread(int cfgMaxSounds);

	void Update();
	void UpdateListenerReal();

	int GetMaxMonoSources(ALCdevice* device, int cfgMaxSounds);
	void GenSources(int alMaxSounds);

	size_t MakeItemFromDef(const SoundItemNameMap& itemDef);
	size_t LoadSoundBuffer(const std::string& filename);

private:
	spring::thread soundThread;
	spring::unordered_map<std::string, size_t> soundMap; // <name, id>
	std::vector<SoundItem*> soundItems;
	std::vector<CSoundSource> soundSources; // fixed-size

	SoundItemNameMap defaultItemNameMap;
	SoundItemDefsMap soundItemDefsMap;

	float masterVolume;

	/// unscaled
	float3 myPos;
	float3 camDir;
	float3 camUp;
	float3 prevVelocity;

	int pitchAdjustMode;

	bool listenerNeedsUpdate;
	bool mute;

	/// we do not play if minimized / iconified
	bool appIsIconified;

	volatile bool soundThreadQuit;
	volatile bool canLoadDefs;
};

#endif // _SOUND_H_
