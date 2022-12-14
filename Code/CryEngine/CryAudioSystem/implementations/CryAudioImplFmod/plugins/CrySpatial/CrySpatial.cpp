⼯䌠灯特杩瑨㈠㄰‹牃瑹步䜠扭⁈ 牃瑹步䜠潲灵‮汁⁬楲桧獴爠獥牥敶⹤਍﻿// Copyright 2001-2019 Crytek GmbH / Crytek Group. All rights reserved.਍ഀ
਍ഀ
#include "stdafx.h"਍ഀ
#include "CrySpatial.h"਍ഀ
#include "CrySpatialMath.h"਍ഀ
#include <algorithm>਍ഀ
਍ഀ
namespace CryAudio਍ഀ
{਍ഀ
namespace Impl਍ഀ
{਍ഀ
namespace Fmod਍ഀ
{਍ഀ
namespace Plugins਍ഀ
{਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::Reset()਍ഀ
{਍ഀ
	m_currentCycle = 0;਍ഀ
	m_delayPrev = 0;਍ഀ
	m_lastSourceDirection = ESourceDirection::None;਍ഀ
਍ഀ
	std::fill(std::begin(m_delayBuffer), std::end(m_delayBuffer), 0.0f);਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::CreateFilters()਍ഀ
{਍ഀ
	DeleteFilters();਍ഀ
਍ഀ
	m_pFilterBankA = new SBiquadIIFilterBank(static_cast<float>(m_sampleRate));਍ഀ
	m_pFilterBankB = new SBiquadIIFilterBank(static_cast<float>(m_sampleRate));਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::DeleteFilters()਍ഀ
{਍ഀ
	delete m_pFilterBankA;਍ഀ
	delete m_pFilterBankB;਍ഀ
਍ഀ
	m_pFilterBankA = nullptr;਍ഀ
	m_pFilterBankB = nullptr;਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
float CrySpatialState::DecibelToVolume(float const decibel)਍ഀ
{਍ഀ
	return powf(10.0f, 0.05f * decibel);਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
float CrySpatialState::VolumeToDecibel(float const volume)਍ഀ
{਍ഀ
	return 20.0f * log10f(volume);਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
float CrySpatialState::ComputeFade(float const volumeFactor, float const strength = 1.5f, EFadeType const fadeType = EFadeType::FadeinHyperbole)਍ഀ
{਍ഀ
	float returnVolume = 0.0f;਍ഀ
਍ഀ
	switch (fadeType)਍ഀ
	{਍ഀ
	case EFadeType::FadeinHyperbole:਍ഀ
		{਍ഀ
			returnVolume = (powf(g_euler, (strength * (volumeFactor - 1.0f))) * volumeFactor);਍ഀ
			break;਍ഀ
		}਍ഀ
	case EFadeType::FadeoutHyperbole:਍ഀ
		{਍ഀ
			returnVolume = (powf(g_euler, (strength * (-volumeFactor))) * (-volumeFactor + 1.0f));਍ഀ
			break;਍ഀ
		}਍ഀ
	case EFadeType::FadeinLogarithmic:਍ഀ
		{਍ഀ
			returnVolume = (powf(g_euler, (strength * (-volumeFactor))) * (volumeFactor - 1.0f)) + 1.0f;਍ഀ
			break;਍ഀ
		}਍ഀ
	case EFadeType::FadeoutLogarithmic:਍ഀ
		{਍ഀ
			returnVolume = (powf(g_euler, (strength * (volumeFactor - 1.0f))) * (-volumeFactor)) + 1.0f;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:    // EFadeType::FadeinHyperbole਍ഀ
		{਍ഀ
			returnVolume = (powf(g_euler, (strength * (volumeFactor - 1.0f))) * volumeFactor);਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	return returnVolume;਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::GetPositionalData(਍ഀ
	float const x /*azimuth side*/,਍ഀ
	float const e /*elevation*/,਍ഀ
	float const y /*azimuth front*/,਍ഀ
	int& out_quadrant,਍ഀ
	float& out_quadrantFine,਍ഀ
	float& out_elevation,਍ഀ
	float& out_distance)਍ഀ
{਍ഀ
	if ((x != 0.0f) || (y != 0.0f))਍ഀ
	{਍ഀ
		float const azimuthRadian = static_cast<float>(acosf(y / (sqrtf((y * y) + (x * x))))) / static_cast<float>(g_pi);਍ഀ
		out_quadrant = static_cast<int>(azimuthRadian * 2.0f);਍ഀ
਍ഀ
		out_quadrantFine = (azimuthRadian == 1.0f) ? 1.0f : fabsf((azimuthRadian * 2.0f)) - out_quadrant;਍ഀ
਍ഀ
		if (x < 0.0f)਍ഀ
		{਍ഀ
			out_quadrant = (out_quadrant == 0) ? 3 : 2;਍ഀ
		}਍ഀ
਍ഀ
		// angle of z/x/y to z/x/0਍ഀ
		float const length2D = sqrtf(y * y + x * x);਍ഀ
		float const length2DSquare = length2D * length2D;਍ഀ
		float const elevationCosProxy = roundf(((length2DSquare) / (sqrtf(length2DSquare + e * e) * sqrtf(length2DSquare))) * 100.0f) * 0.01f;਍ഀ
		float elevationRadianNormalized = (acosf(elevationCosProxy) / static_cast<float>(g_piHalf));਍ഀ
		elevationRadianNormalized = (e > 0) ? elevationRadianNormalized : -elevationRadianNormalized;਍ഀ
਍ഀ
		out_elevation = elevationRadianNormalized;਍ഀ
		out_distance = sqrtf(y * y + x * x + e * e);਍ഀ
	}਍ഀ
	else਍ഀ
	{਍ഀ
		out_quadrant = 0;਍ഀ
		out_quadrantFine = 0.0f;਍ഀ
		out_elevation = 0.0f;਍ഀ
		out_distance = 1.0f;਍ഀ
	}਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::ConsumeInput(float* pInbuffer, float* pOutbuffer, unsigned int const length, int const numChannels)਍ഀ
{਍ഀ
	unsigned int samples = length * numChannels;਍ഀ
	float emitterDistance = 1.0f;਍ഀ
	m_numInputChannels = numChannels;਍ഀ
਍ഀ
	// buffers are interleaved. The Format is:਍ഀ
	// s(1)c(1) s(1)c(2) ..s(1)c(n) | s(2)c(1) s(2)c(2) ..s(2)c(n)਍ഀ
	// s(x) == sample(x)਍ഀ
	// c(x) == channel(x)਍ഀ
਍ഀ
	if (m_numInputChannels != 1)਍ഀ
	{਍ഀ
		for (unsigned int i = 0; i < samples; ++i)਍ഀ
		{਍ഀ
			*pOutbuffer = *pInbuffer * emitterDistance;਍ഀ
			++pOutbuffer;਍ഀ
			++pInbuffer;਍ഀ
		}਍ഀ
	}਍ഀ
	else਍ഀ
	{਍ഀ
		HRTFMonoToBinaural(pInbuffer, pOutbuffer, length);਍ഀ
	}਍ഀ
਍ഀ
	m_currentCycle = (m_currentCycle == 0) ? 2 : (m_currentCycle == 1) ? 2 : 1;਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::HRTFMonoToBinaural(float* pInBuffer, float* pOutBuffer, unsigned int const frameLength)਍ഀ
{਍ഀ
	// Update Listener relative positioning਍ഀ
	GetPositionalData(਍ഀ
		m_position.relative.position.x,਍ഀ
		m_position.relative.position.y,਍ഀ
		m_position.relative.position.z,਍ഀ
		m_quadrant,਍ഀ
		m_quadrantFine,਍ഀ
		m_elevation,਍ഀ
		m_distance);਍ഀ
਍ഀ
	ESourceDirection const sourceDirection = (m_position.relative.position.x > 0.0f) ? ESourceDirection::Right : ESourceDirection::Left;਍ഀ
਍ഀ
	if (m_currentCycle == 0)਍ഀ
	{਍ഀ
		m_lastSourceDirection = sourceDirection;਍ഀ
	}਍ഀ
਍ഀ
	FilterBuffers(pInBuffer, frameLength, sourceDirection);਍ഀ
਍ഀ
	ComputeDelayChannelData(m_currentDelay);਍ഀ
਍ഀ
	m_bufferFadeStrength = static_cast<float>(abs(m_delayPrev - m_currentDelay)) / static_cast<float>(g_maxDelay);਍ഀ
	DelayAndFadeBuffers(m_currentDelay, frameLength, sourceDirection);਍ഀ
਍ഀ
	switch (sourceDirection)਍ഀ
	{਍ഀ
	case ESourceDirection::Right:਍ഀ
		{਍ഀ
			for (unsigned int i = 0; i < frameLength; ++i)਍ഀ
			{਍ഀ
				*pOutBuffer = m_concealedChannelBuffer[i];਍ഀ
				++pOutBuffer;਍ഀ
਍ഀ
				*pOutBuffer = m_directChannelBuffer[i];਍ഀ
				++pOutBuffer;਍ഀ
			}਍ഀ
਍ഀ
			break;਍ഀ
		}਍ഀ
	case ESourceDirection::Left:਍ഀ
		{਍ഀ
			for (unsigned int i = 0; i < frameLength; ++i)਍ഀ
			{਍ഀ
				*pOutBuffer = m_directChannelBuffer[i];਍ഀ
				++pOutBuffer;਍ഀ
਍ഀ
				*pOutBuffer = m_concealedChannelBuffer[i];਍ഀ
				++pOutBuffer;਍ഀ
			}਍ഀ
਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	m_delayPrev = m_currentDelay;਍ഀ
	m_lastSourceDirection = sourceDirection;਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::ComputeDelayChannelData(int& out_delay)਍ഀ
{਍ഀ
	float const elevationFactor = fabsf(m_elevation);਍ഀ
	float const elevationInversedClamp = (elevationFactor > 0.85f) ? 0.0f : 1.0f - (elevationFactor / 0.85f);਍ഀ
਍ഀ
	if ((m_quadrant == 0) || (m_quadrant == 3))਍ഀ
	{਍ഀ
		out_delay = static_cast<int>((g_maxDelay * ComputeFade(m_quadrantFine, 2.4f, EFadeType::FadeinLogarithmic)) * elevationInversedClamp);਍ഀ
	}਍ഀ
	else਍ഀ
	{਍ഀ
		out_delay = static_cast<int>((g_maxDelay * ComputeFade(m_quadrantFine, 2.4f, EFadeType::FadeoutLogarithmic)) * elevationInversedClamp);਍ഀ
	}਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::FilterBuffers(float* pInChannel, unsigned int const inputFrames, ESourceDirection const currentSourceDirection)਍ഀ
{਍ഀ
	float const elevationFactor = fabsf(m_elevation);਍ഀ
	float const elevationFactorInversedClamp = (elevationFactor > 0.85f) ? 0.0f : 1.0f - (elevationFactor / 0.85f);਍ഀ
਍ഀ
	BiquadIIFilter਍ഀ
	* filterDominant00,਍ഀ
	* filterDominant01,਍ഀ
	* filterDominant02,਍ഀ
	* filterDominant03,਍ഀ
	* filterDominant04,਍ഀ
	* filterDominant05,਍ഀ
	* filterDominant06,਍ഀ
	* filterDominant07,਍ഀ
	* filterDominant08,਍ഀ
	* filterDominant09,਍ഀ
	* filterDominant10,਍ഀ
	* filterDominant11,਍ഀ
਍ഀ
	* filterResidual00,਍ഀ
	* filterResidual01,਍ഀ
	* filterResidual02,਍ഀ
	* filterResidual03,਍ഀ
	* filterResidual04,਍ഀ
	* filterResidual05,਍ഀ
	* filterResidual06,਍ഀ
	* filterResidual07,਍ഀ
	* filterResidual08,਍ഀ
	* filterResidual09,਍ഀ
	* filterResidual10,਍ഀ
	* filterResidual11;਍ഀ
਍ഀ
	switch (m_currentCycle)਍ഀ
	{਍ഀ
	case 0:਍ഀ
		{਍ഀ
			filterDominant00 = &m_pFilterBankA->filterBand00;਍ഀ
			filterDominant01 = &m_pFilterBankA->filterBand01;਍ഀ
			filterDominant02 = &m_pFilterBankA->filterBand02;਍ഀ
			filterDominant03 = &m_pFilterBankA->filterBand03;਍ഀ
			filterDominant04 = &m_pFilterBankA->filterBand04;਍ഀ
			filterDominant05 = &m_pFilterBankA->filterBand05;਍ഀ
			filterDominant06 = &m_pFilterBankA->filterBand06;਍ഀ
			filterDominant07 = &m_pFilterBankA->filterBand07;਍ഀ
			filterDominant08 = &m_pFilterBankA->filterBand08;਍ഀ
			filterDominant09 = &m_pFilterBankA->filterBand09;਍ഀ
			filterDominant10 = &m_pFilterBankA->filterBand10;਍ഀ
			filterDominant11 = &m_pFilterBankA->filterBand11;਍ഀ
਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:਍ഀ
		{਍ഀ
			filterDominant00 = &m_pFilterBankA->filterBand00;਍ഀ
			filterDominant01 = &m_pFilterBankA->filterBand01;਍ഀ
			filterDominant02 = &m_pFilterBankA->filterBand02;਍ഀ
			filterDominant03 = &m_pFilterBankA->filterBand03;਍ഀ
			filterDominant04 = &m_pFilterBankA->filterBand04;਍ഀ
			filterDominant05 = &m_pFilterBankA->filterBand05;਍ഀ
			filterDominant06 = &m_pFilterBankA->filterBand06;਍ഀ
			filterDominant07 = &m_pFilterBankA->filterBand07;਍ഀ
			filterDominant08 = &m_pFilterBankA->filterBand08;਍ഀ
			filterDominant09 = &m_pFilterBankA->filterBand09;਍ഀ
			filterDominant10 = &m_pFilterBankA->filterBand10;਍ഀ
			filterDominant11 = &m_pFilterBankA->filterBand11;਍ഀ
਍ഀ
			filterResidual00 = &m_pFilterBankB->filterBand00;਍ഀ
			filterResidual01 = &m_pFilterBankB->filterBand01;਍ഀ
			filterResidual02 = &m_pFilterBankB->filterBand02;਍ഀ
			filterResidual03 = &m_pFilterBankB->filterBand03;਍ഀ
			filterResidual04 = &m_pFilterBankB->filterBand04;਍ഀ
			filterResidual05 = &m_pFilterBankB->filterBand05;਍ഀ
			filterResidual06 = &m_pFilterBankB->filterBand06;਍ഀ
			filterResidual07 = &m_pFilterBankB->filterBand07;਍ഀ
			filterResidual08 = &m_pFilterBankB->filterBand08;਍ഀ
			filterResidual09 = &m_pFilterBankB->filterBand09;਍ഀ
			filterResidual10 = &m_pFilterBankB->filterBand10;਍ഀ
			filterResidual11 = &m_pFilterBankB->filterBand11;਍ഀ
਍ഀ
			break;਍ഀ
		}਍ഀ
	case 2:਍ഀ
		{਍ഀ
			filterResidual00 = &m_pFilterBankA->filterBand00;਍ഀ
			filterResidual01 = &m_pFilterBankA->filterBand01;਍ഀ
			filterResidual02 = &m_pFilterBankA->filterBand02;਍ഀ
			filterResidual03 = &m_pFilterBankA->filterBand03;਍ഀ
			filterResidual04 = &m_pFilterBankA->filterBand04;਍ഀ
			filterResidual05 = &m_pFilterBankA->filterBand05;਍ഀ
			filterResidual06 = &m_pFilterBankA->filterBand06;਍ഀ
			filterResidual07 = &m_pFilterBankA->filterBand07;਍ഀ
			filterResidual08 = &m_pFilterBankA->filterBand08;਍ഀ
			filterDominant09 = &m_pFilterBankB->filterBand09;਍ഀ
			filterDominant10 = &m_pFilterBankB->filterBand10;਍ഀ
			filterDominant11 = &m_pFilterBankB->filterBand11;਍ഀ
਍ഀ
			filterDominant00 = &m_pFilterBankB->filterBand00;਍ഀ
			filterDominant01 = &m_pFilterBankB->filterBand01;਍ഀ
			filterDominant02 = &m_pFilterBankB->filterBand02;਍ഀ
			filterDominant03 = &m_pFilterBankB->filterBand03;਍ഀ
			filterDominant04 = &m_pFilterBankB->filterBand04;਍ഀ
			filterDominant05 = &m_pFilterBankB->filterBand05;਍ഀ
			filterDominant06 = &m_pFilterBankB->filterBand06;਍ഀ
			filterDominant07 = &m_pFilterBankB->filterBand07;਍ഀ
			filterDominant08 = &m_pFilterBankB->filterBand08;਍ഀ
			filterResidual09 = &m_pFilterBankA->filterBand09;਍ഀ
			filterResidual10 = &m_pFilterBankA->filterBand10;਍ഀ
			filterResidual11 = &m_pFilterBankA->filterBand11;਍ഀ
਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	// AZIMUTH COMMON਍ഀ
	//਍ഀ
	// BAND00 500Hz਍ഀ
	int band00Frequency;਍ഀ
	float band00Quality = 3.0f;਍ഀ
	float band00Gain;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band00Frequency = 500 + static_cast<int>(400.0f * ComputeFade(m_quadrantFine, 1.47f, EFadeType::FadeinHyperbole));਍ഀ
			band00Gain = (2.0f - 1.0f * m_quadrantFine) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band00Frequency = 900 + static_cast<int>(400.0f * ComputeFade(m_quadrantFine, 0.6f, EFadeType::FadeinLogarithmic));਍ഀ
			band00Quality = 3.0f - (0.8f * ComputeFade(m_quadrantFine, 1.7f, EFadeType::FadeinLogarithmic));਍ഀ
			band00Gain = (1.0f + (5.0f * ComputeFade(m_quadrantFine, 0.53f, EFadeType::FadeinLogarithmic))) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant00->ComputeCoefficients(band00Frequency, band00Quality, band00Gain);਍ഀ
਍ഀ
	// BAND01 1000Hz਍ഀ
	int band01Frequency;਍ഀ
	float band01Quality;਍ഀ
	float band01Gain;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band01Frequency = (1000 + static_cast<int>(1000.0f * ComputeFade(m_quadrantFine, 0.87f, EFadeType::FadeinHyperbole)));਍ഀ
			band01Quality = 3.5f + (1.0f * ComputeFade(m_quadrantFine, 1.0f, EFadeType::FadeinHyperbole));਍ഀ
			band01Gain = (-6.0f + (1.0f * m_quadrantFine)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band01Frequency = (2000 + static_cast<int>(1500.0f * ComputeFade(m_quadrantFine, 1.39f, EFadeType::FadeinLogarithmic)));਍ഀ
			band01Quality = 4.5f + (1.5f * ComputeFade(m_quadrantFine, 0.55f, EFadeType::FadeinLogarithmic));਍ഀ
			band01Gain = (-5.0f + (2.0f * ComputeFade(m_quadrantFine, 2.29f, EFadeType::FadeinLogarithmic))) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant01->ComputeCoefficients(band01Frequency, band01Quality, band01Gain);਍ഀ
਍ഀ
	// BAND02 3000 Hz਍ഀ
	int band02Frequency;਍ഀ
	float band02Quality;਍ഀ
	float band02Gain;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band02Frequency = 3100 + static_cast<int>(3400.0f * ComputeFade(m_quadrantFine, 2.32f, EFadeType::FadeinHyperbole));਍ഀ
			band02Quality = 1.3f + (-1.2f * ComputeFade(m_quadrantFine, 1.64f, EFadeType::FadeinHyperbole));਍ഀ
			band02Gain = (6.0f - (3.0f * m_quadrantFine)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band02Frequency = 6500 + static_cast<int>(4300.0f * ComputeFade(m_quadrantFine, 1.06f, EFadeType::FadeinLogarithmic));਍ഀ
			band02Quality = 0.1f + (1.2f * ComputeFade(m_quadrantFine, 1.83f, EFadeType::FadeinLogarithmic));਍ഀ
			band02Gain = 3.0f * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant02->ComputeCoefficients(band02Frequency, band02Quality, band02Gain);਍ഀ
਍ഀ
	// BAND03਍ഀ
	int band03Frequency;਍ഀ
	float band03Quality;਍ഀ
	float band03Gain;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band03Frequency = 8300 - static_cast<int>(2300.0f * m_quadrantFine);਍ഀ
			band03Quality = 10.0f - (3.0f * m_quadrantFine);਍ഀ
			band03Gain = ((-6.0f) * m_quadrantFine) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band03Frequency = 6000 - static_cast<int>(2250.0f * m_quadrantFine);਍ഀ
			band03Quality = 7.0f - (2.5f * ComputeFade(m_quadrantFine, 0.77f, EFadeType::FadeinLogarithmic));਍ഀ
			band03Gain = (-6.0f + (3.0f * m_quadrantFine)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant03->ComputeCoefficients(band03Frequency, band03Quality, band03Gain);਍ഀ
਍ഀ
	// BAND04਍ഀ
	int band04Frequency;਍ഀ
	float band04Gain;਍ഀ
	float band04Quality;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band04Frequency = 10650 - static_cast<int>(1650.0f * ComputeFade(m_quadrantFine, 2.3f, EFadeType::FadeinHyperbole));਍ഀ
			band04Quality = (7.7f + (2.3f * m_quadrantFine));਍ഀ
			band04Gain = (-8.0f * m_quadrantFine) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band04Frequency = 9000 - static_cast<int>(1500.0f * ComputeFade(m_quadrantFine, 2.24f, EFadeType::FadeinLogarithmic));਍ഀ
			band04Quality = (10.0f - (2.1f * ComputeFade(m_quadrantFine, 1.64f, EFadeType::FadeinLogarithmic)));਍ഀ
			band04Gain = (-8.0f * ComputeFade(m_quadrantFine, 1.87f, EFadeType::FadeoutHyperbole)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant04->ComputeCoefficients(band04Frequency, band04Quality, band04Gain);਍ഀ
਍ഀ
	// BAND05਍ഀ
	int band05Frequency;਍ഀ
	float band05Gain;਍ഀ
	float band05Quality;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band05Frequency = 12850 - static_cast<int>(3850.0f * m_quadrantFine);਍ഀ
			band05Quality = 10.0f - (5.0f * m_quadrantFine);਍ഀ
			band05Gain = (-1.0f * m_quadrantFine) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band05Frequency = 9000 + static_cast<int>(3000.0f * ComputeFade(m_quadrantFine, 1.17f, EFadeType::FadeinLogarithmic));਍ഀ
			band05Quality = 5.0f - (4.1f * ComputeFade(m_quadrantFine, 1.64f, EFadeType::FadeinLogarithmic));਍ഀ
			band05Gain = (-1.0f + (3.0f * ComputeFade(m_quadrantFine, 1.35f, EFadeType::FadeinLogarithmic))) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant05->ComputeCoefficients(band05Frequency, band05Quality, band05Gain);਍ഀ
਍ഀ
	// ELEVATION਍ഀ
	//਍ഀ
	int band06Frequency;਍ഀ
	float band06Quality;਍ഀ
	float band06Gain;਍ഀ
਍ഀ
	int band07Frequency;਍ഀ
	float band07Quality;਍ഀ
	float band07Gain;਍ഀ
਍ഀ
	int band08Frequency;਍ഀ
	float band08Quality;਍ഀ
	float band08Gain;਍ഀ
਍ഀ
	if (m_elevation > 0)਍ഀ
	{਍ഀ
		band06Frequency = 6000 - static_cast<int>(3700.0f * ComputeFade(elevationFactor, 0.9f, EFadeType::FadeinLogarithmic));਍ഀ
		band07Frequency = 6000 + static_cast<int>(3100.0f * ComputeFade(elevationFactor, 0.4f, EFadeType::FadeinLogarithmic));਍ഀ
		band08Frequency = 4000 + static_cast<int>(8000.0f * ComputeFade(elevationFactor, 1.0f, EFadeType::FadeinLogarithmic));਍ഀ
਍ഀ
		band06Quality = 3.0f + (7.0f * ComputeFade(elevationFactor, 0.8f, EFadeType::FadeinLogarithmic));਍ഀ
		band07Quality = 4.5f + (5.5f * ComputeFade(elevationFactor, 2.7f, EFadeType::FadeinHyperbole));਍ഀ
		band08Quality = 4.5f + (5.5f * ComputeFade(elevationFactor, 1.7f, EFadeType::FadeinLogarithmic));਍ഀ
਍ഀ
		band06Gain = -15.0f * ComputeFade(elevationFactor, 0.8f, EFadeType::FadeinLogarithmic);਍ഀ
		band07Gain = 6.0f * ComputeFade(elevationFactor, 1.7f, EFadeType::FadeinLogarithmic);਍ഀ
		band08Gain = -15.0f * ComputeFade(elevationFactor, 2.5f, EFadeType::FadeinLogarithmic);਍ഀ
	}਍ഀ
	else਍ഀ
	{਍ഀ
		band06Frequency = 6000 + static_cast<int>(4700.0f * elevationFactor);਍ഀ
		band07Frequency = 6000 - static_cast<int>(900.0f * elevationFactor);਍ഀ
		band08Frequency = 4000 - static_cast<int>(800.0f * elevationFactor);਍ഀ
਍ഀ
		band06Quality = 3.0f + (4.7f * elevationFactor);਍ഀ
		band07Quality = 4.5f - (1.2f * elevationFactor);਍ഀ
		band08Quality = 4.5f + (5.5f * elevationFactor);਍ഀ
਍ഀ
		band06Gain = -10.0f * elevationFactor;਍ഀ
		band07Gain = -14.0f * ComputeFade(elevationFactor, 1.5f, EFadeType::FadeinLogarithmic);਍ഀ
		band08Gain = -8.0f * ComputeFade(elevationFactor, 1.0f, EFadeType::FadeinLogarithmic);਍ഀ
	}਍ഀ
਍ഀ
	filterDominant06->ComputeCoefficients(band06Frequency, band06Quality, band06Gain);਍ഀ
	filterDominant07->ComputeCoefficients(band07Frequency, band07Quality, band07Gain);਍ഀ
	filterDominant08->ComputeCoefficients(band08Frequency, band08Quality, band08Gain);਍ഀ
਍ഀ
	// AZIMUTH SPECIFIC਍ഀ
	// 09 Direct, 10 Concealed, 11 Concealed਍ഀ
	//਍ഀ
	int band09Frequency;਍ഀ
	float band09Quality = 1.0f;਍ഀ
	float band09Gain;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band09Frequency = 9000 - static_cast<int>(7700.0f * ComputeFade(m_quadrantFine, 3.11f, EFadeType::FadeinLogarithmic));਍ഀ
			band09Gain = (6.0f * ComputeFade(m_quadrantFine, 1.67f, EFadeType::FadeinLogarithmic)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band09Frequency = 1300 + static_cast<int>(4200.0f * ComputeFade(m_quadrantFine, 0.67f, EFadeType::FadeinHyperbole));਍ഀ
			band09Gain = (6.0f * ComputeFade(m_quadrantFine, 1.06f, EFadeType::FadeoutLogarithmic)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant09->ComputeCoefficients(band09Frequency, band09Quality, band09Gain);਍ഀ
਍ഀ
	int band10Frequency = 1000;਍ഀ
	float band10Quality;਍ഀ
	float band10Gain;਍ഀ
਍ഀ
	int band11Frequency;਍ഀ
	float band11Quality = 1.0f;਍ഀ
	float band11Gain;਍ഀ
਍ഀ
	switch (m_quadrant)਍ഀ
	{਍ഀ
	case 0:     // Fall through਍ഀ
	case 3:਍ഀ
		{਍ഀ
			band10Quality = 0.8f + (1.2f * m_quadrantFine);਍ഀ
			band10Gain = (4.0f * ComputeFade(m_quadrantFine, 3.32f, EFadeType::FadeinHyperbole)) * elevationFactorInversedClamp;਍ഀ
਍ഀ
			band11Frequency = 9000 - static_cast<int>(6000.0f * m_quadrantFine);਍ഀ
			band11Gain = ((-13.0f) * ComputeFade(m_quadrantFine, 0.64f, EFadeType::FadeinHyperbole)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // Fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			band10Frequency += static_cast<int>(1000.0f * m_quadrantFine);਍ഀ
			band10Quality = 2.0f - (1.7f * ComputeFade(m_quadrantFine, 1.64f, EFadeType::FadeinHyperbole));਍ഀ
			band10Gain = (4.0f * ComputeFade(m_quadrantFine, 3.4f, EFadeType::FadeoutLogarithmic)) * elevationFactorInversedClamp;਍ഀ
਍ഀ
			band11Frequency = 3000 + static_cast<int>(5000.0f * ComputeFade(m_quadrantFine, 1.1f, EFadeType::FadeinLogarithmic));਍ഀ
			band11Gain = (-13.0f * ComputeFade(m_quadrantFine, 2.4f, EFadeType::FadeoutLogarithmic)) * elevationFactorInversedClamp;਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	filterDominant10->ComputeCoefficients(band10Frequency, band10Quality, band10Gain);਍ഀ
	filterDominant11->ComputeCoefficients(band11Frequency, band11Quality, band11Gain);਍ഀ
਍ഀ
	float* pChannelDirect = m_directChannelBuffer;਍ഀ
	float* pChannelConcealed = m_concealedChannelBufferIntermediate;਍ഀ
਍ഀ
	switch (m_currentCycle)਍ഀ
	{਍ഀ
	case 0:਍ഀ
		{਍ഀ
			float const fadeFactor = 1.0f / g_smallFadeLength;਍ഀ
਍ഀ
			for (int i = 0; i < g_smallFadeLengthInteger; ++i)਍ഀ
			{਍ഀ
				float sampleFiltered =਍ഀ
					filterDominant08->ProcessSample(਍ഀ
						filterDominant07->ProcessSample(਍ഀ
							filterDominant06->ProcessSample(਍ഀ
								filterDominant05->ProcessSample(਍ഀ
									filterDominant04->ProcessSample(਍ഀ
										filterDominant03->ProcessSample(਍ഀ
											filterDominant02->ProcessSample(਍ഀ
												filterDominant01->ProcessSample(਍ഀ
													filterDominant00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
				*pChannelDirect = filterDominant09->ProcessSample(sampleFiltered) * (i * fadeFactor);਍ഀ
				*pChannelConcealed = filterDominant10->ProcessSample(filterDominant11->ProcessSample(sampleFiltered)) * (i * fadeFactor);਍ഀ
਍ഀ
				++pInChannel;਍ഀ
				++pChannelDirect;਍ഀ
				++pChannelConcealed;਍ഀ
			}਍ഀ
਍ഀ
			int const cyclesRemaining = inputFrames - g_smallFadeLengthInteger;਍ഀ
਍ഀ
			for (int i = 0; i < cyclesRemaining; ++i)਍ഀ
			{਍ഀ
				float const sampleFiltered =਍ഀ
					filterDominant08->ProcessSample(਍ഀ
						filterDominant07->ProcessSample(਍ഀ
							filterDominant06->ProcessSample(਍ഀ
								filterDominant05->ProcessSample(਍ഀ
									filterDominant04->ProcessSample(਍ഀ
										filterDominant03->ProcessSample(਍ഀ
											filterDominant02->ProcessSample(਍ഀ
												filterDominant01->ProcessSample(਍ഀ
													filterDominant00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
				*pChannelDirect = filterDominant09->ProcessSample(sampleFiltered);਍ഀ
				*pChannelConcealed = filterDominant10->ProcessSample(filterDominant11->ProcessSample(sampleFiltered));਍ഀ
਍ഀ
				++pInChannel;਍ഀ
				++pChannelDirect;਍ഀ
				++pChannelConcealed;਍ഀ
			}਍ഀ
਍ഀ
			break;਍ഀ
		}਍ഀ
	case 1:     // fall through਍ഀ
	case 2:਍ഀ
		{਍ഀ
			float const fadeFactor = 1.0f / g_largeFadeLength;਍ഀ
਍ഀ
			if (m_lastSourceDirection == currentSourceDirection)਍ഀ
			{਍ഀ
				for (int i = 0; i < g_largeFadeLengthInteger; ++i) // Blend਍ഀ
				{਍ഀ
					float newFilter =਍ഀ
						filterDominant08->ProcessSample(਍ഀ
							filterDominant07->ProcessSample(਍ഀ
								filterDominant06->ProcessSample(਍ഀ
									filterDominant05->ProcessSample(਍ഀ
										filterDominant04->ProcessSample(਍ഀ
											filterDominant03->ProcessSample(਍ഀ
												filterDominant02->ProcessSample(਍ഀ
													filterDominant01->ProcessSample(਍ഀ
														filterDominant00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
					float oldFilter =਍ഀ
						filterResidual08->ProcessSample(਍ഀ
							filterResidual07->ProcessSample(਍ഀ
								filterResidual06->ProcessSample(਍ഀ
									filterResidual05->ProcessSample(਍ഀ
										filterResidual04->ProcessSample(਍ഀ
											filterResidual03->ProcessSample(਍ഀ
												filterResidual02->ProcessSample(਍ഀ
													filterResidual01->ProcessSample(਍ഀ
														filterResidual00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
					float fadeValue = i * fadeFactor;਍ഀ
਍ഀ
					*pChannelDirect =਍ഀ
						(filterDominant09->ProcessSample(newFilter) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeinLogarithmic))਍ഀ
						+ (filterResidual09->ProcessSample(oldFilter) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeoutLogarithmic));਍ഀ
਍ഀ
					*pChannelConcealed =਍ഀ
						(filterDominant10->ProcessSample(਍ഀ
							 filterDominant11->ProcessSample(newFilter)) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeinLogarithmic))਍ഀ
						+ (filterResidual10->ProcessSample(਍ഀ
								 filterResidual11->ProcessSample(oldFilter)) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeoutLogarithmic));਍ഀ
਍ഀ
					++pInChannel;਍ഀ
					++pChannelDirect;਍ഀ
					++pChannelConcealed;਍ഀ
				}਍ഀ
਍ഀ
				int const cyclesRemaining = inputFrames - g_largeFadeLengthInteger;਍ഀ
਍ഀ
				for (int i = 0; i < cyclesRemaining; ++i) // Write Rest with new Filters਍ഀ
				{਍ഀ
					float const sampleFiltered =਍ഀ
						filterDominant08->ProcessSample(਍ഀ
							filterDominant07->ProcessSample(਍ഀ
								filterDominant06->ProcessSample(਍ഀ
									filterDominant05->ProcessSample(਍ഀ
										filterDominant04->ProcessSample(਍ഀ
											filterDominant03->ProcessSample(਍ഀ
												filterDominant02->ProcessSample(਍ഀ
													filterDominant01->ProcessSample(਍ഀ
														filterDominant00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
					*pChannelDirect = filterDominant09->ProcessSample(sampleFiltered);਍ഀ
					*pChannelConcealed = filterDominant10->ProcessSample(filterDominant11->ProcessSample(sampleFiltered));਍ഀ
਍ഀ
					++pInChannel;਍ഀ
					++pChannelDirect;਍ഀ
					++pChannelConcealed;਍ഀ
				}਍ഀ
			}਍ഀ
			else਍ഀ
			{਍ഀ
				for (int i = 0; i < g_largeFadeLengthInteger; ++i) // Blend਍ഀ
				{਍ഀ
					float newFilter =਍ഀ
						filterDominant08->ProcessSample(਍ഀ
							filterDominant07->ProcessSample(਍ഀ
								filterDominant06->ProcessSample(਍ഀ
									filterDominant05->ProcessSample(਍ഀ
										filterDominant04->ProcessSample(਍ഀ
											filterDominant03->ProcessSample(਍ഀ
												filterDominant02->ProcessSample(਍ഀ
													filterDominant01->ProcessSample(਍ഀ
														filterDominant00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
					float oldFilter =਍ഀ
						filterResidual08->ProcessSample(਍ഀ
							filterResidual07->ProcessSample(਍ഀ
								filterResidual06->ProcessSample(਍ഀ
									filterResidual05->ProcessSample(਍ഀ
										filterResidual04->ProcessSample(਍ഀ
											filterResidual03->ProcessSample(਍ഀ
												filterResidual02->ProcessSample(਍ഀ
													filterResidual01->ProcessSample(਍ഀ
														filterResidual00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
					float fadeValue = i * fadeFactor;਍ഀ
਍ഀ
					*pChannelDirect =਍ഀ
						(filterDominant09->ProcessSample(newFilter) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeinLogarithmic))਍ഀ
						+ (filterResidual10->ProcessSample(਍ഀ
								 filterResidual11->ProcessSample(oldFilter)) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeoutLogarithmic));਍ഀ
਍ഀ
					*pChannelConcealed =਍ഀ
						(filterDominant10->ProcessSample(਍ഀ
							 filterDominant11->ProcessSample(newFilter)) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeinLogarithmic))਍ഀ
						+ (filterResidual09->ProcessSample(oldFilter) * ComputeFade(fadeValue, g_linearFadeStrength, EFadeType::FadeoutLogarithmic));਍ഀ
਍ഀ
					++pInChannel;਍ഀ
					++pChannelDirect;਍ഀ
					++pChannelConcealed;਍ഀ
				}਍ഀ
਍ഀ
				int const cyclesRemaining = inputFrames - g_largeFadeLengthInteger;਍ഀ
਍ഀ
				for (int i = 0; i < cyclesRemaining; ++i) // Write Rest with new Filters਍ഀ
				{਍ഀ
					float sampleFiltered =਍ഀ
						filterDominant08->ProcessSample(਍ഀ
							filterDominant07->ProcessSample(਍ഀ
								filterDominant06->ProcessSample(਍ഀ
									filterDominant05->ProcessSample(਍ഀ
										filterDominant04->ProcessSample(਍ഀ
											filterDominant03->ProcessSample(਍ഀ
												filterDominant02->ProcessSample(਍ഀ
													filterDominant01->ProcessSample(਍ഀ
														filterDominant00->ProcessSample(*pInChannel)))))))));਍ഀ
਍ഀ
					*pChannelDirect = filterDominant09->ProcessSample(sampleFiltered);਍ഀ
					*pChannelConcealed = filterDominant10->ProcessSample(filterDominant11->ProcessSample(sampleFiltered));਍ഀ
਍ഀ
					++pInChannel;਍ഀ
					++pChannelDirect;਍ഀ
					++pChannelConcealed;਍ഀ
				}਍ഀ
			}਍ഀ
			break;਍ഀ
		}਍ഀ
	default:਍ഀ
		{਍ഀ
			break;਍ഀ
		}਍ഀ
	}਍ഀ
}਍ഀ
਍ഀ
//////////////////////////////////////////////////////////////////////////਍ഀ
void CrySpatialState::DelayAndFadeBuffers(਍ഀ
	int const delayCurrent,਍ഀ
	int const maxFrames,਍ഀ
	ESourceDirection const currentSourceDirection)਍ഀ
{਍ഀ
	// Delay Concealed Buffer਍ഀ
	{਍ഀ
		int outSampleOffset = 0;                                                                   // needed to compensate delay changes since the last Engine tick਍ഀ
		int const delayPrev = (currentSourceDirection == m_lastSourceDirection) ? m_delayPrev : 0; // If we switch L/R then prev delay is 0਍ഀ
		float* pInChannelConcealedProxy = m_concealedChannelBufferIntermediate;਍ഀ
		float* pOutChannelConcealedProxy = m_concealedChannelBuffer + delayPrev;਍ഀ
਍ഀ
		memcpy(m_concealedChannelBuffer, m_delayBuffer, sizeof(float) * delayPrev);਍ഀ
਍ഀ
		// Cross fade਍ഀ
		if (delayPrev != delayCurrent)਍ഀ
		{਍ഀ
			if (delayPrev < delayCurrent)਍ഀ
			{਍ഀ
				float* pInChannelConcealedOffsetProxy = m_concealedChannelBufferIntermediate;਍ഀ
				int const sampleGap = delayCurrent - delayPrev;਍ഀ
਍ഀ
				for (int i = 0; i < sampleGap; ++i)਍ഀ
				{਍ഀ
					*pOutChannelConcealedProxy = *pInChannelConcealedOffsetProxy;਍ഀ
					++pOutChannelConcealedProxy;਍ഀ
					++pInChannelConcealedOffsetProxy;਍ഀ
				}਍ഀ
਍ഀ
				float const fadeFactor = 1.0f / g_mediumFadeLength;਍ഀ
਍ഀ
				for (int i = 1; i <= g_mediumFadeLengthInteger; ++i)਍ഀ
				{਍ഀ
					float const fadeValue = i * fadeFactor;਍ഀ
਍ഀ
					*pOutChannelConcealedProxy =਍ഀ
						((*pInChannelConcealedOffsetProxy * ComputeFade(fadeValue, m_bufferFadeStrength, EFadeType::FadeoutLogarithmic)) +਍ഀ
						 (*pInChannelConcealedProxy * ComputeFade(fadeValue, m_bufferFadeStrength, EFadeType::FadeinLogarithmic)));਍ഀ
਍ഀ
					++pOutChannelConcealedProxy;਍ഀ
					++pInChannelConcealedProxy;਍ഀ
					++pInChannelConcealedOffsetProxy;਍ഀ
				}਍ഀ
਍ഀ
				outSampleOffset = g_mediumFadeLengthInteger + sampleGap;਍ഀ
			}਍ഀ
			else      // delay gets smaller਍ഀ
			{਍ഀ
				int const samplesToDrop = delayPrev - delayCurrent;਍ഀ
				float* pInChannelConcealedOffsetProxy = m_concealedChannelBufferIntermediate;਍ഀ
				pInChannelConcealedProxy += samplesToDrop;਍ഀ
਍ഀ
				float const fadeFactor = 1.0f / g_mediumFadeLength;਍ഀ
਍ഀ
				for (int i = 1; i <= g_mediumFadeLength; ++i)਍ഀ
				{਍ഀ
					float const fadeValue = i * fadeFactor;਍ഀ
਍ഀ
					*pOutChannelConcealedProxy =਍ഀ
						(*pInChannelConcealedOffsetProxy * ComputeFade(fadeValue, m_bufferFadeStrength, EFadeType::FadeoutLogarithmic)) +਍ഀ
						(*pInChannelConcealedProxy * ComputeFade(fadeValue, m_bufferFadeStrength, EFadeType::FadeinLogarithmic));਍ഀ
਍ഀ
					++pOutChannelConcealedProxy;਍ഀ
					++pInChannelConcealedOffsetProxy;਍ഀ
					++pInChannelConcealedProxy;਍ഀ
				}਍ഀ
਍ഀ
				outSampleOffset = g_mediumFadeLengthInteger;਍ഀ
			}਍ഀ
		}਍ഀ
਍ഀ
		// Write from current inputBuffer਍ഀ
		int const cycles = (maxFrames - (delayPrev + outSampleOffset));਍ഀ
਍ഀ
		memcpy(pOutChannelConcealedProxy, pInChannelConcealedProxy, sizeof(float) * cycles);਍ഀ
	}਍ഀ
਍ഀ
	// DirectChannel Fades with Delayed signal਍ഀ
	if (currentSourceDirection != m_lastSourceDirection)਍ഀ
	{਍ഀ
		float* pChannelDirectProxy = m_directChannelBuffer;਍ഀ
		float* pDelChannelProxy = m_concealedChannelBuffer;਍ഀ
਍ഀ
		float const fadeFactor = 1.0f / g_mediumFadeLength;਍ഀ
਍ഀ
		for (int i = 0; i < g_mediumFadeLengthInteger; ++i)਍ഀ
		{਍ഀ
			*pChannelDirectProxy =਍ഀ
				(*pChannelDirectProxy * ComputeFade((i * fadeFactor), m_bufferFadeStrength, EFadeType::FadeinLogarithmic))਍ഀ
				+ ((*pDelChannelProxy * ComputeFade((i * fadeFactor), m_bufferFadeStrength, EFadeType::FadeoutLogarithmic)));਍ഀ
਍ഀ
			++pChannelDirectProxy;਍ഀ
			++pDelChannelProxy;਍ഀ
		}਍ഀ
	}਍ഀ
਍ഀ
	// Write new Delay Buffer਍ഀ
	if (delayCurrent > 0)਍ഀ
	{਍ഀ
		int const offset = maxFrames - delayCurrent;਍ഀ
		float* pInChannelConcealed = m_concealedChannelBufferIntermediate + offset;਍ഀ
਍ഀ
		memcpy(m_delayBuffer, pInChannelConcealed, sizeof(float) * delayCurrent);਍ഀ
	}਍ഀ
}਍ഀ
} // namespace Plugins਍ഀ
} // namespace Fmod਍ഀ
} // namespace Impl਍ഀ
} // namespace CryAudio