﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SynthPanels;
using GeneralUtils;

namespace InputOutputSynthFactory
{
    class MIDIInputWrapper : AbstractSynthPanel
    {
		public MIDIInputWrapper(int id, SynthDelegateHolder deleHolder, Facilities facilities)
			: base(new MIDIInputPanel(), id, deleHolder, facilities)
        {

        }

    }
}
