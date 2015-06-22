//#include <stdafx.h>
#include "Oscillator.h"
#include <string.h>
#include <math.h>
#include "OscillatorKind.h"
//#include "..\..\AudioEngineDLL\MIDIChannelMessage.h"
#include "MIDIChannelMessage.h"


const OscillatorKind Oscillator::kinna;

Oscillator::Oscillator(ModuleServices* pService) : SimpleGenerator(pService, Oscillator::kinna.getPropertyNumber(), &Oscillator::kinna),
PhaseInPort(ElaborationUnitPort::INPUT_PORT,ElaborationUnitPort::AUDIO_PORT,ElaborationUnitPort::SINGLE_PORT),
AmplitudeInPort(ElaborationUnitPort::INPUT_PORT,ElaborationUnitPort::AUDIO_PORT,ElaborationUnitPort::SINGLE_PORT),
MainOutPort(ElaborationUnitPort::OUTPUT_PORT,ElaborationUnitPort::AUDIO_PORT,ElaborationUnitPort::MULTI_PORT),
MIDIInPort(ElaborationUnitPort::INPUT_PORT,ElaborationUnitPort::MIDI_PORT,ElaborationUnitPort::SINGLE_PORT)
{
	iNumInput = 3;
	iNumOutput = 1;
	PhaseInPort.setName(OscillatorKind::PhasePortName);
	AmplitudeInPort.setName(OscillatorKind::AmplitudePortName);
	MainOutPort.setName(OscillatorKind::MainOutPortName);
	MIDIInPort.setName(OscillatorKind::MIDIPortName);
	m_WaveKind = e_Sine;
	//m_WaveKind = e_Square;
	m_Amplitude = 1.0;
	m_FreqAccumulator = 0.0;

	m_NumOn = 200;

	m_Acc = 0;
	m_bLevelHigh = true;

	m_SamplingFrequency = 48000.0;
	m_SamplingTime = 1.0 / m_SamplingFrequency;

	m_pPhaseInBuffer = NULL;
	m_pAmplitudeInBuffer = NULL;

	m_bActive = false;

	m_SamplesBufferMaxSize = 0;

	EngineSettings* engineSettings = m_pModuleServices->getEngineSettings();
	double samplingPeriod = 1.0 / engineSettings->samplingFrequence;
	initVoices(samplingPeriod);
}

void Oscillator::initVoices(EAG_SAMPLE_TYPE updatePeriod)
{
	/*
	for(int i=0;i<MIDIChannelMessage::NumMIDINotes;i++)
	{
		m_pVoices[i] = new OscillatorVoice(updatePeriod);
	}
	*/
}

Oscillator::~Oscillator()
{
	/*
	if( m_pPhaseInBuffer )
		delete m_pPhaseInBuffer;
	if( m_pAmplitudeInBuffer )
		delete m_pAmplitudeInBuffer;
	for(int i=0;i<MIDIChannelMessage::NumMIDINotes;i++)
	{
		delete m_pVoices[i];
	}
	*/
	this->m_pModuleServices->pLogger->writeLine("Oscillator destructor id=%d", iId);
}

/*
void Oscillator::setSamplesBufferMaximumSize(int size)
{
	m_SamplesBufferMaxSize = size;
	//Allocate two new buffers to hold input samples
	if( m_pPhaseInBuffer )
		delete [] m_pPhaseInBuffer;
	if( m_pAmplitudeInBuffer )
		delete [] m_pAmplitudeInBuffer;
	m_pAmplitudeInBuffer = new EAG_SAMPLE_TYPE[size];
	m_pPhaseInBuffer = new EAG_SAMPLE_TYPE[size];
	//Tell the buffer size to connected EUs
	ElaborationUnitPort* pPort = PhaseInPort.getNthEUPort(0);
	if( pPort )
	{
		ElaborationUnit* pEU;
		pEU = PhaseInPort.getNthEU(0);
		pEU->setSamplesBufferMaximumSize(size);
	}
	else
		for(int i=0;i<size;i++)
			m_pPhaseInBuffer[i] = EAG_SAMPLE_ZERO;
	pPort = AmplitudeInPort.getNthEUPort(0);
	if( pPort )
	{
		ElaborationUnit* pEU;
		pEU = AmplitudeInPort.getNthEU(0);
		pEU->setSamplesBufferMaximumSize(size);
	}
	else
		for(int i=0;i<size;i++)
			m_pAmplitudeInBuffer[i] = EAG_SAMPLE_ONE;
}
*/

EAG_SAMPLE_TYPE getSineSample(SimpleVoice& simpleVoice)
{
	return sin( simpleVoice.m_TimeAccumulator * 2.0 * 3.14159265358979323846 / simpleVoice.m_Period );
}

EAG_SAMPLE_TYPE getSquareSample(SimpleVoice& simpleVoice)
{
	double currentAccu = simpleVoice.m_TimeAccumulator - ((int)(simpleVoice.m_TimeAccumulator / simpleVoice.m_Period))*simpleVoice.m_Period;
	if (currentAccu >= (simpleVoice.m_Period / 2.0))
		return +1.0;
	else
		return -1.0;
}

EAG_SAMPLE_TYPE getSawSample(SimpleVoice& simpleVoice, double samplingFrequence)
{
	EAG_SAMPLE_TYPE increment = 1.0 / (samplingFrequence / simpleVoice.m_Period);
	EAG_SAMPLE_TYPE level = simpleVoice.m_TimeAccumulator * increment;

	return level;
}

EAG_SAMPLE_TYPE getTriangleSample(SimpleVoice& simpleVoice, double samplingFrequence)
{
	double currentAccu = simpleVoice.m_TimeAccumulator - ((int)(simpleVoice.m_TimeAccumulator / simpleVoice.m_Period))*simpleVoice.m_Period;
	EAG_SAMPLE_TYPE increment = 1.0 / (2 * samplingFrequence / simpleVoice.m_Period);
	EAG_SAMPLE_TYPE level = simpleVoice.m_TimeAccumulator * increment;
	if (currentAccu >= (simpleVoice.m_Period / 2.0))
	{
		// Growing
		return level;
	}
	else
	{
		// De-Growing
		return 1.0 - level;
	}
}

#define POLYPHONIC_ATTENUATION .5

SimpleGenerator::SampleCalculationResult Oscillator::calculateSample(EAG_SAMPLE_TYPE& result, SimpleVoice& simpleVoice)
{
	switch( m_WaveKind )
	{
	case e_Sine:
		{
			result = getSineSample(simpleVoice);
			break;
		}
	case e_Square:
		{
			result = getSquareSample(simpleVoice);
			break;
		}
	case e_Triangle:
		{
			result = getTriangleSample(simpleVoice, m_pModuleServices->getEngineSettings()->samplingFrequence);
			break;
		}
	case e_Saw:
		{
			result = getSawSample(simpleVoice, m_pModuleServices->getEngineSettings()->samplingFrequence);
			break;
		}
	default:
		this->m_pModuleServices->pLogger->writeLine("Undefined waveform!");
	}

	result *= POLYPHONIC_ATTENUATION;
	//this->m_pModuleServices->pLogger->writeLine("Wave=%f", result);

	return CALCULATION_CONTINUE;
}

bool Oscillator::IsPortMine(ElaborationUnitPort* pPort)
{
	if( (pPort == &PhaseInPort) ||
		(pPort == &AmplitudeInPort) ||
		(pPort == &MainOutPort) ||
		(pPort == &MIDIInPort)
		)
		return true;
	else
		return false;
}

ElaborationUnitPort* Oscillator::getNthInputPort(int n)
{
	switch(n)
	{
		case C_PhaseInIndex:
			return &PhaseInPort;
		case C_AmplitudeInIndex:
			return &AmplitudeInPort;
		case C_MIDIInIndex:
			return &MIDIInPort;
		default:
			return NULL;
	}
}

ElaborationUnitPort* Oscillator::getNthOutputPort(int n)
{
	if( n == C_MainOutIndex)
		return &MainOutPort;
	else
		return NULL;
}

bool Oscillator::setInputEU(ElaborationUnitPort* pPort, ElaborationUnit* pInputEU, ElaborationUnitPort* pInputPort)
{
	if( !IsPortMine(pPort) )
		return false;

	if( pPort == &PhaseInPort )
	{
		PhaseInPort.setNthEUandPort(pInputEU,pInputPort,0);
		pInputEU->setSamplesBufferMaximumSize(m_SamplesBufferMaxSize);
		return true;
	}
	else
	{
		if( pPort == &AmplitudeInPort )
		{
			AmplitudeInPort.setNthEUandPort(pInputEU,pInputPort,0);
			pInputEU->setSamplesBufferMaximumSize(m_SamplesBufferMaxSize);
			return true;
		}
		else
		{
			if( pPort == &MIDIInPort )
			{
				MIDIInPort.setNthEUandPort(pInputEU,pInputPort,0);
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

bool Oscillator::setOutputEU(ElaborationUnitPort* pPort, ElaborationUnit* pOutputEU, ElaborationUnitPort* pOutputPort)
{
	if( pPort == &MainOutPort )
	{
		MainOutPort.setNthEUandPort(pOutputEU,pOutputPort,C_MainOutIndex);
		return true;
	}
	else
		return false;
}

ElaborationUnitPort* Oscillator::getInputPortByEU(ElaborationUnit* pEU, int& n)
{
	if( MIDIInPort.getNthEU(0) == pEU )
	{
		n = 1;
		return &MIDIInPort;
	}
	if( AmplitudeInPort.getNthEU(0) == pEU )
	{
		n = 1;
		return &AmplitudeInPort;
	}
	if( PhaseInPort.getNthEU(0) == pEU )
	{
		n = 1;
		return &PhaseInPort;
	}
	return NULL;
}

ElaborationUnitPort* Oscillator::getOutputPortByEU(ElaborationUnit* pEU, int& n)
{
	ElaborationUnit* peu = MainOutPort.getNthEU(0);
	if( peu == pEU )
	{
		n = 1;
		return &MainOutPort;
	}
	return NULL;
}

int Oscillator::getInputPortNumber(void)
{
	return 3;
}

int Oscillator::getOutputPortNumber(void)
{
	return 1;
}

void Oscillator::allocate(void)
{
	SimpleGenerator::allocate();
}
void Oscillator::deallocate(void)
{
	SimpleGenerator::deallocate();
}

void Oscillator::play(void)
{
	int i = 0;
}

void Oscillator::pause(void)
{
}


void Oscillator::stop(void)
{
	this->m_pModuleServices->pLogger->writeLine("Stopping oscillator");
}
const EUKind* Oscillator::getKind(void)
{
	return s_GetKind();
}

const EUKind* Oscillator::s_GetKind(void)
{
	return (EUKind *) &kinna;
}


