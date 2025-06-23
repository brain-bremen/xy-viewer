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

#include "XYViewer.h"
#include "XYViewerEditor.h"

using namespace XYViewerPlugin;


XYViewer::XYViewer() 
    : GenericProcessor("XY Viewer")
{

}


XYViewer::~XYViewer()
{

}


AudioProcessorEditor* XYViewer::createEditor()
{
    editor = std::make_unique<XYViewerEditor>(this);
    return editor.get();
}


void XYViewer::updateSettings()
{


}


void XYViewer::process(AudioBuffer<float>& buffer)
{

    checkForEvents(true);
}


void XYViewer::handleTTLEvent(TTLEventPtr event)
{

}


void XYViewer::handleSpike(SpikePtr spike)
{

}


//void XYViewerPlugin::handleBroadcastMessage(String message)
//{

//}


void XYViewer::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void XYViewer::loadCustomParametersFromXml(XmlElement* parentElement)
{

}