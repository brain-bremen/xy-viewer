/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2022 Open Ephys
    Copyright (C) 2025 Joscha Schmiedt <schmiedt@uni-bremen.de>

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "XYViewerPlugin.h"

#include "XYViewerPluginEditor.h"


XYViewerPlugin::XYViewerPlugin() 
    : GenericProcessor("Visualizer Plugin")
{

}


XYViewerPlugin::~XYViewerPlugin()
{

}


AudioProcessorEditor* XYViewerPlugin::createEditor()
{
    editor = std::make_unique<XYViewerPluginEditor>(this);
    return editor.get();
}


void XYViewerPlugin::updateSettings()
{


}


void XYViewerPlugin::process(AudioBuffer<float>& buffer)
{

    checkForEvents(true);
	 
}


void XYViewerPlugin::handleTTLEvent(TTLEventPtr event)
{

}


void XYViewerPlugin::handleSpike(SpikePtr spike)
{

}


//void XYViewerPlugin::handleBroadcastMessage(String message)
//{

//}


void XYViewerPlugin::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void XYViewerPlugin::loadCustomParametersFromXml(XmlElement* parentElement)
{

}