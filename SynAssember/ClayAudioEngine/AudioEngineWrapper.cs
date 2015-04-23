﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using SynthPanels;
using System.IO;
using System.Windows.Controls;
using System.Windows.Input;


namespace ClayAudioEngine
{

	public partial class AudioEngineWrapper
    {

		private int m_NumFactories = 0;
		private List<ElaborationUnitFactory> m_Factories;


		public int init(StringBuilder path, Int32 hwnd, int samplingFrequence, int bitResolution, int numChannels)
		{
			deleHolder = new SynthDelegateHolder();
			deleHolder.writeEUDProp = new WriteEUDoubleProperty(EUPropertyPlumbing.writeEUDoubleProperty);
			deleHolder.writeEUIProp = new WriteEUIntegerProperty(EUPropertyPlumbing.writeEUIntegerProperty);
			deleHolder.readEUDprop = new ReadEUDoubleProperty(EUPropertyPlumbing.readEUDoubleProperty);
			deleHolder.readEUIProp = new ReadEUIntegerProperty(EUPropertyPlumbing.readEUIntegerProperty);
            String currentFolder = Directory.GetCurrentDirectory();
            // Try to init engine
            int result = -1;
            try
            {
                result = initEngine(path, hwnd, samplingFrequence, bitResolution, numChannels);
            }
            catch (Exception)
            {
                return result;
            }

            try
            {
                queryEUFactories();
                Directory.SetCurrentDirectory(currentFolder);
                euPlumbing = initPlumbing();
                querySynthPanels(path);
            }
            catch (Exception)
            {
                releaseEngine();
                return -1;
            }
            return result;
        }

        private EUPropertyPlumbing initPlumbing()
        {
            EUPropertyPlumbing plumbing = new EUPropertyPlumbing();

            return plumbing;
        }

		List<WriteEUDoubleProperty> dPropertyList = new List<WriteEUDoubleProperty>();

		SynthDelegateHolder deleHolder;

        private void querySynthPanels(StringBuilder path)
        {

			List<String> requestedFactories = new List<string>();
			foreach(ElaborationUnitFactory factory in m_Factories)
			{
				requestedFactories.Add(factory.getName());
			}

			SynthPanelManager.getDefault().initSynthPanelsFactories(path, requestedFactories, deleHolder);
		}

        public void release()
        {
            releaseEngine();
        }

        public void _setHwnd(Int32 hwnd)
        {
            setHwndEngine(hwnd);
        }

        public int getNumFactories()
        {
            m_NumFactories = getFactoryNumber();
            return m_NumFactories;
        }
        public String getFactoryName(int factoryIndex)
        {
            if (factoryIndex > m_NumFactories)
                return "";
            else
                return getNthFactoryName(factoryIndex);
        }

        public IList<ElaborationUnitFactory> getFactories()
        {
            return m_Factories;
        }

        public int createNewAlgorithm()
        {
            return createAlgorithm();
        }

		public int addElaborationUnit(int algoId, int euId)
		{
			return addElaborationUnitToAlgorithm(algoId, euId);
		}

        private const int PHYSICAL_CATEGORY = 1;
        private const int VIRTUAL_CATEGORY = 0;

        public int createNewVirtualElaborationUnit(int factoryIndex, int euIndex)
        {
            return createNewElaborationUnit(factoryIndex, VIRTUAL_CATEGORY, euIndex, 0);
        }

        public int createNewPhysicalElaborationUnit(int factoryIndex, int euIndex, int euInstance, int physicalInstanceId)
        {
            return createElaborationUnit(factoryIndex, PHYSICAL_CATEGORY, euIndex, physicalInstanceId);
        }

        public int createNewElaborationUnit(int factoryIndex, int euIndex, int euInstance, int physicalInstanceId)
        {
            //TODO: fix the category issue
            return createElaborationUnit(factoryIndex, 0, euIndex, physicalInstanceId);
        }

        public String getElaborationUnitCreationError(int error)
        {
            return "";
        }

        public int connectElaborationUnits(int algorithmId, int sourceId, int sourcePortIndex, int destinationId, int destinationPortIndex)
        {
            try
            {
                return connectElaboratioUnits(algorithmId, sourceId, sourcePortIndex, destinationId, destinationPortIndex);
            }
            catch (Exception)
            {
                releaseEngine();
                return -1;
            }
        }

        internal int writeEUProperty(int elaborationUnitIndex, int propertyIndex, String valueStr)
        {
            setEUProperty(elaborationUnitIndex, propertyIndex, valueStr);
            return 0;
        }

        internal int writeEUDProperty(int elaborationUnitIndex, int propertyIndex, double value)
        {
            return setEUDProperty(elaborationUnitIndex, propertyIndex, value);
        }

		internal int writeEUIProperty(int elaborationUnitIndex, int propertyIndex, int value)
		{
			return setEUIProperty(elaborationUnitIndex, propertyIndex, value);
		}

		internal double readEUDProperty(int elaborationUnitIndex, int propertyIndex)
		{
			return getEUDProperty(elaborationUnitIndex, propertyIndex);
		}

		internal int readEUIProperty(int elaborationUnitIndex, int propertyIndex)
		{
			return getEUIProperty(elaborationUnitIndex, propertyIndex);
		}

        public EUPropertyPlumbing euPlumbing;

        public ISynthPanel createNewPanel(String factoryName, String name, int id)
        {
            return SynthPanelManager.getDefault().createSynthPanel(factoryName, name, id);
        }

		internal PCKeyboardProcessor keybProcessor = new PCKeyboardProcessor();

		public byte[] FirePCKeyboardEvent(KeyEventArgs args)
		{
			byte[] msg = keybProcessor.ProcessPCKeyboardEvent(args);
			if(msg!=null)
			{
				sendMIDIMessage(keybProcessor.EUId, msg);
			}
			return msg;
		}
		
		public int PlayAlgorithm(int id)
		{
			return playAlgorithm(id);
		}

		public int StopAlgorithm(int id)
		{
			return stopAlgorithm(id);
		}

		static AudioEngineWrapper audioWrapper = null;
		public static AudioEngineWrapper getDefault()
		{
			if (audioWrapper == null)
			{
				audioWrapper = new AudioEngineWrapper();
			}
			return audioWrapper;
		}
    }
}


