


#include "AudioEngineAPI.h"
#include "windows.h"
#include <stdio.h>


#include "Utilities.h"

void displayAudioElements(void)
{
	//Factories scan
	int numFactories = ::getFactoryNumber();
	printf("Factories found = %d\n", numFactories);
	for(int factoryIndex=0;factoryIndex<numFactories;factoryIndex++)
	{
		const char* factoryName = ::getNthFactoryName(factoryIndex);
		printf("#############################################################\n");
		printf("Factory name = %s\n", factoryName);
		for(int categoryIndex=0;categoryIndex<2;categoryIndex++)
		{
			int numEU = ::getElaborationUnitNumber(factoryIndex, categoryIndex);
			printf("Contains %d %s ElaborationUnit(s)\n", numEU, categoryIndex==0 ? "Virtual" : "Physical");
			for(int elaborationUnitIndex=0;elaborationUnitIndex<numEU;elaborationUnitIndex++)
			{
				printf("-------------------------------------------------------------\n");
				const char* elaborationUnitName = ::getNthElaborationUnitName(factoryIndex,categoryIndex, elaborationUnitIndex);
				printf("ElaborationUnit name = %s \n",elaborationUnitName);

				int numInstances = ::getElaborationUnitInstanceNumber(factoryIndex, categoryIndex, elaborationUnitIndex);
				for(int instanceIndex=0;instanceIndex<numInstances;instanceIndex++)
				{
					wchar_t* instanceName = new wchar_t[1000];
					::getNthInstanceName(instanceName, 1000, factoryIndex, categoryIndex, elaborationUnitIndex, instanceIndex);
					wprintf(L"Instance #%d: name:%s\n", instanceIndex, instanceName);
				}

				printf("Input ports:\n");
				int numInPorts = ::getNthElaborationUnitInputPortNumber(factoryIndex, categoryIndex, elaborationUnitIndex);
				for(int inPortIndex = 0;inPortIndex<numInPorts;inPortIndex++)
				{
					const char *inPortName = ::getNthElaborationInputUnitPortName(factoryIndex, categoryIndex, elaborationUnitIndex,inPortIndex);
					wchar_t* pPortTypeBuffer = new wchar_t[1000];
					int bufferLen = 1000;
					::getNthElaborationUnitInputPortType(factoryIndex, categoryIndex, elaborationUnitIndex, inPortIndex, pPortTypeBuffer, bufferLen);
					printf("Port # %d name: %s Type: %S\n", inPortIndex, inPortName, pPortTypeBuffer);
				}

				printf("Output ports:\n");
				int numOutPorts = ::getNthElaborationUnitOutputPortNumber(factoryIndex, categoryIndex, elaborationUnitIndex);
				for(int outPortIndex = 0;outPortIndex<numOutPorts;outPortIndex++)
				{
					const char *outPortName = ::getNthElaborationUnitOutputPortName(factoryIndex,categoryIndex, elaborationUnitIndex,outPortIndex);
					wchar_t* pPortTypeBuffer = new wchar_t[1000];
					int bufferLen = 1000;
					::getNthElaborationUnitOutputPortType(factoryIndex, categoryIndex, elaborationUnitIndex, outPortIndex, pPortTypeBuffer, bufferLen);
 					printf("Port # %d name: %s Type: %S\n", outPortIndex, outPortName, pPortTypeBuffer);
				}

				int numProps = ::getElaborationUnitPropertyNumber(factoryIndex,categoryIndex,elaborationUnitIndex);
				printf("Controllable Properties number=%d\n", numProps);
				for(int propIndex=0;propIndex<numProps;propIndex++)
				{
					wchar_t* pBuffer = new wchar_t[1000];
					::getNthEUPropertyName(factoryIndex, categoryIndex, elaborationUnitIndex, propIndex, pBuffer, 1000);
					printf("Property name: %s\n", pBuffer);
				}

				printf("-------------------------------------------------------------\n");
			}
			
		}
		printf("#############################################################\n");
	}
}

HWND getConsoleHwnd(void)
   {
       #define MY_BUFSIZE 1024 // Buffer size for console window titles.
       HWND hwndFound;         // This is what is returned to the caller.
       wchar_t pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
                                           // WindowTitle.
       wchar_t pszOldWindowTitle[MY_BUFSIZE]; // Contains original
                                           // WindowTitle.

       // Fetch current window title.

	   ::GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

       // Format a "unique" NewWindowTitle.

       wsprintf(pszNewWindowTitle,L"%d/%d", GetTickCount(), GetCurrentProcessId());

       // Change current window title.

       SetConsoleTitle(pszNewWindowTitle);

       // Ensure window title has been updated.

       Sleep(40);

       // Look for NewWindowTitle.

       hwndFound=FindWindow(NULL, pszNewWindowTitle);

       // Restore original window title.

       SetConsoleTitle(pszOldWindowTitle);

       return(hwndFound);
   }
